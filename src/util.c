#include "maturita.h"

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <unistd.h>
#else
#include <unistd.h>
#endif

/* ------------------------------------------------------------------ */
/* Portable data / progress paths                                     */
/* ------------------------------------------------------------------ */

char *app_progress_dir;
char *app_progress_u1;
char *app_progress_u2;
char *app_progress_u3;
char *app_settings_file;
char *app_progress_net;
char *app_progress_hw;
char *app_progress_mluvnice;
char *app_progress_cetba;

static char *dup_dirname(const char *path) {
    char *tmp = g_strdup(path);
    char *dir;

    if (!tmp)
        return NULL;
    dir = g_path_get_dirname(tmp);
    g_free(tmp);
    return dir;
}

/* Absolute path of this process's executable, or NULL. */
char *app_executable_path(void) {
#ifdef _WIN32
    wchar_t wbuf[MAX_PATH];
    DWORD n = GetModuleFileNameW(NULL, wbuf, MAX_PATH);
    char *out;

    if (n == 0 || n >= MAX_PATH)
        return NULL;
    out = g_utf16_to_utf8((const gunichar2 *)wbuf, -1, NULL, NULL, NULL);
    return out;
#elif defined(__APPLE__)
    char *buf;
    uint32_t size = 0;

    _NSGetExecutablePath(NULL, &size);
    if (size == 0)
        return NULL;
    buf = g_malloc(size);
    if (_NSGetExecutablePath(buf, &size) != 0) {
        g_free(buf);
        return NULL;
    }
    {
        char *real = g_canonicalize_filename(buf, NULL);

        g_free(buf);
        return real ? real : NULL;
    }
#else
    return g_file_read_link("/proc/self/exe", NULL);
#endif
}

static gboolean dir_has_stylesheet(const char *dir) {
    char *css;
    gboolean ok;

    if (!dir || !dir[0])
        return FALSE;
    css = g_build_filename(dir, CSS_FILE, NULL);
    ok = g_file_test(css, G_FILE_TEST_IS_REGULAR);
    g_free(css);
    return ok;
}

static char *macos_resources_dir(const char *exe) {
    char *macos_dir;
    char *contents;
    char *resources;

    if (!exe)
        return NULL;
    macos_dir = dup_dirname(exe);
    if (!macos_dir)
        return NULL;
    if (!g_str_has_suffix(macos_dir, "MacOS")) {
        g_free(macos_dir);
        return NULL;
    }
    contents = dup_dirname(macos_dir);
    g_free(macos_dir);
    if (!contents)
        return NULL;
    resources = g_build_filename(contents, "Resources", NULL);
    g_free(contents);
    if (!dir_has_stylesheet(resources)) {
        g_free(resources);
        return NULL;
    }
    return resources;
}

/* Directory next to Foo.app when exe is .../Foo.app/Contents/MacOS/maturita. */
static char *macos_app_parent_dir(const char *exe) {
    char *macos_dir;
    char *contents;
    char *app_bundle;
    char *parent;

    if (!exe)
        return NULL;
    macos_dir = dup_dirname(exe);
    if (!macos_dir || !g_str_has_suffix(macos_dir, "MacOS")) {
        g_free(macos_dir);
        return NULL;
    }
    contents = dup_dirname(macos_dir);
    g_free(macos_dir);
    if (!contents)
        return NULL;
    app_bundle = dup_dirname(contents);
    g_free(contents);
    if (!app_bundle)
        return NULL;
    parent = dup_dirname(app_bundle);
    g_free(app_bundle);
    return parent;
}

static void set_progress_paths(const char *write_root) {
    g_free(app_progress_dir);
    g_free(app_progress_u1);
    g_free(app_progress_u2);
    g_free(app_progress_u3);
    g_free(app_settings_file);
    g_free(app_progress_net);
    g_free(app_progress_hw);
    g_free(app_progress_mluvnice);
    g_free(app_progress_cetba);

    app_progress_dir = g_build_filename(write_root, PROGRESS_DIR_NAME, NULL);
    app_progress_u1 = g_build_filename(app_progress_dir, "unit1.conf", NULL);
    app_progress_u2 = g_build_filename(app_progress_dir, "unit2.conf", NULL);
    app_progress_u3 = g_build_filename(app_progress_dir, "unit3.conf", NULL);
    app_settings_file = g_build_filename(app_progress_dir, "settings.conf", NULL);
    app_progress_net = g_build_filename(app_progress_dir, "net.conf", NULL);
    app_progress_hw = g_build_filename(app_progress_dir, "hw.conf", NULL);
    app_progress_mluvnice = g_build_filename(app_progress_dir, "mluvnice.conf", NULL);
    app_progress_cetba = g_build_filename(app_progress_dir, "cetba.conf", NULL);
}

/* Resolve read-only asset root, chdir there, and point progress at a writable
 * location (next to the AppImage / .app, or beside the binary / source tree). */
