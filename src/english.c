#include "graduately.h"

#include <string.h>

#ifdef G_OS_UNIX
#include <sys/wait.h>
#endif

/* English across four gymnasium years (RVP G, foreign language, maturita
 * skills). Lesson text lives in english_lessons.inc. */

#define EN_YEARS 4
#define EN_N     8

typedef struct {
    const NetSlide *slides;
    int n_slides;
    const char *reading;
    const ChoiceQ *readq;
    const char **read_hints;
    int n_read;
    const char *listening;
    const ChoiceQ *listenq;
    const char **listen_hints;
    int n_listen;
    const TypedQ *gaps;
    int n_gaps;
    const char *write_prompt;
    const char *write_model;
    const char *write_keys;
    int write_min;
    const ChoiceQ *quiz;
    const char **quiz_hints;
    int n_quiz;
} EnLesson;

#include "english_lessons.inc"

typedef struct {
    const ChoiceQ *q;
    GtkToggleButton *tb[4];
    GtkWidget *hint;
} EnPick;

typedef struct {
    int year;
    int index;
    const EnLesson *lesson;
    GtkWidget *feedback;
    GtkWidget *model;
    GArray *picks;
    GArray *entries;
    GArray *gap_hints;
    GtkWidget *essay;
    GtkWidget *script;
    GtkWidget *play_note;
    gboolean played;
    gboolean busy;
    gboolean fallback;
    GPid pid;
    guint timer;
    int tick;
    gchar **sentences;
} EnEx;

typedef struct {
    int year;
    GtkWidget *scroll;
    GtkWidget *fixed;
    GtkWidget *rail;
    GtkWidget *nodes[EN_N];
    GtkWidget *labels[EN_N];
    double cx[EN_N];
    double cy[EN_N];
    int cols, rows, cw, ch;
    guint idle;
    double last_avail;
    int last_cols, last_rows, last_cw, last_ch;
    cairo_surface_t *cache;
    int cache_w, cache_h;
} EngMap;

static NetLesson en_rt[EN_YEARS][EN_N];
static EngMap en_maps[EN_YEARS];
static char en_map_page[EN_YEARS][16];
static char en_unit_page[EN_YEARS][EN_N][16];
static char en_ex_page[EN_YEARS][EN_N][16];
static char en_title_key[EN_YEARS][EN_N][16];
static char en_sub_key[EN_YEARS][EN_N][20];
static gboolean en_ready;

static void en_prepare(void) {
    if (en_ready)
        return;
    en_ready = TRUE;
    for (int y = 0; y < EN_YEARS; y++) {
        g_snprintf(en_map_page[y], sizeof en_map_page[y], "en%dmap", y + 1);
        for (int i = 0; i < EN_N; i++) {
            g_snprintf(en_unit_page[y][i], sizeof en_unit_page[y][i],
                       "en%dunit%d", y + 1, i + 1);
            g_snprintf(en_ex_page[y][i], sizeof en_ex_page[y][i],
                       "en%dex%d", y + 1, i + 1);
            g_snprintf(en_title_key[y][i], sizeof en_title_key[y][i],
                       "en_y%d_u%d", y + 1, i + 1);
            g_snprintf(en_sub_key[y][i], sizeof en_sub_key[y][i],
                       "en_y%d_u%d_sub", y + 1, i + 1);
            en_rt[y][i].unit_page = en_unit_page[y][i];
            en_rt[y][i].ex_page = en_ex_page[y][i];
            en_rt[y][i].n_slides = (guint)en_lessons[y][i].n_slides;
        }
    }
}

static void en_save_progress(void) {
    GKeyFile *kf = g_key_file_new();
    gchar *data;
    GError *err = NULL;

    for (int y = 0; y < EN_YEARS; y++) {
        for (int i = 0; i < EN_N; i++) {
            char key[8];

            g_snprintf(key, sizeof key, "%d-%d", y + 1, i + 1);
            g_key_file_set_boolean(kf, "done", key, en_rt[y][i].done);
        }
    }
    data = g_key_file_to_data(kf, NULL, NULL);
    if (!g_file_set_contents(PROGRESS_EN, data, -1, &err)) {
        g_warning("english progress: %s", err->message);
        g_error_free(err);
    }
    g_free(data);
    g_key_file_free(kf);
}

void en_load_progress(void) {
    GKeyFile *kf = g_key_file_new();
    GError *err = NULL;

    en_prepare();
    if (!g_key_file_load_from_file(kf, PROGRESS_EN, G_KEY_FILE_NONE, &err)) {
        if (err)
            g_error_free(err);
        g_key_file_free(kf);
        return;
    }
    for (int y = 0; y < EN_YEARS; y++) {
        for (int i = 0; i < EN_N; i++) {
            char key[8];

            g_snprintf(key, sizeof key, "%d-%d", y + 1, i + 1);
            en_rt[y][i].done = g_key_file_get_boolean(kf, "done", key, NULL);
        }
    }
    g_key_file_free(kf);
}

