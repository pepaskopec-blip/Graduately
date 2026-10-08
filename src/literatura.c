#include "graduately.h"

/* Literature for Český jazyk a literatura.
 * Four years follow the chronology used in secondary-school syllabi
 * (RVP G, literární komunikace, and RVP for vocational schools,
 * estetické vzdělávání): antiquity through the present, then a lesson
 * on interpreting a text. Each lesson is a short overview and a quiz. */

typedef struct {
    const char *map_page;
    const char *back_page;
    const char *title_key;
    const char *sub_key;
    const char *unit_keys[LIT_N];
    char **progress_path;
    NetLesson *lessons;
    GtkWidget *scroll;
    GtkWidget *fixed;
    GtkWidget *rail;
    GtkWidget *nodes[LIT_N];
    GtkWidget *labels[LIT_N];
    double cx[LIT_N];
    double cy[LIT_N];
    int cols;
    int rows;
    int cw;
    int ch;
    guint idle;
    double last_avail;
    int last_cols;
    int last_rows;
    int last_cw;
    int last_ch;
    cairo_surface_t *rail_cache;
    int cache_w;
    int cache_h;
    int note_last_w;
} LitTrack;

static LitTrack lit_tracks[4];

static void lit_ensure(void);

static void lit_point(LitTrack *T, double t, double *ox, double *oy) {
    int k = (int)t;
    double u = t - (double)k;
    double u2, u3;
    double p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y;

    if (k < 0) { k = 0; u = 0.0; }
    if (k >= LIT_N - 1) { k = LIT_N - 2; u = 1.0; }

    p1x = T->cx[k];     p1y = T->cy[k];
    p2x = T->cx[k + 1]; p2y = T->cy[k + 1];
    if (k - 1 >= 0) { p0x = T->cx[k - 1]; p0y = T->cy[k - 1]; }
    else            { p0x = p1x - (p2x - p1x); p0y = p1y - (p2y - p1y); }
    if (k + 2 < LIT_N) { p3x = T->cx[k + 2]; p3y = T->cy[k + 2]; }
    else               { p3x = p2x + (p2x - p1x); p3y = p2y + (p2y - p1y); }

    u2 = u * u;
    u3 = u2 * u;
    *ox = 0.5 * (2.0 * p1x + (-p0x + p2x) * u
                 + (2.0 * p0x - 5.0 * p1x + 4.0 * p2x - p3x) * u2
                 + (-p0x + 3.0 * p1x - 3.0 * p2x + p3x) * u3);
    *oy = 0.5 * (2.0 * p1y + (-p0y + p2y) * u
                 + (2.0 * p0y - 5.0 * p1y + 4.0 * p2y - p3y) * u2
                 + (-p0y + 3.0 * p1y - 3.0 * p2y + p3y) * u3);
}

static void lit_path(LitTrack *T, cairo_t *cr, double t0, double t1) {
    const double steps_per_unit = 24.0;
    int n = (int)((t1 - t0) * steps_per_unit);
    double x, y;
    int s;

    if (n < 1)
        n = 1;
    lit_point(T, t0, &x, &y);
    cairo_move_to(cr, x, y);
    for (s = 1; s <= n; s++) {
        lit_point(T, t0 + (t1 - t0) * (double)s / (double)n, &x, &y);
        cairo_line_to(cr, x, y);
    }
}

static void lit_geometry(LitTrack *T, double avail) {
    const double phase = 2.0 * G_PI * 1.7 / (double)(LIT_N - 1);
    int rows;
    int r, c, i;

    for (rows = 1; rows <= LIT_N; rows++) {
        int cols = (LIT_N + rows - 1) / rows;
        if (2.0 * ROAD_MX + (cols - 1) * PATH_SPAC <= avail + 1.0)
            break;
    }
    if (rows > LIT_N)
        rows = LIT_N;
    T->rows = rows;
    T->cols = (LIT_N + rows - 1) / rows;
    if (T->cols < 1)
        T->cols = 1;
    T->cw = (int)(2.0 * ROAD_MX + (T->cols - 1) * PATH_SPAC);
    T->ch = (int)(2.0 * ROAD_MY + (rows - 1) * ROAD_GAP);

    for (r = 0; r < rows; r++) {
        int base = r * T->cols;
        int len = MIN(T->cols, LIT_N - base);
        int fwd = (r % 2) == 0;
        for (c = 0; c < len; c++) {
            int cc = fwd ? c : (T->cols - 1 - c);
            i = base + c;
            T->cx[i] = ROAD_MX + cc * PATH_SPAC;
            T->cy[i] = ROAD_MY + r * ROAD_GAP
                       + ROAD_WAVE * sin((double)i * phase);
        }
    }
}

