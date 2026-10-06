#include "graduately.h"

#include <string.h>

/* Mathematics: elementary review plus four gymnasium years.
 * Lessons are slides, then typed examples. A few lessons also
 * ask for a point snapped to the grid. */

#define MATH_CELL 28.0
#define MATH_PAD 18.0
#define MATH_GX 10
#define MATH_GY 8
#define MATH_PTS 16

typedef struct {
    const char *map_page;
    const char *back_page;
    const char *title_key;
    const char *sub_key;
    char **progress_path;
    NetLesson *lessons;
    const char *unit_keys[MATH_N];
    const char *sub_keys[MATH_N];
    GtkWidget *nodes[MATH_N];
    int note_last_w;
} MathTrack;

typedef struct {
    const char *spec;
    int ux[MATH_PTS];
    int uy[MATH_PTS];
    int n;
    int fx[8];
    int fy[8];
    int nf;
    int need;
    int checked;
    int correct;
    GtkWidget *area;
} MathBoard;

typedef struct {
    const MathItem *items;
    int n;
    MathTrack *track;
    int lesson_id;
    GtkWidget *feedback;
    GtkWidget **entries;
    MathBoard **boards;
    GtkWidget **hints;
} MathExCtx;

static MathTrack math_tracks[5];
static gboolean math_ready;
static char math_key_store[5][MATH_N][32];
static char math_sub_store[5][MATH_N][40];

static void math_ensure(void) {
    static const char *prefixes[] = {"mat0", "mat", "mat2", "mat3", "mat4"};
    static const char *maps[] = {
        "mat0map", "matmap", "mat2map", "mat3map", "mat4map",
    };
    static const char *titles[] = {
        "mat0_year", "mat_year1", "mat_year2", "mat_year3", "mat_year4",
    };
    static const char *subs[] = {
        "mat0_sub", "mat_sub", "mat2_sub", "mat3_sub", "mat4_sub",
    };
    NetLesson *lessons[5] = {
        mat0_lessons, mat_lessons, mat2_lessons, mat3_lessons, mat4_lessons,
    };
    char **paths[5] = {
        &app_progress_mat0, &app_progress_mat, &app_progress_mat2,
        &app_progress_mat3, &app_progress_mat4,
    };
    int y, i;

    if (math_ready)
        return;
    for (y = 0; y < 5; y++) {
        MathTrack *T = &math_tracks[y];

        T->map_page = maps[y];
        T->back_page = "matyears";
        T->title_key = titles[y];
        T->sub_key = subs[y];
        T->progress_path = paths[y];
        T->lessons = lessons[y];
        T->note_last_w = -1;
        for (i = 0; i < MATH_N; i++) {
            g_snprintf(math_key_store[y][i], sizeof math_key_store[y][i],
                       "%s_unit%d", prefixes[y], i + 1);
            g_snprintf(math_sub_store[y][i], sizeof math_sub_store[y][i],
                       "%s_unit%d_sub", prefixes[y], i + 1);
            T->unit_keys[i] = math_key_store[y][i];
            T->sub_keys[i] = math_sub_store[y][i];
        }
    }
    math_ready = TRUE;
}

static void math_save(MathTrack *T) {
    GKeyFile *kf;
    gchar *data;

    if (!T->progress_path || !*T->progress_path)
        return;
    kf = g_key_file_new();
    for (int i = 0; i < MATH_N; i++) {
        gchar key[8];

        g_snprintf(key, sizeof key, "%d", i + 1);
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

        g_file_set_contents(*T->progress_path, data, -1, &err);
        if (err)
            g_error_free(err);
        g_free(data);
    }
}

static void math_load_one(MathTrack *T) {
    GKeyFile *kf;
    GError *err = NULL;

    if (!T->progress_path || !*T->progress_path)
        return;
    kf = g_key_file_new();
    if (!g_key_file_load_from_file(kf, *T->progress_path, G_KEY_FILE_NONE, &err)) {
        if (err)
            g_error_free(err);
        g_key_file_free(kf);
        return;
    }
    for (int i = 0; i < MATH_N; i++) {
        gchar key[8];

        g_snprintf(key, sizeof key, "%d", i + 1);
        T->lessons[i].done = g_key_file_get_boolean(kf, "done", key, NULL);
    }
    g_key_file_free(kf);
}