static void en_refresh(void) {
    for (int y = 0; y < EN_YEARS; y++) {
        for (int i = 0; i < EN_N; i++) {
            GtkWidget *node = en_maps[y].nodes[i];
            GtkWidget *icon = en_rt[y][i].done_icon;

            if (!node)
                continue;
            if (en_rt[y][i].done) {
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
}

static void mark_en_done(int year, int index) {
    if (year < 0 || year >= EN_YEARS || index < 0 || index >= EN_N)
        return;
    if (en_rt[year][index].done)
        return;
    en_rt[year][index].done = TRUE;
    en_save_progress();
    en_refresh();
    refresh_stats_ui();
}

void progress_for_en(ProgressSum *out) {
    out->total_ex += EN_YEARS * EN_N;
    out->open_units += EN_YEARS * EN_N;
    for (int y = 0; y < EN_YEARS; y++) {
        for (int i = 0; i < EN_N; i++) {
            if (en_rt[y][i].done) {
                out->done_ex++;
                out->done_units++;
            }
        }
    }
}

/* ------------------------------------------------------------------ */
/* Path                                                                */
/* ------------------------------------------------------------------ */

static void en_point(EngMap *m, double t, double *ox, double *oy) {
    int k = (int)t;
    double u = t - (double)k;
    double u2, u3;
    double p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y;

    if (k < 0) { k = 0; u = 0.0; }
    if (k >= EN_N - 1) { k = EN_N - 2; u = 1.0; }

    p1x = m->cx[k];     p1y = m->cy[k];
    p2x = m->cx[k + 1]; p2y = m->cy[k + 1];
    if (k - 1 >= 0) { p0x = m->cx[k - 1]; p0y = m->cy[k - 1]; }
    else            { p0x = p1x - (p2x - p1x); p0y = p1y - (p2y - p1y); }
    if (k + 2 < EN_N) { p3x = m->cx[k + 2]; p3y = m->cy[k + 2]; }
    else              { p3x = p2x + (p2x - p1x); p3y = p2y + (p2y - p1y); }

    u2 = u * u;
    u3 = u2 * u;
    *ox = 0.5 * (2.0 * p1x + (-p0x + p2x) * u
                 + (2.0 * p0x - 5.0 * p1x + 4.0 * p2x - p3x) * u2
                 + (-p0x + 3.0 * p1x - 3.0 * p2x + p3x) * u3);
    *oy = 0.5 * (2.0 * p1y + (-p0y + p2y) * u
                 + (2.0 * p0y - 5.0 * p1y + 4.0 * p2y - p3y) * u2
                 + (-p0y + 3.0 * p1y - 3.0 * p2y + p3y) * u3);
}

static void en_path(EngMap *m, cairo_t *cr, double t0, double t1) {
    int n = (int)((t1 - t0) * 24.0);
    double x, y;

    if (n < 1)
        n = 1;
    en_point(m, t0, &x, &y);
    cairo_move_to(cr, x, y);
    for (int s = 1; s <= n; s++) {
        en_point(m, t0 + (t1 - t0) * (double)s / (double)n, &x, &y);
        cairo_line_to(cr, x, y);
    }
}

static void en_geometry(EngMap *m, double avail) {
    const double phase = 2.0 * G_PI * 1.7 / (double)(EN_N - 1);
    int rows;

    for (rows = 1; rows <= EN_N; rows++) {
        int cols = (EN_N + rows - 1) / rows;
        if (2.0 * ROAD_MX + (cols - 1) * PATH_SPAC <= avail + 1.0)
            break;
    }
    if (rows > EN_N)
        rows = EN_N;
    m->rows = rows;
    m->cols = (EN_N + rows - 1) / rows;
    if (m->cols < 1)
        m->cols = 1;
    m->cw = (int)(2.0 * ROAD_MX + (m->cols - 1) * PATH_SPAC);
    m->ch = (int)(2.0 * ROAD_MY + (rows - 1) * ROAD_GAP);

    for (int r = 0; r < rows; r++) {
        int base = r * m->cols;
        int len = MIN(m->cols, EN_N - base);
        int fwd = (r % 2) == 0;

        for (int c = 0; c < len; c++) {
            int i = base + c;
            int cc = fwd ? c : (m->cols - 1 - c);

            m->cx[i] = ROAD_MX + cc * PATH_SPAC;
            m->cy[i] = ROAD_MY + r * ROAD_GAP
                       + ROAD_WAVE * sin((double)i * phase);
        }
    }
}

static void en_apply_layout(EngMap *m) {
    if (!m->fixed)
        return;
    if (m->cols == m->last_cols && m->rows == m->last_rows &&
        m->cw == m->last_cw && m->ch == m->last_ch)
        return;
    m->last_cols = m->cols;
    m->last_rows = m->rows;
    m->last_cw = m->cw;
    m->last_ch = m->ch;
    if (m->cache) {
        cairo_surface_destroy(m->cache);
        m->cache = NULL;
    }
    for (int i = 0; i < EN_N; i++) {
        if (!m->nodes[i] || !m->labels[i])
            continue;
        gtk_fixed_move(GTK_FIXED(m->fixed), m->nodes[i],
                       (int)(m->cx[i] - NODE_SIZE / 2.0),
                       (int)(m->cy[i] - NODE_SIZE / 2.0));
        gtk_fixed_move(GTK_FIXED(m->fixed), m->labels[i],
                       (int)(m->cx[i] - (PATH_SPAC - 20.0) / 2.0),
                       (int)(m->cy[i] + NODE_SIZE / 2.0 + 10.0));
    }
    gtk_widget_set_size_request(m->fixed, m->cw, m->ch);
    gtk_widget_set_size_request(m->rail, m->cw, m->ch);
    gtk_fixed_move(GTK_FIXED(m->fixed), m->rail, 0, 0);
    gtk_widget_queue_draw(m->rail);
}

static void en_relayout(EngMap *m) {
    GtkAdjustment *hadj;
    double avail;

    if (!m->scroll)
        return;
    hadj = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(m->scroll));
    avail = gtk_adjustment_get_page_size(hadj);
    if (avail < 1.0)
        return;
    if (fabs(avail - m->last_avail) < 1.0)
        return;
    m->last_avail = avail;
    en_geometry(m, avail);
    en_apply_layout(m);
}

static gboolean en_relayout_idle(gpointer data) {
    EngMap *m = data;

    m->idle = 0;
    en_relayout(m);
    return G_SOURCE_REMOVE;
}

static void en_adjust_notify(GtkAdjustment *adj, GParamSpec *ps, gpointer data) {
    EngMap *m = data;

    (void)adj;
    (void)ps;
    if (m->idle == 0)
        m->idle = g_idle_add(en_relayout_idle, m);
}

static void en_render_rail(EngMap *m, cairo_t *cr) {
    const Rgb rail = color_from_hex(app_theme.rail);
    const Rgb muted = color_from_hex(app_theme.subtext);

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
    cairo_new_path(cr);
    en_path(m, cr, 0.0, (double)(EN_N - 1));
    cairo_set_line_width(cr, 12);
    cairo_set_source_rgb(cr, rail.r, rail.g, rail.b);
    cairo_stroke(cr);
    cairo_set_source_rgba(cr, muted.r, muted.g, muted.b, 0.17);
    cairo_set_dash(cr, (double[]){2.0, 40.0}, 2, 0.0);
    cairo_new_path(cr);
    en_path(m, cr, 0.0, (double)(EN_N - 1));
    cairo_stroke(cr);
    cairo_set_dash(cr, NULL, 0, 0.0);
}

static void en_draw_rail(GtkDrawingArea *area, cairo_t *cr,
                         int width, int height, gpointer data) {
    EngMap *m = data;

    (void)area;
    (void)width;
    (void)height;
    if (!m->cache || m->cache_w != m->cw || m->cache_h != m->ch) {
        cairo_t *rcr;

        if (m->cache)
            cairo_surface_destroy(m->cache);
        m->cache_w = m->cw > 0 ? m->cw : 1;
        m->cache_h = m->ch > 0 ? m->ch : 1;
        m->cache = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                              m->cache_w, m->cache_h);
        rcr = cairo_create(m->cache);
        en_render_rail(m, rcr);
        cairo_destroy(rcr);
    }
    cairo_set_source_surface(cr, m->cache, 0, 0);
    cairo_paint(cr);
}

void en_rail_theme_reset(void) {
    for (int y = 0; y < EN_YEARS; y++) {
        if (en_maps[y].cache) {
            cairo_surface_destroy(en_maps[y].cache);
            en_maps[y].cache = NULL;
        }
        if (en_maps[y].rail)
            gtk_widget_queue_draw(en_maps[y].rail);
    }
}

static void en_open_unit(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L)
        return;
    L->idx = 0;
    if (L->stack) {
        char name[16];

        g_snprintf(name, sizeof name, "slide%u", L->idx);
        gtk_stack_set_visible_child_name(GTK_STACK(L->stack), name);
        if (L->prev_btn)
            gtk_widget_set_sensitive(L->prev_btn, FALSE);
        if (L->next_btn)
            gtk_button_set_label(GTK_BUTTON(L->next_btn), tr("net_slide_next"));
    }
    gtk_stack_set_visible_child_name(main_stack, L->unit_page);
}