static void lit_apply_layout(LitTrack *T) {
    if (!T->fixed)
        return;
    if (T->cols == T->last_cols && T->rows == T->last_rows &&
        T->cw == T->last_cw && T->ch == T->last_ch)
        return;

    T->last_cols = T->cols;
    T->last_rows = T->rows;
    T->last_cw = T->cw;
    T->last_ch = T->ch;
    if (T->rail_cache) {
        cairo_surface_destroy(T->rail_cache);
        T->rail_cache = NULL;
    }

    for (int i = 0; i < LIT_N; i++) {
        if (!T->nodes[i] || !T->labels[i])
            continue;
        gtk_fixed_move(GTK_FIXED(T->fixed), T->nodes[i],
                       (int)(T->cx[i] - NODE_SIZE / 2.0),
                       (int)(T->cy[i] - NODE_SIZE / 2.0));
        gtk_fixed_move(GTK_FIXED(T->fixed), T->labels[i],
                       (int)(T->cx[i] - (PATH_SPAC - 20.0) / 2.0),
                       (int)(T->cy[i] + NODE_SIZE / 2.0 + 10.0));
    }

    gtk_widget_set_size_request(T->fixed, T->cw, T->ch);
    gtk_widget_set_size_request(T->rail, T->cw, T->ch);
    gtk_fixed_move(GTK_FIXED(T->fixed), T->rail, 0, 0);
    gtk_widget_queue_draw(T->rail);
}

static void lit_relayout(LitTrack *T) {
    GtkAdjustment *hadj;
    double avail;

    if (!T->scroll)
        return;
    hadj = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(T->scroll));
    avail = gtk_adjustment_get_page_size(hadj);
    if (avail < 1.0)
        return;
    if (fabs(avail - T->last_avail) < 1.0)
        return;
    T->last_avail = avail;
    lit_geometry(T, avail);
    lit_apply_layout(T);
}

static gboolean lit_relayout_idle(gpointer data) {
    LitTrack *T = data;

    T->idle = 0;
    lit_relayout(T);
    return G_SOURCE_REMOVE;
}

static void lit_relayout_later(LitTrack *T) {
    if (T->idle == 0)
        T->idle = g_idle_add(lit_relayout_idle, T);
}

static void lit_adjust_notify(GtkAdjustment *adj, GParamSpec *ps,
                              gpointer data) {
    (void)adj;
    (void)ps;
    lit_relayout_later(data);
}

static void lit_render_rail(LitTrack *T, cairo_t *cr) {
    const double t_end = (double)(LIT_N - 1);
    const Rgb rail = color_from_hex(app_theme.rail);
    const Rgb muted = color_from_hex(app_theme.subtext);

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

    cairo_new_path(cr);
    lit_path(T, cr, 0.0, t_end);
    cairo_set_line_width(cr, 12);
    cairo_set_source_rgb(cr, rail.r, rail.g, rail.b);
    cairo_stroke(cr);

    cairo_set_source_rgba(cr, muted.r, muted.g, muted.b, 0.17);
    cairo_set_dash(cr, (double[]){2.0, 40.0}, 2, 0.0);
    cairo_new_path(cr);
    lit_path(T, cr, 0.0, t_end);
    cairo_stroke(cr);
    cairo_set_dash(cr, NULL, 0, 0.0);
}

static void lit_draw_rail(GtkDrawingArea *area, cairo_t *cr,
                          int width, int height, gpointer data) {
    LitTrack *T = data;
    cairo_t *rcr;

    (void)area;
    (void)width;
    (void)height;

    if (!T->rail_cache || T->cache_w != T->cw || T->cache_h != T->ch) {
        if (T->rail_cache)
            cairo_surface_destroy(T->rail_cache);
        T->cache_w = T->cw > 0 ? T->cw : 1;
        T->cache_h = T->ch > 0 ? T->ch : 1;
        T->rail_cache = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                    T->cache_w, T->cache_h);
        rcr = cairo_create(T->rail_cache);
        lit_render_rail(T, rcr);
        cairo_destroy(rcr);
    }

    cairo_set_source_surface(cr, T->rail_cache, 0, 0);
    cairo_paint(cr);
}

