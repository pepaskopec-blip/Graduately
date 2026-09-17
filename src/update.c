#include "maturita.h"

#include <string.h>

#ifdef _WIN32
#include <process.h>
#define update_getpid() ((long)_getpid())
#else
#include <unistd.h>
#define update_getpid() ((long)getpid())
#endif

/* ------------------------------------------------------------------ */
/* Self-update                                                        */
/* ------------------------------------------------------------------ */

/* Networking runs through the `curl` command rather than a linked HTTP
 * library. curl already handles TLS, redirects and proxies, it ships with
 * macOS, with every desktop Linux and with Windows 10 and later, and using it
 * keeps the three bundling scripts free of another shared library to ship.
 * Every call is asynchronous, so a slow or unreachable network never blocks
 * the interface.
 *
 * Packages are not published as releases. They live on the `builds` branch of
 * the repository, which CI rewrites on every change to main, so an update is
 * simply whatever that branch currently holds. The branch also carries a
 * VERSION file naming the commit its packages were built from; comparing that
 * against the commit baked into this binary is the whole check.
 *
 * raw.githubusercontent.com is cached by Fastly by branch name, so a URL that
 * says `/builds/VERSION` can keep serving yesterday's file after CI rewrote
 * the branch. The check therefore reads the branch tip from the Atom feed
 * (plain curl, no API token) and then fetches VERSION and the package from
 * that commit SHA. Serving files from raw.githubusercontent.com still avoids
 * the REST API and its unauthenticated rate limit, and needs no JSON parser.
 *
 * An application cannot reliably replace its own files while it is running, so
 * all three platforms follow the same shape: unpack the new version into a
 * staging directory beside the current install, write a small script that
 * waits for this process to exit before swapping it in, then launch that
 * script detached and quit. Staging beside the install keeps the final move on
 * one filesystem and proves up front that the location is writable. */

#define UPDATE_ATOM_URL   "https://github.com/" UPDATE_REPO \
                          "/commits/" UPDATE_BRANCH ".atom"
#define UPDATE_STAGING_DIR ".maturita-update"

#ifdef _WIN32
#define NULL_DEVICE "NUL"
#else
#define NULL_DEVICE "/dev/null"
#endif

typedef enum {
    UPDATE_IDLE,
    UPDATE_CHECKING,
    UPDATE_UP_TO_DATE,
    UPDATE_AVAILABLE,
    UPDATE_DOWNLOADING,
    UPDATE_STAGED,
    UPDATE_FAILED,
} UpdateState;

/* How this copy is installed, which decides what the update has to replace: a
 * macOS bundle, a Windows install directory, or a single AppImage file. Builds
 * run straight from the source tree cannot update themselves. */
typedef enum {
    INSTALL_UNSUPPORTED,
    INSTALL_MACOS_BUNDLE,
    INSTALL_WINDOWS_DIR,
    INSTALL_APPIMAGE,
} InstallKind;

static struct {
    UpdateState state;
    gboolean interactive;   /* user asked, so also report "up to date" */
    char *tip;              /* current commit of the builds branch itself */
    char *latest;           /* commit the published packages were built from */
    char *staging;          /* scratch directory beside the install */
    char *archive;          /* asset downloaded into the staging directory */
    char *source;           /* unpacked version the swap script moves in */
    char *script;           /* swap script, run after this process exits */
    char *error;
    goffset total;          /* asset size, 0 when the server did not say */
    guint poll_id;
    GtkWidget *banner;
    GtkWidget *banner_label;
    GtkWidget *banner_action;
    GtkWidget *status;      /* status line in the settings popover */
    GtkWidget *action;      /* check / update / restart button */
    GtkWidget *progress;
} up;

/* ------------------------------------------------------------------ */
/* Build identity                                                     */
/* ------------------------------------------------------------------ */

/* Commits are not ordered, so there is nothing to compare: the branch only
 * ever holds the current build, and anything else means this copy is behind.
 * Short and full forms count as the same commit, so the interface can show a
 * short one without confusing the check. */