static void en_add_node(EngMap *m, GtkFixed *fixed, int index) {
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
    en_rt[m->year][index].done_icon = icon;
    g_signal_connect(card, "clicked", G_CALLBACK(en_open_unit),
                     &en_rt[m->year][index]);

    m->nodes[index] = card;
    gtk_fixed_put(fixed, card, 0, 0);

    name = gtk_label_new(NULL);
    gtk_widget_set_size_request(name, (int)(PATH_SPAC - 20.0), -1);
    gtk_widget_set_halign(name, GTK_ALIGN_CENTER);
    gtk_label_set_justify(GTK_LABEL(name), GTK_JUSTIFY_CENTER);
    gtk_label_set_wrap(GTK_LABEL(name), TRUE);
    gtk_widget_add_css_class(name, "unit-name");
    i18n_bind(name, en_title_key[m->year][index], 0);
    m->labels[index] = name;
    gtk_fixed_put(fixed, name, 0, 0);
}

static GtkWidget *en_year_button(int year) {
    static const char *keys[] = {"en_year1", "en_year2", "en_year3", "en_year4"};
    static const char *subs[] = {"en_y1_sub", "en_y2_sub", "en_y3_sub", "en_y4_sub"};
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
    gtk_widget_set_size_request(btn, -1, 112);

    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 14);
    gtk_widget_set_margin_start(row, 18);
    gtk_widget_set_margin_end(row, 18);
    gtk_widget_set_margin_top(row, 8);
    gtk_widget_set_margin_bottom(row, 8);
    gtk_button_set_child(GTK_BUTTON(btn), row);

    num_text = g_strdup_printf("%d", year);
    num = gtk_label_new(num_text);
    g_free(num_text);
    gtk_widget_add_css_class(num, "unit-number");
    gtk_widget_set_valign(num, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(row), num);

    texts = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_hexpand(texts, TRUE);
    gtk_box_append(GTK_BOX(row), texts);

    title = gtk_label_new(NULL);
    i18n_bind(title, keys[year - 1], 0);
    gtk_widget_set_halign(title, GTK_ALIGN_START);
    gtk_widget_add_css_class(title, "unit-name");
    gtk_box_append(GTK_BOX(texts), title);

    sub = gtk_label_new(NULL);
    i18n_bind(sub, subs[year - 1], 0);
    gtk_widget_set_halign(sub, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(sub), TRUE);
    gtk_label_set_xalign(GTK_LABEL(sub), 0);
    gtk_widget_add_css_class(sub, "meaning");
    gtk_widget_set_visible(sub, TRUE);
    gtk_box_append(GTK_BOX(texts), sub);

    g_object_set_data_full(G_OBJECT(btn), "target",
                           g_strdup(en_map_page[year - 1]), g_free);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_nav_clicked), NULL);
    return btn;
}

