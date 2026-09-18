#include "maturita.h"

/* ------------------------------------------------------------------ */
/* Activate                                                           */
/* ------------------------------------------------------------------ */

void unit_meta_init(void) {
    static const char *titles[NUM_UNITS] = {
        "Neue Freunde", "Aus aller Welt", "Bei uns zu Hause",
        "Schule und Freizeit", "Guten Appetit!", "Mein Tagesablauf",
        "Meine Freunde", "Wir treffen uns in Salzburg",
        "Mein Haus ist meine Burg", "Urlaub in Österreich",
    };
    static const char *pages[NUM_UNITS] = {
        "unit1", "unit2", "unit3", NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    };
    static const char *tags[NUM_UNITS] = {
        "u1", "u2", "u3", NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    };

    for (int i = 0; i < NUM_UNITS; i++) {
        units[i].title = titles[i];
        units[i].page = pages[i];
        units[i].ex_tag = tags[i];
        units[i].sub_key = NULL;
        units[i].unlocked = (i < NUM_UNLOCKED);
    }
    units[0].sub_key = "unit1_sub";
    units[0].progress_file = PROGRESS_U1;
    units[1].sub_key = "unit2_sub";
    units[1].progress_file = PROGRESS_U2;
    units[2].sub_key = "unit3_sub";
    units[2].progress_file = PROGRESS_U3;

    /* Off-path branch hanging above exercise 2 of unit 1. */
    units[0].has_branch = TRUE;
    units[0].branch_name = "Vokabeltraining";
    units[0].branch_target = "u1vocab";
    units[0].branch_ex = MAX_UNIT_EX;
    units[0].trans_sections = u1_trans_sections;
    units[0].n_trans_sections = u1_trans_sections_n;

    /* Same vocabulary-training branch for unit 2 ("Aus aller Welt"). */
    units[1].has_branch = TRUE;
    units[1].branch_name = "Vokabeltraining";
    units[1].branch_target = "u2vocab";
    units[1].branch_ex = MAX_UNIT_EX;
    units[1].trans_sections = u2_trans_sections;
    units[1].n_trans_sections = u2_trans_sections_n;

    /* Same vocabulary-training branch for unit 3 ("Bei uns zu Hause"). */
    units[2].has_branch = TRUE;
    units[2].branch_name = "Vokabeltraining";
    units[2].branch_target = "u3vocab";
    units[2].branch_ex = MAX_UNIT_EX;
    units[2].trans_sections = u3_trans_sections;
    units[2].n_trans_sections = u3_trans_sections_n;
}

void unit_configure(int idx, const char *const *names, int n) {
    UnitCtx *u = &units[idx];

    u->n_ex = n;
    for (int i = 1; i <= n; i++)
        u->ex_names[i] = names[i];
}

GtkWidget *build_exercise_page(UnitCtx *u, int n) {
    if (u->ex_tag[1] == '1') {
        switch (n) {
            case 1:  return build_ex1(u);
            case 2:  return build_assembly(u, "Sätze bilden", "sub_assembly", 2,
                                           ex2_items, ex2_meaning,
                                           4);
            case 3:  return build_choice(u, "Was ist richtig?", "sub_choice_num",
                                         3, ex3_questions, ex3_meaning,
                                         4);
            case 4:  return build_ex4(u);
            case 5:  return build_ex5(u);
            case 6:  return build_ex6(u);
            case 7:  return build_ex7(u);
            case 8:  return build_ex8(u);
            case 9:  return build_choice(u, "Wer? Wie? Wo?", "sub_wer",
                                         9, ex9_questions, ex9_meaning,
                                         5);
            case 10: return build_assembly(u, "Wörter trennen", "sub_assembly",
                                           10, ex10_items, ex10_meaning,
                                           5);
            case 11: return build_assign(u, "Grußformen", "sub_gruss",
                                         11, ex11_items,
                                         12,
                                         ex11_groups, 2,
                                         ex11_meaning);
            case 12: return build_ex12(u);
            case 13: return build_assign(u, "Länder", "sub_land",
                                         13, ex13_items,
                                         10,
                                         ex13_groups, 3,
                                         ex13_meaning);
            default: return NULL;
        }
    } else if (u->ex_tag[1] == '2') {
        switch (n) {
            case 1:  return u2ex1(u);
            case 2:  return u2ex2(u);
            case 3:  return u2ex3(u);
            case 4:  return u2ex4(u);
            case 5:  return u2ex5(u);
            case 6:  return u2ex6(u);
            case 7:  return u2ex7(u);
            case 8:  return u2ex8(u);
            case 9:  return u2ex9(u);
            case 10: return u2ex10(u);
            case 11: return u2ex11(u);
            case 12: return u2ex12(u);
            case 13: return u2ex13(u);
            case 14: return u2ex14(u);
            case 15: return u2ex15(u);
            case 16: return u2ex16(u);
            case 17: return u2ex17(u);
            case 18: return u2ex18(u);
            case 19: return u2ex19(u);
            default: return NULL;
        }
    } else {
        switch (n) {
            case 1:  return u3ex1(u);
            case 2:  return u3ex2(u);
            case 3:  return u3ex3(u);
            case 4:  return u3ex4(u);
            case 5:  return u3ex5(u);
            case 6:  return u3ex6(u);
            case 7:  return u3ex7(u);
            case 8:  return u3ex8(u);
            case 9:  return u3ex9(u);
            case 10: return u3ex10(u);
            case 11: return u3ex11(u);
            case 12: return u3ex12(u);
            case 13: return u3ex13(u);
            case 14: return u3ex14(u);
            case 15: return u3ex15(u);
            default: return NULL;
        }
    }
}