void setup_portable_paths(void) {
    const char *appdir = g_getenv("APPDIR");
    const char *appimage = g_getenv("APPIMAGE");
    char *exe = app_executable_path();
    char *exe_dir = exe ? dup_dirname(exe) : NULL;
    char *data_dir = NULL;
    char *write_root = NULL;
    char *cwd;

    if (appdir && dir_has_stylesheet(appdir))
        data_dir = g_strdup(appdir);
    if (!data_dir)
        data_dir = macos_resources_dir(exe);
    if (!data_dir && exe_dir && dir_has_stylesheet(exe_dir))
        data_dir = g_strdup(exe_dir);

    cwd = g_get_current_dir();
    if (!data_dir && dir_has_stylesheet(cwd))
        data_dir = g_strdup(cwd);
    if (!data_dir && cwd)
        data_dir = g_strdup(cwd);

    if (appimage && appimage[0])
        write_root = dup_dirname(appimage);
    if (!write_root)
        write_root = macos_app_parent_dir(exe);
    if (!write_root && exe_dir)
        write_root = g_strdup(exe_dir);
    if (!write_root && data_dir)
        write_root = g_strdup(data_dir);
    if (!write_root)
        write_root = g_strdup(".");

    set_progress_paths(write_root);

    if (data_dir && g_chdir(data_dir) != 0)
        g_warning("Could not chdir to data directory '%s'", data_dir);

    g_free(cwd);
    g_free(write_root);
    g_free(data_dir);
    g_free(exe_dir);
    g_free(exe);
}

char *normalize_answer(const char *input) {
    GString *pre = g_string_new(NULL);
    GString *out = g_string_new(NULL);
    const gchar *down = g_utf8_strdown(input ? input : "", -1);
    gchar *decomp;
    const gchar *p;
    gboolean prev_space = FALSE;

    /* German special letters first, so ä/ö/ü/ß keep their two-letter
     * keyboard fallbacks (ae/oe/ue/ss). */
    for (p = down; *p; p = g_utf8_next_char(p)) {
        gunichar ch = g_utf8_get_char(p);

        switch (ch) {
            case 0x00e4: g_string_append(pre, "ae"); break; /* ä */
            case 0x00f6: g_string_append(pre, "oe"); break; /* ö */
            case 0x00fc: g_string_append(pre, "ue"); break; /* ü */
            case 0x00df: g_string_append(pre, "ss"); break; /* ß */
            default:     g_string_append_unichar(pre, ch); break;
        }
    }

    /* Decompose and drop combining accent marks so Czech letters such as
     * é/ě/ř/ů/ž/š/č are treated as their base letters. */
    decomp = g_utf8_normalize(pre->str, -1, G_NORMALIZE_NFD);
    if (!decomp)
        decomp = g_strdup(pre->str);

    for (p = decomp; *p; p = g_utf8_next_char(p)) {
        gunichar ch = g_utf8_get_char(p);

        if (g_unichar_combining_class(ch) != 0)
            continue;   /* accent mark */

        if (g_unichar_isspace(ch)) {
            if (out->len > 0 && !prev_space)
                g_string_append_c(out, ' ');
            prev_space = TRUE;
            continue;
        }

        if (!g_unichar_isalnum(ch))
            continue;   /* ignore punctuation and symbols */

        prev_space = FALSE;
        g_string_append_unichar(out, ch);
    }

    if (out->len > 0 && out->str[out->len - 1] == ' ')
        g_string_truncate(out, out->len - 1);

    g_free(decomp);
    g_free((gpointer)down);
    g_string_free(pre, TRUE);
    return g_string_free(out, FALSE);
}

void shuffle_indices(int *arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = g_random_int_range(0, i + 1);
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

void flow_clear(GtkFlowBox *fb) {
    GtkWidget *c = gtk_widget_get_first_child(GTK_WIDGET(fb));
    while (c) {
        GtkWidget *next = gtk_widget_get_next_sibling(c);
        gtk_flow_box_remove(fb, c);
        c = next;
    }
}

void answer_mark(GtkWidget *widget, gboolean ok) {
    if (ok) {
        gtk_widget_remove_css_class(widget, "answer-wrong");
        gtk_widget_add_css_class(widget, "answer-ok");
    } else {
        gtk_widget_remove_css_class(widget, "answer-ok");
        gtk_widget_add_css_class(widget, "answer-wrong");
    }
}

void set_feedback(GtkWidget *label, gboolean ok, const char *text) {
    gtk_widget_remove_css_class(label, "feedback-ok");
    gtk_widget_remove_css_class(label, "feedback-err");
    gtk_widget_remove_css_class(label, "feedback-warn");
    gtk_widget_add_css_class(label, ok ? "feedback-ok" : "feedback-err");
    gtk_label_set_text(GTK_LABEL(label), text);
}

/* Yellow "almost there" note: close to a correct answer but incomplete. */
void set_feedback_warn(GtkWidget *label, const char *text) {
    gtk_widget_remove_css_class(label, "feedback-ok");
    gtk_widget_remove_css_class(label, "feedback-err");
    gtk_widget_remove_css_class(label, "feedback-warn");
    gtk_widget_add_css_class(label, "feedback-warn");
    gtk_label_set_text(GTK_LABEL(label), text);
}

/* True if `s` contains any German special letter (ä, ö, ü or ß). */
gboolean text_has_german_umlaut(const char *s) {
    const char *p = s ? s : "";

    while (*p) {
        gunichar c = g_utf8_get_char(p);

        if (c == 0x00e4 || c == 0x00f6 || c == 0x00fc || c == 0x00df)
            return TRUE;
        p = g_utf8_next_char(p);
    }
    return FALSE;
}

/* Small bilingual note shown under the exercise title whenever the answer
 * the student has to type may contain a German special letter. */
void add_umlaut_note(GtkWidget *body) {
    GtkWidget *n = gtk_label_new(NULL);

    i18n_bind(n, "hint_umlauts", 0);
    gtk_widget_set_halign(n, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(n), TRUE);
    gtk_widget_set_margin_bottom(n, 2);
    gtk_widget_add_css_class(n, "hint");
    gtk_box_append(GTK_BOX(body), n);
}