void math_load_progress(void) {
    math_ensure();
    for (int y = 0; y < 5; y++)
        math_load_one(&math_tracks[y]);
}

void math_rail_theme_reset(void) {
}

static void math_refresh(MathTrack *T) {
    for (int i = 0; i < MATH_N; i++) {
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

static void math_mark_done(MathTrack *T, int lesson_id) {
    if (lesson_id < 0 || lesson_id >= MATH_N)
        return;
    if (T->lessons[lesson_id].done)
        return;
    T->lessons[lesson_id].done = TRUE;
    math_save(T);
    math_refresh(T);
    refresh_stats_ui();
}

void progress_for_math(ProgressSum *out) {
    if (!out)
        return;
    math_ensure();
    for (int y = 0; y < 5; y++) {
        MathTrack *T = &math_tracks[y];

        out->total_ex += MATH_N;
        out->open_units += MATH_N;
        for (int i = 0; i < MATH_N; i++) {
            if (T->lessons[i].done) {
                out->done_ex++;
                out->done_units++;
            }
        }
    }
}

static void math_open_unit(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L)
        return;
    L->idx = 0;
    if (L->stack) {
        char name[16];

        g_snprintf(name, sizeof name, "s%u", L->idx);
        gtk_stack_set_visible_child_name(GTK_STACK(L->stack), name);
        if (L->prev_btn)
            gtk_widget_set_sensitive(L->prev_btn, FALSE);
        if (L->next_btn)
            gtk_button_set_label(GTK_BUTTON(L->next_btn), tr("net_slide_next"));
    }
    gtk_stack_set_visible_child_name(main_stack, L->unit_page);
}

static GtkWidget *math_lesson_button(MathTrack *T, int index) {
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
    g_signal_connect(btn, "clicked", G_CALLBACK(math_open_unit),
                     &T->lessons[index]);
    return btn;
}

static GtkWidget *math_build_list(MathTrack *T) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *list;

    math_ensure();
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

    for (int i = 0; i < MATH_N; i++)
        gtk_box_append(GTK_BOX(list), math_lesson_button(T, i));
    math_refresh(T);
    return page;
}

GtkWidget *build_mat0map_page(void) { return math_build_list(&math_tracks[0]); }
GtkWidget *build_matmap_page(void) { return math_build_list(&math_tracks[1]); }
GtkWidget *build_mat2map_page(void) { return math_build_list(&math_tracks[2]); }
GtkWidget *build_mat3map_page(void) { return math_build_list(&math_tracks[3]); }
GtkWidget *build_mat4map_page(void) { return math_build_list(&math_tracks[4]); }

static GtkWidget *math_year_button(const char *badge, const char *target,
                                   const char *title_key, const char *sub_key) {
    GtkWidget *btn;
    GtkWidget *row;
    GtkWidget *num;
    GtkWidget *texts;
    GtkWidget *title;
    GtkWidget *sub;

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

    num = gtk_label_new(badge);
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

GtkWidget *build_matyears_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *list;
    static const char *badges[] = {"ZŠ", "1.", "2.", "3.", "4."};
    static const char *targets[] = {
        "mat0map", "matmap", "mat2map", "mat3map", "mat4map",
    };
    static const char *titles[] = {
        "mat0_year", "mat_year1", "mat_year2", "mat_year3", "mat_year4",
    };
    static const char *subs[] = {
        "mat0_sub", "mat_sub", "mat2_sub", "mat3_sub", "mat4_sub",
    };

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("subjects", "Matematika", "mat_years_sub"));

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

    for (int y = 0; y < 5; y++)
        gtk_box_append(GTK_BOX(list),
                       math_year_button(badges[y], targets[y], titles[y], subs[y]));
    return page;
}

