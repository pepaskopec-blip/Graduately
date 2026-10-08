#include "graduately.h"

/* Základy přírodopisných věd: chemistry and biology share one subject.
 * Each area is a vertical list of lessons, then slides and a quiz. */

typedef struct {
    const char *map_page;
    const char *back_page;
    const char *title_key;
    const char *sub_key;
    char **progress_path;
    NetLesson *lessons;
    const char *unit_keys[SCI_N];
    const char *sub_keys[SCI_N];
    GtkWidget *nodes[SCI_N];
    int note_last_w;
} SciTrack;

static SciTrack sci_tracks[2];
static GtkWidget *sci_rail;
static gboolean sci_ready;

static void sci_ensure(void) {
    static const char *chem_units[] = {
        "chem_unit1", "chem_unit2", "chem_unit3", "chem_unit4",
        "chem_unit5", "chem_unit6", "chem_unit7", "chem_unit8",
    };
    static const char *chem_subs[] = {
        "chem_unit1_sub", "chem_unit2_sub", "chem_unit3_sub", "chem_unit4_sub",
        "chem_unit5_sub", "chem_unit6_sub", "chem_unit7_sub", "chem_unit8_sub",
    };
    static const char *bio_units[] = {
        "bio_unit1", "bio_unit2", "bio_unit3", "bio_unit4",
        "bio_unit5", "bio_unit6", "bio_unit7", "bio_unit8",
    };
    static const char *bio_subs[] = {
        "bio_unit1_sub", "bio_unit2_sub", "bio_unit3_sub", "bio_unit4_sub",
        "bio_unit5_sub", "bio_unit6_sub", "bio_unit7_sub", "bio_unit8_sub",
    };

    if (sci_ready)
        return;
    sci_tracks[0].map_page = "chemmap";
    sci_tracks[0].back_page = "scimap";
    sci_tracks[0].title_key = "Chemie";
    sci_tracks[0].sub_key = "chem_sub";
    sci_tracks[0].progress_path = &app_progress_chem;
    sci_tracks[0].lessons = chem_lessons;
    sci_tracks[0].note_last_w = -1;
    sci_tracks[1].map_page = "biomap";
    sci_tracks[1].back_page = "scimap";
    sci_tracks[1].title_key = "Biologie";
    sci_tracks[1].sub_key = "bio_sub";
    sci_tracks[1].progress_path = &app_progress_bio;
    sci_tracks[1].lessons = bio_lessons;
    sci_tracks[1].note_last_w = -1;
    for (int i = 0; i < SCI_N; i++) {
        sci_tracks[0].unit_keys[i] = chem_units[i];
        sci_tracks[0].sub_keys[i] = chem_subs[i];
        sci_tracks[1].unit_keys[i] = bio_units[i];
        sci_tracks[1].sub_keys[i] = bio_subs[i];
    }
    sci_ready = TRUE;
}