/* First 7–40 hex digits on the line; ignores comments and CDN junk. */
static char *parse_commit(const char *text) {
    const char *p;
    size_t n;

    if (!text)
        return NULL;
    for (p = text; *p; p++) {
        if (g_ascii_isxdigit(*p)) {
            n = 0;
            while (g_ascii_isxdigit(p[n]))
                n++;
            if (n >= 7 && n <= 40)
                return g_strndup(p, n);
            p += n;
            if (!*p)
                break;
        }
    }
    return NULL;
}

/* First Grit::Commit/<40-hex> in a GitHub Atom feed is the branch tip. */
static char *parse_atom_commit(const char *text) {
    const char *p;

    if (!text)
        return NULL;
    p = strstr(text, "Commit/");
    while (p) {
        const char *s = p + 7;
        size_t n = 0;

        while (g_ascii_isxdigit(s[n]))
            n++;
        if (n == 40)
            return g_strndup(s, 40);
        p = strstr(s, "Commit/");
    }
    return NULL;
}

/* Pin raw.githubusercontent.com to a commit so Fastly cannot serve a stale
 * branch-named file. Falls back to the branch name if the tip is unknown. */
static char *builds_raw_url(const char *name) {
    const char *ref = (up.tip && up.tip[0]) ? up.tip : UPDATE_BRANCH;

    return g_strdup_printf("https://raw.githubusercontent.com/%s/%s/%s",
                           UPDATE_REPO, ref, name);
}

static gboolean same_commit(const char *a, const char *b) {
    char *ca = parse_commit(a);
    char *cb = parse_commit(b);
    size_t n;
    gboolean ok;

    if (!ca || !cb) {
        g_free(ca);
        g_free(cb);
        return FALSE;
    }
    n = MIN(strlen(ca), strlen(cb));
    ok = n >= 7 && g_ascii_strncasecmp(ca, cb, n) == 0;
    g_free(ca);
    g_free(cb);
    return ok;
}

/* Only builds published by CI carry a commit; see APP_COMMIT in maturita.h. */
static gboolean is_published_build(void) {
    return g_ascii_isxdigit(APP_COMMIT[0]) && strlen(APP_COMMIT) >= 7;
}

/* Commit hashes are unreadable at full length; seven characters is what git
 * itself shows. */
static char *short_commit(const char *sha) {
    return g_strndup(sha ? sha : "", 7);
}

/* ------------------------------------------------------------------ */
/* Install layout                                                     */
/* ------------------------------------------------------------------ */

/* The asset published for this platform, or NULL when the release carries
 * none. Only arm64 macOS and x86_64 Linux / Windows are built. */
static const char *asset_name(void) {
#if defined(_WIN32)
    return "maturita-windows-x64.zip";
#elif defined(__APPLE__)
#if defined(__aarch64__) || defined(__arm64__)
    return "maturita-macos-arm64.zip";
#else
    return NULL;
#endif
#elif defined(__x86_64__)
    return "maturita-linux-x86_64.AppImage";
#else
    return NULL;
#endif
}

/* What the update replaces, plus the directory holding it. */
static InstallKind install_target(char **target_out, char **parent_out) {
    char *exe = app_executable_path();
    InstallKind kind = INSTALL_UNSUPPORTED;
    char *target = NULL;

    *target_out = NULL;
    *parent_out = NULL;

#ifndef _WIN32
    {
        const char *appimage = g_getenv("APPIMAGE");

        if (appimage && appimage[0]) {
            target = g_strdup(appimage);
            kind = INSTALL_APPIMAGE;
        }
    }
#endif

#ifdef __APPLE__
    /* .../Maturita.app/Contents/MacOS/maturita -> .../Maturita.app */
    if (!target && exe) {
        char *macos_dir = g_path_get_dirname(exe);
        char *contents = g_path_get_dirname(macos_dir);
        char *bundle = g_path_get_dirname(contents);

        if (g_str_has_suffix(macos_dir, "MacOS") &&
            g_str_has_suffix(bundle, ".app")) {
            target = g_strdup(bundle);
            kind = INSTALL_MACOS_BUNDLE;
        }
        g_free(bundle);
        g_free(contents);
        g_free(macos_dir);
    }
#endif

#ifdef _WIN32
    /* The Windows package is a plain directory holding the exe and the GTK
     * runtime, so the directory itself is what gets refreshed. */
    if (!target && exe) {
        target = g_path_get_dirname(exe);
        kind = INSTALL_WINDOWS_DIR;
    }
#endif

    if (target) {
        *target_out = target;
        *parent_out = g_path_get_dirname(target);
    }
    g_free(exe);
    return kind;
}