static void math_slide_apply(NetLesson *L) {
    char name[16];

    if (!L || !L->stack)
        return;
    g_snprintf(name, sizeof name, "s%u", L->idx);
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

static void math_slide_prev(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L || L->idx == 0)
        return;
    L->idx--;
    math_slide_apply(L);
}

static void math_slide_next(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L)
        return;
    if (L->idx + 1 < L->n_slides) {
        L->idx++;
        math_slide_apply(L);
    } else {
        gtk_stack_set_visible_child_name(main_stack, L->ex_page);
    }
}

static void math_rescale(MathTrack *T, int w) {
    int body, head, kick;

    if (w < 160)
        w = 160;
    body = w / 46;
    if (body < 14)
        body = 14;
    if (body > 26)
        body = 26;
    head = body + 14;
    if (head > 46)
        head = 46;
    kick = body - 3;
    if (kick < 11)
        kick = 11;
    for (int i = 0; i < MATH_N; i++)
        net_rescale_lesson_notes(&T->lessons[i], body, head, kick);
}

static gboolean math_note_tick(GtkWidget *w, GdkFrameClock *clock, gpointer data) {
    MathTrack *T = data;
    int width = gtk_widget_get_allocated_width(w);

    (void)clock;
    if (width != T->note_last_w) {
        T->note_last_w = width;
        math_rescale(T, width);
    }
    return G_SOURCE_CONTINUE;
}

GtkWidget *math_unit_page(int year, NetLesson *L, const char *title_key,
                          const char *sub_key, const NetSlide *slides,
                          guint n_slides) {
    MathTrack *T;
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *nav;

    math_ensure();
    if (year < 0 || year > 4)
        year = 0;
    T = &math_tracks[year];

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
        g_snprintf(name, sizeof name, "s%u", s);
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

    gtk_widget_add_tick_callback(L->stack, math_note_tick, T, NULL);
    T->note_last_w = -1;
    math_rescale(T, 0);
    net_notes_target = NULL;

    nav = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(nav), TRUE);
    gtk_widget_set_margin_top(nav, 10);
    gtk_box_append(GTK_BOX(page), nav);

    L->prev_btn = gtk_button_new_with_label(tr("net_slide_prev"));
    gtk_widget_add_css_class(L->prev_btn, "btn-primary");
    g_signal_connect(L->prev_btn, "clicked", G_CALLBACK(math_slide_prev), L);
    gtk_box_append(GTK_BOX(nav), L->prev_btn);

    L->next_btn = gtk_button_new_with_label(tr("net_slide_next"));
    gtk_widget_add_css_class(L->next_btn, "btn-primary");
    g_signal_connect(L->next_btn, "clicked", G_CALLBACK(math_slide_next), L);
    gtk_box_append(GTK_BOX(nav), L->next_btn);

    L->idx = 0;
    math_slide_apply(L);
    return page;
}

void math_lessons_apply_lang(void) {
    math_ensure();
    for (int y = 0; y < 5; y++) {
        for (int i = 0; i < MATH_N; i++)
            math_slide_apply(&math_tracks[y].lessons[i]);
    }
}

static void paint_hex(cairo_t *cr, unsigned hex) {
    cairo_set_source_rgb(cr, ((hex >> 16) & 255) / 255.0,
                         ((hex >> 8) & 255) / 255.0, (hex & 255) / 255.0);
}

static int board_px_w(void) {
    return (int)(MATH_PAD * 2 + MATH_GX * MATH_CELL);
}

static int board_px_h(void) {
    return (int)(MATH_PAD * 2 + MATH_GY * MATH_CELL);
}

static void board_to_screen(int gx, int gy, double *sx, double *sy) {
    *sx = MATH_PAD + gx * MATH_CELL;
    *sy = board_px_h() - MATH_PAD - gy * MATH_CELL;
}