void lit_rail_theme_reset(void) {
    for (int y = 0; y < 4; y++) {
        LitTrack *T = &lit_tracks[y];

        if (T->rail_cache) {
            cairo_surface_destroy(T->rail_cache);
            T->rail_cache = NULL;
        }
        if (T->rail)
            gtk_widget_queue_draw(T->rail);
    }
}

static void lit_save(LitTrack *T) {
    GKeyFile *kf;
    char *path;
    gchar *data;

    if (!T->progress_path || !*T->progress_path)
        return;
    path = *T->progress_path;
    kf = g_key_file_new();
    for (int i = 0; i < LIT_N; i++) {
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

static void lit_load_one(LitTrack *T) {
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
    for (int i = 0; i < LIT_N; i++) {
        gchar key[8];

        g_snprintf(key, sizeof(key), "%d", i + 1);
        T->lessons[i].done = g_key_file_get_boolean(kf, "done", key, NULL);
    }
    g_key_file_free(kf);
}

void lit_load_progress(void) {
    lit_ensure();
    for (int y = 0; y < 4; y++)
        lit_load_one(&lit_tracks[y]);
}

static void lit_refresh(LitTrack *T) {
    for (int i = 0; i < LIT_N; i++) {
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

static void lit_mark_done(LitTrack *T, int lesson_id) {
    if (lesson_id < 0 || lesson_id >= LIT_N)
        return;
    if (T->lessons[lesson_id].done)
        return;
    T->lessons[lesson_id].done = TRUE;
    lit_save(T);
    lit_refresh(T);
    refresh_stats_ui();
}

void progress_for_lit(ProgressSum *out) {
    if (!out)
        return;
    lit_ensure();
    for (int y = 0; y < 4; y++) {
        LitTrack *T = &lit_tracks[y];

        out->total_ex += LIT_N;
        out->open_units += LIT_N;
        for (int i = 0; i < LIT_N; i++) {
            if (T->lessons[i].done) {
                out->done_ex++;
                out->done_units++;
            }
        }
    }
}

static void lit_open_unit(GtkButton *button, gpointer data) {
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

static void lit_add_node(LitTrack *T, GtkFixed *fixed, int index) {
    GtkWidget *card;
    GtkWidget *vbox;
    GtkWidget *number;
    GtkWidget *icon;
    GtkWidget *name;
    char *text;

    card = gtk_button_new();
    gtk_widget_add_css_class(card, "unit-node");
    gtk_widget_add_css_class(card, "current");
    gtk_widget_set_can_focus(card, FALSE);
    gtk_widget_set_size_request(card, (int)NODE_SIZE, (int)NODE_SIZE);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
    gtk_button_set_child(GTK_BUTTON(card), vbox);

    text = g_strdup_printf("%d", index + 1);
    number = gtk_label_new(text);
    g_free(text);
    gtk_widget_add_css_class(number, "unit-number");
    gtk_box_append(GTK_BOX(vbox), number);

    icon = icon_area_new(draw_check_icon, 0.1176, 0.1176, 0.1804, 18);
    gtk_widget_set_visible(icon, FALSE);
    gtk_box_append(GTK_BOX(vbox), icon);
    T->lessons[index].done_icon = icon;
    g_signal_connect(card, "clicked", G_CALLBACK(lit_open_unit),
                     &T->lessons[index]);

    T->nodes[index] = card;
    gtk_fixed_put(fixed, card, 0, 0);

    name = gtk_label_new(NULL);
    i18n_bind(name, T->unit_keys[index], 0);
    gtk_widget_set_size_request(name, (int)(PATH_SPAC - 20.0), -1);
    gtk_widget_set_halign(name, GTK_ALIGN_CENTER);
    gtk_label_set_justify(GTK_LABEL(name), GTK_JUSTIFY_CENTER);
    gtk_label_set_wrap(GTK_LABEL(name), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(name), 18);
    gtk_widget_add_css_class(name, "unit-name");
    T->labels[index] = name;
    gtk_fixed_put(fixed, name, 0, 0);
}

static GtkWidget *lit_build_map(LitTrack *T) {
    GtkWidget *page;

    lit_ensure();
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *fixed;
    GtkWidget *rail;
    GtkAdjustment *ha;
    GtkAdjustment *va;

    T->last_avail = -1.0;
    lit_geometry(T, 1000.0);

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
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);
    T->scroll = scroll;

    wrap = gtk_center_box_new();
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), wrap);

    fixed = gtk_fixed_new();
    gtk_widget_set_halign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(fixed, T->cw, T->ch);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), fixed);
    T->fixed = fixed;

    rail = gtk_drawing_area_new();
    gtk_widget_set_size_request(rail, T->cw, T->ch);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(rail), lit_draw_rail,
                                   T, NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rail, 0, 0);
    T->rail = rail;

    for (int i = 0; i < LIT_N; i++)
        lit_add_node(T, GTK_FIXED(fixed), i);

    ha = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scroll));
    va = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));
    g_signal_connect(ha, "notify::page-size",
                     G_CALLBACK(lit_adjust_notify), T);
    g_signal_connect(va, "notify::page-size",
                     G_CALLBACK(lit_adjust_notify), T);

    lit_apply_layout(T);
    lit_relayout_later(T);
    lit_refresh(T);
    return page;
}

