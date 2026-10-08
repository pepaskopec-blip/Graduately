#include "graduately.h"

/* Year 1 of Občanská nauka.
 * Thematic unit "Člověk v lidském společenství" from the SOV framework
 * (společenskovědní vzdělávání) and the MŠMT model syllabus, which schools
 * place in the first year. Years 2, 3 and 4 are open. */

#define ON_ENTRY(n) \
    { .n_slides = 2, .unit_page = "onunit" #n, .ex_page = "onex" #n }

NetLesson on_lessons[ON_LESSONS] = {
    ON_ENTRY(1),  ON_ENTRY(2),  ON_ENTRY(3),  ON_ENTRY(4),
    ON_ENTRY(5),  ON_ENTRY(6),  ON_ENTRY(7),  ON_ENTRY(8),
    ON_ENTRY(9),  ON_ENTRY(10), ON_ENTRY(11), ON_ENTRY(12),
    ON_ENTRY(13), ON_ENTRY(14), ON_ENTRY(15), ON_ENTRY(16),
    ON_ENTRY(17),
};

#undef ON_ENTRY

GtkWidget *on_scroll;
GtkWidget *on_fixed;
GtkWidget *on_rail;
GtkWidget *on_nodes[ON_UNITS];
GtkWidget *on_labels[ON_UNITS];
double on_cx[ON_UNITS];
double on_cy[ON_UNITS];
int on_cols = 1;
int on_rows = 1;
int on_cw = (int)(2.0 * ROAD_MX + (ON_UNITS - 1) * PATH_SPAC);
int on_ch = (int)(2.0 * ROAD_MY);
guint on_idle;
double on_last_avail = -1.0;
int on_last_cols = -1;
int on_last_rows = -1;
int on_last_cw = -1;
int on_last_ch = -1;
cairo_surface_t *on_rail_cache;
int on_cache_w;
int on_cache_h;

static void on_point(double t, double *ox, double *oy) {
    int k = (int)t;
    double u = t - (double)k;
    double u2, u3;
    double p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y;

    if (k < 0) { k = 0; u = 0.0; }
    if (k >= ON_UNITS - 1) { k = ON_UNITS - 2; u = 1.0; }

    p1x = on_cx[k];     p1y = on_cy[k];
    p2x = on_cx[k + 1]; p2y = on_cy[k + 1];
    if (k - 1 >= 0) { p0x = on_cx[k - 1]; p0y = on_cy[k - 1]; }
    else            { p0x = p1x - (p2x - p1x); p0y = p1y - (p2y - p1y); }
    if (k + 2 < ON_UNITS) { p3x = on_cx[k + 2]; p3y = on_cy[k + 2]; }
    else                  { p3x = p2x + (p2x - p1x); p3y = p2y + (p2y - p1y); }

    u2 = u * u;
    u3 = u2 * u;
    *ox = 0.5 * (2.0 * p1x + (-p0x + p2x) * u
                 + (2.0 * p0x - 5.0 * p1x + 4.0 * p2x - p3x) * u2
                 + (-p0x + 3.0 * p1x - 3.0 * p2x + p3x) * u3);
    *oy = 0.5 * (2.0 * p1y + (-p0y + p2y) * u
                 + (2.0 * p0y - 5.0 * p1y + 4.0 * p2y - p3y) * u2
                 + (-p0y + 3.0 * p1y - 3.0 * p2y + p3y) * u3);
}

static void on_path(cairo_t *cr, double t0, double t1) {
    const double steps_per_unit = 24.0;
    int n = (int)((t1 - t0) * steps_per_unit);
    double x, y;
    int s;

    if (n < 1)
        n = 1;
    on_point(t0, &x, &y);
    cairo_move_to(cr, x, y);
    for (s = 1; s <= n; s++) {
        on_point(t0 + (t1 - t0) * (double)s / (double)n, &x, &y);
        cairo_line_to(cr, x, y);
    }
}

static void on_geometry(double avail) {
    const double phase = 2.0 * G_PI * 1.7 / (double)(ON_UNITS - 1);
    int rows;
    int r, c, i;

    for (rows = 1; rows <= ON_UNITS; rows++) {
        int cols = (ON_UNITS + rows - 1) / rows;
        if (2.0 * ROAD_MX + (cols - 1) * PATH_SPAC <= avail + 1.0)
            break;
    }
    if (rows > ON_UNITS)
        rows = ON_UNITS;
    on_rows = rows;
    on_cols = (ON_UNITS + rows - 1) / rows;
    if (on_cols < 1)
        on_cols = 1;
    on_cw = (int)(2.0 * ROAD_MX + (on_cols - 1) * PATH_SPAC);
    on_ch = (int)(2.0 * ROAD_MY + (rows - 1) * ROAD_GAP);

    for (r = 0; r < rows; r++) {
        int base = r * on_cols;
        int len = MIN(on_cols, ON_UNITS - base);
        int fwd = (r % 2) == 0;
        for (c = 0; c < len; c++) {
            i = base + c;
            int cc = fwd ? c : (on_cols - 1 - c);
            on_cx[i] = ROAD_MX + cc * PATH_SPAC;
            on_cy[i] = ROAD_MY + r * ROAD_GAP
                       + ROAD_WAVE * sin((double)i * phase);
        }
    }
}

static void on_apply_layout(void) {
    if (!on_fixed)
        return;

    if (on_cols == on_last_cols && on_rows == on_last_rows &&
        on_cw == on_last_cw && on_ch == on_last_ch)
        return;

    on_last_cols = on_cols;
    on_last_rows = on_rows;
    on_last_cw = on_cw;
    on_last_ch = on_ch;
    if (on_rail_cache) {
        cairo_surface_destroy(on_rail_cache);
        on_rail_cache = NULL;
    }

    for (int i = 0; i < ON_UNITS; i++) {
        if (!on_nodes[i] || !on_labels[i])
            continue;
        gtk_fixed_move(GTK_FIXED(on_fixed), on_nodes[i],
                       (int)(on_cx[i] - NODE_SIZE / 2.0),
                       (int)(on_cy[i] - NODE_SIZE / 2.0));
        gtk_fixed_move(GTK_FIXED(on_fixed), on_labels[i],
                       (int)(on_cx[i] - (PATH_SPAC - 20.0) / 2.0),
                       (int)(on_cy[i] + NODE_SIZE / 2.0 + 10.0));
    }

    gtk_widget_set_size_request(on_fixed, on_cw, on_ch);
    gtk_widget_set_size_request(on_rail, on_cw, on_ch);
    gtk_fixed_move(GTK_FIXED(on_fixed), on_rail, 0, 0);
    gtk_widget_queue_draw(on_rail);
}

static void on_relayout(void) {
    GtkAdjustment *hadj;
    double avail;

    if (!on_scroll)
        return;
    hadj = gtk_scrolled_window_get_hadjustment(
        GTK_SCROLLED_WINDOW(on_scroll));
    avail = gtk_adjustment_get_page_size(hadj);
    if (avail < 1.0)
        return;
    if (fabs(avail - on_last_avail) < 1.0)
        return;
    on_last_avail = avail;
    on_geometry(avail);
    on_apply_layout();
}

static gboolean on_relayout_idle(gpointer data) {
    (void)data;
    on_idle = 0;
    on_relayout();
    return G_SOURCE_REMOVE;
}

static void on_relayout_later(void) {
    if (on_idle == 0)
        on_idle = g_idle_add(on_relayout_idle, NULL);
}

static void on_adjust_notify(GtkAdjustment *adj, GParamSpec *ps,
                             gpointer data) {
    (void)adj;
    (void)ps;
    (void)data;
    on_relayout_later();
}

static void on_render_rail_to(cairo_t *cr) {
    const double t_end = (double)(ON_UNITS - 1);
    const Rgb rail = color_from_hex(app_theme.rail);
    const Rgb muted = color_from_hex(app_theme.subtext);

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

    cairo_new_path(cr);
    on_path(cr, 0.0, t_end);
    cairo_set_line_width(cr, 12);
    cairo_set_source_rgb(cr, rail.r, rail.g, rail.b);
    cairo_stroke(cr);

    cairo_set_source_rgba(cr, muted.r, muted.g, muted.b, 0.17);
    cairo_set_dash(cr, (double[]){2.0, 40.0}, 2, 0.0);
    cairo_new_path(cr);
    on_path(cr, 0.0, t_end);
    cairo_stroke(cr);
    cairo_set_dash(cr, NULL, 0, 0.0);
}

static void on_draw_rail(GtkDrawingArea *area, cairo_t *cr,
                         int width, int height, gpointer data) {
    (void)area;
    (void)width;
    (void)height;
    (void)data;

    if (!on_rail_cache || on_cache_w != on_cw || on_cache_h != on_ch) {
        cairo_t *rcr;

        if (on_rail_cache)
            cairo_surface_destroy(on_rail_cache);
        on_cache_w = on_cw;
        on_cache_h = on_ch;
        if (on_cache_w < 1)
            on_cache_w = 1;
        if (on_cache_h < 1)
            on_cache_h = 1;
        on_rail_cache = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                    on_cache_w, on_cache_h);
        rcr = cairo_create(on_rail_cache);
        on_render_rail_to(rcr);
        cairo_destroy(rcr);
    }

    cairo_set_source_surface(cr, on_rail_cache, 0, 0);
    cairo_paint(cr);
}