static gboolean board_snap(double x, double y, int *gx, int *gy) {
    double cx = (x - MATH_PAD) / MATH_CELL;
    double cy = (board_px_h() - MATH_PAD - y) / MATH_CELL;
    int ix = (int)lround(cx);
    int iy = (int)lround(cy);

    if (ix < 0 || ix > MATH_GX || iy < 0 || iy > MATH_GY)
        return FALSE;
    if (fabs(cx - ix) > 0.45 || fabs(cy - iy) > 0.45)
        return FALSE;
    *gx = ix;
    *gy = iy;
    return TRUE;
}

static void board_draw(GtkDrawingArea *area, cairo_t *cr, int width, int height,
                       gpointer data) {
    MathBoard *b = data;
    int i;
    static const char *names[] = {"A", "B", "C", "D"};

    (void)area;
    (void)width;
    (void)height;
    paint_hex(cr, app_theme.surface1);
    cairo_rectangle(cr, 0, 0, board_px_w(), board_px_h());
    cairo_fill(cr);

    cairo_set_line_width(cr, 1);
    cairo_set_source_rgba(cr,
                          ((app_theme.subtext >> 16) & 255) / 255.0,
                          ((app_theme.subtext >> 8) & 255) / 255.0,
                          (app_theme.subtext & 255) / 255.0, 0.55);
    for (i = 0; i <= MATH_GX; i++) {
        double x = MATH_PAD + i * MATH_CELL;

        cairo_move_to(cr, x, MATH_PAD);
        cairo_line_to(cr, x, board_px_h() - MATH_PAD);
    }
    for (i = 0; i <= MATH_GY; i++) {
        double y = MATH_PAD + i * MATH_CELL;

        cairo_move_to(cr, MATH_PAD, y);
        cairo_line_to(cr, board_px_w() - MATH_PAD, y);
    }
    cairo_stroke(cr);

    paint_hex(cr, app_theme.text);
    cairo_set_line_width(cr, 1.6);
    cairo_move_to(cr, MATH_PAD, board_px_h() - MATH_PAD);
    cairo_line_to(cr, board_px_w() - MATH_PAD, board_px_h() - MATH_PAD);
    cairo_move_to(cr, MATH_PAD, board_px_h() - MATH_PAD);
    cairo_line_to(cr, MATH_PAD, MATH_PAD);
    cairo_stroke(cr);

    if (b->nf >= 2) {
        double x0, y0, x1, y1;

        paint_hex(cr, app_theme.accent);
        cairo_set_line_width(cr, 2);
        board_to_screen(b->fx[0], b->fy[0], &x0, &y0);
        board_to_screen(b->fx[1], b->fy[1], &x1, &y1);
        cairo_move_to(cr, x0, y0);
        cairo_line_to(cr, x1, y1);
        if (b->nf >= 3) {
            double x2, y2;

            board_to_screen(b->fx[2], b->fy[2], &x2, &y2);
            cairo_move_to(cr, x0, y0);
            cairo_line_to(cr, x2, y2);
        }
        cairo_stroke(cr);
    }

    for (i = 0; i < b->nf && i < 4; i++) {
        double x, y;

        board_to_screen(b->fx[i], b->fy[i], &x, &y);
        paint_hex(cr, app_theme.accent);
        cairo_arc(cr, x, y, 5.5, 0, 2 * G_PI);
        cairo_fill(cr);
        paint_hex(cr, app_theme.text);
        cairo_set_font_size(cr, 13);
        cairo_move_to(cr, x + 7, y - 7);
        cairo_show_text(cr, names[i]);
    }
    for (i = 0; i < b->n; i++) {
        double x, y;
        char label[8];

        board_to_screen(b->ux[i], b->uy[i], &x, &y);
        if (b->checked)
            paint_hex(cr, b->correct ? app_theme.success : app_theme.error);
        else
            paint_hex(cr, app_theme.success);
        cairo_arc(cr, x, y, 5.5, 0, 2 * G_PI);
        cairo_fill(cr);
        paint_hex(cr, app_theme.text);
        g_snprintf(label, sizeof label, "%d", i + 1);
        cairo_set_font_size(cr, 13);
        cairo_move_to(cr, x + 7, y + 14);
        cairo_show_text(cr, label);
    }
}