static GtkWidget *lit_year_button(int year, const char *target,
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

GtkWidget *build_lityears_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *list;
    static const char *targets[] = {"litmap", "lit2map", "lit3map", "lit4map"};
    static const char *titles[] = {
        "lit_year1", "lit_year2", "lit_year3", "lit_year4",
    };
    static const char *subs[] = {"lit_sub", "lit2_sub", "lit3_sub", "lit4_sub"};

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("czechmap", "Literatura", "lit_years_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 18);
    gtk_box_append(GTK_BOX(page), scroll);

    list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_halign(list, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(list, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(list, 420, -1);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list);

    for (int y = 0; y < 4; y++)
        gtk_box_append(GTK_BOX(list),
                       lit_year_button(y + 1, targets[y], titles[y], subs[y]));
    return page;
}

GtkWidget *build_litmap_page(void) { return lit_build_map(&lit_tracks[0]); }
GtkWidget *build_lit2map_page(void) { return lit_build_map(&lit_tracks[1]); }
GtkWidget *build_lit3map_page(void) { return lit_build_map(&lit_tracks[2]); }
GtkWidget *build_lit4map_page(void) { return lit_build_map(&lit_tracks[3]); }

static void lit_slide_apply(NetLesson *L) {
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

static void lit_slide_prev(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L || L->idx == 0)
        return;
    L->idx--;
    lit_slide_apply(L);
}

static void lit_slide_next(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L)
        return;
    if (L->idx + 1 < L->n_slides) {
        L->idx++;
        lit_slide_apply(L);
    } else {
        show_page(L->ex_page, 1);
    }
}

static void lit_rescale(LitTrack *T, int w) {
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
    for (int i = 0; i < LIT_N; i++)
        net_rescale_lesson_notes(&T->lessons[i], body, head, kick);
}

static gboolean lit_note_tick(GtkWidget *w, GdkFrameClock *clock,
                              gpointer data) {
    LitTrack *T = data;
    int width = gtk_widget_get_allocated_width(w);

    (void)clock;
    if (width != T->note_last_w) {
        T->note_last_w = width;
        lit_rescale(T, width);
    }
    return G_SOURCE_CONTINUE;
}

GtkWidget *lit_unit_page(int year, NetLesson *L, const char *title_key,
                         const char *sub_key, const NetSlide *slides,
                         guint n_slides) {
    LitTrack *T;

    lit_ensure();
    T = &lit_tracks[year];
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *nav;

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

    gtk_widget_add_tick_callback(L->stack, lit_note_tick, T, NULL);
    T->note_last_w = -1;
    lit_rescale(T, 0);
    net_notes_target = NULL;

    nav = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(nav), TRUE);
    gtk_widget_set_margin_top(nav, 10);
    gtk_box_append(GTK_BOX(page), nav);

    L->prev_btn = gtk_button_new_with_label(tr("net_slide_prev"));
    gtk_widget_add_css_class(L->prev_btn, "btn-primary");
    g_signal_connect(L->prev_btn, "clicked", G_CALLBACK(lit_slide_prev), L);
    gtk_box_append(GTK_BOX(nav), L->prev_btn);

    L->next_btn = gtk_button_new_with_label(tr("net_slide_next"));
    gtk_widget_add_css_class(L->next_btn, "btn-primary");
    g_signal_connect(L->next_btn, "clicked", G_CALLBACK(lit_slide_next), L);
    gtk_box_append(GTK_BOX(nav), L->next_btn);

    L->idx = 0;
    lit_slide_apply(L);
    return page;
}