void on_rail_theme_reset(void) {
    if (on_rail_cache) {
        cairo_surface_destroy(on_rail_cache);
        on_rail_cache = NULL;
    }
    if (on_rail)
        gtk_widget_queue_draw(on_rail);
}

static void on_open_unit(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L)
        return;
    L->idx = 0;
    if (L->stack) {
        char name[16];

        g_snprintf(name, sizeof(name), "on_slide%u", L->idx);
        gtk_stack_set_visible_child_name(GTK_STACK(L->stack), name);
        if (L->prev_btn)
            gtk_widget_set_sensitive(L->prev_btn, FALSE);
        if (L->next_btn)
            gtk_button_set_label(GTK_BUTTON(L->next_btn), tr("net_slide_next"));
    }
    show_page(L->unit_page, 1);
}

static void on_add_node(GtkFixed *fixed, int index) {
    int num = index + 1;
    gboolean locked = index >= ON_LESSONS;
    GtkWidget *card;
    GtkWidget *vbox;
    GtkWidget *number;
    GtkWidget *icon;
    GtkWidget *name;
    char *text;
    static char unit_keys[ON_LESSONS][16];
    static gboolean unit_keys_ready = FALSE;

    if (!unit_keys_ready) {
        for (int i = 0; i < ON_LESSONS; i++)
            g_snprintf(unit_keys[i], sizeof unit_keys[i], "on_unit%d", i + 1);
        unit_keys_ready = TRUE;
    }

    card = gtk_button_new();
    gtk_widget_add_css_class(card, "unit-node");
    gtk_widget_set_can_focus(card, FALSE);
    gtk_widget_set_sensitive(card, !locked);
    gtk_widget_set_size_request(card, (int)NODE_SIZE, (int)NODE_SIZE);

    if (locked)
        gtk_widget_add_css_class(card, "locked");
    else
        gtk_widget_add_css_class(card, "current");

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
    gtk_button_set_child(GTK_BUTTON(card), vbox);

    text = g_strdup_printf("%d", num);
    number = gtk_label_new(text);
    g_free(text);
    gtk_widget_add_css_class(number, "unit-number");
    gtk_box_append(GTK_BOX(vbox), number);

    if (locked) {
        icon = icon_area_new(draw_lock_icon, 0.3451, 0.3569, 0.4392, 16);
        gtk_box_append(GTK_BOX(vbox), icon);
    } else {
        icon = icon_area_new(draw_check_icon, 0.1176, 0.1176, 0.1804, 18);
        gtk_widget_set_visible(icon, FALSE);
        gtk_box_append(GTK_BOX(vbox), icon);
        on_lessons[index].done_icon = icon;
        g_signal_connect(card, "clicked", G_CALLBACK(on_open_unit),
                         &on_lessons[index]);
    }

    on_nodes[index] = card;
    gtk_fixed_put(fixed, card, 0, 0);

    name = gtk_label_new(NULL);
    gtk_widget_set_size_request(name, (int)(PATH_SPAC - 20.0), -1);
    gtk_widget_set_halign(name, GTK_ALIGN_CENTER);
    gtk_label_set_justify(GTK_LABEL(name), GTK_JUSTIFY_CENTER);
    gtk_label_set_wrap(GTK_LABEL(name), TRUE);
    gtk_widget_add_css_class(name, "unit-name");
    if (index < ON_LESSONS) {
        i18n_bind(name, unit_keys[index], 0);
    } else {
        gtk_widget_add_css_class(name, "unit-name-locked");
    }
    on_labels[index] = name;
    gtk_fixed_put(fixed, name, 0, 0);
}

static GtkWidget *on_year_button(int year, const char *target) {
    static const char *year_keys[] = {
        "on_year1", "on_year2", "on_year3", "on_year4",
    };
    gboolean locked = target == NULL;
    GtkWidget *btn;
    GtkWidget *row;
    GtkWidget *num;
    GtkWidget *texts;
    GtkWidget *title;
    char *num_text;

    btn = gtk_button_new();
    gtk_widget_add_css_class(btn, "unit-node");
    gtk_widget_set_can_focus(btn, FALSE);
    gtk_widget_set_hexpand(btn, TRUE);
    gtk_widget_set_size_request(btn, -1, 72);
    gtk_widget_set_sensitive(btn, !locked);
    if (locked)
        gtk_widget_add_css_class(btn, "locked");
    else
        gtk_widget_add_css_class(btn, "current");

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
    i18n_bind(title, year_keys[year - 1], 0);
    gtk_widget_set_halign(title, GTK_ALIGN_START);
    gtk_widget_add_css_class(title, "unit-name");
    gtk_box_append(GTK_BOX(texts), title);

    if (locked) {
        GtkWidget *sub = gtk_label_new(NULL);
        GtkWidget *icon = icon_area_new(draw_lock_icon, 0.3451, 0.3569,
                                        0.4392, 18);

        i18n_bind(sub, "on_year_locked_sub", 0);
        gtk_widget_set_halign(sub, GTK_ALIGN_START);
        gtk_widget_add_css_class(sub, "unit-name-locked");
        gtk_box_append(GTK_BOX(texts), sub);
        gtk_widget_set_valign(icon, GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(row), icon);
    } else {
        g_object_set_data_full(G_OBJECT(btn), "target",
                               g_strdup(target), g_free);
        g_signal_connect(btn, "clicked", G_CALLBACK(on_nav_clicked), NULL);
    }

    return btn;
}