GtkWidget *build_enyears_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *list;

    en_prepare();
    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);
    gtk_box_append(GTK_BOX(page),
                   top_bar("subjects", "English", "en_years_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 18);
    gtk_box_append(GTK_BOX(page), scroll);

    list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_halign(list, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(list, 460, -1);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list);
    for (int y = 1; y <= EN_YEARS; y++)
        gtk_box_append(GTK_BOX(list), en_year_button(y));
    return page;
}

GtkWidget *build_enmap_page(int year) {
    EngMap *m;
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *fixed;
    GtkWidget *rail;
    GtkAdjustment *ha;
    GtkAdjustment *va;
    static const char *year_keys[] = {
        "en_year1", "en_year2", "en_year3", "en_year4",
    };
    static const char *sub_keys[] = {
        "en_y1_sub", "en_y2_sub", "en_y3_sub", "en_y4_sub",
    };

    en_prepare();
    m = &en_maps[year];
    m->year = year;
    en_geometry(m, 1000.0);

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);
    gtk_box_append(GTK_BOX(page),
                   top_bar("enyears", year_keys[year], sub_keys[year]));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);
    m->scroll = scroll;

    wrap = gtk_center_box_new();
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), wrap);

    fixed = gtk_fixed_new();
    gtk_widget_set_halign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(fixed, m->cw, m->ch);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), fixed);
    m->fixed = fixed;

    rail = gtk_drawing_area_new();
    gtk_widget_set_size_request(rail, m->cw, m->ch);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(rail), en_draw_rail, m, NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rail, 0, 0);
    m->rail = rail;

    for (int i = 0; i < EN_N; i++)
        en_add_node(m, GTK_FIXED(fixed), i);

    ha = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scroll));
    va = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));
    g_signal_connect(ha, "notify::page-size", G_CALLBACK(en_adjust_notify), m);
    g_signal_connect(va, "notify::page-size", G_CALLBACK(en_adjust_notify), m);
    en_refresh();
    return page;
}

/* ------------------------------------------------------------------ */
/* Slides                                                              */
/* ------------------------------------------------------------------ */

static void en_slide_apply(NetLesson *L) {
    char name[16];

    if (!L || !L->stack)
        return;
    g_snprintf(name, sizeof name, "slide%u", L->idx);
    gtk_stack_set_visible_child_name(GTK_STACK(L->stack), name);
    if (L->prev_btn) {
        gtk_widget_set_sensitive(L->prev_btn, L->idx > 0);
        gtk_button_set_label(GTK_BUTTON(L->prev_btn), tr("net_slide_prev"));
    }
    if (L->next_btn)
        gtk_button_set_label(GTK_BUTTON(L->next_btn),
                             L->idx + 1 >= L->n_slides ? tr("net_slide_start")
                                                       : tr("net_slide_next"));
}

static int en_note_w = -1;
static gboolean en_note_ready;

static void en_rescale_notes(int w) {
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
    for (int y = 0; y < EN_YEARS; y++)
        for (int i = 0; i < EN_N; i++)
            net_rescale_lesson_notes(&en_rt[y][i], body, head, kick);
}

static gboolean en_note_tick(GtkWidget *w, GdkFrameClock *clock, gpointer data) {
    int width = gtk_widget_get_allocated_width(w);

    (void)clock;
    (void)data;
    if (width != en_note_w) {
        en_note_w = width;
        en_rescale_notes(width);
    }
    return G_SOURCE_CONTINUE;
}

static void en_slide_prev(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (L && L->idx > 0) {
        L->idx--;
        en_slide_apply(L);
    }
}

static void en_slide_next(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L)
        return;
    if (L->idx + 1 < L->n_slides) {
        L->idx++;
        en_slide_apply(L);
    } else {
        gtk_stack_set_visible_child_name(main_stack, L->ex_page);
    }
}

void en_lessons_apply_lang(void) {
    for (int y = 0; y < EN_YEARS; y++)
        for (int i = 0; i < EN_N; i++)
            en_slide_apply(&en_rt[y][i]);
}