static int board_has_fixed(MathBoard *b, int x, int y) {
    for (int i = 0; i < b->nf; i++)
        if (b->fx[i] == x && b->fy[i] == y)
            return 1;
    return 0;
}

static void board_pressed(GtkGestureClick *gesture, int n_press, double x, double y,
                          gpointer data) {
    MathBoard *b = data;
    int gx, gy, i;

    (void)gesture;
    (void)n_press;
    if (!board_snap(x, y, &gx, &gy) || board_has_fixed(b, gx, gy))
        return;
    for (i = 0; i < b->n; i++) {
        if (b->ux[i] == gx && b->uy[i] == gy) {
            memmove(&b->ux[i], &b->ux[i + 1], (b->n - i - 1) * sizeof(int));
            memmove(&b->uy[i], &b->uy[i + 1], (b->n - i - 1) * sizeof(int));
            b->n--;
            b->checked = 0;
            gtk_widget_queue_draw(b->area);
            return;
        }
    }
    if (b->n >= b->need || b->n >= MATH_PTS)
        return;
    b->ux[b->n] = gx;
    b->uy[b->n] = gy;
    b->n++;
    b->checked = 0;
    gtk_widget_queue_draw(b->area);
}

static void board_undo(GtkButton *button, gpointer data) {
    MathBoard *b = data;

    (void)button;
    if (b->n <= 0)
        return;
    b->n--;
    b->checked = 0;
    gtk_widget_queue_draw(b->area);
}

static void board_clear(GtkButton *button, gpointer data) {
    MathBoard *b = data;

    (void)button;
    b->n = 0;
    b->checked = 0;
    gtk_widget_queue_draw(b->area);
}

static void math_ex_check(GtkButton *button, gpointer data) {
    MathExCtx *ctx = data;
    int ok = 0;

    (void)button;
    for (int i = 0; i < ctx->n; i++) {
        int good = 0;

        if (ctx->boards[i]) {
            MathBoard *b = ctx->boards[i];

            good = math_draw_ok(b->spec, b->ux, b->uy, b->n);
            b->checked = 1;
            b->correct = good;
            gtk_widget_queue_draw(b->area);
        } else if (ctx->entries[i]) {
            const char *text = gtk_editable_get_text(GTK_EDITABLE(ctx->entries[i]));

            gtk_widget_remove_css_class(ctx->entries[i], "ok");
            gtk_widget_remove_css_class(ctx->entries[i], "wrong");
            good = math_answer_ok(ctx->items[i].answer, text);
            gtk_widget_add_css_class(ctx->entries[i], good ? "ok" : "wrong");
        }
        if (good)
            ok++;
        if (ctx->hints[i])
            gtk_widget_set_visible(ctx->hints[i], TRUE);
    }
    if (ok == ctx->n) {
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        math_mark_done(ctx->track, ctx->lesson_id);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry_short"));
    }
}

