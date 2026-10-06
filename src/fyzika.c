#include "graduately.h"

/* Physics across four secondary-school years. Each year is a list of
 * lessons, then slides and a quiz. The syllabus follows the gymnasium
 * outline: mechanics, thermal physics and waves, electromagnetism,
 * then optics, relativity, the micro-world and astrophysics. */

typedef struct {
    const char *map_page;
    const char *back_page;
    const char *title_key;
    const char *sub_key;
    char **progress_path;
    NetLesson *lessons;
    const char *unit_keys[FYZ_N];
    const char *sub_keys[FYZ_N];
    GtkWidget *nodes[FYZ_N];
    int note_last_w;
} FyzTrack;

static FyzTrack fyz_tracks[4];
static gboolean fyz_ready;

static void fyz_ensure(void) {
    static const char *keys0[] = {
        "fyz_unit1", "fyz_unit2", "fyz_unit3", "fyz_unit4",
        "fyz_unit5", "fyz_unit6", "fyz_unit7", "fyz_unit8",
    };
    static const char *subs0[] = {
        "fyz_unit1_sub", "fyz_unit2_sub", "fyz_unit3_sub", "fyz_unit4_sub",
        "fyz_unit5_sub", "fyz_unit6_sub", "fyz_unit7_sub", "fyz_unit8_sub",
    };
    static const char *keys1[] = {
        "fyz2_unit1", "fyz2_unit2", "fyz2_unit3", "fyz2_unit4",
        "fyz2_unit5", "fyz2_unit6", "fyz2_unit7", "fyz2_unit8",
    };
    static const char *subs1[] = {
        "fyz2_unit1_sub", "fyz2_unit2_sub", "fyz2_unit3_sub", "fyz2_unit4_sub",
        "fyz2_unit5_sub", "fyz2_unit6_sub", "fyz2_unit7_sub", "fyz2_unit8_sub",
    };
    static const char *keys2[] = {
        "fyz3_unit1", "fyz3_unit2", "fyz3_unit3", "fyz3_unit4",
        "fyz3_unit5", "fyz3_unit6", "fyz3_unit7", "fyz3_unit8",
    };
    static const char *subs2[] = {
        "fyz3_unit1_sub", "fyz3_unit2_sub", "fyz3_unit3_sub", "fyz3_unit4_sub",
        "fyz3_unit5_sub", "fyz3_unit6_sub", "fyz3_unit7_sub", "fyz3_unit8_sub",
    };
    static const char *keys3[] = {
        "fyz4_unit1", "fyz4_unit2", "fyz4_unit3", "fyz4_unit4",
        "fyz4_unit5", "fyz4_unit6", "fyz4_unit7", "fyz4_unit8",
    };
    static const char *subs3[] = {
        "fyz4_unit1_sub", "fyz4_unit2_sub", "fyz4_unit3_sub", "fyz4_unit4_sub",
        "fyz4_unit5_sub", "fyz4_unit6_sub", "fyz4_unit7_sub", "fyz4_unit8_sub",
    };
    static const char *const *all_keys[4] = {keys0, keys1, keys2, keys3};
    static const char *const *all_subs[4] = {subs0, subs1, subs2, subs3};
    static const char *maps[] = {"fyzmap", "fyz2map", "fyz3map", "fyz4map"};
    static const char *titles[] = {
        "fyz_year1", "fyz_year2", "fyz_year3", "fyz_year4",
    };
    static const char *subs[] = {"fyz_sub", "fyz2_sub", "fyz3_sub", "fyz4_sub"};
    NetLesson *lessons[4] = {
        fyz_lessons, fyz2_lessons, fyz3_lessons, fyz4_lessons,
    };
    char **paths[4] = {
        &app_progress_fyz, &app_progress_fyz2,
        &app_progress_fyz3, &app_progress_fyz4,
    };

    if (fyz_ready)
        return;
    for (int y = 0; y < 4; y++) {
        FyzTrack *T = &fyz_tracks[y];

        T->map_page = maps[y];
        T->back_page = "fyzyears";
        T->title_key = titles[y];
        T->sub_key = subs[y];
        T->progress_path = paths[y];
        T->lessons = lessons[y];
        T->note_last_w = -1;
        for (int i = 0; i < FYZ_N; i++) {
            T->unit_keys[i] = all_keys[y][i];
            T->sub_keys[i] = all_subs[y][i];
        }
    }
    fyz_ready = TRUE;
}