static GtkWidget *build_en_unit_page(int year, int index) {
    NetLesson *L = &en_rt[year][index];
    const EnLesson *lesson = &en_lessons[year][index];
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *nav;

    net_lesson_notes_ensure(L);
    net_notes_target = L;
    L->n_slides = (guint)lesson->n_slides;

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(page, 28);
    gtk_widget_set_margin_end(page, 28);
    gtk_widget_set_margin_top(page, 20);
    gtk_widget_set_margin_bottom(page, 20);
    gtk_box_append(GTK_BOX(page),
                   top_bar(en_map_page[year], en_title_key[year][index],
                           en_sub_key[year][index]));

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

    for (guint s = 0; s < L->n_slides; s++) {
        GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
        char name[16];

        g_snprintf(name, sizeof name, "slide%u", s);
        gtk_widget_set_margin_top(card, 8);
        gtk_widget_set_margin_bottom(card, 8);
        net_note_kicker(card, lesson->slides[s].kicker);
        net_note_title(card, lesson->slides[s].title);
        for (int i = 0; i < 8 && lesson->slides[s].line[i]; i++)
            net_note_line(card, lesson->slides[s].line[i], FALSE);
        if (lesson->slides[s].tip)
            net_note_line(card, lesson->slides[s].tip, TRUE);
        gtk_stack_add_named(GTK_STACK(L->stack), card, name);
    }

    nav = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(nav), TRUE);
    gtk_widget_set_margin_top(nav, 10);
    gtk_box_append(GTK_BOX(page), nav);
    L->prev_btn = gtk_button_new_with_label(tr("net_slide_prev"));
    gtk_widget_add_css_class(L->prev_btn, "btn-primary");
    gtk_box_append(GTK_BOX(nav), L->prev_btn);
    g_signal_connect(L->prev_btn, "clicked", G_CALLBACK(en_slide_prev), L);
    L->next_btn = gtk_button_new_with_label(tr("net_slide_next"));
    gtk_widget_add_css_class(L->next_btn, "btn-primary");
    gtk_box_append(GTK_BOX(nav), L->next_btn);
    g_signal_connect(L->next_btn, "clicked", G_CALLBACK(en_slide_next), L);
    if (!en_note_ready) {
        gtk_widget_add_tick_callback(L->stack, en_note_tick, NULL, NULL);
        en_note_ready = TRUE;
    }
    en_rescale_notes(0);
    net_notes_target = NULL;
    en_slide_apply(L);
    return page;
}

/* ------------------------------------------------------------------ */
/* Exercise                                                            */
/* ------------------------------------------------------------------ */

static GtkWidget *en_heading(GtkWidget *body, const char *key) {
    GtkWidget *label = gtk_label_new(NULL);

    i18n_bind(label, key, 0);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_widget_add_css_class(label, "quiz-heading");
    gtk_widget_set_margin_top(label, 8);
    gtk_box_append(GTK_BOX(body), label);
    return label;
}