void lit_lessons_apply_lang(void) {
    lit_ensure();
    for (int y = 0; y < 4; y++) {
        for (int i = 0; i < LIT_N; i++)
            lit_slide_apply(&lit_tracks[y].lessons[i]);
    }
}

typedef struct {
    const ChoiceQ *qs;
    int n;
    int n_opts;
    LitTrack *track;
    int lesson_id;
    GtkWidget *feedback;
    GtkToggleButton **toggles;
    GtkWidget **hints;
} LitMcqCtx;

static void lit_mcq_check(GtkButton *button, gpointer data) {
    LitMcqCtx *ctx = data;
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
        lit_mark_done(ctx->track, ctx->lesson_id);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry_short"));
    }
}

GtkWidget *lit_mcq_page(int year, int lesson_id, const char *back_page,
                        const char *title_key, const char *heading_key,
                        const ChoiceQ *qs, const char **hints, int n) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *body;
    GtkWidget *check;
    GtkWidget *btns;
    GtkWidget *head;
    GtkWidget *intro;
    LitMcqCtx *ctx = g_new0(LitMcqCtx, 1);
    int n_opts = qs[0].n_options;

    lit_ensure();

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
    ctx->track = &lit_tracks[year];
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
    g_signal_connect(check, "clicked", G_CALLBACK(lit_mcq_check), ctx);

    ctx->feedback = gtk_label_new("");
    gtk_widget_set_halign(ctx->feedback, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(body), ctx->feedback);
    return page;
}

static gboolean lit_ready;

static void lit_tracks_init(void) {
    static const char *keys0[] = {
        "lit_unit1", "lit_unit2", "lit_unit3", "lit_unit4",
        "lit_unit5", "lit_unit6", "lit_unit7", "lit_unit8",
    };
    static const char *keys1[] = {
        "lit2_unit1", "lit2_unit2", "lit2_unit3", "lit2_unit4",
        "lit2_unit5", "lit2_unit6", "lit2_unit7", "lit2_unit8",
    };
    static const char *keys2[] = {
        "lit3_unit1", "lit3_unit2", "lit3_unit3", "lit3_unit4",
        "lit3_unit5", "lit3_unit6", "lit3_unit7", "lit3_unit8",
    };
    static const char *keys3[] = {
        "lit4_unit1", "lit4_unit2", "lit4_unit3", "lit4_unit4",
        "lit4_unit5", "lit4_unit6", "lit4_unit7", "lit4_unit8",
    };
    static const char *const *all_keys[4] = {keys0, keys1, keys2, keys3};
    static const char *maps[] = {"litmap", "lit2map", "lit3map", "lit4map"};
    static const char *titles[] = {
        "lit_year1", "lit_year2", "lit_year3", "lit_year4",
    };
    static const char *subs[] = {"lit_sub", "lit2_sub", "lit3_sub", "lit4_sub"};
    NetLesson *lessons[4] = {
        lit_lessons, lit2_lessons, lit3_lessons, lit4_lessons,
    };
    char **paths[4] = {
        &app_progress_lit, &app_progress_lit2,
        &app_progress_lit3, &app_progress_lit4,
    };

    for (int y = 0; y < 4; y++) {
        LitTrack *T = &lit_tracks[y];

        T->map_page = maps[y];
        T->back_page = "lityears";
        T->title_key = titles[y];
        T->sub_key = subs[y];
        T->progress_path = paths[y];
        T->lessons = lessons[y];
        T->last_avail = -1.0;
        T->last_cols = -1;
        T->note_last_w = -1;
        for (int i = 0; i < LIT_N; i++)
            T->unit_keys[i] = all_keys[y][i];
    }
}

static void lit_ensure(void) {
    if (lit_ready)
        return;
    lit_ready = TRUE;
    lit_tracks_init();
}