static void fyz_save(FyzTrack *T) {
    GKeyFile *kf;
    char *path;
    gchar *data;

    if (!T->progress_path || !*T->progress_path)
        return;
    path = *T->progress_path;
    kf = g_key_file_new();
    for (int i = 0; i < FYZ_N; i++) {
        gchar key[8];

        g_snprintf(key, sizeof(key), "%d", i + 1);
        g_key_file_set_boolean(kf, "done", key, T->lessons[i].done);
    }
    if (g_mkdir_with_parents(PROGRESS_DIR, 0755) != 0) {
        g_key_file_free(kf);
        return;
    }
    data = g_key_file_to_data(kf, NULL, NULL);
    g_key_file_free(kf);
    if (data) {
        GError *err = NULL;

        g_file_set_contents(path, data, -1, &err);
        if (err)
            g_error_free(err);
        g_free(data);
    }
}

static void fyz_load_one(FyzTrack *T) {
    GKeyFile *kf;
    GError *err = NULL;
    const char *path;

    if (!T->progress_path || !*T->progress_path)
        return;
    path = *T->progress_path;
    kf = g_key_file_new();
    if (!g_key_file_load_from_file(kf, path, G_KEY_FILE_NONE, &err)) {
        if (err)
            g_error_free(err);
        g_key_file_free(kf);
        return;
    }
    for (int i = 0; i < FYZ_N; i++) {
        gchar key[8];

        g_snprintf(key, sizeof(key), "%d", i + 1);
        T->lessons[i].done = g_key_file_get_boolean(kf, "done", key, NULL);
    }
    g_key_file_free(kf);
}

void fyz_load_progress(void) {
    fyz_ensure();
    for (int y = 0; y < 4; y++)
        fyz_load_one(&fyz_tracks[y]);
}

void fyz_rail_theme_reset(void) {
}

static void fyz_refresh(FyzTrack *T) {
    for (int i = 0; i < FYZ_N; i++) {
        GtkWidget *node = T->nodes[i];
        GtkWidget *icon = T->lessons[i].done_icon;

        if (!node)
            continue;
        if (T->lessons[i].done) {
            gtk_widget_remove_css_class(node, "current");
            gtk_widget_add_css_class(node, "done");
            if (icon)
                gtk_widget_set_visible(icon, TRUE);
        } else {
            gtk_widget_remove_css_class(node, "done");
            gtk_widget_add_css_class(node, "current");
            if (icon)
                gtk_widget_set_visible(icon, FALSE);
        }
    }
}

static void fyz_mark_done(FyzTrack *T, int lesson_id) {
    if (lesson_id < 0 || lesson_id >= FYZ_N)
        return;
    if (T->lessons[lesson_id].done)
        return;
    T->lessons[lesson_id].done = TRUE;
    fyz_save(T);
    fyz_refresh(T);
    refresh_stats_ui();
}

void progress_for_fyz(ProgressSum *out) {
    if (!out)
        return;
    fyz_ensure();
    for (int y = 0; y < 4; y++) {
        FyzTrack *T = &fyz_tracks[y];

        out->total_ex += FYZ_N;
        out->open_units += FYZ_N;
        for (int i = 0; i < FYZ_N; i++) {
            if (T->lessons[i].done) {
                out->done_ex++;
                out->done_units++;
            }
        }
    }
}