static GtkWidget *en_passage(GtkWidget *body, const char *text, gboolean hide) {
    GtkWidget *label = gtk_label_new(text);

    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_wrap_mode(GTK_LABEL(label), PANGO_WRAP_WORD_CHAR);
    gtk_label_set_selectable(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_add_css_class(label, "quiz-intro");
    gtk_widget_set_visible(label, !hide);
    gtk_box_append(GTK_BOX(body), label);
    return label;
}

static void en_add_mcq(EnEx *ctx, GtkWidget *body, const ChoiceQ *qs,
                       const char **hints, int n) {
    for (int i = 0; i < n; i++) {
        EnPick pick;
        GtkWidget *card;

        memset(&pick, 0, sizeof pick);
        pick.q = &qs[i];
        card = mcq_append_question(body, i + 1, &qs[i], pick.tb);
        if (hints && hints[i])
            pick.hint = meaning_add(card, hints[i]);
        g_array_append_val(ctx->picks, pick);
    }
}

static int en_word_count(const char *norm) {
    int n = 0;
    const char *p = norm ? norm : "";

    while (*p) {
        while (*p == ' ')
            p++;
        if (!*p)
            break;
        n++;
        while (*p && *p != ' ')
            p++;
    }
    return n;
}

static gboolean en_has_word(const char *norm, const char *word) {
    char *key = normalize_answer(word);
    size_t n;
    const char *p;
    gboolean ok = FALSE;

    if (!key || !key[0]) {
        g_free(key);
        return TRUE;
    }
    n = strlen(key);
    for (p = norm; p && (p = strstr(p, key)); p += n) {
        gboolean left = p == norm || p[-1] == ' ';
        gboolean right = p[n] == '\0' || p[n] == ' ';

        if (left && right) {
            ok = TRUE;
            break;
        }
    }
    g_free(key);
    return ok;
}

static void en_split_script(EnEx *ctx) {
    GPtrArray *parts = g_ptr_array_new();
    const char *text = ctx->lesson->listening;
    const char *start = text;
    const char *p = text;

    while (p && *p) {
        if ((*p == '.' || *p == '?' || *p == '!') && p[1] == ' ') {
            g_ptr_array_add(parts, g_strndup(start, (gsize)(p - start + 1)));
            p += 2;
            start = p;
            continue;
        }
        p++;
    }
    if (start && *start)
        g_ptr_array_add(parts, g_strdup(start));
    if (parts->len == 0)
        g_ptr_array_add(parts, g_strdup(text ? text : ""));
    g_ptr_array_add(parts, NULL);
    ctx->sentences = (gchar **)g_ptr_array_free(parts, FALSE);
    ctx->tick = 0;
}

static gboolean en_tick(gpointer data) {
    EnEx *ctx = data;
    GString *shown;
    int i;

    if (!ctx->sentences || !ctx->sentences[ctx->tick]) {
        ctx->timer = 0;
        ctx->busy = FALSE;
        if (ctx->script && ctx->lesson->listening)
            gtk_label_set_text(GTK_LABEL(ctx->script), ctx->lesson->listening);
        return G_SOURCE_REMOVE;
    }
    shown = g_string_new(NULL);
    for (i = 0; i <= ctx->tick && ctx->sentences[i]; i++) {
        if (i)
            g_string_append_c(shown, ' ');
        g_string_append(shown, ctx->sentences[i]);
    }
    gtk_label_set_text(GTK_LABEL(ctx->script), shown->str);
    gtk_widget_set_visible(ctx->script, TRUE);
    g_string_free(shown, TRUE);
    ctx->tick++;
    if (!ctx->sentences[ctx->tick]) {
        ctx->timer = 0;
        ctx->busy = FALSE;
        return G_SOURCE_REMOVE;
    }
    return G_SOURCE_CONTINUE;
}

static void en_start_fallback(EnEx *ctx) {
    if (ctx->fallback)
        return;
    ctx->fallback = TRUE;
    ctx->played = TRUE;
    ctx->busy = TRUE;
    if (ctx->play_note) {
        gtk_label_set_text(GTK_LABEL(ctx->play_note), tr("en_listen_fallback"));
        gtk_widget_set_visible(ctx->play_note, TRUE);
    }
    if (!ctx->sentences)
        en_split_script(ctx);
    ctx->tick = 0;
    if (ctx->timer)
        g_source_remove(ctx->timer);
    ctx->timer = g_timeout_add(2200, en_tick, ctx);
    en_tick(ctx);
}

static void en_voice_done(GPid pid, gint status, gpointer data) {
    EnEx *ctx = data;
    gboolean ok = FALSE;

    g_spawn_close_pid(pid);
    if (ctx->pid == pid)
        ctx->pid = 0;
#ifdef G_OS_UNIX
    ok = WIFEXITED(status) && WEXITSTATUS(status) == 0;
#else
    ok = status == 0;
#endif
    if (!ok) {
        ctx->busy = FALSE;
        en_start_fallback(ctx);
        return;
    }
    ctx->busy = FALSE;
    if (ctx->script && ctx->lesson->listening) {
        gtk_label_set_text(GTK_LABEL(ctx->script), ctx->lesson->listening);
        gtk_widget_set_visible(ctx->script, TRUE);
    }
}

static gboolean en_spawn(char **argv, GPid *pid) {
    GError *err = NULL;

    if (!argv[0] || !g_find_program_in_path(argv[0]))
        return FALSE;
    if (!g_spawn_async(NULL, argv, NULL,
                       G_SPAWN_SEARCH_PATH | G_SPAWN_DO_NOT_REAP_CHILD,
                       NULL, NULL, pid, &err)) {
        g_clear_error(&err);
        return FALSE;
    }
    return TRUE;
}

static void en_play_clicked(GtkButton *button, gpointer data) {
    EnEx *ctx = data;
    char *text;

    (void)button;
    if (!ctx->lesson->listening || ctx->busy)
        return;
    ctx->played = TRUE;
    ctx->busy = TRUE;
    text = (char *)ctx->lesson->listening;
#ifdef __APPLE__
    {
        char *tries[][8] = {
            {"say", "-v", "Samantha", "-r", "150", text, NULL},
            {"say", "-v", "Daniel", "-r", "150", text, NULL},
            {"say", "-r", "150", text, NULL},
        };
        for (int i = 0; i < 3; i++) {
            if (en_spawn(tries[i], &ctx->pid)) {
                g_child_watch_add(ctx->pid, en_voice_done, ctx);
                return;
            }
        }
    }
#else
    {
        char *a[] = {"espeak-ng", "-v", "en", "-s", "140", text, NULL};
        char *b[] = {"espeak", "-v", "en", "-s", "140", text, NULL};
        char *c[] = {"spd-say", "-l", "en", text, NULL};
        char **tries[] = {a, b, c};

        for (int i = 0; i < 3; i++) {
            if (en_spawn(tries[i], &ctx->pid)) {
                g_child_watch_add(ctx->pid, en_voice_done, ctx);
                return;
            }
        }
    }
#endif
    ctx->busy = FALSE;
    en_start_fallback(ctx);
}

static void en_check(GtkButton *button, gpointer data) {
    EnEx *ctx = data;
    int mcq_ok = 0;
    int gap_ok = 0;
    gboolean write_ok = TRUE;
    gboolean all;

    (void)button;
    if (ctx->lesson->listening && !ctx->played) {
        set_feedback(ctx->feedback, FALSE, tr("en_listen_first"));
        return;
    }

    for (guint i = 0; i < ctx->picks->len; i++) {
        EnPick *p = &g_array_index(ctx->picks, EnPick, i);
        int active = -1;

        for (int o = 0; o < p->q->n_options; o++) {
            gtk_widget_remove_css_class(GTK_WIDGET(p->tb[o]), "ok");
            gtk_widget_remove_css_class(GTK_WIDGET(p->tb[o]), "wrong");
            if (gtk_toggle_button_get_active(p->tb[o]))
                active = o;
        }
        if (active == p->q->correct) {
            mcq_ok++;
            if (active >= 0)
                gtk_widget_add_css_class(GTK_WIDGET(p->tb[active]), "ok");
        } else if (active >= 0) {
            gtk_widget_add_css_class(GTK_WIDGET(p->tb[active]), "wrong");
        }
        if (p->hint)
            gtk_widget_set_visible(p->hint, TRUE);
    }

    for (guint i = 0; i < ctx->entries->len; i++) {
        GtkWidget *entry = g_array_index(ctx->entries, GtkWidget *, i);
        const char *txt = gtk_editable_get_text(GTK_EDITABLE(entry));
        char *norm = normalize_answer(txt);
        gboolean ok = answer_accepts(norm, ctx->lesson->gaps[i].answers);

        gtk_widget_remove_css_class(entry, "answer-ok");
        gtk_widget_remove_css_class(entry, "answer-wrong");
        gtk_widget_add_css_class(entry, ok ? "answer-ok" : "answer-wrong");
        if (ok)
            gap_ok++;
        g_free(norm);
        if (i < ctx->gap_hints->len) {
            GtkWidget *hint = g_array_index(ctx->gap_hints, GtkWidget *, i);

            if (hint)
                gtk_widget_set_visible(hint, TRUE);
        }
    }

    if (ctx->essay && ctx->lesson->write_prompt) {
        GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ctx->essay));
        GtkTextIter a, b;
        char *raw;
        char *norm;
        gboolean keys = TRUE;

        gtk_text_buffer_get_bounds(buf, &a, &b);
        raw = gtk_text_buffer_get_text(buf, &a, &b, FALSE);
        norm = normalize_answer(raw);
        if (ctx->lesson->write_keys) {
            gchar **parts = g_strsplit(ctx->lesson->write_keys, "|", -1);

            for (int i = 0; parts[i]; i++) {
                if (parts[i][0] && !en_has_word(norm, parts[i]))
                    keys = FALSE;
            }
            g_strfreev(parts);
        }
        write_ok = keys && en_word_count(norm) >= ctx->lesson->write_min;
        g_free(norm);
        g_free(raw);
    }

    all = mcq_ok == (int)ctx->picks->len
          && gap_ok == ctx->lesson->n_gaps
          && write_ok;
    if (all) {
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        if (ctx->model)
            gtk_widget_set_visible(ctx->model, TRUE);
        mark_en_done(ctx->year, ctx->index);
    } else if (ctx->essay && !write_ok &&
               mcq_ok == (int)ctx->picks->len &&
               gap_ok == ctx->lesson->n_gaps) {
        set_feedback(ctx->feedback, FALSE, tr("en_write_short"));
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry_short"));
    }
}