static int ui_scale_last_w = -1;
static guint ui_scale_timer = 0;
static int ui_scale_pending_w = 0;

static gboolean ui_scale_apply(gpointer data) {
    double scale;

    (void)data;
    ui_scale_timer = 0;
    scale = 1.0 + (ui_scale_pending_w - 760) / 1900.0;
    if (scale < 1.0)
        scale = 1.0;
    if (scale > 1.5)
        scale = 1.5;
    scale = (double)((int)(scale * 20.0 + 0.5)) / 20.0;
    if (scale != app_ui_scale) {
        app_ui_scale = scale;
        apply_theme();
    }
    return G_SOURCE_REMOVE;
}

/* Grow the interface's text as the window gets wider, so it stays comfortable
 * when maximised on a large or high-DPI display. Debounced to avoid reloading
 * the stylesheet on every frame while dragging. */
static gboolean ui_scale_tick(GtkWidget *w, GdkFrameClock *clock,
                              gpointer data) {
    int width = gtk_widget_get_allocated_width(w);

    (void)clock;
    (void)data;
    if (width < 200 || width == ui_scale_last_w)
        return G_SOURCE_CONTINUE;
    ui_scale_last_w = width;
    ui_scale_pending_w = width;
    if (ui_scale_timer)
        g_source_remove(ui_scale_timer);
    ui_scale_timer = g_timeout_add(120, ui_scale_apply, NULL);
    return G_SOURCE_CONTINUE;
}

static const char *find_app_icon_file(void) {
    static const char *paths[] = {
        ICON_FILE,
        "../assets/app-icon.png",
        ICON_THEME_DIR "/hicolor/512x512/apps/" ICON_NAME ".png",
        SHARE_DIR "/icons/hicolor/512x512/apps/" ICON_NAME ".png",
        ICON_FILE_ALT,
        NULL,
    };
    int i;

    for (i = 0; paths[i]; i++) {
        if (g_file_test(paths[i], G_FILE_TEST_IS_REGULAR))
            return paths[i];
    }
    return NULL;
}

/* Kept alive for gdk_toplevel_set_icon_list() which does not take ownership. */
static GList *app_icon_textures;

static void clear_app_icon_textures(void) {
    g_list_free_full(app_icon_textures, g_object_unref);
    app_icon_textures = NULL;
}

/* X11 / some compositors use the toplevel icon list for the taskbar tile. */
static void apply_toplevel_icon(GtkWindow *window) {
    GdkSurface *surface;
    const char *path;
    GError *err = NULL;
    GdkTexture *tex;

    surface = gtk_native_get_surface(GTK_NATIVE(window));
    if (!surface || !GDK_IS_TOPLEVEL(surface))
        return;

    path = find_app_icon_file();
    if (!path)
        return;

    tex = gdk_texture_new_from_filename(path, &err);
    if (!tex) {
        g_clear_error(&err);
        return;
    }

    clear_app_icon_textures();
    app_icon_textures = g_list_append(NULL, tex);
    gdk_toplevel_set_icon_list(GDK_TOPLEVEL(surface), app_icon_textures);
}

static void on_window_realize_icon(GtkWidget *widget, gpointer data) {
    (void)data;
    apply_toplevel_icon(GTK_WINDOW(widget));
}