/* An update is only worth offering when a release carries an asset for this
 * platform and the running copy is one this code knows how to replace. */
static gboolean update_supported(void) {
    char *target = NULL;
    char *parent = NULL;
    gboolean ok = install_target(&target, &parent) != INSTALL_UNSUPPORTED;

    g_free(target);
    g_free(parent);
    return ok && asset_name() != NULL;
}

/* Depth-first delete; GLib has no recursive remove. */
static void remove_tree(const char *path) {
    GDir *dir;

    if (!path || !g_file_test(path, G_FILE_TEST_EXISTS))
        return;

    if (g_file_test(path, G_FILE_TEST_IS_DIR) &&
        !g_file_test(path, G_FILE_TEST_IS_SYMLINK)) {
        const char *name;

        dir = g_dir_open(path, 0, NULL);
        if (dir) {
            while ((name = g_dir_read_name(dir))) {
                char *child = g_build_filename(path, name, NULL);

                remove_tree(child);
                g_free(child);
            }
            g_dir_close(dir);
        }
        g_rmdir(path);
    } else {
        g_remove(path);
    }
}

/* An interrupted swap leaves the staging directory behind; drop it so it does
 * not sit next to the install forever. */
void update_clear_staging(void) {
    char *target = NULL;
    char *parent = NULL;

    if (install_target(&target, &parent) != INSTALL_UNSUPPORTED) {
        char *stale = g_build_filename(parent, UPDATE_STAGING_DIR, NULL);

        remove_tree(stale);
        g_free(stale);
    }
    g_free(target);
    g_free(parent);
}

/* ------------------------------------------------------------------ */
/* Interface state                                                    */
/* ------------------------------------------------------------------ */

static void update_set_state(UpdateState state);

static void update_fail(const char *message) {
    g_free(up.error);
    up.error = g_strdup(message);
    update_set_state(UPDATE_FAILED);
}

static void update_refresh_ui(void) {
    gboolean busy = up.state == UPDATE_CHECKING ||
                    up.state == UPDATE_DOWNLOADING;
    gboolean banner = up.state == UPDATE_AVAILABLE ||
                      up.state == UPDATE_DOWNLOADING ||
                      up.state == UPDATE_STAGED;
    const char *status = "";
    const char *action = "update_check";
    char *owned = NULL;

    switch (up.state) {
        case UPDATE_IDLE:
        case UPDATE_UP_TO_DATE: {
            char *mine = short_commit(APP_COMMIT);

            owned = g_strdup_printf(tr(up.state == UPDATE_IDLE
                                           ? "update_current"
                                           : "update_uptodate"), mine);
            status = owned;
            g_free(mine);
            break;
        }
        case UPDATE_CHECKING:
            status = tr("update_checking");
            break;
        case UPDATE_AVAILABLE: {
            char *newest = short_commit(up.latest);

            owned = g_strdup_printf(tr("update_available"), newest);
            status = owned;
            action = "update_install";
            g_free(newest);
            break;
        }
        case UPDATE_DOWNLOADING:
            status = tr("update_downloading");
            action = "update_install";
            break;
        case UPDATE_STAGED:
            status = tr("update_staged");
            action = "update_restart";
            break;
        case UPDATE_FAILED:
            status = up.error ? up.error : tr("update_failed");
            break;
    }

    if (up.status)
        gtk_label_set_text(GTK_LABEL(up.status), status);
    if (up.action) {
        gtk_button_set_label(GTK_BUTTON(up.action), tr(action));
        gtk_widget_set_sensitive(up.action, !busy);
    }
    if (up.progress)
        gtk_widget_set_visible(up.progress, up.state == UPDATE_DOWNLOADING);

    if (up.banner) {
        gtk_widget_set_visible(up.banner, banner);
        if (banner) {
            gtk_label_set_text(GTK_LABEL(up.banner_label), status);
            gtk_button_set_label(GTK_BUTTON(up.banner_action), tr(action));
            gtk_widget_set_sensitive(up.banner_action, !busy);
        }
    }

    g_free(owned);
}