static void fyz_open_unit(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L)
        return;
    L->idx = 0;
    if (L->stack) {
        char name[16];

        g_snprintf(name, sizeof(name), "s%u", L->idx);
        gtk_stack_set_visible_child_name(GTK_STACK(L->stack), name);
        if (L->prev_btn)
            gtk_widget_set_sensitive(L->prev_btn, FALSE);
        if (L->next_btn)
            gtk_button_set_label(GTK_BUTTON(L->next_btn), tr("net_slide_next"));
    }
    gtk_stack_set_visible_child_name(main_stack, L->unit_page);
}

static GtkWidget *fyz_lesson_button(FyzTrack *T, int index) {
    GtkWidget *btn;
    GtkWidget *row;
    GtkWidget *num;
    GtkWidget *texts;
    GtkWidget *title;
    GtkWidget *sub;
    GtkWidget *icon;
    char *num_text;

    btn = gtk_button_new();
    gtk_widget_add_css_class(btn, "unit-node");
    gtk_widget_add_css_class(btn, "current");
    gtk_widget_set_can_focus(btn, FALSE);
    gtk_widget_set_hexpand(btn, TRUE);
    gtk_widget_set_size_request(btn, -1, 84);

    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 14);
    gtk_widget_set_halign(row, GTK_ALIGN_START);
    gtk_widget_set_valign(row, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_start(row, 18);
    gtk_widget_set_margin_end(row, 18);
    gtk_button_set_child(GTK_BUTTON(btn), row);

    num_text = g_strdup_printf("%d", index + 1);
    num = gtk_label_new(num_text);
    g_free(num_text);
    gtk_widget_add_css_class(num, "unit-number");
    gtk_widget_set_valign(num, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(row), num);

    texts = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_halign(texts, GTK_ALIGN_START);
    gtk_widget_set_hexpand(texts, TRUE);
    gtk_box_append(GTK_BOX(row), texts);

    title = gtk_label_new(NULL);
    i18n_bind(title, T->unit_keys[index], 0);
    gtk_widget_set_halign(title, GTK_ALIGN_START);
    gtk_widget_add_css_class(title, "unit-name");
    gtk_box_append(GTK_BOX(texts), title);

    sub = gtk_label_new(NULL);
    i18n_bind(sub, T->sub_keys[index], 0);
    gtk_widget_set_halign(sub, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(sub), TRUE);
    gtk_label_set_xalign(GTK_LABEL(sub), 0.0);
    gtk_widget_add_css_class(sub, "hint");
    gtk_box_append(GTK_BOX(texts), sub);

    icon = icon_area_new(draw_check_icon, 0.1176, 0.1176, 0.1804, 18);
    gtk_widget_set_visible(icon, FALSE);
    gtk_widget_set_valign(icon, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(row), icon);
    T->lessons[index].done_icon = icon;
    T->nodes[index] = btn;
    g_signal_connect(btn, "clicked", G_CALLBACK(fyz_open_unit),
                     &T->lessons[index]);
    return btn;
}

static GtkWidget *fyz_build_list(FyzTrack *T) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *list;

    fyz_ensure();
    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar(T->back_page, T->title_key, T->sub_key));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 18);
    gtk_box_append(GTK_BOX(page), scroll);

    list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_halign(list, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(list, GTK_ALIGN_START);
    gtk_widget_set_size_request(list, 520, -1);
    gtk_widget_set_margin_bottom(list, 12);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list);

    for (int i = 0; i < FYZ_N; i++)
        gtk_box_append(GTK_BOX(list), fyz_lesson_button(T, i));
    fyz_refresh(T);
    return page;
}

GtkWidget *build_fyzmap_page(void) { return fyz_build_list(&fyz_tracks[0]); }
GtkWidget *build_fyz2map_page(void) { return fyz_build_list(&fyz_tracks[1]); }
GtkWidget *build_fyz3map_page(void) { return fyz_build_list(&fyz_tracks[2]); }
GtkWidget *build_fyz4map_page(void) { return fyz_build_list(&fyz_tracks[3]); }