/* Prepend ./share to XDG_DATA_DIRS so Wayland can find our .desktop file and
 * hicolor icons without a system install (matches APP_ID). */
static void setup_portable_share_dir(void) {
    char *abs;
    const char *old;
    char *neu;

    if (!g_file_test(SHARE_DIR "/applications", G_FILE_TEST_IS_DIR) &&
        !g_file_test(SHARE_DIR "/icons", G_FILE_TEST_IS_DIR))
        return;

    abs = g_canonicalize_filename(SHARE_DIR, NULL);
    if (!abs)
        return;

    old = g_getenv("XDG_DATA_DIRS");
    if (old && old[0])
        neu = g_strconcat(abs, G_SEARCHPATH_SEPARATOR_S, old, NULL);
    else
        neu = g_strconcat(abs,
                          G_SEARCHPATH_SEPARATOR_S "/usr/local/share"
                          G_SEARCHPATH_SEPARATOR_S "/usr/share",
                          NULL);
    g_setenv("XDG_DATA_DIRS", neu, TRUE);
    g_free(neu);
    g_free(abs);
}

/* Register icon theme paths + window icon-name (Linux), AppKit Dock (macOS),
 * and toplevel textures (X11 / fallbacks). Windows uses the .ico embedded
 * via maturita.rc at link time. */
static void setup_window_icon(GtkWindow *window) {
    GtkIconTheme *theme;
    static const char *dirs[] = {
        ICON_THEME_DIR,
        ICON_THEME_DIR_ALT,
        SHARE_DIR "/icons",
        NULL,
    };
    const char *icon_file;
    int i;

    theme = gtk_icon_theme_get_for_display(gdk_display_get_default());
    for (i = 0; dirs[i]; i++) {
        if (g_file_test(dirs[i], G_FILE_TEST_IS_DIR))
            gtk_icon_theme_add_search_path(theme, dirs[i]);
    }

    if (gtk_icon_theme_has_icon(theme, ICON_NAME)) {
        gtk_window_set_default_icon_name(ICON_NAME);
        gtk_window_set_icon_name(window, ICON_NAME);
    } else {
        g_warning("App icon '%s' not found (tried %s/, %s/, %s/icons/)",
                  ICON_NAME, ICON_THEME_DIR, ICON_THEME_DIR_ALT, SHARE_DIR);
    }

    icon_file = find_app_icon_file();
    if (icon_file)
        macos_set_dock_icon(icon_file);
#ifdef __APPLE__
    else
        g_warning("Dock icon PNG not found (tried %s and %s)",
                  ICON_FILE, ICON_FILE_ALT);
#endif

    g_signal_connect(window, "realize",
                     G_CALLBACK(on_window_realize_icon), NULL);
}

static gboolean open_settings_later(gpointer data) {
    (void)data;
    settings_open();
    return G_SOURCE_REMOVE;
}

static gboolean save_window_png(const char *path) {
    GtkWidget *widget;
    int width, height;
    GdkPaintable *paintable;
    GtkSnapshot *snapshot;
    GskRenderNode *node;
    cairo_surface_t *surface;
    cairo_t *cr;
    cairo_status_t st;

    if (!main_window || !path || !path[0])
        return FALSE;

    widget = GTK_WIDGET(main_window);
    width = gtk_widget_get_width(widget);
    height = gtk_widget_get_height(widget);
    if (width < 80 || height < 80)
        return FALSE;

    paintable = gtk_widget_paintable_new(widget);
    snapshot = gtk_snapshot_new();
    gdk_paintable_snapshot(paintable, snapshot, width, height);
    node = gtk_snapshot_free_to_node(snapshot);
    g_object_unref(paintable);
    if (!node)
        return FALSE;

    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cr = cairo_create(surface);
    gsk_render_node_draw(node, cr);
    gsk_render_node_unref(node);
    cairo_destroy(cr);
    st = cairo_surface_write_to_png(surface, path);
    cairo_surface_destroy(surface);
    return st == CAIRO_STATUS_SUCCESS;
}

static void smoke_quit(int code) {
    GtkApplication *app = main_window
        ? gtk_window_get_application(main_window) : NULL;

    if (app)
        g_application_quit(G_APPLICATION(app));
    else
        exit(code);
}