static void update_set_state(UpdateState state) {
    up.state = state;
    update_refresh_ui();
}

/* Called by apply_language() so switching language relabels the banner. */
void update_apply_lang(void) {
    update_refresh_ui();
}

/* ------------------------------------------------------------------ */
/* Running curl                                                       */
/* ------------------------------------------------------------------ */

/* Spawn curl and hand its stdout to `done`. FALSE means curl itself could not
 * be started, the one failure worth its own message. */
static gboolean curl_async(const char *const *argv, GAsyncReadyCallback done) {
    GSubprocess *proc;

    proc = g_subprocess_newv(argv,
                             G_SUBPROCESS_FLAGS_STDOUT_PIPE |
                                 G_SUBPROCESS_FLAGS_STDERR_SILENCE,
                             NULL);
    if (!proc)
        return FALSE;
    g_subprocess_communicate_utf8_async(proc, NULL, NULL, done, NULL);
    return TRUE;
}

/* ------------------------------------------------------------------ */
/* Staging and the swap script                                        */
/* ------------------------------------------------------------------ */

/* The macOS asset holds a single .app whose name we should not assume. */
static char *find_app_bundle(const char *dir) {
    GDir *d = g_dir_open(dir, 0, NULL);
    const char *name;
    char *found = NULL;

    if (!d)
        return NULL;
    while (!found && (name = g_dir_read_name(d))) {
        if (g_str_has_suffix(name, ".app"))
            found = g_build_filename(dir, name, NULL);
    }
    g_dir_close(d);
    return found;
}

/* Each script takes: pid target source staging. They all wait for this
 * process to exit, swap the new version in, relaunch and clean up after
 * themselves. Rolling back on a failed move keeps a broken half-update from
 * leaving the user with nothing to start. */
static const char *swap_script_body(InstallKind kind) {
    switch (kind) {
        case INSTALL_MACOS_BUNDLE:
            return "#!/bin/sh\n"
                   "pid=$1; target=$2; source=$3; staging=$4\n"
                   "n=0\n"
                   "while kill -0 \"$pid\" 2>/dev/null && [ $n -lt 600 ]; do\n"
                   "  sleep 0.2; n=$((n+1))\n"
                   "done\n"
                   "rm -rf \"$target.old\"\n"
                   "mv \"$target\" \"$target.old\" 2>/dev/null\n"
                   "if mv \"$source\" \"$target\" 2>/dev/null; then\n"
                   "  xattr -cr \"$target\" 2>/dev/null\n"
                   "  rm -rf \"$target.old\"\n"
                   "else\n"
                   "  mv \"$target.old\" \"$target\" 2>/dev/null\n"
                   "fi\n"
                   "open -n \"$target\"\n"
                   "rm -rf \"$staging\"\n"
                   "rm -f \"$0\"\n";
        case INSTALL_APPIMAGE:
            return "#!/bin/sh\n"
                   "pid=$1; target=$2; source=$3; staging=$4\n"
                   "n=0\n"
                   "while kill -0 \"$pid\" 2>/dev/null && [ $n -lt 600 ]; do\n"
                   "  sleep 0.2; n=$((n+1))\n"
                   "done\n"
                   "if mv \"$source\" \"$target\" 2>/dev/null; then\n"
                   "  chmod +x \"$target\"\n"
                   "fi\n"
                   "\"$target\" >/dev/null 2>&1 &\n"
                   "rm -rf \"$staging\"\n"
                   "rm -f \"$0\"\n";
        case INSTALL_WINDOWS_DIR:
            /* Copies over the install without mirroring, so the progress files
             * kept inside it survive. ping is the sleep that works without a
             * console window. */
            return "@echo off\r\n"
                   "set PID=%~1\r\n"
                   "set TARGET=%~2\r\n"
                   "set SOURCE=%~3\r\n"
                   "set STAGING=%~4\r\n"
                   ":wait\r\n"
                   "tasklist /FI \"PID eq %PID%\" 2>nul"
                   " | find \"%PID%\" >nul\r\n"
                   "if not errorlevel 1 (\r\n"
                   "  ping -n 2 127.0.0.1 >nul\r\n"
                   "  goto wait\r\n"
                   ")\r\n"
                   "xcopy \"%SOURCE%\\*\" \"%TARGET%\\\" /E /I /Y /Q >nul\r\n"
                   "start \"\" \"%TARGET%\\maturita.exe\"\r\n"
                   "rmdir /s /q \"%STAGING%\"\r\n"
                   "del \"%~f0\"\r\n";
        case INSTALL_UNSUPPORTED:
        default:
            return NULL;
    }
}