static void sci_save(SciTrack *T) {
    GKeyFile *kf;
    char *path;
    gchar *data;

    if (!T->progress_path || !*T->progress_path)
        return;
    path = *T->progress_path;
    kf = g_key_file_new();
    for (int i = 0; i < SCI_N; i++) {
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

static void sci_load_one(SciTrack *T) {
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
    for (int i = 0; i < SCI_N; i++) {
        gchar key[8];

        g_snprintf(key, sizeof(key), "%d", i + 1);
        T->lessons[i].done = g_key_file_get_boolean(kf, "done", key, NULL);
    }
    g_key_file_free(kf);
}

void sci_load_progress(void) {
    sci_ensure();
    sci_load_one(&sci_tracks[0]);
    sci_load_one(&sci_tracks[1]);
}

static void sci_refresh(SciTrack *T) {
    for (int i = 0; i < SCI_N; i++) {
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

static void sci_mark_done(SciTrack *T, int lesson_id) {
    if (lesson_id < 0 || lesson_id >= SCI_N)
        return;
    if (T->lessons[lesson_id].done)
        return;
    T->lessons[lesson_id].done = TRUE;
    sci_save(T);
    sci_refresh(T);
    refresh_stats_ui();
}

void progress_for_sci(ProgressSum *out) {
    if (!out)
        return;
    sci_ensure();
    for (int a = 0; a < 2; a++) {
        SciTrack *T = &sci_tracks[a];

        out->total_ex += SCI_N;
        out->open_units += SCI_N;
        for (int i = 0; i < SCI_N; i++) {
            if (T->lessons[i].done) {
                out->done_ex++;
                out->done_units++;
            }
        }
    }
}

static void sci_open_unit(GtkButton *button, gpointer data) {
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
    show_page(L->unit_page, 1);
}

static GtkWidget *sci_lesson_button(SciTrack *T, int index) {
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
    g_signal_connect(btn, "clicked", G_CALLBACK(sci_open_unit),
                     &T->lessons[index]);
    return btn;
}

static GtkWidget *sci_build_list(SciTrack *T) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *list;

    sci_ensure();
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

    for (int i = 0; i < SCI_N; i++)
        gtk_box_append(GTK_BOX(list), sci_lesson_button(T, i));
    sci_refresh(T);
    return page;
}

GtkWidget *build_chemmap_page(void) { return sci_build_list(&sci_tracks[0]); }
GtkWidget *build_biomap_page(void) { return sci_build_list(&sci_tracks[1]); }

#define SCI_BUBBLE SUB_BUBBLE
#define SCI_LABEL_W 180.0
#define SCI_CW 560
#define SCI_CH 280

static double sci_cx[2];
static double sci_cy[2];

static void draw_sci_rail(GtkDrawingArea *area, cairo_t *cr,
                          int width, int height, gpointer data) {
    const Rgb rail = color_from_hex(app_theme.rail);
    const Rgb slate = color_from_hex(app_theme.surface1);

    (void)area;
    (void)width;
    (void)height;
    (void)data;

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

    cairo_move_to(cr, sci_cx[0], sci_cy[0]);
    cairo_line_to(cr, sci_cx[1], sci_cy[1]);
    cairo_set_line_width(cr, 14);
    cairo_set_source_rgb(cr, rail.r, rail.g, rail.b);
    cairo_stroke(cr);

    cairo_move_to(cr, sci_cx[0], sci_cy[0]);
    cairo_line_to(cr, sci_cx[1], sci_cy[1]);
    cairo_set_line_width(cr, 9);
    cairo_set_source_rgb(cr, slate.r, slate.g, slate.b);
    cairo_stroke(cr);
}

void sci_rail_theme_reset(void) {
    if (sci_rail)
        gtk_widget_queue_draw(sci_rail);
}

GtkWidget *build_scimap_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *fixed;
    GtkWidget *rail;
    static const char *keys[] = {"Chemie", "Biologie"};
    static const char *initials[] = {"C", "B"};
    static const char *targets[] = {"chemmap", "biomap"};

    sci_cx[0] = 160.0;
    sci_cy[0] = 110.0;
    sci_cx[1] = 400.0;
    sci_cy[1] = 110.0;

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("subjects", "Základy Přírodopisných věd", "sci_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);

    wrap = gtk_center_box_new();
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), wrap);

    fixed = gtk_fixed_new();
    gtk_widget_set_halign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(fixed, SCI_CW, SCI_CH);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), fixed);

    rail = gtk_drawing_area_new();
    gtk_widget_set_size_request(rail, SCI_CW, SCI_CH);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(rail), draw_sci_rail,
                                   NULL, NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rail, 0, 0);
    sci_rail = rail;

    for (int i = 0; i < 2; i++) {
        GtkWidget *btn = gtk_button_new();
        GtkWidget *letter = gtk_label_new(initials[i]);
        GtkWidget *name = gtk_label_new(NULL);

        gtk_widget_add_css_class(btn, "unit-node");
        gtk_widget_add_css_class(btn, "current");
        gtk_widget_set_can_focus(btn, FALSE);
        gtk_widget_set_size_request(btn, (int)SCI_BUBBLE, (int)SCI_BUBBLE);
        gtk_widget_add_css_class(letter, "unit-number");
        gtk_button_set_child(GTK_BUTTON(btn), letter);
        g_object_set_data_full(G_OBJECT(btn), "target",
                               g_strdup(targets[i]), g_free);
        g_signal_connect(btn, "clicked", G_CALLBACK(on_nav_clicked), NULL);

        i18n_bind(name, keys[i], 0);
        gtk_widget_set_size_request(name, (int)SCI_LABEL_W, -1);
        gtk_widget_set_halign(name, GTK_ALIGN_CENTER);
        gtk_label_set_justify(GTK_LABEL(name), GTK_JUSTIFY_CENTER);
        gtk_label_set_wrap(GTK_LABEL(name), TRUE);
        gtk_widget_add_css_class(name, "unit-name");

        gtk_fixed_put(GTK_FIXED(fixed), btn, 0, 0);
        gtk_fixed_put(GTK_FIXED(fixed), name, 0, 0);
        gtk_fixed_move(GTK_FIXED(fixed), btn,
                       (int)(sci_cx[i] - SCI_BUBBLE / 2.0),
                       (int)(sci_cy[i] - SCI_BUBBLE / 2.0));
        gtk_fixed_move(GTK_FIXED(fixed), name,
                       (int)(sci_cx[i] - SCI_LABEL_W / 2.0),
                       (int)(sci_cy[i] + SCI_BUBBLE / 2.0 + 10.0));
    }
    return page;
}