GtkWidget *build_onyears_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *list;
    int y;
    static const char *targets[] = {"onmap", "on2map", "on3map", "on4map"};

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("subjects", "Občanská nauka", "on_years_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 18);
    gtk_box_append(GTK_BOX(page), scroll);

    list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_halign(list, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(list, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(list, 360, -1);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list);

    for (y = 1; y <= 4; y++)
        gtk_box_append(GTK_BOX(list), on_year_button(y, targets[y - 1]));

    return page;
}

GtkWidget *build_onmap_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *fixed;
    GtkWidget *rail;
    GtkAdjustment *ha;
    GtkAdjustment *va;

    on_geometry(1000.0);

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("onyears", "on_year1", "on_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);
    on_scroll = scroll;

    wrap = gtk_center_box_new();
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), wrap);

    fixed = gtk_fixed_new();
    gtk_widget_set_halign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(fixed, on_cw, on_ch);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), fixed);
    on_fixed = fixed;

    rail = gtk_drawing_area_new();
    gtk_widget_set_size_request(rail, on_cw, on_ch);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(rail), on_draw_rail,
                                   NULL, NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rail, 0, 0);
    on_rail = rail;

    for (int i = 0; i < ON_UNITS; i++)
        on_add_node(GTK_FIXED(fixed), i);

    ha = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scroll));
    va = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));
    g_signal_connect(ha, "notify::page-size",
                     G_CALLBACK(on_adjust_notify), NULL);
    g_signal_connect(va, "notify::page-size",
                     G_CALLBACK(on_adjust_notify), NULL);

    on_apply_layout();
    on_relayout_later();
    refresh_on_completion_ui();

    return page;
}

static void on_slide_apply(NetLesson *L);
static void on_slide_prev(GtkButton *button, gpointer data);
static void on_slide_next(GtkButton *button, gpointer data);

static int on_note_last_w = -1;

static void on_rescale_notes_at(int w) {
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
    for (int i = 0; i < ON_LESSONS; i++)
        net_rescale_lesson_notes(&on_lessons[i], body, head, kick);
}

static gboolean on_note_tick(GtkWidget *w, GdkFrameClock *clock,
                             gpointer data) {
    int width = gtk_widget_get_allocated_width(w);

    (void)clock;
    (void)data;
    if (width != on_note_last_w) {
        on_note_last_w = width;
        on_rescale_notes_at(width);
    }
    return G_SOURCE_CONTINUE;
}

static GtkWidget *build_on_unit_page(NetLesson *L, const char *title_key,
                                     const char *sub_key,
                                     const NetSlide *slides, guint n_slides) {
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

    gtk_box_append(GTK_BOX(page), top_bar("onmap", title_key, sub_key));

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
        g_snprintf(name, sizeof(name), "on_slide%u", s);
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

    gtk_widget_add_tick_callback(L->stack, on_note_tick, NULL, NULL);
    on_note_last_w = -1;
    on_rescale_notes_at(0);
    net_notes_target = NULL;

    nav = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(nav), TRUE);
    gtk_widget_set_margin_top(nav, 10);
    gtk_box_append(GTK_BOX(page), nav);

    L->prev_btn = gtk_button_new_with_label(tr("net_slide_prev"));
    gtk_widget_add_css_class(L->prev_btn, "btn-primary");
    g_signal_connect(L->prev_btn, "clicked", G_CALLBACK(on_slide_prev), L);
    gtk_box_append(GTK_BOX(nav), L->prev_btn);

    L->next_btn = gtk_button_new_with_label(tr("net_slide_next"));
    gtk_widget_add_css_class(L->next_btn, "btn-primary");
    g_signal_connect(L->next_btn, "clicked", G_CALLBACK(on_slide_next), L);
    gtk_box_append(GTK_BOX(nav), L->next_btn);

    L->idx = 0;
    on_slide_apply(L);
    return page;
}

static void on_slide_apply(NetLesson *L) {
    char name[16];

    if (!L || !L->stack)
        return;
    g_snprintf(name, sizeof(name), "on_slide%u", L->idx);
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

static void on_slide_prev(GtkButton *button, gpointer data) {
    NetLesson *L = data;
    (void)button;
    if (!L) return;
    if (L->idx > 0) {
        L->idx--;
        on_slide_apply(L);
    }
}

static void on_slide_next(GtkButton *button, gpointer data) {
    NetLesson *L = data;
    (void)button;
    if (!L) return;
    if (L->idx + 1 < L->n_slides) {
        L->idx++;
        on_slide_apply(L);
    } else {
        show_page(L->ex_page, 1);
    }
}

void on_lessons_apply_lang(void) {
    for (int i = 0; i < ON_LESSONS; i++)
        on_slide_apply(&on_lessons[i]);
}

static void on_mcq_check(GtkButton *button, gpointer data) {
    NetMcqCtx *ctx = data;
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
        mark_on_done(ctx->lesson_id);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry_short"));
    }
}

static GtkWidget *build_on_mcq_page(int lesson_id, const char *back_page,
                                    const char *title_key,
                                    const char *heading_key,
                                    const ChoiceQ *qs, const char **hints,
                                    int n) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *body;
    GtkWidget *check;
    GtkWidget *btns;
    GtkWidget *head;
    GtkWidget *intro;
    NetMcqCtx *ctx = g_new0(NetMcqCtx, 1);
    int n_opts = qs[0].n_options;

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
    g_signal_connect(check, "clicked", G_CALLBACK(on_mcq_check), ctx);

    ctx->feedback = gtk_label_new("");
    gtk_widget_set_halign(ctx->feedback, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(body), ctx->feedback);

    return page;
}

static GtkWidget *build_on_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Smysl", "Proč se učíme občanskou nauku",
            "Tip: občanem se člověk stává jednáním, ne jen dokladem.",
            {
                "Předmět připravuje na aktivní a odpovědný život v demokratické společnosti.",
                "Směřuje k tomu, aby člověk jednal slušně a nejen pro vlastní prospěch.",
                "Učí kriticky myslet, nenechat se manipulovat a rozumět světu kolem sebe.",
                "Navazuje na občanskou výchovu ze základní školy a prohlubuje ji.",
                NULL,
            },
        },
        {
            "2 / 2   •   Ročník", "Co patří do prvního ročníku",
            "Tip: název celku je Člověk v lidském společenství.",
            {
                "První ročník bere osobnost, učení, vztahy, zdraví a sociální skupiny.",
                "Patří sem i společnost, kultura, soužití lidí a náboženství.",
                "Součástí je i hospodaření domácnosti a sociální zajištění.",
                "Politický systém, právo a filozofie přicházejí v dalších ročnících.",
                NULL,
            },
        },
    };

    return build_on_unit_page(&on_lessons[0], "on_unit1", "on_unit1_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on_unit1_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jaký je hlavní cíl občanské nauky?",
         {
          "Naučit se nazpaměť všechny zákony",
          "Připravit na odpovědný život v demokratické společnosti",
          "Nahradit dějepis",
          "Naučit účetnictví firmy",
         },
         4, 1},
        {"Na co výuka navazuje?",
         {
          "Jen na odborný výcvik",
          "Na občanskou výchovu ze základní školy",
          "Jen na matematiku",
          "Na tělesnou výchovu",
         },
         4, 1},
        {"Který tematický celek patří do 1. ročníku?",
         {
          "Člověk a trestní právo",
          "Člověk v lidském společenství",
          "Dějiny filozofie",
          "Hospodářská politika státu",
         },
         4, 1},
        {"Co má žák díky předmětu umět?",
         {
          "Kriticky myslet a nenechat se manipulovat",
          "Ignorovat názory druhých",
          "Řídit se jen vlastní výhodou",
          "Vyhýbat se každé diskusi",
         },
         4, 0},
    };
    static const char *hints[] = {
        "Cíl je odpovědný život v demokracii",
        "Navazuje na občanskou výchovu ze ZŠ",
        "1. ročník = Člověk v lidském společenství",
        "Kritické myšlení je součást předmětu",
    };
    return build_on_mcq_page(0, "onunit1", "on_ex1_title",
                             "on_quiz1_head", qs, hints, 4);
}