static gboolean write_swap_script(InstallKind kind) {
    const char *body = swap_script_body(kind);
    const char *name = kind == INSTALL_WINDOWS_DIR ? "maturita-update.cmd"
                                                   : "maturita-update.sh";

    if (!body)
        return FALSE;

    g_free(up.script);
    up.script = g_build_filename(g_get_tmp_dir(), name, NULL);
    if (!g_file_set_contents(up.script, body, -1, NULL))
        return FALSE;
    g_chmod(up.script, 0755);
    return TRUE;
}

/* Unpack the asset and write the swap script. Nothing outside the staging
 * directory is touched until the user restarts. */
static void install_staged(void) {
    char *target = NULL;
    char *parent = NULL;
    InstallKind kind = install_target(&target, &parent);
    char *source = NULL;
    gboolean ok;

    if (kind == INSTALL_APPIMAGE) {
        /* The asset already is the executable; there is nothing to unpack. */
        source = g_strdup(up.archive);
        ok = TRUE;
    } else {
        char *unpacked = g_build_filename(up.staging, "new", NULL);
        const char *argv[] = {"tar", "-xf", up.archive, "-C", unpacked, NULL};
        GSubprocess *proc;

        /* bsdtar reads zip archives and ships with both macOS and Windows 10
         * and later, so no separate unzip tool is needed. */
        ok = g_mkdir_with_parents(unpacked, 0755) == 0;
        if (ok) {
            proc = g_subprocess_newv(argv, G_SUBPROCESS_FLAGS_STDERR_SILENCE,
                                     NULL);
            ok = proc && g_subprocess_wait_check(proc, NULL, NULL);
            g_clear_object(&proc);
        }
        if (ok && kind == INSTALL_MACOS_BUNDLE) {
            source = find_app_bundle(unpacked);
            ok = source != NULL;
        } else if (ok) {
            source = g_strdup(unpacked);
        }
        g_free(unpacked);
    }

    if (ok)
        ok = write_swap_script(kind);

    g_free(up.source);
    up.source = source;
    g_free(target);
    g_free(parent);

    if (ok)
        update_set_state(UPDATE_STAGED);
    else
        update_fail(tr("update_err_stage"));
}

/* ------------------------------------------------------------------ */
/* Download                                                           */
/* ------------------------------------------------------------------ */

/* curl writes straight to disk, so the partial file's size is the progress. */
static gboolean download_poll(gpointer data) {
    GStatBuf st;

    (void)data;
    if (up.state != UPDATE_DOWNLOADING) {
        up.poll_id = 0;
        return G_SOURCE_REMOVE;
    }
    if (up.progress && up.archive && g_stat(up.archive, &st) == 0) {
        if (up.total > 0)
            gtk_progress_bar_set_fraction(
                GTK_PROGRESS_BAR(up.progress),
                CLAMP((double)st.st_size / (double)up.total, 0.0, 1.0));
        else
            gtk_progress_bar_pulse(GTK_PROGRESS_BAR(up.progress));
    }
    return G_SOURCE_CONTINUE;
}