static GtkWidget *fyz_year_button(int year, const char *target,
                                  const char *title_key, const char *sub_key) {
    GtkWidget *btn;
    GtkWidget *row;
    GtkWidget *num;
    GtkWidget *texts;
    GtkWidget *title;
    GtkWidget *sub;
    char *num_text;

    btn = gtk_button_new();
    gtk_widget_add_css_class(btn, "unit-node");
    gtk_widget_add_css_class(btn, "current");
    gtk_widget_set_can_focus(btn, FALSE);
    gtk_widget_set_hexpand(btn, TRUE);
    gtk_widget_set_size_request(btn, -1, 84);

    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 14);
    gtk_widget_set_halign(row, GTK_ALIGN_START);
    gtk_widget_set_valign(row, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_start(row, 18);
    gtk_widget_set_margin_end(row, 18);
    gtk_button_set_child(GTK_BUTTON(btn), row);

    num_text = g_strdup_printf("%d", year);
    num = gtk_label_new(num_text);
    g_free(num_text);
    gtk_widget_add_css_class(num, "unit-number");
    gtk_widget_set_valign(num, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(row), num);

    texts = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_halign(texts, GTK_ALIGN_START);
    gtk_widget_set_hexpand(texts, TRUE);
    gtk_box_append(GTK_BOX(row), texts);

    title = gtk_label_new(NULL);
    i18n_bind(title, title_key, 0);
    gtk_widget_set_halign(title, GTK_ALIGN_START);
    gtk_widget_add_css_class(title, "unit-name");
    gtk_box_append(GTK_BOX(texts), title);

    sub = gtk_label_new(NULL);
    i18n_bind(sub, sub_key, 0);
    gtk_widget_set_halign(sub, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(sub), TRUE);
    gtk_label_set_xalign(GTK_LABEL(sub), 0.0);
    gtk_widget_add_css_class(sub, "hint");
    gtk_box_append(GTK_BOX(texts), sub);

    g_object_set_data_full(G_OBJECT(btn), "target", g_strdup(target), g_free);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_nav_clicked), NULL);
    return btn;
}

GtkWidget *build_fyzyears_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *list;
    static const char *targets[] = {"fyzmap", "fyz2map", "fyz3map", "fyz4map"};
    static const char *titles[] = {
        "fyz_year1", "fyz_year2", "fyz_year3", "fyz_year4",
    };
    static const char *subs[] = {"fyz_sub", "fyz2_sub", "fyz3_sub", "fyz4_sub"};

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("subjects", "Fyzika", "fyz_years_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 18);
    gtk_box_append(GTK_BOX(page), scroll);

    list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_halign(list, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(list, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(list, 480, -1);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list);

    for (int y = 0; y < 4; y++)
        gtk_box_append(GTK_BOX(list),
                       fyz_year_button(y + 1, targets[y], titles[y], subs[y]));
    return page;
}

static void fyz_slide_apply(NetLesson *L) {
    char name[16];

    if (!L || !L->stack)
        return;
    g_snprintf(name, sizeof(name), "s%u", L->idx);
    gtk_stack_set_visible_child_name(GTK_STACK(L->stack), name);
    if (L->prev_btn) {
        gtk_button_set_label(GTK_BUTTON(L->prev_btn), tr("net_slide_prev"));
        gtk_widget_set_sensitive(L->prev_btn, L->idx > 0);
    }
    if (L->next_btn) {
        gtk_button_set_label(
            GTK_BUTTON(L->next_btn),
            L->idx + 1 >= L->n_slides ? tr("net_slide_start")
                                      : tr("net_slide_next"));
    }
}

static void fyz_slide_prev(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L || L->idx == 0)
        return;
    L->idx--;
    fyz_slide_apply(L);
}

static void fyz_slide_next(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L)
        return;
    if (L->idx + 1 < L->n_slides) {
        L->idx++;
        fyz_slide_apply(L);
    } else {
        gtk_stack_set_visible_child_name(main_stack, L->ex_page);
    }
}