static gboolean shot_later(gpointer data) {
    const char *path = g_getenv("MATURITA_SHOT");
    int *tries = data;

    if (!save_window_png(path)) {
        if (tries && *tries < 20) {
            (*tries)++;
            return G_SOURCE_CONTINUE;
        }
        g_printerr("MATURITA_SHOT: could not capture window\n");
        smoke_quit(1);
        return G_SOURCE_REMOVE;
    }
    if (g_getenv("MATURITA_SHOT_QUIT"))
        smoke_quit(0);
    return G_SOURCE_REMOVE;
}

static gboolean run_smoke_test(gpointer data) {
    static const char *pages[] = {
        "welcome", "subjects", "roadmap", "stats",
        "netyears", "netmap", "hwmap", "czechmap",
        "mluvnice", "readinglist",
        "cetba1984", "cetba1984quiz", "cetba1984dej",
        "cetbaFuks", "cetbaFuksQuiz", "cetbaFuksDej",
        "hwunit1", "hwex1", "hwunit2", "hwex2", "hwunit3", "hwex3",
        "unit1", "unit2", "unit3",
        "u1e1", "u1e13", "u2e1", "u2e19", "u3e1", "u3e15",
        "u1vocab", "u2vocab", "u3vocab",
        "mluve1", "mluve20",
        "netunit1", "netex1", "netunit10", "netex10",
        "netunit27", "netex27",
        NULL
    };
    const char *report = g_getenv("MATURITA_SMOKE");
    FILE *f;
    int missing = 0;
    int i;

    (void)data;
    f = report && report[0] ? fopen(report, "w") : stdout;
    if (!f) {
        g_printerr("MATURITA_SMOKE: cannot write %s\n", report);
        smoke_quit(1);
        return G_SOURCE_REMOVE;
    }

    fprintf(f, "commit=%s\n", APP_COMMIT);
    fprintf(f, "css=%s\n",
            g_file_test(CSS_FILE, G_FILE_TEST_IS_REGULAR) ? "ok" : "MISSING");
    fprintf(f, "changelog=%s\n",
            g_file_test(CHANGELOG_FILE, G_FILE_TEST_IS_REGULAR) ? "ok" : "MISSING");
    fprintf(f, "progress_dir=%s\n", app_progress_dir ? app_progress_dir : "?");

    for (i = 0; pages[i]; i++) {
        GtkWidget *child = gtk_stack_get_child_by_name(main_stack, pages[i]);

        if (child) {
            fprintf(f, "OK %s\n", pages[i]);
        } else {
            fprintf(f, "MISSING %s\n", pages[i]);
            missing++;
        }
    }

    fprintf(f, "missing=%d\n", missing);
    if (f != stdout)
        fclose(f);
    smoke_quit(missing ? 1 : 0);
    return G_SOURCE_REMOVE;
}