static GtkWidget *build_en_ex_page(int year, int index) {
    const EnLesson *lesson = &en_lessons[year][index];
    EnEx *ctx = g_new0(EnEx, 1);
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *body;
    GtkWidget *check;

    ctx->year = year;
    ctx->index = index;
    ctx->lesson = lesson;
    ctx->picks = g_array_new(FALSE, FALSE, sizeof(EnPick));
    ctx->entries = g_array_new(FALSE, FALSE, sizeof(GtkWidget *));
    ctx->gap_hints = g_array_new(FALSE, TRUE, sizeof(GtkWidget *));

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);
    gtk_box_append(GTK_BOX(page),
                   top_bar(en_unit_page[year][index],
                           en_title_key[year][index], NULL));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 12);
    gtk_box_append(GTK_BOX(page), scroll);

    body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_hexpand(body, TRUE);
    gtk_widget_set_margin_bottom(body, 12);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), body);

    if (lesson->reading) {
        en_heading(body, lesson->n_read ? "en_sec_read" : "en_sec_read");
        en_passage(body, lesson->reading, FALSE);
        if (lesson->n_read)
            en_add_mcq(ctx, body, lesson->readq, lesson->read_hints, lesson->n_read);
    }

    if (lesson->listening) {
        GtkWidget *play;
        GtkWidget *note;

        en_heading(body, "en_sec_listen");
        note = gtk_label_new(NULL);
        i18n_bind(note, "en_listen_note", 0);
        gtk_label_set_wrap(GTK_LABEL(note), TRUE);
        gtk_label_set_xalign(GTK_LABEL(note), 0);
        gtk_widget_set_halign(note, GTK_ALIGN_START);
        gtk_widget_add_css_class(note, "quiz-intro");
        gtk_box_append(GTK_BOX(body), note);

        play = gtk_button_new();
        i18n_bind(play, "en_play", 1);
        gtk_widget_add_css_class(play, "btn-primary");
        gtk_widget_set_halign(play, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(body), play);
        g_signal_connect(play, "clicked", G_CALLBACK(en_play_clicked), ctx);

        ctx->play_note = gtk_label_new("");
        gtk_label_set_wrap(GTK_LABEL(ctx->play_note), TRUE);
        gtk_widget_set_halign(ctx->play_note, GTK_ALIGN_START);
        gtk_widget_add_css_class(ctx->play_note, "meaning");
        gtk_widget_set_visible(ctx->play_note, FALSE);
        gtk_box_append(GTK_BOX(body), ctx->play_note);

        ctx->script = en_passage(body, "", TRUE);
        if (lesson->n_listen)
            en_add_mcq(ctx, body, lesson->listenq, lesson->listen_hints,
                       lesson->n_listen);
    }

    if (lesson->n_gaps) {
        en_heading(body, "en_sec_gap");
        for (int i = 0; i < lesson->n_gaps; i++) {
            GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
            GtkWidget *prompt = gtk_label_new(NULL);
            GtkWidget *entry = gtk_entry_new();
            GtkWidget *hint = NULL;
            char *text = g_strdup_printf("%d.)  %s", i + 1, lesson->gaps[i].prompt);

            gtk_widget_add_css_class(card, "quiz-card");
            gtk_label_set_text(GTK_LABEL(prompt), text);
            g_free(text);
            gtk_label_set_wrap(GTK_LABEL(prompt), TRUE);
            gtk_label_set_xalign(GTK_LABEL(prompt), 0);
            gtk_widget_set_halign(prompt, GTK_ALIGN_START);
            gtk_box_append(GTK_BOX(card), prompt);
            gtk_editable_set_width_chars(GTK_EDITABLE(entry), 24);
            gtk_widget_set_halign(entry, GTK_ALIGN_START);
            gtk_box_append(GTK_BOX(card), entry);
            if (lesson->gaps[i].meaning)
                hint = meaning_add(card, lesson->gaps[i].meaning);
            gtk_box_append(GTK_BOX(body), card);
            g_array_append_val(ctx->entries, entry);
            g_array_append_val(ctx->gap_hints, hint);
        }
    }

    if (lesson->write_prompt) {
        GtkWidget *prompt;
        GtkWidget *view;

        en_heading(body, "en_sec_write");
        prompt = gtk_label_new(lesson->write_prompt);
        gtk_label_set_wrap(GTK_LABEL(prompt), TRUE);
        gtk_label_set_xalign(GTK_LABEL(prompt), 0);
        gtk_widget_set_halign(prompt, GTK_ALIGN_START);
        gtk_widget_add_css_class(prompt, "quiz-q");
        gtk_box_append(GTK_BOX(body), prompt);

        view = gtk_text_view_new();
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
        gtk_widget_set_size_request(view, -1, 140);
        gtk_widget_add_css_class(view, "quiz-card");
        gtk_box_append(GTK_BOX(body), view);
        ctx->essay = view;

        ctx->model = gtk_label_new(lesson->write_model);
        gtk_label_set_wrap(GTK_LABEL(ctx->model), TRUE);
        gtk_label_set_selectable(GTK_LABEL(ctx->model), TRUE);
        gtk_label_set_xalign(GTK_LABEL(ctx->model), 0);
        gtk_widget_set_halign(ctx->model, GTK_ALIGN_START);
        gtk_widget_add_css_class(ctx->model, "meaning");
        gtk_widget_set_visible(ctx->model, FALSE);
        gtk_box_append(GTK_BOX(body), ctx->model);
    }

    if (lesson->n_quiz) {
        en_heading(body, "en_sec_quiz");
        en_add_mcq(ctx, body, lesson->quiz, lesson->quiz_hints, lesson->n_quiz);
    }

    check = gtk_button_new();
    i18n_bind(check, "check", 1);
    gtk_widget_add_css_class(check, "btn-primary");
    gtk_widget_set_halign(check, GTK_ALIGN_START);
    gtk_widget_set_margin_top(check, 8);
    gtk_box_append(GTK_BOX(body), check);
    g_signal_connect(check, "clicked", G_CALLBACK(en_check), ctx);

    ctx->feedback = gtk_label_new("");
    gtk_widget_set_halign(ctx->feedback, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(ctx->feedback), TRUE);
    gtk_box_append(GTK_BOX(body), ctx->feedback);
    return page;
}