static void fyz_rescale(FyzTrack *T, int w) {
    int body, head, kick;

    if (w < 160)
        w = 160;
    body = w / 46;
    if (body < 14) body = 14;
    if (body > 26) body = 26;
    head = body + 14;
    if (head > 46) head = 46;
    kick = body - 3;
    if (kick < 11) kick = 11;
    for (int i = 0; i < FYZ_N; i++)
        net_rescale_lesson_notes(&T->lessons[i], body, head, kick);
}

static gboolean fyz_note_tick(GtkWidget *w, GdkFrameClock *clock,
                              gpointer data) {
    FyzTrack *T = data;
    int width = gtk_widget_get_allocated_width(w);

    (void)clock;
    if (width != T->note_last_w) {
        T->note_last_w = width;
        fyz_rescale(T, width);
    }
    return G_SOURCE_CONTINUE;
}

GtkWidget *fyz_unit_page(int year, NetLesson *L, const char *title_key,
                         const char *sub_key, const NetSlide *slides,
                         guint n_slides) {
    FyzTrack *T;
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *nav;

    fyz_ensure();
    if (year < 0 || year > 3)
        year = 0;
    T = &fyz_tracks[year];

    net_lesson_notes_ensure(L);
    net_notes_target = L;
    L->n_slides = n_slides;

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(page, 28);
    gtk_widget_set_margin_end(page, 28);
    gtk_widget_set_margin_top(page, 20);
    gtk_widget_set_margin_bottom(page, 20);

    gtk_box_append(GTK_BOX(page), top_bar(T->map_page, title_key, sub_key));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 10);
    gtk_box_append(GTK_BOX(page), scroll);

    L->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(L->stack),
                                  GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), L->stack);

    for (guint s = 0; s < n_slides; s++) {
        GtkWidget *stage = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        GtkWidget *card;
        char name[16];

        gtk_widget_set_vexpand(stage, TRUE);
        g_snprintf(name, sizeof(name), "s%u", s);
        gtk_stack_add_named(GTK_STACK(L->stack), stage, name);

        card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
        gtk_widget_add_css_class(card, "notes-card");
        gtk_widget_set_valign(card, GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(stage), card);

        net_note_kicker(card, slides[s].kicker);
        net_note_title(card, slides[s].title);
        for (int i = 0; i < 8 && slides[s].line[i]; i++)
            net_note_line(card, slides[s].line[i], FALSE);
        if (slides[s].tip)
            net_note_line(card, slides[s].tip, TRUE);
    }

    gtk_widget_add_tick_callback(L->stack, fyz_note_tick, T, NULL);
    T->note_last_w = -1;
    fyz_rescale(T, 0);
    net_notes_target = NULL;

    nav = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(nav), TRUE);
    gtk_widget_set_margin_top(nav, 10);
    gtk_box_append(GTK_BOX(page), nav);

    L->prev_btn = gtk_button_new_with_label(tr("net_slide_prev"));
    gtk_widget_add_css_class(L->prev_btn, "btn-primary");
    g_signal_connect(L->prev_btn, "clicked", G_CALLBACK(fyz_slide_prev), L);
    gtk_box_append(GTK_BOX(nav), L->prev_btn);

    L->next_btn = gtk_button_new_with_label(tr("net_slide_next"));
    gtk_widget_add_css_class(L->next_btn, "btn-primary");
    g_signal_connect(L->next_btn, "clicked", G_CALLBACK(fyz_slide_next), L);
    gtk_box_append(GTK_BOX(nav), L->next_btn);

    L->idx = 0;
    fyz_slide_apply(L);
    return page;
}

void fyz_lessons_apply_lang(void) {
    fyz_ensure();
    for (int y = 0; y < 4; y++) {
        for (int i = 0; i < FYZ_N; i++)
            fyz_slide_apply(&fyz_tracks[y].lessons[i]);
    }
}

typedef struct {
    const ChoiceQ *qs;
    int n;
    int n_opts;
    FyzTrack *track;
    int lesson_id;
    GtkWidget *feedback;
    GtkToggleButton **toggles;
    GtkWidget **hints;
} FyzMcqCtx;