static GtkWidget *build_on_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Učení", "Celoživotní vzdělávání",
            "Tip: škola je začátek učení, ne jeho konec.",
            {
                "Vzdělání nekončí maturitou. Člověk se učí celý život.",
                "Učení pomáhá zvládat změny v práci i v osobním životě.",
                "Patří sem škola, samostudium i zkušenost z praxe.",
                "Smyslem je rozumět světu a umět se rozhodovat.",
                NULL,
            },
        },
        {
            "2 / 2   •   Čas", "Volný čas",
            "Tip: odpočinek je součást učení, ne jeho opak.",
            {
                "Volný čas je prostor po povinnostech, ne jen nicnedělání.",
                "Dá se využít k odpočinku, koníčkům, vztahům i dalšímu učení.",
                "Pasivní konzumace, třeba nekonečné scrollování, volný čas spíš spolkne.",
                "Střídání učení a odpočinku chrání soustředění.",
                NULL,
            },
        },
    };

    return build_on_unit_page(&on_lessons[1], "on_unit2", "on_unit2_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on_unit2_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co znamená celoživotní vzdělávání?",
         {
          "Že se člověk učí i po škole",
          "Že maturita stačí navždy",
          "Že se učí jen ve škole",
          "Že se učí až v důchodu",
         },
         4, 0},
        {"Co je volný čas?",
         {
          "Čas po splnění povinností",
          "Jen hodiny tělocviku",
          "Čas, kdy se nesmí odpočívat",
          "Povinná brigáda",
         },
         4, 0},
        {"Proč nestačí trávit volný čas jen pasivně u obrazovky?",
         {
          "Odpočinek a vlastní aktivita tím mizí",
          "Zákon to vždycky zakazuje",
          "Obrazovka ruší elektřinu v domě",
          "Člověk se tím automaticky naučí jazyk",
         },
         4, 0},
        {"Jak spolu souvisí učení a odpočinek?",
         {
          "Odpočinek učení vždycky kazí",
          "Střídání učení a odpočinku pomáhá soustředění",
          "Učit se má člověk jen v noci",
          "Odpočinek patří jen do prázdnin",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Učení pokračuje i po škole",
        "Volný čas je po povinnostech",
        "Pasivní konzumace volný čas spolkne",
        "Odpočinek soustředění podporuje",
    };
    return build_on_mcq_page(1, "onunit2", "on_ex2_title",
                             "on_quiz2_head", qs, hints, 4);
}


static GtkWidget *build_on_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Složky", "Z čeho se osobnost skládá",
            "Tip: osobnost není hotová nálepka, vyvíjí se.",
            {
                "Osobnost má tělesnou i duševní stránku a obě se ovlivňují.",
                "Patří sem temperament, charakter, schopnosti a motivace.",
                "Temperament je spíš vrozený styl reagování, charakter se utváří.",
                "Schopnosti se dají cvičit a motivace dává jednání směr.",
                NULL,
            },
        },
        {
            "2 / 2   •   Druzí", "Socializace a empatie",
            "Tip: empatie znamená představit si, jaké to je být na místě druhého.",
            {
                "Socializace je proces, kterým se člověk stává členem společnosti.",
                "Učí se v rodině, ve škole, mezi vrstevníky i z médií.",
                "Sebepoznání pomáhá jednat s druhými s empatií.",
                "Porozumění sobě je základ slušného jednání s ostatními.",
                NULL,
            },
        },
    };

    return build_on_unit_page(&on_lessons[2], "on_unit3", "on_unit3_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on_unit3_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je temperament?",
         {
          "Naučený seznam pravidel",
          "Spíše vrozený styl reagování",
          "Školní známka",
          "Druh povolání",
         },
         4, 1},
        {"Co je socializace?",
         {
          "Proces, kterým se člověk začleňuje do společnosti",
          "Pouze lékařské vyšetření",
          "Stěhování do jiného státu",
          "Trest za přestupek",
         },
         4, 0},
        {"Čím se charakter liší od temperamentu?",
         {
          "Charakter se utváří, temperament je spíš vrozený",
          "Jsou to přesná synonyma",
          "Charakter je jen barva vlasů",
          "Temperament se nedá nijak popsat",
         },
         4, 0},
        {"K čemu je empatie?",
         {
          "Aby člověk druhého přehlížel",
          "Aby si dovedl představit situaci druhého",
          "Aby vyhrál každou hádku",
          "Aby se vyhnul všem vztahům",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Temperament je spíš vrozený styl",
        "Socializace = začlenění do společnosti",
        "Charakter se utváří",
        "Empatie je pohled očima druhého",
    };
    return build_on_mcq_page(2, "onunit3", "on_ex3_title",
                             "on_quiz3_head", qs, hints, 4);
}


static GtkWidget *build_on_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Etapy", "Od dětství ke stáří",
            "Tip: etapy se překrývají, nejsou to ostré čáry.",
            {
                "Život má etapy: dětství, dospívání, dospělost a stáří.",
                "Každá etapa má typické úkoly i možnosti.",
                "Dospívání je čas hledání identity a větší samostatnosti.",
                "Stáří není jen úbytek sil, přináší i zkušenost.",
                NULL,
            },
        },
        {
            "2 / 2   •   Generace", "Mezigenerační vztahy",
            "Tip: respekt platí z obou stran, ne jen směrem nahoru.",
            {
                "Generace se navzájem potřebují, i když mají jiné zkušenosti.",
                "Děti a rodiče mají vůči sobě práva i povinnosti.",
                "Respekt ke starším neznamená vzdát se vlastního názoru.",
                "Náročné situace, třeba nemoc nebo ztráta, se v rodině řeší společně.",
                NULL,
            },
        },
    };

    return build_on_unit_page(&on_lessons[3], "on_unit4", "on_unit4_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on_unit4_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Která etapa je typická hledáním identity?",
         {
          "Kojenecké období",
          "Dospívání",
          "Jen stáří",
          "Jen předškolní věk",
         },
         4, 1},
        {"Co platí o mezigeneračních vztazích?",
         {
          "Mladší a starší se nepotřebují",
          "Generace se mohou doplňovat zkušeností",
          "Respekt znamená nemít vlastní názor",
          "Povinnosti mají jen děti",
         },
         4, 1},
        {"Jak chápat etapy života?",
         {
          "Jako pevné přihrádky beze změn",
          "Jako období s typickými úkoly, která se mohou překrývat",
          "Jako trest",
          "Jako jen školní ročníky",
         },
         4, 1},
        {"Co stáří přináší vedle omezení?",
         {
          "Jen samé ztráty",
          "Zkušenost a často i nadhled",
          "Povinnost odejít ze společnosti",
          "Konec všech vztahů",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Identita se hledá hlavně v dospívání",
        "Generace se doplňují",
        "Etapy se překrývají",
        "Stáří nese i zkušenost",
    };
    return build_on_mcq_page(3, "onunit4", "on_ex4_title",
                             "on_quiz4_head", qs, hints, 4);
}