void add_en_pages(GtkStack *stack) {
    en_prepare();
    for (int y = 0; y < EN_YEARS; y++) {
        gtk_stack_add_named(stack, build_enmap_page(y), en_map_page[y]);
        for (int i = 0; i < EN_N; i++) {
            gtk_stack_add_named(stack, build_en_unit_page(y, i),
                                en_unit_page[y][i]);
            gtk_stack_add_named(stack, build_en_ex_page(y, i),
                                en_ex_page[y][i]);
        }
    }
}

void draw_uk_flag(GtkDrawingArea *area, cairo_t *cr,
                  int width, int height, gpointer data) {
    double cx = width / 2.0;
    double cy = height / 2.0;
    double r = MIN(width, height) / 2.0 - 2.0;
    double w = (double)width;
    double h = (double)height;

    (void)area;
    (void)data;
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_save(cr);
    cairo_arc(cr, cx, cy, r, 0.0, 2.0 * G_PI);
    cairo_clip(cr);

    cairo_set_source_rgb(cr, 0.01, 0.14, 0.40);
    cairo_paint(cr);

    cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_set_line_width(cr, w * 0.16);
    cairo_move_to(cr, 0, 0);
    cairo_line_to(cr, w, h);
    cairo_move_to(cr, w, 0);
    cairo_line_to(cr, 0, h);
    cairo_stroke(cr);

    cairo_set_source_rgb(cr, 0.80, 0.05, 0.15);
    cairo_set_line_width(cr, w * 0.055);
    cairo_move_to(cr, 0, 0);
    cairo_line_to(cr, w, h);
    cairo_move_to(cr, w, 0);
    cairo_line_to(cr, 0, h);
    cairo_stroke(cr);

    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_rectangle(cr, 0, h * 0.38, w, h * 0.24);
    cairo_rectangle(cr, w * 0.38, 0, w * 0.24, h);
    cairo_fill(cr);

    cairo_set_source_rgb(cr, 0.80, 0.05, 0.15);
    cairo_rectangle(cr, 0, h * 0.43, w, h * 0.14);
    cairo_rectangle(cr, w * 0.43, 0, w * 0.14, h);
    cairo_fill(cr);

    cairo_restore(cr);
}