static void download_done(GObject *src, GAsyncResult *res, gpointer data) {
    GSubprocess *proc = G_SUBPROCESS(src);
    gboolean ok;

    (void)data;
    g_subprocess_communicate_utf8_finish(proc, res, NULL, NULL, NULL);
    ok = g_subprocess_get_successful(proc);
    g_object_unref(proc);

    if (up.poll_id) {
        g_source_remove(up.poll_id);
        up.poll_id = 0;
    }

    if (!ok) {
        update_fail(tr("update_err_download"));
        return;
    }
    install_staged();
}

/* Content-Length of the asset, read from the headers of every redirect hop.
 * The last one is the file itself; the hops before it report zero. curl has no
 * --write-out variable for this, since a HEAD downloads no body. */
static goffset content_length_from_headers(const char *headers) {
    char **lines = g_strsplit(headers ? headers : "", "\n", -1);
    goffset total = 0;

    for (guint i = 0; lines[i]; i++) {
        if (g_ascii_strncasecmp(lines[i], "content-length:", 15) == 0) {
            goffset v = (goffset)g_ascii_strtoll(lines[i] + 15, NULL, 10);

            if (v > 0)
                total = v;
        }
    }
    g_strfreev(lines);
    return total;
}

/* The asset size only makes the progress bar meaningful, so a failure here
 * costs nothing: the bar falls back to pulsing. */
static void size_probe_done(GObject *src, GAsyncResult *res, gpointer data) {
    GSubprocess *proc = G_SUBPROCESS(src);
    char *out = NULL;
    char *url;
    const char *argv[8];

    (void)data;
    g_subprocess_communicate_utf8_finish(proc, res, &out, NULL, NULL);
    up.total = content_length_from_headers(out);
    g_free(out);
    g_object_unref(proc);

    if (up.state != UPDATE_DOWNLOADING)
        return;

    url = builds_raw_url(asset_name());
    argv[0] = "curl";
    argv[1] = "-fsSL";
    argv[2] = "--max-time";
    argv[3] = "1800";
    argv[4] = "-o";
    argv[5] = up.archive;
    argv[6] = url;
    argv[7] = NULL;

    if (!curl_async(argv, download_done))
        update_fail(tr("update_err_curl"));
    else
        up.poll_id = g_timeout_add(250, download_poll, NULL);
    g_free(url);
}

static void update_start_download(void) {
    char *target = NULL;
    char *parent = NULL;
    char *url;
    const char *argv[10];

    if (install_target(&target, &parent) == INSTALL_UNSUPPORTED ||
        !asset_name()) {
        g_free(target);
        g_free(parent);
        update_fail(tr("update_err_unsupported"));
        return;
    }

    /* Staging beside the install keeps the final move on one filesystem, and
     * fails here, before anything is downloaded, if we cannot write there. */
    g_free(up.staging);
    up.staging = g_build_filename(parent, UPDATE_STAGING_DIR, NULL);
    remove_tree(up.staging);
    g_free(target);
    g_free(parent);
    if (g_mkdir_with_parents(up.staging, 0755) != 0) {
        update_fail(tr("update_err_readonly"));
        return;
    }

    g_free(up.archive);
    up.archive = g_build_filename(up.staging, asset_name(), NULL);
    up.total = 0;
    update_set_state(UPDATE_DOWNLOADING);

    /* HEAD the asset with its headers on stdout, to size the progress bar. */
    url = builds_raw_url(asset_name());
    argv[0] = "curl";
    argv[1] = "-fsSLI";
    argv[2] = "--max-time";
    argv[3] = "20";
    argv[4] = "-D";
    argv[5] = "-";
    argv[6] = "-o";
    argv[7] = NULL_DEVICE;
    argv[8] = url;
    argv[9] = NULL;

    if (!curl_async(argv, size_probe_done))
        update_fail(tr("update_err_curl"));
    g_free(url);
}

/* ------------------------------------------------------------------ */
/* Check                                                              */
/* ------------------------------------------------------------------ */

/* A failed background check stays silent; only an explicit one reports. */
static void check_giveup(void) {
    if (up.interactive)
        update_fail(tr("update_err_network"));
    else
        update_set_state(UPDATE_IDLE);
}