static GtkWidget *build_on_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Zátěž", "Náročné životní situace",
            "Tip: říct si o pomoc není ostuda.",
            {
                "Patří sem stres, konflikt, nemoc, ztráta nebo velká změna.",
                "Krátký stres může pomoct podat výkon, dlouhý škodí.",
                "Není slabost přiznat, že situace přerůstá síly.",
                "Pomoc se hledá u blízkých, ve škole i u odborníků.",
                NULL,
            },
        },
        {
            "2 / 2   •   Péče", "Psychohygiena",
            "Tip: duševní zdraví je součást zdraví, ne luxus.",
            {
                "Psychohygiena je péče o duševní zdraví.",
                "Patří sem spánek, pohyb, vztahy a umění říct ne.",
                "Pomáhá režim dne a čas bez neustálých oznámení.",
                "Když potíže trvají, je na místě odborná pomoc.",
                NULL,
            },
        },
    };

    return build_on_unit_page(&on_lessons[4], "on_unit5", "on_unit5_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on_unit5_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je psychohygiena?",
         {
          "Péče o duševní zdraví",
          "Jen čištění zubů",
          "Trest za stres",
          "Druh diety",
         },
         4, 0},
        {"Kdy je v pořádku hledat pomoc?",
         {
          "Nikdy, člověk si má poradit sám",
          "Když náročná situace přerůstá síly",
          "Jen po dosažení plnoletosti",
          "Jen když to nařídí soud",
         },
         4, 1},
        {"Co dlouhodobý stres dělá?",
         {
          "Vždycky zlepšuje zdraví",
          "Může škodit tělu i psychice",
          "Nemá žádný vliv",
          "Nahrazuje spánek",
         },
         4, 1},
        {"Co do psychohygieny patří?",
         {
          "Spánek, pohyb a hranice",
          "Jen energetické nápoje",
          "Nevyspání před zkouškou",
          "Izolace od všech lidí",
         },
         4, 0},
    };
    static const char *hints[] = {
        "Psychohygiena = péče o duševní zdraví",
        "Pomoc je na místě, když síly nestačí",
        "Dlouhý stres škodí",
        "Spánek, pohyb a hranice",
    };
    return build_on_mcq_page(4, "onunit5", "on_ex5_title",
                             "on_quiz5_head", qs, hints, 4);
}


static GtkWidget *build_on_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Etiketa", "Pravidla slušného chování",
            "Tip: slušnost se pozná tam, kde z ní nic nekápne.",
            {
                "Slušné chování je způsob, jak spolu lidé vycházejí bez ponižování.",
                "Patří sem pozdrav, poděkování, omluva a dochvilnost.",
                "Etiketa se liší situací: škola, úřad, divadlo nebo doprava.",
                "Zdvořilost není slabost ani předstírání.",
                NULL,
            },
        },
        {
            "2 / 2   •   Vztahy", "Kvalita mezilidských vztahů",
            "Tip: souhlas se dá kdykoli odvolat.",
            {
                "Vztahy potřebují respekt, důvěru a čas.",
                "Patří sem rodina, přátelství i partnerský vztah.",
                "Partnerský vztah stojí na souhlasu a respektu k hranicím.",
                "Špatný vztah není povinnost vydržet za každou cenu.",
                NULL,
            },
        },
    };

    return build_on_unit_page(&on_lessons[5], "on_unit6", "on_unit6_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on_unit6_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je etiketa?",
         {
          "Seznam trestů",
          "Zásady slušného chování v dané situaci",
          "Jen oblečení na ples",
          "Povinnost souhlasit s každým",
         },
         4, 1},
        {"Na čem stojí partnerský vztah?",
         {
          "Na nátlaku",
          "Na respektu a souhlasu",
          "Na tom, že jeden rozhoduje o všem",
          "Na tajnostech před rodinou",
         },
         4, 1},
        {"Kdy se slušnost pozná nejlíp?",
         {
          "Když z ní člověk něco má",
          "I tam, kde z ní nic nekápne",
          "Jen před učitelem",
          "Jen na internetu",
         },
         4, 1},
        {"Platí, že souhlas se dá vzít zpět?",
         {
          "Ne, jednou stačí navždy",
          "Ano, souhlas se dá odvolat",
          "Jen písemně u notáře",
          "Jen se souhlasem skupiny",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Etiketa = slušné chování podle situace",
        "Vztah stojí na respektu a souhlasu",
        "Slušnost i tam, kde z ní nic není",
        "Souhlas se dá odvolat",
    };
    return build_on_mcq_page(5, "onunit6", "on_ex6_title",
                             "on_quiz6_head", qs, hints, 4);
}


static GtkWidget *build_on_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Řeč", "Verbální a neverbální komunikace",
            "Tip: já-věta popisuje pocit, aniž by hned útočila.",
            {
                "Komunikace je verbální i neverbální: výraz, postoj a tón.",
                "Aktivní naslouchání znamená opravdu slyšet, ne jen čekat na slovo.",
                "Nedorozumění často vznikne z domněnky, ne ze zlého úmyslu.",
                "V online textu chybí tón, proto zpráva snadno urazí.",
                NULL,
            },
        },
        {
            "2 / 2   •   Střet", "Řešení konfliktů",
            "Tip: kompromis není prohra, když ho unesou obě strany.",
            {
                "Konflikt je střet zájmů nebo názorů, ne automaticky násilí.",
                "Dá se řešit domluvou, kompromisem nebo pomocí třetí strany.",
                "Vyhrát za každou cenu vztah často zničí.",
                "Násilí, vydírání a ponižování nejsou řešení konfliktu.",
                NULL,
            },
        },
    };

    return build_on_unit_page(&on_lessons[6], "on_unit7", "on_unit7_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on_unit7_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je neverbální komunikace?",
         {
          "Jen psaný dopis",
          "Výraz, postoj a tón",
          "Pouze zákon",
          "Školní řád",
         },
         4, 1},
        {"Co je konflikt?",
         {
          "Vždycky rvačka",
          "Střet zájmů nebo názorů",
          "Jen mezinárodní válka",
          "Ticho",
         },
         4, 1},
        {"Proč online text snadno urazí?",
         {
          "Chybí v něm tón a výraz",
          "Internet zakazuje slušnost",
          "Písmo je vždycky ironické",
          "Zprávy čte jen učitel",
         },
         4, 0},
        {"Co není řešení konfliktu?",
         {
          "Kompromis",
          "Domluva",
          "Násilí a ponižování",
          "Pomoc třetí strany",
         },
         4, 2},
    };
    static const char *hints[] = {
        "Neverbální = výraz, postoj, tón",
        "Konflikt je střet, ne automaticky násilí",
        "Bez tónu se text snáz urazí",
        "Násilí konflikt neřeší",
    };
    return build_on_mcq_page(6, "onunit7", "on_ex7_title",
                             "on_quiz7_head", qs, hints, 4);
}


static GtkWidget *build_on_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Zdraví", "Tělo i psychika",
            "Tip: zdraví není jen nepřítomnost rýmy.",
            {
                "Zdraví je tělesná i duševní pohoda, nejen absence nemoci.",
                "Ovlivňuje ho životní styl, vztahy i prostředí.",
                "Člověk má odpovědnost za své návyky, ne za všechno, co ho postihne.",
                "Prevence je spolehlivější než dohánět následky.",
                NULL,
            },
        },
        {
            "2 / 2   •   Návyky", "Životní styl",
            "Tip: životní styl je souhrn návyků, ne jednorázové předsevzetí.",
            {
                "Patří sem spánek, pohyb, strava a vztahy.",
                "Rizikový životní styl zdraví oslabuje postupně.",
                "Odpočinek a režim dne jsou součást stylu, ne odměna na konci.",
                "Když něco nejde, je v pořádku říct si o pomoc.",
                NULL,
            },
        },
    };

    return build_on_unit_page(&on_lessons[7], "on_unit8", "on_unit8_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on_unit8_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak se ve škole bere zdraví?",
         {
          "Jen jako absence rýmy",
          "Jako tělesná i duševní pohoda",
          "Jen jako sportovní výkon",
          "Jako školní předmět chemie",
         },
         4, 1},
        {"Co je životní styl?",
         {
          "Jedno předsevzetí na Nový rok",
          "Souhrn návyků, kterými člověk žije",
          "Jen jídelníček",
          "Povinná dieta",
         },
         4, 1},
        {"K čemu je prevence?",
         {
          "Aby se následky doháněly později",
          "Aby se problémům předcházelo",
          "Aby se zrušila lékařská péče",
          "Aby se k lékaři nechodilo nikdy",
         },
         4, 1},
        {"Platí, že za každou nemoc může jen vlastní styl?",
         {
          "Ano, vždy",
          "Ne, styl zdraví ovlivňuje, ale ne všechno člověk ovlivní",
          "Nemoc neexistuje",
          "Styl s tím nesouvisí vůbec",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Zdraví je tělesné i duševní",
        "Styl = souhrn návyků",
        "Prevence předchází následkům",
        "Styl ovlivňuje, ale neurčuje vše",
    };
    return build_on_mcq_page(7, "onunit8", "on_ex8_title",
                             "on_quiz8_head", qs, hints, 4);
}