static void fyz_mcq_check(GtkButton *button, gpointer data) {
    FyzMcqCtx *ctx = data;
    int ok = 0;

    (void)button;
    for (int i = 0; i < ctx->n; i++) {
        int active = -1;

        for (int o = 0; o < ctx->n_opts; o++) {
            GtkToggleButton *tb = ctx->toggles[i * ctx->n_opts + o];

            gtk_widget_remove_css_class(GTK_WIDGET(tb), "ok");
            gtk_widget_remove_css_class(GTK_WIDGET(tb), "wrong");
            if (gtk_toggle_button_get_active(tb))
                active = o;
        }
        if (active == ctx->qs[i].correct) {
            ok++;
            gtk_widget_add_css_class(
                GTK_WIDGET(ctx->toggles[i * ctx->n_opts + active]), "ok");
        } else if (active >= 0) {
            gtk_widget_add_css_class(
                GTK_WIDGET(ctx->toggles[i * ctx->n_opts + active]), "wrong");
        }
        if (ctx->hints && ctx->hints[i])
            gtk_widget_set_visible(ctx->hints[i], TRUE);
    }
    if (ok == ctx->n) {
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        fyz_mark_done(ctx->track, ctx->lesson_id);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry_short"));
    }
}

GtkWidget *fyz_mcq_page(int year, int lesson_id, const char *back_page,
                        const char *title_key, const char *heading_key,
                        const ChoiceQ *qs, const char **hints, int n) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *body;
    GtkWidget *check;
    GtkWidget *btns;
    GtkWidget *head;
    GtkWidget *intro;
    FyzMcqCtx *ctx = g_new0(FyzMcqCtx, 1);
    int n_opts = qs[0].n_options;

    fyz_ensure();
    if (year < 0 || year > 3)
        year = 0;

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page), top_bar(back_page, title_key, NULL));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 12);
    gtk_box_append(GTK_BOX(page), scroll);

    body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_widget_set_halign(body, GTK_ALIGN_FILL);
    gtk_widget_set_hexpand(body, TRUE);
    gtk_widget_set_margin_start(body, 4);
    gtk_widget_set_margin_end(body, 4);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), body);

    head = gtk_label_new(NULL);
    i18n_bind(head, heading_key, 0);
    gtk_widget_set_halign(head, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(head), TRUE);
    gtk_widget_set_margin_top(head, 4);
    gtk_widget_add_css_class(head, "quiz-heading");
    gtk_box_append(GTK_BOX(body), head);

    intro = gtk_label_new(NULL);
    i18n_bind(intro, "net_quiz_intro", 0);
    gtk_widget_set_halign(intro, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(intro), TRUE);
    gtk_widget_add_css_class(intro, "quiz-intro");
    gtk_widget_set_margin_bottom(intro, 4);
    gtk_box_append(GTK_BOX(body), intro);

    ctx->qs = qs;
    ctx->n = n;
    ctx->n_opts = n_opts;
    ctx->track = &fyz_tracks[year];
    ctx->lesson_id = lesson_id;
    ctx->toggles = g_new0(GtkToggleButton *, n * n_opts);
    ctx->hints = g_new0(GtkWidget *, n);

    for (int i = 0; i < n; i++) {
        GtkWidget *card = mcq_append_question(body, i + 1, &qs[i],
                                              &ctx->toggles[i * n_opts]);

        ctx->hints[i] = meaning_add(card, hints[i]);
    }

    btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_margin_top(btns, 8);
    gtk_widget_set_margin_bottom(btns, 8);
    gtk_box_append(GTK_BOX(body), btns);

    check = gtk_button_new();
    i18n_bind(check, "check", 1);
    gtk_widget_add_css_class(check, "btn-primary");
    gtk_widget_set_halign(check, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(btns), check);
    g_signal_connect(check, "clicked", G_CALLBACK(fyz_mcq_check), ctx);

    ctx->feedback = gtk_label_new("");
    gtk_widget_set_halign(ctx->feedback, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(body), ctx->feedback);
    return page;
}