static void check_done(GObject *src, GAsyncResult *res, gpointer data) {
    GSubprocess *proc = G_SUBPROCESS(src);
    char *out = NULL;
    gboolean ok;

    (void)data;
    g_subprocess_communicate_utf8_finish(proc, res, &out, NULL, NULL);
    ok = g_subprocess_get_successful(proc);
    g_object_unref(proc);

    if (out)
        g_strstrip(out);
    if (!ok || !out || !*out) {
        g_free(out);
        check_giveup();
        return;
    }

    g_free(up.latest);
    up.latest = parse_commit(out);
    g_free(out);
    if (!up.latest) {
        check_giveup();
        return;
    }

    if (same_commit(APP_COMMIT, up.latest))
        update_set_state(up.interactive ? UPDATE_UP_TO_DATE : UPDATE_IDLE);
    else if (update_supported())
        update_set_state(UPDATE_AVAILABLE);
    else if (up.interactive)
        /* A newer build exists, but this copy cannot swap itself out. */
        update_fail(tr("update_err_unsupported"));
    else
        update_set_state(UPDATE_IDLE);
}

static void tip_done(GObject *src, GAsyncResult *res, gpointer data) {
    GSubprocess *proc = G_SUBPROCESS(src);
    char *out = NULL;
    gboolean ok;
    char *url;
    const char *argv[6];

    (void)data;
    g_subprocess_communicate_utf8_finish(proc, res, &out, NULL, NULL);
    ok = g_subprocess_get_successful(proc);
    g_object_unref(proc);

    g_free(up.tip);
    up.tip = (ok && out) ? parse_atom_commit(out) : NULL;
    g_free(out);
    if (!up.tip) {
        check_giveup();
        return;
    }

    url = builds_raw_url("VERSION");
    argv[0] = "curl";
    argv[1] = "-fsSL";
    argv[2] = "--max-time";
    argv[3] = "20";
    argv[4] = url;
    argv[5] = NULL;

    if (!curl_async(argv, check_done))
        check_giveup();
    g_free(url);
}

void update_check_async(gboolean interactive) {
    const char *argv[6];

    if (up.state == UPDATE_CHECKING || up.state == UPDATE_DOWNLOADING ||
        up.state == UPDATE_STAGED)
        return;

    up.interactive = interactive;

    /* Nothing to compare against without a baked-in commit. */
    if (!is_published_build()) {
        if (interactive)
            update_fail(tr("update_err_devbuild"));
        return;
    }

    update_set_state(UPDATE_CHECKING);

    argv[0] = "curl";
    argv[1] = "-fsSL";
    argv[2] = "--max-time";
    argv[3] = "20";
    argv[4] = UPDATE_ATOM_URL;
    argv[5] = NULL;

    if (!curl_async(argv, tip_done)) {
        if (interactive)
            update_fail(tr("update_err_curl"));
        else
            update_set_state(UPDATE_IDLE);
    }
}

/* ------------------------------------------------------------------ */
/* Restart into the new version                                       */
/* ------------------------------------------------------------------ */

static void update_restart(void) {
    char *target = NULL;
    char *parent = NULL;
    InstallKind kind = install_target(&target, &parent);
    char *pid = g_strdup_printf("%ld", update_getpid());
    GSubprocess *proc;
    const char *argv[8];
    int i = 0;

    if (kind == INSTALL_UNSUPPORTED || !up.script || !up.source ||
        !up.staging) {
        g_free(pid);
        g_free(target);
        g_free(parent);
        update_fail(tr("update_err_stage"));
        return;
    }

#ifdef _WIN32
    argv[i++] = "cmd";
    argv[i++] = "/c";
#else
    argv[i++] = "/bin/sh";
#endif
    argv[i++] = up.script;
    argv[i++] = pid;
    argv[i++] = target;
    argv[i++] = up.source;
    argv[i++] = up.staging;
    argv[i] = NULL;

    /* The child outlives us on purpose: it is what puts the new version in
     * place once this process is gone. */
    proc = g_subprocess_newv(argv, G_SUBPROCESS_FLAGS_NONE, NULL);
    g_free(pid);
    g_free(target);
    g_free(parent);

    if (!proc) {
        update_fail(tr("update_err_stage"));
        return;
    }
    g_object_unref(proc);

    if (main_window) {
        GtkApplication *app = gtk_window_get_application(main_window);

        if (app)
            g_application_quit(G_APPLICATION(app));
        else
            gtk_window_close(main_window);
    }
}