static GtkWidget *build_on_unit9_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Formy", "Co je závislost",
            "Tip: závislost se týká i věcí, které nejsou zakázaná látka.",
            {
                "Závislost je nutkavá potřeba, která přebírá řízení života.",
                "Patří sem alkohol, nikotin, jiné drogy, hazard i digitální závislost.",
                "Nejde jen o slabou vůli. Mění se i odměňování v mozku.",
                "Ohrožuje zdraví, vztahy, školu i peníze.",
                NULL,
            },
        },
        {
            "2 / 2   •   Pomoc", "Proč je to nebezpečné a kam se obrátit",
            "Tip: první krok je pojmenovat problém, ne ho skrývat.",
            {
                "Pro společnost znamená závislost i náklady, násilí a zátěž rodin.",
                "Prevence je informace, hranice a prostředí, kde se dá říct ne.",
                "Pomoc nabízejí odborné služby, linky pomoci a školní poradenství.",
                "Čím dřív se člověk ozve, tím snáz se závislost brzdí.",
                NULL,
            },
        },
    };

    return build_on_unit_page(&on_lessons[8], "on_unit9", "on_unit9_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on_unit9_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je závislost?",
         {
          "Občasná chuť",
          "Nutkavá potřeba, která přebírá řízení života",
          "Jen nelegální droga",
          "Každý sportovní návyk",
         },
         4, 1},
        {"Které z uvedeného může být závislost?",
         {
          "Jen heroin",
          "I hazard a nadužívání digitálních médií",
          "Jedna káva denně",
          "Čtení knih",
         },
         4, 1},
        {"Proč závislost není jen slabá vůle?",
         {
          "Zasahuje i odměňování v mozku",
          "Protože vůle neexistuje",
          "Protože ji způsobuje jen škola",
          "Protože ji léčí trest",
         },
         4, 0},
        {"Kam se obrátit?",
         {
          "Nikam",
          "Na odbornou pomoc, linku nebo školní poradenství",
          "Na někoho, kdo látku taky bere a radí v tom pokračovat",
          "Na náhodnou reklamu",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Závislost přebírá řízení života",
        "I hazard a digitální nadužívání",
        "Mění se odměňování v mozku",
        "Pomoc je linka, odborník, škola",
    };
    return build_on_mcq_page(8, "onunit9", "on_ex9_title",
                             "on_quiz9_head", qs, hints, 4);
}


static GtkWidget *build_on_unit10_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Skupiny", "Primární a sekundární skupiny",
            "Tip: skupina člověka formuje, i když si to nepřizná.",
            {
                "Sociální skupina jsou lidé spojení vztahem a pravidly.",
                "Primární skupiny, třeba rodina a blízcí přátelé, jsou osobní.",
                "Sekundární, třeba třída, práce nebo spolek, jsou účelovější.",
                "Člověk je zároveň členem více skupin.",
                NULL,
            },
        },
        {
            "2 / 2   •   Rodina", "Rodina a její funkce",
            "Tip: důležitější než jedna šablona je, jestli rodina své funkce plní.",
            {
                "Rodina je základní sociální útvar a první místo socializace.",
                "Má funkci citovou, výchovnou, ekonomickou i ochrannou.",
                "Rodina nemusí mít jen jednu podobu, aby tyhle funkce plnila.",
                "Práva a povinnosti mezi rodiči a dětmi nejsou jednosměrné.",
                NULL,
            },
        },
    };

    return build_on_unit_page(&on_lessons[9], "on_unit10", "on_unit10_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on_unit10_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je primární skupina?",
         {
          "Anonymní dav na koncertě",
          "Blízká skupina, třeba rodina",
          "Celý stát",
          "Fronta v obchodě",
         },
         4, 1},
        {"Která funkce rodině patří?",
         {
          "Vydávat zákony",
          "Citová a výchovná",
          "Řídit armádu",
          "Stanovit daně",
         },
         4, 1},
        {"Platí, že člověk patří jen do jedné skupiny?",
         {
          "Ano",
          "Ne, je členem více skupin najednou",
          "Patří jen do školy",
          "Skupiny neexistují",
         },
         4, 1},
        {"Co je socializace v rodině?",
         {
          "První učení se životu mezi lidmi",
          "Pouze dědické řízení",
          "Stěhování",
          "Školní test",
         },
         4, 0},
    };
    static const char *hints[] = {
        "Rodina je typická primární skupina",
        "Rodina má citovou a výchovnou funkci",
        "Člověk je ve více skupinách",
        "V rodině začíná socializace",
    };
    return build_on_mcq_page(9, "onunit10", "on_ex10_title",
                             "on_quiz10_head", qs, hints, 4);
}


static GtkWidget *build_on_unit11_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Místo", "Komunita a sousedství",
            "Tip: komunita není totéž co dav.",
            {
                "Komunita jsou lidé, které spojuje místo, zájem nebo sounáležitost.",
                "Sousedství je každodenní soužití v místě, kde člověk žije.",
                "Solidarita v komunitě pomáhá v krizi i v běžných věcech.",
                "Ve velkém městě komunita být může, ale hůř se tvoří.",
                NULL,
            },
        },
        {
            "2 / 2   •   Dav", "Dav, publikum, populace a veřejnost",
            "Tip: argument všichni to dělají nic nedokazuje.",
            {
                "Dav je dočasné seskupení, kde emoce snadno přebijí rozum.",
                "Publikum sleduje program a většinou se řídí pravidly místa.",
                "Populace je souhrn obyvatel.",
                "Veřejnost je ta část lidí, která se vztahuje ke společným věcem.",
                "V davu člověk snáz udělá to, co by sám neudělal.",
                NULL,
            },
        },
    };

    return build_on_unit_page(&on_lessons[10], "on_unit11", "on_unit11_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on_unit11_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Čím se dav liší od komunity?",
         {
          "Ničím",
          "Dav je dočasný a snadno v něm převáží emoce",
          "Dav je vždycky rodina",
          "Komunita nemá žádná pravidla",
         },
         4, 1},
        {"Co je populace?",
         {
          "Diváci v sále",
          "Souhrn obyvatel",
          "Politická strana",
          "Jedna školní třída",
         },
         4, 1},
        {"Co hrozí v davu?",
         {
          "Že člověk udělá něco, co by sám neudělal",
          "Že se automaticky zlepší morálka",
          "Že emoce zmizí",
          "Že se dav rozpustí rodinným právem",
         },
         4, 0},
        {"Co je veřejnost?",
         {
          "Jen diváci televize",
          "Lidé, kteří se vztahují ke společným věcem",
          "Uzavřená sekta",
          "Jedna domácnost",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Dav je dočasný a emotivní",
        "Populace = obyvatelé",
        "V davu člověk snáz překročí vlastní mez",
        "Veřejnost se vztahuje ke společným věcem",
    };
    return build_on_mcq_page(10, "onunit11", "on_ex11_title",
                             "on_quiz11_head", qs, hints, 4);
}