void activate(GtkApplication *app, gpointer user_data) {
    GtkWindow *window;
    GtkWidget *headerbar;
    GtkWidget *title_label;
    GtkWidget *welcome_page;
    GtkWidget *roadmap_page;
    GtkEventController *keys;

    (void)user_data;

    load_settings();
    unit_meta_init();
    unit_configure(0, u1_ex_names, 13);
    unit_configure(1, u2_ex_names, 19);
    unit_configure(2, u3_ex_names, 15);
    load_progress();
    mluvnice_init();
    app_theme = theme_palette(app_theme_id, app_color_mode);

    window = GTK_WINDOW(gtk_application_window_new(app));
    main_window = window;
    gtk_window_set_title(window, "maturita.c");
    gtk_window_set_default_size(window, 1080, 720);
    setup_window_icon(window);

    headerbar = gtk_header_bar_new();
    gtk_widget_add_css_class(headerbar, "titlebar");
    title_label = gtk_label_new("maturita.c");
    gtk_widget_add_css_class(title_label, "app-title");
    gtk_header_bar_set_title_widget(GTK_HEADER_BAR(headerbar), title_label);
    gtk_window_set_titlebar(window, headerbar);
    apply_theme();
    gtk_header_bar_pack_end(GTK_HEADER_BAR(headerbar), build_settings_button());
    gtk_header_bar_pack_end(GTK_HEADER_BAR(headerbar), build_stats_button());
    gtk_header_bar_pack_end(GTK_HEADER_BAR(headerbar), build_search_button());

    main_stack = GTK_STACK(gtk_stack_new());
    gtk_stack_set_transition_type(GTK_STACK(main_stack),
                                  GTK_STACK_TRANSITION_TYPE_CROSSFADE);

    /* The update banner sits above the pages and stays hidden until there is
     * something to report, so it never shifts the layout unprompted. */
    {
        GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        GtkWidget *overlay = gtk_overlay_new();

        gtk_box_append(GTK_BOX(root), build_update_banner());
        gtk_widget_set_vexpand(GTK_WIDGET(main_stack), TRUE);
        gtk_box_append(GTK_BOX(root), GTK_WIDGET(main_stack));
        gtk_overlay_set_child(GTK_OVERLAY(overlay), root);
        gtk_window_set_child(window, overlay);
        search_attach(GTK_OVERLAY(overlay));
    }

    welcome_page = build_welcome_page();
    roadmap_page = build_roadmap_page();
    gtk_stack_add_named(main_stack, welcome_page, "welcome");
    gtk_stack_add_named(main_stack, build_subjects_page(), "subjects");
    gtk_stack_add_named(main_stack, roadmap_page, "roadmap");
    gtk_stack_add_named(main_stack, build_stats_page(), "stats");
    gtk_stack_add_named(main_stack, build_netyears_page(), "netyears");
    gtk_stack_add_named(main_stack, build_netmap_page(), "netmap");
    gtk_stack_add_named(main_stack, build_hwmap_page(), "hwmap");
    gtk_stack_add_named(main_stack, build_czechmap_page(), "czechmap");
    gtk_stack_add_named(main_stack, build_mluvnice_page(), "mluvnice");
    for (int n = 1; n <= 20; n++) {
        char name[16];

        g_snprintf(name, sizeof(name), "mluve%d", n);
        gtk_stack_add_named(main_stack,
                            build_mluvnice_exercise_page(n), name);
    }
    gtk_stack_add_named(main_stack, build_readinglist_page(), "readinglist");
    gtk_stack_add_named(main_stack, build_cetba1984_page(), "cetba1984");
    gtk_stack_add_named(main_stack, build_cetba1984_quiz_page(),
                        "cetba1984quiz");
    gtk_stack_add_named(main_stack, build_cetba1984_plot_page(),
                        "cetba1984dej");
    gtk_stack_add_named(main_stack, build_cetba_fuks_page(), "cetbaFuks");
    gtk_stack_add_named(main_stack, build_cetba_fuks_quiz_page(),
                        "cetbaFuksQuiz");
    gtk_stack_add_named(main_stack, build_cetba_fuks_plot_page(),
                        "cetbaFuksDej");
    gtk_stack_add_named(main_stack, build_hw_unit1_page(), "hwunit1");
    gtk_stack_add_named(main_stack, build_hw_unit1_exercise_page(), "hwex1");
    gtk_stack_add_named(main_stack, build_hw_unit2_page(), "hwunit2");
    gtk_stack_add_named(main_stack, build_hw_unit2_exercise_page(), "hwex2");
    gtk_stack_add_named(main_stack, build_hw_unit3_page(), "hwunit3");
    gtk_stack_add_named(main_stack, build_hw_unit3_exercise_page(), "hwex3");
    {
        typedef GtkWidget *(*NetBuilder)(void);
        static const struct {
            const char *unit_name;
            const char *ex_name;
            NetBuilder build_unit;
            NetBuilder build_ex;
        } net_pages[] = {
            {"netunit1",  "netex1",  build_net_unit1_page,  build_net_exercise_page},
            {"netunit2",  "netex2",  build_net_unit2_page,  build_net_unit2_exercise_page},
            {"netunit3",  "netex3",  build_net_unit3_page,  build_net_unit3_exercise_page},
            {"netunit4",  "netex4",  build_net_unit4_page,  build_net_unit4_exercise_page},
            {"netunit5",  "netex5",  build_net_unit5_page,  build_net_unit5_exercise_page},
            {"netunit6",  "netex6",  build_net_unit6_page,  build_net_unit6_exercise_page},
            {"netunit7",  "netex7",  build_net_unit7_page,  build_net_unit7_exercise_page},
            {"netunit8",  "netex8",  build_net_unit8_page,  build_net_unit8_exercise_page},
            {"netunit9",  "netex9",  build_net_unit9_page,  build_net_unit9_exercise_page},
            {"netunit10", "netex10", build_net_unit10_page, build_net_unit10_exercise_page},
            {"netunit11", "netex11", build_net_unit11_page, build_net_unit11_exercise_page},
            {"netunit12", "netex12", build_net_unit12_page, build_net_unit12_exercise_page},
            {"netunit13", "netex13", build_net_unit13_page, build_net_unit13_exercise_page},
            {"netunit14", "netex14", build_net_unit14_page, build_net_unit14_exercise_page},
            {"netunit15", "netex15", build_net_unit15_page, build_net_unit15_exercise_page},
            {"netunit16", "netex16", build_net_unit16_page, build_net_unit16_exercise_page},
            {"netunit17", "netex17", build_net_unit17_page, build_net_unit17_exercise_page},
            {"netunit18", "netex18", build_net_unit18_page, build_net_unit18_exercise_page},
            {"netunit19", "netex19", build_net_unit19_page, build_net_unit19_exercise_page},
            {"netunit20", "netex20", build_net_unit20_page, build_net_unit20_exercise_page},
            {"netunit21", "netex21", build_net_unit21_page, build_net_unit21_exercise_page},
            {"netunit22", "netex22", build_net_unit22_page, build_net_unit22_exercise_page},
            {"netunit23", "netex23", build_net_unit23_page, build_net_unit23_exercise_page},
            {"netunit24", "netex24", build_net_unit24_page, build_net_unit24_exercise_page},
            {"netunit25", "netex25", build_net_unit25_page, build_net_unit25_exercise_page},
            {"netunit26", "netex26", build_net_unit26_page, build_net_unit26_exercise_page},
            {"netunit27", "netex27", build_net_unit27_page, build_net_unit27_exercise_page},
        };

        for (guint i = 0; i < G_N_ELEMENTS(net_pages); i++) {
            gtk_stack_add_named(main_stack, net_pages[i].build_unit(),
                                net_pages[i].unit_name);
            gtk_stack_add_named(main_stack, net_pages[i].build_ex(),
                                net_pages[i].ex_name);
        }
    }

    for (int i = 0; i < NUM_UNLOCKED; i++) {
        UnitCtx *u = &units[i];
        GtkWidget *map = build_unit_page(u);

        if (map)
            gtk_stack_add_named(main_stack, map, u->page);
        for (int n = 1; n <= u->n_ex; n++) {
            GtkWidget *page = build_exercise_page(u, n);
            char name[16];

            if (!page)
                continue;
            g_snprintf(name, sizeof(name), "%se%d", u->ex_tag, n);
            gtk_stack_add_named(main_stack, page, name);
        }
        if (u->has_branch && u->branch_target) {
            GtkWidget *branch = build_translate(u);

            if (branch)
                gtk_stack_add_named(main_stack, branch, u->branch_target);
        }
    }

    {
        const char *start = g_getenv("MATURITA_PAGE");
        const char *size = g_getenv("MATURITA_SIZE");
        int w, h;

        if (start && *start)
            gtk_stack_set_visible_child_name(main_stack, start);
        else
            gtk_stack_set_visible_child_name(main_stack, "welcome");
        if (size && sscanf(size, "%dx%d", &w, &h) == 2)
            gtk_window_set_default_size(window, w, h);
    }

    refresh_completion_ui();

    keys = gtk_event_controller_key_new();
    gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(keys),
                                               GTK_PHASE_CAPTURE);
    g_signal_connect(keys, "key-pressed",
                     G_CALLBACK(on_window_key_pressed), window);
    gtk_widget_add_controller(GTK_WIDGET(window), GTK_EVENT_CONTROLLER(keys));

    apply_theme();
    apply_language();

    gtk_widget_add_tick_callback(GTK_WIDGET(window), ui_scale_tick, NULL, NULL);

    update_init();

    gtk_window_present(window);

    if (g_getenv("MATURITA_SETTINGS"))
        g_timeout_add(350, open_settings_later, NULL);
    if (g_getenv("MATURITA_SMOKE"))
        g_timeout_add(200, run_smoke_test, NULL);
    if (g_getenv("MATURITA_SHOT")) {
        static int shot_tries;

        shot_tries = 0;
        g_timeout_add(g_getenv("MATURITA_SETTINGS") ? 700 : 400,
                      shot_later, &shot_tries);
    }
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    /* Resolve style.css / icons next to the binary (or AppImage / .app) and
     * point progress files at a writable location before any I/O. */
    setup_portable_paths();

    /* Must run before GtkApplication so Wayland can resolve our .desktop
     * file / icons via the application id. */
    setup_portable_share_dir();

    app = gtk_application_new(APP_ID, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    clear_app_icon_textures();
    g_object_unref(app);

    return status;
}