/* ------------------------------------------------------------------ */
/* Widgets                                                            */
/* ------------------------------------------------------------------ */

static void on_update_action(GtkButton *btn, gpointer data) {
    (void)btn;
    (void)data;

    switch (up.state) {
        case UPDATE_AVAILABLE:
            update_start_download();
            break;
        case UPDATE_STAGED:
            update_restart();
            break;
        case UPDATE_DOWNLOADING:
        case UPDATE_CHECKING:
            break;
        default:
            update_check_async(TRUE);
            break;
    }
}

static void on_update_dismiss(GtkButton *btn, gpointer data) {
    (void)btn;
    (void)data;
    if (up.banner)
        gtk_widget_set_visible(up.banner, FALSE);
}

/* Strip shown above the page stack once an update is waiting. */
GtkWidget *build_update_banner(void) {
    GtkWidget *bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *label = gtk_label_new("");
    GtkWidget *action = gtk_button_new_with_label("");
    GtkWidget *later = gtk_button_new();

    gtk_widget_add_css_class(bar, "update-banner");
    gtk_widget_set_visible(bar, FALSE);

    gtk_widget_set_hexpand(label, TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_widget_add_css_class(label, "update-banner-text");
    gtk_box_append(GTK_BOX(bar), label);

    gtk_widget_add_css_class(action, "update-banner-btn");
    gtk_widget_set_valign(action, GTK_ALIGN_CENTER);
    g_signal_connect(action, "clicked", G_CALLBACK(on_update_action), NULL);
    gtk_box_append(GTK_BOX(bar), action);

    i18n_bind(later, "update_later", 1);
    gtk_widget_add_css_class(later, "update-banner-later");
    gtk_widget_set_valign(later, GTK_ALIGN_CENTER);
    g_signal_connect(later, "clicked", G_CALLBACK(on_update_dismiss), NULL);
    gtk_box_append(GTK_BOX(bar), later);

    up.banner = bar;
    up.banner_label = label;
    up.banner_action = action;
    return bar;
}

/* "Updates" section of the settings popover. */
GtkWidget *build_update_box(void) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *status = gtk_label_new("");
    GtkWidget *action = gtk_button_new_with_label("");
    GtkWidget *progress = gtk_progress_bar_new();

    gtk_widget_set_hexpand(status, TRUE);
    gtk_label_set_xalign(GTK_LABEL(status), 0.0);
    gtk_label_set_wrap(GTK_LABEL(status), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(status), 30);
    gtk_widget_add_css_class(status, "update-status");
    gtk_box_append(GTK_BOX(row), status);

    gtk_widget_add_css_class(action, "update-btn");
    gtk_widget_set_valign(action, GTK_ALIGN_CENTER);
    g_signal_connect(action, "clicked", G_CALLBACK(on_update_action), NULL);
    gtk_box_append(GTK_BOX(row), action);
    gtk_box_append(GTK_BOX(box), row);

    gtk_widget_add_css_class(progress, "update-progress");
    gtk_widget_set_visible(progress, FALSE);
    gtk_box_append(GTK_BOX(box), progress);

    up.status = status;
    up.action = action;
    up.progress = progress;
    update_refresh_ui();
    return box;
}

/* ------------------------------------------------------------------ */
/* Startup                                                            */
/* ------------------------------------------------------------------ */

static gboolean update_startup_check(gpointer data) {
    (void)data;
    update_check_async(FALSE);
    return G_SOURCE_REMOVE;
}

/* Leaves the first seconds to the interface, then looks for a new release. */
void update_init(void) {
    update_clear_staging();
    g_timeout_add_seconds(3, update_startup_check, NULL);
}