static GtkWidget *build_on_unit12_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Parta", "Vrstevníci a tlak skupiny",
            "Tip: parta může podržet i strhnout.",
            {
                "Vrstevnická skupina silně ovlivňuje dospívání.",
                "Tlak skupiny může pomoct i ublížit.",
                "Solidarita je ochota nést část zátěže druhého.",
                "Solidarita není totéž co slepá poslušnost partě.",
                NULL,
            },
        },
        {
            "2 / 2   •   Hranice", "Migranti, azylanti a emigranti",
            "Tip: azyl není totéž co turistika.",
            {
                "Migrant je člověk, který se stěhuje přes hranice, z různých důvodů.",
                "Azylant žádá o ochranu, protože mu doma hrozí pronásledování.",
                "Emigrant ze své země odchází, imigrant do země přichází.",
                "Soužití má přínosy i třenice. Předsudek není argument.",
                NULL,
            },
        },
    };

    return build_on_unit_page(&on_lessons[11], "on_unit12", "on_unit12_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on_unit12_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je tlak vrstevníků?",
         {
          "Vliv party na rozhodování",
          "Zákon parlamentu",
          "Druh daně",
          "Lékařská diagnóza",
         },
         4, 0},
        {"Kdo je azylant?",
         {
          "Turista na dovolené",
          "Člověk, který žádá ochranu před pronásledováním",
          "Každý, kdo má pas",
          "Jen student na výměně",
         },
         4, 1},
        {"Jak se liší emigrant a imigrant?",
         {
          "Nijak",
          "Emigrant odchází, imigrant přichází",
          "Imigrant je vždycky azylant",
          "Emigrant je jen z Evropské unie",
         },
         4, 1},
        {"Co je solidarita?",
         {
          "Slepá poslušnost",
          "Ochota nést část zátěže druhého",
          "Povinnost souhlasit s davem",
          "Trest",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Tlak vrstevníků ovlivňuje rozhodování",
        "Azylant žádá ochranu před pronásledováním",
        "Emigrant odchází, imigrant přichází",
        "Solidarita = nést část zátěže",
    };
    return build_on_mcq_page(11, "onunit12", "on_ex12_title",
                             "on_quiz12_head", qs, hints, 4);
}


static GtkWidget *build_on_unit13_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Typy", "Tradiční, moderní a pozdně moderní společnost",
            "Tip: moderní tu není pochvala, ale popis.",
            {
                "Tradiční společnost stála na zvyku, rodině a pomalém tempu změn.",
                "Moderní společnost přinesla průmysl, města a větší pohyb lidí.",
                "Pozdně moderní společnost je rychlá, propojená a méně přehledná.",
                "Člověk v ní má víc voleb a zároveň víc nejistoty.",
                NULL,
            },
        },
        {
            "2 / 2   •   Vrstvy", "Vrstvy, elity a chudoba",
            "Tip: chudoba není morální cejch.",
            {
                "Společenské vrstvy se liší majetkem, vzděláním, prestiží i mocí.",
                "Elity jsou skupiny s velkým vlivem, ne automaticky ti lepší.",
                "Sociální nerovnost a chudoba v demokracii nezmizely.",
                "V tísni se člověk může obrátit na sociální služby a úřady.",
                NULL,
            },
        },
    };

    return build_on_unit_page(&on_lessons[12], "on_unit13", "on_unit13_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on_unit13_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Čím se vyznačuje tradiční společnost?",
         {
          "Rychlými globálními změnami",
          "Zvykem a pomalejším tempem změn",
          "Jen internetem",
          "Zrušením rodiny",
         },
         4, 1},
        {"Co jsou společenské elity?",
         {
          "Automaticky morálně nejlepší lidé",
          "Skupiny s velkým vlivem",
          "Jen sportovci",
          "Lidé bez příjmu",
         },
         4, 1},
        {"Platí, že v demokracii chudoba neexistuje?",
         {
          "Ano",
          "Ne, sociální nerovnost a chudoba zůstávají",
          "Chudoba je jen ve středověku",
          "Ústava nerovnost tím, že existuje, zruší",
         },
         4, 1},
        {"Kam se obrátit v sociální tísni?",
         {
          "Nikam, pomoc neexistuje",
          "Na sociální služby a příslušné úřady",
          "Jen na lichváře",
          "Na náhodnou reklamu",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Tradiční společnost stojí na zvyku",
        "Elity = skupiny s vlivem",
        "Chudoba v demokracii nemizí",
        "Tíseň řeší sociální služby a úřady",
    };
    return build_on_mcq_page(12, "onunit13", "on_ex13_title",
                             "on_quiz13_head", qs, hints, 4);
}


static GtkWidget *build_on_unit14_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Pojmy", "Pohlaví a gender",
            "Tip: rovnost neznamená, že jsou všichni ve všem stejní.",
            {
                "Pohlaví je biologická danost.",
                "Gender jsou společenské role a očekávání spojená s muži a ženami.",
                "Tahle očekávání se v čase a mezi kulturami liší.",
                "Rovnost znamená stejnou důstojnost a šanci, ne stejný životopis.",
                NULL,
            },
        },
        {
            "2 / 2   •   Praxe", "Kde se nerovnost ukazuje",
            "Tip: pozná se na konkrétním jednání, ne na heslu.",
            {
                "Nerovnost se může ukázat v odměňování, v péči o domácnost i v násilí.",
                "Porušení rovnosti se soudí podle skutků, ne podle názvu.",
                "Diskuse má stát na faktech, ne na posměchu.",
                "Respekt k druhému nezávisí na tom, jestli s ním ve všem souhlasím.",
                NULL,
            },
        },
    };

    return build_on_unit_page(&on_lessons[13], "on_unit14", "on_unit14_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on_unit14_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak se ve výuce rozlišuje pohlaví a gender?",
         {
          "Jsou to přesná synonyma",
          "Pohlaví je biologické, gender jsou společenské role a očekávání",
          "Gender je jen lékařská diagnóza",
          "Pohlaví je krevní skupina",
         },
         4, 1},
        {"Co rovnost pohlaví znamená?",
         {
          "Že jsou všichni ve všem stejní",
          "Stejnou důstojnost a šanci",
          "Že nesmějí existovat žádné záliby",
          "Že jeden rozhoduje za druhé",
         },
         4, 1},
        {"Kde se nerovnost může projevit?",
         {
          "Nikde",
          "Třeba v odměňování nebo v dělbě péče",
          "Jen v barvě očí",
          "Jen ve sportovním rekordu",
         },
         4, 1},
        {"Čím se má taková diskuse řídit?",
         {
          "Posměchem",
          "Fakty a respektem",
          "Tím, kdo křičí víc",
          "Zákazem otázek",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Gender = společenské role a očekávání",
        "Rovnost = stejná důstojnost a šance",
        "Ukazuje se třeba v odměně a péči",
        "Diskuse stojí na faktech",
    };
    return build_on_mcq_page(13, "onunit14", "on_ex14_title",
                             "on_quiz14_head", qs, hints, 4);
}


static GtkWidget *build_on_unit15_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Dvě vrstvy", "Hmotná a duchovní kultura",
            "Tip: mobil je hmotná kultura, jazyk v něm je duchovní.",
            {
                "Kultura je to, co lidé vytvářejí a předávají, ne jen vysoké umění.",
                "Hmotná kultura jsou věci: stavby, nástroje, oděv.",
                "Duchovní kultura jsou myšlenky, jazyk, věda, umění a normy.",
                "Obě vrstvy se prolínají.",
                NULL,
            },
        },
        {
            "2 / 2   •   Péče", "Věda, umění a kulturní hodnoty",
            "Tip: kulturní hodnota není jen to, co se líbí mně.",
            {
                "Věda a umění jsou významné části duchovní kultury.",
                "Péče o kulturní hodnoty znamená neničit je a rozumět jim.",
                "Kultura se mění, proto není jeden seznam správných věcí navždy.",
                "Bez kultury by společnost neměla společnou řeč ani paměť.",
                NULL,
            },
        },
    };

    return build_on_unit_page(&on_lessons[14], "on_unit15", "on_unit15_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on_unit15_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je hmotná kultura?",
         {
          "Jen city",
          "Věci, které lidé vytvořili",
          "Pouze náboženství",
          "Jen zákony",
         },
         4, 1},
        {"Kam patří jazyk a věda?",
         {
          "Do hmotné kultury",
          "Do duchovní kultury",
          "Mimo kulturu",
          "Jen do matematiky",
         },
         4, 1},
        {"Proč se o kulturní hodnoty pečuje?",
         {
          "Aby zmizely",
          "Aby se neničily a zůstaly srozumitelné",
          "Protože se nesmějí nikdy měnit",
          "Protože kultura je jen soukromý vkus",
         },
         4, 1},
        {"Platí, že kultura je jen opera a galerie?",
         {
          "Ano",
          "Ne, kultura je širší způsob, jak lidé žijí a tvoří",
          "Kultura je jen jídlo",
          "Kultura je jen sport",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Hmotná kultura = vytvořené věci",
        "Jazyk a věda jsou duchovní kultura",
        "Hodnoty se chrání a vykládají",
        "Kultura je širší než vysoké umění",
    };
    return build_on_mcq_page(14, "onunit15", "on_ex15_title",
                             "on_quiz15_head", qs, hints, 4);
}