static void sci_slide_apply(NetLesson *L) {
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

static void sci_slide_prev(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L || L->idx == 0)
        return;
    L->idx--;
    sci_slide_apply(L);
}

static void sci_slide_next(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L)
        return;
    if (L->idx + 1 < L->n_slides) {
        L->idx++;
        sci_slide_apply(L);
    } else {
        show_page(L->ex_page, 1);
    }
}

static void sci_rescale(SciTrack *T, int w) {
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
    for (int i = 0; i < SCI_N; i++)
        net_rescale_lesson_notes(&T->lessons[i], body, head, kick);
}

static gboolean sci_note_tick(GtkWidget *w, GdkFrameClock *clock,
                              gpointer data) {
    SciTrack *T = data;
    int width = gtk_widget_get_allocated_width(w);

    (void)clock;
    if (width != T->note_last_w) {
        T->note_last_w = width;
        sci_rescale(T, width);
    }
    return G_SOURCE_CONTINUE;
}

GtkWidget *sci_unit_page(int area, NetLesson *L, const char *title_key,
                         const char *sub_key, const NetSlide *slides,
                         guint n_slides) {
    SciTrack *T;
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *nav;

    sci_ensure();
    if (area < 0 || area > 1)
        area = 0;
    T = &sci_tracks[area];

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

    gtk_widget_add_tick_callback(L->stack, sci_note_tick, T, NULL);
    T->note_last_w = -1;
    sci_rescale(T, 0);
    net_notes_target = NULL;

    nav = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(nav), TRUE);
    gtk_widget_set_margin_top(nav, 10);
    gtk_box_append(GTK_BOX(page), nav);

    L->prev_btn = gtk_button_new_with_label(tr("net_slide_prev"));
    gtk_widget_add_css_class(L->prev_btn, "btn-primary");
    g_signal_connect(L->prev_btn, "clicked", G_CALLBACK(sci_slide_prev), L);
    gtk_box_append(GTK_BOX(nav), L->prev_btn);

    L->next_btn = gtk_button_new_with_label(tr("net_slide_next"));
    gtk_widget_add_css_class(L->next_btn, "btn-primary");
    g_signal_connect(L->next_btn, "clicked", G_CALLBACK(sci_slide_next), L);
    gtk_box_append(GTK_BOX(nav), L->next_btn);

    L->idx = 0;
    sci_slide_apply(L);
    return page;
}

void sci_lessons_apply_lang(void) {
    sci_ensure();
    for (int a = 0; a < 2; a++) {
        for (int i = 0; i < SCI_N; i++)
            sci_slide_apply(&sci_tracks[a].lessons[i]);
    }
}

typedef struct {
    const ChoiceQ *qs;
    int n;
    int n_opts;
    SciTrack *track;
    int lesson_id;
    GtkWidget *feedback;
    GtkToggleButton **toggles;
    GtkWidget **hints;
} SciMcqCtx;

static void sci_mcq_check(GtkButton *button, gpointer data) {
    SciMcqCtx *ctx = data;
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
        sci_mark_done(ctx->track, ctx->lesson_id);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry_short"));
    }
}

GtkWidget *sci_mcq_page(int area, int lesson_id, const char *back_page,
                        const char *title_key, const char *heading_key,
                        const ChoiceQ *qs, const char **hints, int n) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *body;
    GtkWidget *check;
    GtkWidget *btns;
    GtkWidget *head;
    GtkWidget *intro;
    SciMcqCtx *ctx = g_new0(SciMcqCtx, 1);
    int n_opts = qs[0].n_options;

    sci_ensure();
    if (area < 0 || area > 1)
        area = 0;

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
    ctx->track = &sci_tracks[area];
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
    g_signal_connect(check, "clicked", G_CALLBACK(sci_mcq_check), ctx);

    ctx->feedback = gtk_label_new("");
    gtk_widget_set_halign(ctx->feedback, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(body), ctx->feedback);
    return page;
}