GtkWidget *math_practice_page(int year, int lesson_id, const char *back_page,
                              const char *title_key, const char *heading_key,
                              const MathItem *items, int n) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *body;
    GtkWidget *check;
    GtkWidget *head;
    GtkWidget *intro;
    MathExCtx *ctx = g_new0(MathExCtx, 1);

    math_ensure();
    if (year < 0 || year > 4)
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

    body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_set_hexpand(body, TRUE);
    gtk_widget_set_margin_start(body, 4);
    gtk_widget_set_margin_end(body, 4);
    gtk_widget_set_margin_bottom(body, 12);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), body);

    head = gtk_label_new(NULL);
    i18n_bind(head, heading_key, 0);
    gtk_widget_set_halign(head, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(head), TRUE);
    gtk_widget_add_css_class(head, "quiz-heading");
    gtk_box_append(GTK_BOX(body), head);

    intro = gtk_label_new(NULL);
    i18n_bind(intro, "math_quiz_intro", 0);
    gtk_widget_set_halign(intro, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(intro), TRUE);
    gtk_label_set_xalign(GTK_LABEL(intro), 0);
    gtk_widget_add_css_class(intro, "quiz-intro");
    gtk_box_append(GTK_BOX(body), intro);

    ctx->items = items;
    ctx->n = n;
    ctx->track = &math_tracks[year];
    ctx->lesson_id = lesson_id;
    ctx->entries = g_new0(GtkWidget *, n);
    ctx->boards = g_new0(MathBoard *, n);
    ctx->hints = g_new0(GtkWidget *, n);

    for (int i = 0; i < n; i++) {
        GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
        GtkWidget *prompt = gtk_label_new(items[i].prompt);
        GtkWidget *hint = gtk_label_new(items[i].hint);

        gtk_widget_set_halign(card, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(body), card);
        gtk_label_set_wrap(GTK_LABEL(prompt), TRUE);
        gtk_label_set_xalign(GTK_LABEL(prompt), 0);
        gtk_widget_set_halign(prompt, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(card), prompt);

        if (math_is_draw(items[i].answer)) {
            MathBoard *b = g_new0(MathBoard, 1);
            GtkWidget *area = gtk_drawing_area_new();
            GtkGesture *click = gtk_gesture_click_new();
            GtkWidget *tools = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
            GtkWidget *undo = gtk_button_new();
            GtkWidget *clear = gtk_button_new();
            GtkWidget *note = gtk_label_new(NULL);

            b->spec = items[i].answer;
            b->need = math_draw_need(items[i].answer);
            if (b->need < 1)
                b->need = 1;
            b->nf = math_draw_fixed(items[i].answer, b->fx, b->fy, 8);
            b->area = area;
            ctx->boards[i] = b;

            gtk_widget_set_size_request(area, board_px_w(), board_px_h());
            gtk_widget_set_halign(area, GTK_ALIGN_START);
            gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(area), board_px_w());
            gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(area), board_px_h());
            gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), board_draw, b, NULL);
            gtk_widget_add_controller(area, GTK_EVENT_CONTROLLER(click));
            g_signal_connect(click, "pressed", G_CALLBACK(board_pressed), b);
            gtk_box_append(GTK_BOX(card), area);

            i18n_bind(note, "math_draw_hint", 0);
            gtk_label_set_wrap(GTK_LABEL(note), TRUE);
            gtk_widget_add_css_class(note, "hint");
            gtk_widget_set_halign(note, GTK_ALIGN_START);
            gtk_box_append(GTK_BOX(card), note);

            i18n_bind(undo, "math_undo", 1);
            i18n_bind(clear, "math_clear", 1);
            g_signal_connect(undo, "clicked", G_CALLBACK(board_undo), b);
            g_signal_connect(clear, "clicked", G_CALLBACK(board_clear), b);
            gtk_box_append(GTK_BOX(tools), undo);
            gtk_box_append(GTK_BOX(tools), clear);
            gtk_box_append(GTK_BOX(card), tools);
        } else {
            GtkWidget *entry = gtk_entry_new();

            gtk_entry_set_placeholder_text(GTK_ENTRY(entry), tr("math_answer_ph"));
            gtk_widget_set_hexpand(entry, TRUE);
            gtk_widget_set_size_request(entry, 280, -1);
            gtk_box_append(GTK_BOX(card), entry);
            ctx->entries[i] = entry;
        }

        gtk_label_set_wrap(GTK_LABEL(hint), TRUE);
        gtk_label_set_xalign(GTK_LABEL(hint), 0);
        gtk_widget_set_halign(hint, GTK_ALIGN_START);
        gtk_widget_add_css_class(hint, "hint");
        gtk_widget_set_visible(hint, FALSE);
        gtk_box_append(GTK_BOX(card), hint);
        ctx->hints[i] = hint;
    }

    check = gtk_button_new();
    i18n_bind(check, "check", 1);
    gtk_widget_add_css_class(check, "btn-primary");
    gtk_widget_set_halign(check, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(body), check);
    g_signal_connect(check, "clicked", G_CALLBACK(math_ex_check), ctx);

    ctx->feedback = gtk_label_new("");
    gtk_widget_set_halign(ctx->feedback, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(body), ctx->feedback);
    return page;
}