static GtkWidget *build_on_unit16_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Víra", "Víra, ateismus a církve",
            "Tip: ateismus není urážka a víra není diagnóza.",
            {
                "Věřící člověk vztahuje život k tomu, co ho přesahuje.",
                "Ateista takový náboženský vztah nesdílí.",
                "V České republice žijí věřící i ateisté a obojí má v demokracii místo.",
                "Církev je organizované společenství věřících, ne totéž co víra sama.",
                "Mezi velká náboženství patří křesťanství, islám, hinduismus, buddhismus a judaismus.",
                NULL,
            },
        },
        {
            "2 / 2   •   Riziko", "Sekty a náboženský fundamentalismus",
            "Tip: varovné je, když skupina zakazuje otázky a kontakt s blízkými.",
            {
                "Sekta v tomto významu je uzavřená skupina, která člena izoluje a kontroluje.",
                "Nebezpečí je manipulace, tlak na peníze a odříznutí od rodiny.",
                "Fundamentalismus bere vlastní výklad jako jediný a často útočí na druhé.",
                "Svoboda víry neznamená právo ubližovat.",
                NULL,
            },
        },
    };

    return build_on_unit_page(&on_lessons[15], "on_unit16", "on_unit16_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on_unit16_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je ateismus?",
         {
          "Zakázaný názor",
          "Postoj, který nesdílí náboženskou víru",
          "Druh církve",
          "Povinnost státu",
         },
         4, 1},
        {"Co je církev?",
         {
          "Každá budova s věží",
          "Organizované společenství věřících",
          "Totéž co ateismus",
          "Státní úřad",
         },
         4, 1},
        {"Čím je nebezpečná manipulativní sekta?",
         {
          "Tím, že má jiný svátek",
          "Izolací, kontrolou a tlakem",
          "Tím, že lidé věří",
          "Tím, že se schází legálně",
         },
         4, 1},
        {"Co náboženský fundamentalismus často dělá?",
         {
          "Vede k dialogu za každou cenu",
          "Bere vlastní výklad jako jediný přípustný",
          "Ruší náboženství",
          "Je totéž co ateismus",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Ateismus nesdílí náboženskou víru",
        "Církev je organizované společenství",
        "Sekta izoluje a kontroluje",
        "Fundamentalismus trvá na jediném výkladu",
    };
    return build_on_mcq_page(15, "onunit16", "on_ex16_title",
                             "on_quiz16_head", qs, hints, 4);
}


static GtkWidget *build_on_unit17_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Rozpočet", "Příjmy, výdaje a majetek",
            "Tip: nejdřív povinné výdaje, pak zbytek.",
            {
                "Majetek člověk nabývá prací, darem, dědictvím nebo podnikáním.",
                "Rozpočet domácnosti porovnává příjmy a výdaje.",
                "Příjmy i výdaje jsou pravidelné a nepravidelné.",
                "Schodkový rozpočet se musí řešit, přebytek se dá odložit i na stáří.",
                NULL,
            },
        },
        {
            "2 / 2   •   Tíseň", "Krize, zajištění a úvěr",
            "Tip: kdo smlouvě nerozumí, ještě nepodepisuje.",
            {
                "Zodpovědné hospodaření znamená neutrácet to, co domácnost nemá.",
                "Krize je ztráta příjmu, nemoc nebo dluh, který nejde splatit.",
                "Pomoc nabízí sociální zabezpečení, dávky a poradenství, ne lichva.",
                "Úvěr je nástroj, ne dar: splácí se i s navýšením.",
                NULL,
            },
        },
    };

    return build_on_unit_page(&on_lessons[16], "on_unit17", "on_unit17_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on_unit17_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co porovnává rozpočet domácnosti?",
         {
          "Jen sny",
          "Příjmy a výdaje",
          "Počasí",
          "Školní známky",
         },
         4, 1},
        {"Co je schodkový rozpočet?",
         {
          "Výdaje jsou vyšší než příjmy",
          "Příjmy jsou vyšší než výdaje",
          "Rozpočet bez čísel",
          "Rozpočet státu",
         },
         4, 0},
        {"Jak se má řešit finanční tíseň?",
         {
          "Další lichvou",
          "Poradenstvím a legální sociální pomocí, ne drahým dluhem",
          "Tím, že se smlouvy nečtou",
          "Ignorováním splátek",
         },
         4, 1},
        {"Co platí o úvěru?",
         {
          "Je to dar",
          "Splácí se i s navýšením",
          "Nemusí se vracet",
          "Je totéž co mzda",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Rozpočet porovnává příjmy a výdaje",
        "Schodek = výdaje nad příjmy",
        "Tíseň se neřeší lichvou",
        "Úvěr se vrací i s navýšením",
    };
    return build_on_mcq_page(16, "onunit17", "on_ex17_title",
                             "on_quiz17_head", qs, hints, 4);
}


void add_on_pages(GtkStack *stack) {
    typedef GtkWidget *(*OnBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        OnBuilder build_unit;
        OnBuilder build_ex;
    } pages[] = {
        {"onunit1", "onex1", build_on_unit1_page, build_on_unit1_exercise_page},
        {"onunit2", "onex2", build_on_unit2_page, build_on_unit2_exercise_page},
        {"onunit3", "onex3", build_on_unit3_page, build_on_unit3_exercise_page},
        {"onunit4", "onex4", build_on_unit4_page, build_on_unit4_exercise_page},
        {"onunit5", "onex5", build_on_unit5_page, build_on_unit5_exercise_page},
        {"onunit6", "onex6", build_on_unit6_page, build_on_unit6_exercise_page},
        {"onunit7", "onex7", build_on_unit7_page, build_on_unit7_exercise_page},
        {"onunit8", "onex8", build_on_unit8_page, build_on_unit8_exercise_page},
        {"onunit9", "onex9", build_on_unit9_page, build_on_unit9_exercise_page},
        {"onunit10", "onex10", build_on_unit10_page, build_on_unit10_exercise_page},
        {"onunit11", "onex11", build_on_unit11_page, build_on_unit11_exercise_page},
        {"onunit12", "onex12", build_on_unit12_page, build_on_unit12_exercise_page},
        {"onunit13", "onex13", build_on_unit13_page, build_on_unit13_exercise_page},
        {"onunit14", "onex14", build_on_unit14_page, build_on_unit14_exercise_page},
        {"onunit15", "onex15", build_on_unit15_page, build_on_unit15_exercise_page},
        {"onunit16", "onex16", build_on_unit16_page, build_on_unit16_exercise_page},
        {"onunit17", "onex17", build_on_unit17_page, build_on_unit17_exercise_page},
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
