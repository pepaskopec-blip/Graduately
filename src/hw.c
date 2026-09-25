#include "maturita.h"

/* ---- Technical equipment (Technické vybavení) --------------------- */

NetLesson hw_lessons[HW_LESSONS] = {
    { .n_slides = HW_SLIDES,  .unit_page = "hwunit1", .ex_page = "hwex1" },
    { .n_slides = HW2_SLIDES, .unit_page = "hwunit2", .ex_page = "hwex2" },
    { .n_slides = HW3_SLIDES, .unit_page = "hwunit3", .ex_page = "hwex3" },
    { .n_slides = HW4_SLIDES, .unit_page = "hwunit4", .ex_page = "hwex4" },
    { .n_slides = HW5_SLIDES, .unit_page = "hwunit5", .ex_page = "hwex5" },
    { .n_slides = HW6_SLIDES, .unit_page = "hwunit6", .ex_page = "hwex6" },
    { .n_slides = HW7_SLIDES, .unit_page = "hwunit7", .ex_page = "hwex7" },
    { .n_slides = HW8_SLIDES, .unit_page = "hwunit8", .ex_page = "hwex8" },
    { .n_slides = HW9_SLIDES, .unit_page = "hwunit9", .ex_page = "hwex9" },
#define HW_ENTRY(n) { .n_slides = 2, .unit_page = "hwunit" #n, .ex_page = "hwex" #n }
    HW_ENTRY(10), HW_ENTRY(11), HW_ENTRY(12), HW_ENTRY(13), HW_ENTRY(14),
    HW_ENTRY(15), HW_ENTRY(16), HW_ENTRY(17), HW_ENTRY(18), HW_ENTRY(19),
    HW_ENTRY(20), HW_ENTRY(21), HW_ENTRY(22), HW_ENTRY(23), HW_ENTRY(24),
    HW_ENTRY(25), HW_ENTRY(26), HW_ENTRY(27), HW_ENTRY(28), HW_ENTRY(29),
#undef HW_ENTRY
};

void hw_rail_theme_reset(void);
void refresh_hw_completion_ui(void);

GtkWidget *hw_scroll;
GtkWidget *hw_fixed;
GtkWidget *hw_rail;
GtkWidget *hw_nodes[HW_UNITS];
GtkWidget *hw_labels[HW_UNITS];
double hw_cx[HW_UNITS];
double hw_cy[HW_UNITS];
int hw_cols = 1;
int hw_rows = 1;
int hw_cw = (int)(2.0 * ROAD_MX + (HW_UNITS - 1) * PATH_SPAC);
int hw_ch = (int)(2.0 * ROAD_MY);
guint hw_idle;
double hw_last_avail = -1.0;
int hw_last_cols = -1;
int hw_last_rows = -1;
int hw_last_cw = -1;
int hw_last_ch = -1;
cairo_surface_t *hw_rail_cache;
int hw_cache_w;
int hw_cache_h;

void hw_point(double t, double *ox, double *oy) {
    int k = (int)t;
    double u = t - (double)k;
    double u2, u3;
    double p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y;

    if (k < 0) { k = 0; u = 0.0; }
    if (k >= HW_UNITS - 1) { k = HW_UNITS - 2; u = 1.0; }

    p1x = hw_cx[k];     p1y = hw_cy[k];
    p2x = hw_cx[k + 1]; p2y = hw_cy[k + 1];
    if (k - 1 >= 0) { p0x = hw_cx[k - 1]; p0y = hw_cy[k - 1]; }
    else            { p0x = p1x - (p2x - p1x); p0y = p1y - (p2y - p1y); }
    if (k + 2 < HW_UNITS) { p3x = hw_cx[k + 2]; p3y = hw_cy[k + 2]; }
    else                   { p3x = p2x + (p2x - p1x); p3y = p2y + (p2y - p1y); }

    u2 = u * u;
    u3 = u2 * u;
    *ox = 0.5 * (2.0 * p1x + (-p0x + p2x) * u
                 + (2.0 * p0x - 5.0 * p1x + 4.0 * p2x - p3x) * u2
                 + (-p0x + 3.0 * p1x - 3.0 * p2x + p3x) * u3);
    *oy = 0.5 * (2.0 * p1y + (-p0y + p2y) * u
                 + (2.0 * p0y - 5.0 * p1y + 4.0 * p2y - p3y) * u2
                 + (-p0y + 3.0 * p1y - 3.0 * p2y + p3y) * u3);
}

void hw_path(cairo_t *cr, double t0, double t1) {
    const double steps_per_unit = 24.0;
    int n = (int)((t1 - t0) * steps_per_unit);
    double x, y;
    int s;

    if (n < 1)
        n = 1;
    hw_point(t0, &x, &y);
    cairo_move_to(cr, x, y);
    for (s = 1; s <= n; s++) {
        hw_point(t0 + (t1 - t0) * (double)s / (double)n, &x, &y);
        cairo_line_to(cr, x, y);
    }
}

void hw_geometry(double avail) {
    const double phase = 2.0 * G_PI * 1.7 / (double)(HW_UNITS - 1);
    int rows;
    int r, c, i;

    for (rows = 1; rows <= HW_UNITS; rows++) {
        int cols = (HW_UNITS + rows - 1) / rows;
        if (2.0 * ROAD_MX + (cols - 1) * PATH_SPAC <= avail + 1.0)
            break;
    }
    if (rows > HW_UNITS)
        rows = HW_UNITS;
    hw_rows = rows;
    hw_cols = (HW_UNITS + rows - 1) / rows;
    if (hw_cols < 1)
        hw_cols = 1;
    hw_cw = (int)(2.0 * ROAD_MX + (hw_cols - 1) * PATH_SPAC);
    hw_ch = (int)(2.0 * ROAD_MY + (rows - 1) * ROAD_GAP);

    for (r = 0; r < rows; r++) {
        int base = r * hw_cols;
        int len = MIN(hw_cols, HW_UNITS - base);
        int fwd = (r % 2) == 0;
        for (c = 0; c < len; c++) {
            i = base + c;
            int cc = fwd ? c : (hw_cols - 1 - c);
            hw_cx[i] = ROAD_MX + cc * PATH_SPAC;
            hw_cy[i] = ROAD_MY + r * ROAD_GAP
                        + ROAD_WAVE * sin((double)i * phase);
        }
    }
}

void hw_apply_layout(void) {
    if (!hw_fixed)
        return;

    if (hw_cols == hw_last_cols && hw_rows == hw_last_rows &&
        hw_cw == hw_last_cw && hw_ch == hw_last_ch)
        return;

    hw_last_cols = hw_cols;
    hw_last_rows = hw_rows;
    hw_last_cw = hw_cw;
    hw_last_ch = hw_ch;
    if (hw_rail_cache) {
        cairo_surface_destroy(hw_rail_cache);
        hw_rail_cache = NULL;
    }

    for (int i = 0; i < HW_UNITS; i++) {
        if (!hw_nodes[i] || !hw_labels[i])
            continue;
        gtk_fixed_move(GTK_FIXED(hw_fixed), hw_nodes[i],
                       (int)(hw_cx[i] - NODE_SIZE / 2.0),
                       (int)(hw_cy[i] - NODE_SIZE / 2.0));
        gtk_fixed_move(GTK_FIXED(hw_fixed), hw_labels[i],
                       (int)(hw_cx[i] - (PATH_SPAC - 20.0) / 2.0),
                       (int)(hw_cy[i] + NODE_SIZE / 2.0 + 10.0));
    }

    gtk_widget_set_size_request(hw_fixed, hw_cw, hw_ch);
    gtk_widget_set_size_request(hw_rail, hw_cw, hw_ch);
    gtk_fixed_move(GTK_FIXED(hw_fixed), hw_rail, 0, 0);
    gtk_widget_queue_draw(hw_rail);
}

void hw_relayout(void) {
    GtkAdjustment *hadj;
    double avail;

    if (!hw_scroll)
        return;
    hadj = gtk_scrolled_window_get_hadjustment(
        GTK_SCROLLED_WINDOW(hw_scroll));
    avail = gtk_adjustment_get_page_size(hadj);
    if (avail < 1.0)
        return;
    if (fabs(avail - hw_last_avail) < 1.0)
        return;
    hw_last_avail = avail;
    hw_geometry(avail);
    hw_apply_layout();
}

gboolean hw_relayout_idle(gpointer data) {
    (void)data;
    hw_idle = 0;
    hw_relayout();
    return G_SOURCE_REMOVE;
}

void hw_relayout_later(void) {
    if (hw_idle == 0)
        hw_idle = g_idle_add(hw_relayout_idle, NULL);
}

void hw_adjust_notify(GtkAdjustment *adj, GParamSpec *ps,
                              gpointer data) {
    (void)adj;
    (void)ps;
    (void)data;
    hw_relayout_later();
}

void hw_render_rail_to(cairo_t *cr) {
    const double t_end = (double)(HW_UNITS - 1);
    const Rgb rail = color_from_hex(app_theme.rail);
    const Rgb muted = color_from_hex(app_theme.subtext);

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

    cairo_new_path(cr);
    hw_path(cr, 0.0, t_end);
    cairo_set_line_width(cr, 12);
    cairo_set_source_rgb(cr, rail.r, rail.g, rail.b);
    cairo_stroke(cr);

    cairo_set_source_rgba(cr, muted.r, muted.g, muted.b, 0.17);
    cairo_set_dash(cr, (double[]){2.0, 40.0}, 2, 0.0);
    cairo_new_path(cr);
    hw_path(cr, 0.0, t_end);
    cairo_stroke(cr);
    cairo_set_dash(cr, NULL, 0, 0.0);
}

void hw_draw_rail(GtkDrawingArea *area, cairo_t *cr,
                          int width, int height, gpointer data) {
    (void)area;
    (void)width;
    (void)height;
    (void)data;

    if (!hw_rail_cache || hw_cache_w != hw_cw || hw_cache_h != hw_ch) {
        cairo_t *rcr;

        if (hw_rail_cache)
            cairo_surface_destroy(hw_rail_cache);
        hw_cache_w = hw_cw;
        hw_cache_h = hw_ch;
        if (hw_cache_w < 1)
            hw_cache_w = 1;
        if (hw_cache_h < 1)
            hw_cache_h = 1;
        hw_rail_cache = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                    hw_cache_w, hw_cache_h);
        rcr = cairo_create(hw_rail_cache);
        hw_render_rail_to(rcr);
        cairo_destroy(rcr);
    }

    cairo_set_source_surface(cr, hw_rail_cache, 0, 0);
    cairo_paint(cr);
}

void hw_rail_theme_reset(void) {
    if (hw_rail_cache) {
        cairo_surface_destroy(hw_rail_cache);
        hw_rail_cache = NULL;
    }
    if (hw_rail)
        gtk_widget_queue_draw(hw_rail);
}

void hw_add_node(GtkFixed *fixed, int index) {
    int num = index + 1;
    gboolean locked = index >= HW_LESSONS;
    GtkWidget *card;
    GtkWidget *vbox;
    GtkWidget *number;
    GtkWidget *icon;
    GtkWidget *name;
    char *text;
    static char unit_keys[HW_LESSONS][16];
    static gboolean unit_keys_ready = FALSE;

    if (!unit_keys_ready) {
        for (int i = 0; i < HW_LESSONS; i++)
            g_snprintf(unit_keys[i], sizeof unit_keys[i], "hw_unit%d", i + 1);
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
        hw_lessons[index].done_icon = icon;
        g_signal_connect(card, "clicked", G_CALLBACK(net_open_unit),
                         &hw_lessons[index]);
    }

    hw_nodes[index] = card;
    gtk_fixed_put(fixed, card, 0, 0);

    name = gtk_label_new(NULL);
    gtk_widget_set_size_request(name, (int)(PATH_SPAC - 20.0), -1);
    gtk_widget_set_halign(name, GTK_ALIGN_CENTER);
    gtk_label_set_justify(GTK_LABEL(name), GTK_JUSTIFY_CENTER);
    gtk_label_set_wrap(GTK_LABEL(name), TRUE);
    gtk_widget_add_css_class(name, "unit-name");
    if (index < HW_LESSONS) {
        i18n_bind(name, unit_keys[index], 0);
    } else {
        gtk_widget_add_css_class(name, "unit-name-locked");
    }
    hw_labels[index] = name;
    gtk_fixed_put(fixed, name, 0, 0);
}

static GtkWidget *hw_year_button(int year, gboolean locked) {
    static const char *year_keys[] = {
        "hw_year1", "hw_year2", "hw_year3", "hw_year4",
    };
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

        i18n_bind(sub, "hw_year_locked_sub", 0);
        gtk_widget_set_halign(sub, GTK_ALIGN_START);
        gtk_widget_add_css_class(sub, "unit-name-locked");
        gtk_box_append(GTK_BOX(texts), sub);
        gtk_widget_set_valign(icon, GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(row), icon);
    } else {
        g_object_set_data_full(G_OBJECT(btn), "target",
                               g_strdup("hwmap"), g_free);
        g_signal_connect(btn, "clicked", G_CALLBACK(on_nav_clicked), NULL);
    }

    return btn;
}

GtkWidget *build_hwyears_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *list;
    int y;

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("subjects", "Technické vybavení", "hw_years_sub"));

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
        gtk_box_append(GTK_BOX(list), hw_year_button(y, y > 1));

    return page;
}

GtkWidget *build_hwmap_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *fixed;
    GtkWidget *rail;
    GtkAdjustment *ha;
    GtkAdjustment *va;

    hw_geometry(1000.0);

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("hwyears", "hw_year1", "hw_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);
    hw_scroll = scroll;

    wrap = gtk_center_box_new();
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), wrap);

    fixed = gtk_fixed_new();
    gtk_widget_set_halign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(fixed, hw_cw, hw_ch);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), fixed);
    hw_fixed = fixed;

    rail = gtk_drawing_area_new();
    gtk_widget_set_size_request(rail, hw_cw, hw_ch);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(rail), hw_draw_rail,
                                   NULL, NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rail, 0, 0);
    hw_rail = rail;

    for (int i = 0; i < HW_UNITS; i++)
        hw_add_node(GTK_FIXED(fixed), i);

    ha = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scroll));
    va = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));
    g_signal_connect(ha, "notify::page-size",
                     G_CALLBACK(hw_adjust_notify), NULL);
    g_signal_connect(va, "notify::page-size",
                     G_CALLBACK(hw_adjust_notify), NULL);

    hw_apply_layout();
    hw_relayout_later();
    refresh_hw_completion_ui();

    return page;
}


void hw_slide_apply(NetLesson *L);
void hw_slide_prev(GtkButton *button, gpointer data);
void hw_slide_next(GtkButton *button, gpointer data);

/* ---- Unit 1: Architektura počítače -------------------------------- */

static GtkWidget *hw_note_host;
static int hw_note_last_w = -1;

static void hw_rescale_notes_at(int w) {
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
    for (int i = 0; i < HW_LESSONS; i++)
        net_rescale_lesson_notes(&hw_lessons[i], body, head, kick);
}

static gboolean hw_note_tick(GtkWidget *w, GdkFrameClock *clock,
                             gpointer data) {
    int width = gtk_widget_get_allocated_width(w);

    (void)clock;
    (void)data;
    if (width != hw_note_last_w) {
        hw_note_last_w = width;
        hw_rescale_notes_at(width);
    }
    return G_SOURCE_CONTINUE;
}

static GtkWidget *build_hw_unit_page(NetLesson *L, const char *title_key,
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

    gtk_box_append(GTK_BOX(page), top_bar("hwmap", title_key, sub_key));

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
    hw_note_host = L->stack;

    for (guint s = 0; s < n_slides; s++) {
        GtkWidget *stage = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        GtkWidget *card;
        char name[16];

        gtk_widget_set_vexpand(stage, TRUE);
        g_snprintf(name, sizeof(name), "hw_slide%u", s);
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

    gtk_widget_add_tick_callback(L->stack, hw_note_tick, NULL, NULL);
    hw_note_last_w = -1;
    hw_rescale_notes_at(0);
    net_notes_target = NULL;

    nav = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(nav), TRUE);
    gtk_widget_set_margin_top(nav, 10);
    gtk_box_append(GTK_BOX(page), nav);

    L->prev_btn = gtk_button_new_with_label(tr("net_slide_prev"));
    gtk_widget_add_css_class(L->prev_btn, "btn-primary");
    g_signal_connect(L->prev_btn, "clicked", G_CALLBACK(hw_slide_prev), L);
    gtk_box_append(GTK_BOX(nav), L->prev_btn);

    L->next_btn = gtk_button_new_with_label(tr("net_slide_next"));
    gtk_widget_add_css_class(L->next_btn, "btn-primary");
    g_signal_connect(L->next_btn, "clicked", G_CALLBACK(hw_slide_next), L);
    gtk_box_append(GTK_BOX(nav), L->next_btn);

    L->idx = 0;
    hw_slide_apply(L);
    return page;
}

void hw_slide_apply(NetLesson *L) {
    char name[16];

    if (!L || !L->stack)
        return;
    g_snprintf(name, sizeof(name), "hw_slide%u", L->idx);
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

void hw_slide_prev(GtkButton *button, gpointer data) {
    NetLesson *L = data;
    (void)button;
    if (!L) return;
    if (L->idx > 0) {
        L->idx--;
        hw_slide_apply(L);
    }
}

void hw_slide_next(GtkButton *button, gpointer data) {
    NetLesson *L = data;
    (void)button;
    if (!L) return;
    if (L->idx + 1 < L->n_slides) {
        L->idx++;
        hw_slide_apply(L);
    } else {
        gtk_stack_set_visible_child_name(main_stack, L->ex_page);
    }
}

void hw_lessons_apply_lang(void) {
    for (int i = 0; i < HW_LESSONS; i++)
        hw_slide_apply(&hw_lessons[i]);
}

GtkWidget *build_hw_unit1_page(void) {
    static const NetSlide slides[HW_SLIDES] = {
        {
            "1 / 3   •   Úvod", "Architektura počítače",
            "Architektura popisuje, z jakých částí se počítač skládá "
            "a jak spolu spolupracují.",
            {
                "Architektura počítače je základní model, podle kterého "
                "jsou počítače navrhovány.",
                "Nejznámější model je Von Neumannovo schéma.",
                "Používá se dodnes u většiny běžných počítačů.",
                NULL,
            },
        },
        {
            "2 / 3   •   Osobnost", "John von Neumann",
            "Tip: zapamatuj si jméno – podle něj se jmenuje celé schéma.",
            {
                "John von Neumann byl americký matematik.",
                "Pracoval na poli digitálních počítačů a operační teorie "
                "kvantové mechaniky.",
                "Je autorem Von Neumannovy algebry.",
                "Byl tvůrcem teorie her.",
                "Podílel se na vývoji atomové a vodíkové bomby.",
                NULL,
            },
        },
        {
            "3 / 3   •   Schéma", "Von Neumannovo schéma",
            "ALU = aritmeticko-logická jednotka",
            {
                "Von Neumannovo schéma má pět hlavních částí:",
                "ALU – aritmeticko-logická jednotka (výpočty a logika)",
                "Řadič – řídí činnost ostatních částí",
                "Operační paměť – uchovává program i data",
                "Vstupní jednotka – přivádí data do počítače",
                "Výstupní jednotka – předává výsledky ven",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[0], "hw_unit1", "hw_unit1_sub",
                               slides, HW_SLIDES);
}

/* ---- Quiz --------------------------------------------------------- */

static void hw_mcq_check(GtkButton *button, gpointer data) {
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
        mark_hw_done(ctx->lesson_id);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry_short"));
    }
}

static GtkWidget *build_hw_mcq_page(int lesson_id, const char *back_page,
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
    g_signal_connect(check, "clicked", G_CALLBACK(hw_mcq_check), ctx);

    ctx->feedback = gtk_label_new("");
    gtk_widget_set_halign(ctx->feedback, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(body), ctx->feedback);

    return page;
}

GtkWidget *build_hw_unit1_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo je autorem Von Neumannova schématu?",
         {"Alan Turing", "John von Neumann", "Bill Gates", "Steve Jobs"},
         4, 1},
        {"Čím byl John von Neumann?",
         {"Americký matematik", "Německý fyzik", "Anglický chemik",
          "Francouzský lékař"},
         4, 0},
        {"Co znamená zkratka ALU?",
         {"Automatická logická síť", "Aritmeticko-logická jednotka",
          "Analogová linková sestava", "Adresová listová sekce"},
         4, 1},
        {"Která část Von Neumannova schématu řídí ostatní části?",
         {"Operační paměť", "Vstupní jednotka", "Řadič", "Výstupní jednotka"},
         4, 2},
        {"Kde se podle Von Neumannova schématu uchovává program i data?",
         {"Jen na pevném disku", "V operační paměti", "Jen ve vstupní jednotce",
          "Jen v ALU"},
         4, 1},
        {"Čím se John von Neumann také zabýval?",
         {"Jen malířstvím", "Teorií her", "Jen botanikou", "Jen hudbou"},
         4, 1},
    };
    static const char *hints[] = {
        "John von Neumann – americký matematik",
        "Americký matematik s objevy v digitálních počítačích",
        "ALU = aritmeticko-logická jednotka",
        "Řadič řídí činnost ostatních částí",
        "Operační paměť uchovává program i data",
        "Byl tvůrcem teorie her",
    };
    return build_hw_mcq_page(0, "hwunit1", "hw_ex1_title", "hw_quiz1_head",
                             qs, hints, 6);
}

GtkWidget *build_hw_unit2_page(void) {
    static const NetSlide slides[HW2_SLIDES] = {
        {
            "1 / 6   •   Přehled", "Historie počítačů",
            "Generace se dělí podle klíčové technologie své doby.",
            {
                "1. Předchůdci – do 30. let 19. století",
                "2. Nultá generace – 1938–1944",
                "3. První generace – 1944–1955",
                "4. Druhá generace – 1955–1964",
                "5. Třetí generace – 1964–1971",
                "Další snímky shrnují znaky jednotlivých generací.",
                NULL,
            },
        },
        {
            "2 / 6   •   0. generace", "Počítače 0. generace (1938–1944)",
            "Tip: počítačů bylo málo a často sloužily armádě.",
            {
                "Existovalo jen několik počítačů na světě.",
                "Většinou měly vojenský účel.",
                NULL,
            },
        },
        {
            "3 / 6   •   1. generace", "Počítače 1. generace (1944–1955)",
            "Klíčový objev: elektronka",
            {
                "Objev elektronky umožnil tuto generaci.",
                "Ještě neexistoval software v dnešním smyslu.",
                "Data a programy se zadávaly pomocí děrných štítků.",
                NULL,
            },
        },
        {
            "4 / 6   •   2. generace", "Počítače 2. generace (1955–1964)",
            "Klíčový objev: tranzistor",
            {
                "Objev tranzistoru nahradil elektronky.",
                "Používala se hlavní externí paměť.",
                "Počítače měly menší rozměry než v 1. generaci.",
                NULL,
            },
        },
        {
            "5 / 6   •   3. generace", "Počítače 3. generace (1964–1971)",
            "Klíčový objev: integrovaný obvod",
            {
                "Objev integrovaného obvodu.",
                "Stále se používala hlavní externí paměť.",
                "Rozměry počítačů se dál zmenšovaly.",
                NULL,
            },
        },
        {
            "6 / 6   •   4. generace", "Počítače 4. generace",
            "Klíčové: programovatelné mikroprocesory",
            {
                "Nastupují programovatelné mikroprocesory.",
                "1972 – první počítačová hra",
                "1973 – první disketa",
                "1979 – první CD",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[1], "hw_unit2", "hw_unit2_sub",
                               slides, HW2_SLIDES);
}

GtkWidget *build_hw_unit2_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdy probíhala nultá generace počítačů?",
         {"1938–1944", "1944–1955", "1955–1964", "1964–1971"}, 4, 0},
        {"Co bylo typické pro počítače 0. generace?",
         {"Byly v každé domácnosti", "Jen několik kusů, často vojenský účel",
          "Měly mikroprocesory", "Používaly CD"},
         4, 1},
        {"Která technologie patří k 1. generaci?",
         {"Tranzistor", "Elektronka", "Integrovaný obvod", "Mikroprocesor"},
         4, 1},
        {"Čím se zadávaly data v 1. generaci?",
         {"USB fleškou", "Děrnými štítky", "CD", "Disketou"}, 4, 1},
        {"Která technologie patří k 2. generaci?",
         {"Elektronka", "Tranzistor", "Mikroprocesor", "CD"}, 4, 1},
        {"Která technologie patří k 3. generaci?",
         {"Elektronka", "Tranzistor", "Integrovaný obvod", "Disketa"},
         4, 2},
        {"Co je typické pro 4. generaci?",
         {"Jen děrné štítky", "Programovatelné mikroprocesory",
          "Jen vojenské použití", "Žádný software"},
         4, 1},
        {"Kdy vznikla první počítačová hra (podle lekce)?",
         {"1964", "1972", "1973", "1979"}, 4, 1},
    };
    static const char *hints[] = {
        "Nultá generace: 1938–1944",
        "Jen několik počítačů, většinou vojenský účel",
        "1. generace – objev elektronky",
        "1. generace – děrné štítky",
        "2. generace – objev tranzistoru",
        "3. generace – integrovaný obvod",
        "4. generace – programovatelné mikroprocesory",
        "1972 – první počítačová hra",
    };
    return build_hw_mcq_page(1, "hwunit2", "hw_ex2_title", "hw_quiz2_head",
                             qs, hints, 8);
}

GtkWidget *build_hw_unit3_page(void) {
    static const NetSlide slides[HW3_SLIDES] = {
        {
            "1 / 2   •   Bit", "Bit – nejmenší jednotka informace",
            "Bit = binary digit (dvojková číslice)",
            {
                "Bit je dvojková číslice – nabývá hodnoty 0 nebo 1.",
                "Je to nejmenší zobrazitelná jednotka informace.",
                "Z bitů se skládají větší jednotky (např. byte).",
                NULL,
            },
        },
        {
            "2 / 2   •   Byte", "Byte – adresovatelná jednotka paměti",
            "Tip: 1 B = 8 b",
            {
                "Byte je nejmenší adresovatelná jednotka paměti.",
                "Počítač tedy paměť adresuje po bytech, ne po jednotlivých "
                "bitech.",
                "Jeden byte obvykle obsahuje 8 bitů.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[2], "hw_unit3", "hw_unit3_sub",
                               slides, HW3_SLIDES);
}

GtkWidget *build_hw_unit3_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je bit?",
         {"Největší jednotka paměti", "Dvojková číslice – nejmenší jednotka "
          "informace", "Jen klávesa na klávesnici", "Typ procesoru"},
         4, 1},
        {"Jaké hodnoty může nabývat bit?",
         {"Jen 0", "Jen 1", "0 nebo 1", "0 až 255"}, 4, 2},
        {"Co je byte?",
         {"Nejmenší adresovatelná jednotka paměti",
          "Nejmenší zobrazitelná jednotka informace",
          "Jen síťový kabel", "Jen grafická karta"},
         4, 0},
        {"Kolik bitů má obvykle jeden byte?",
         {"2", "4", "8", "16"}, 4, 2},
    };
    static const char *hints[] = {
        "Bit = dvojková číslice, nejmenší jednotka informace",
        "Bit nabývá hodnoty 0 nebo 1",
        "Byte = nejmenší adresovatelná jednotka paměti",
        "1 B = 8 b",
    };
    return build_hw_mcq_page(2, "hwunit3", "hw_ex3_title", "hw_quiz3_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit4_page(void) {
    static const NetSlide slides[HW4_SLIDES] = {
        {
            "1 / 2   •   Integer", "Proměnná typu integer (celá čísla)",
            "Tip: n bitů → 2ⁿ různých hodnot (např. 0 až 2ⁿ − 1)",
            {
                "Celá čísla se ukládají binárně v pevné řádové čárce.",
                "Mají pevný počet bitů – standardně 16 nebo 32 "
                "(tedy 2 nebo 4 byty).",
                "Při 16bitovém formátu je 2¹⁶ = 65 536 různých hodnot; "
                "u nezáporných čísel interval 0 až 2¹⁶ − 1, tedy 0–65 535.",
                "Obecně u n bitů je M = 2ⁿ různých hodnot "
                "(u 16 bitů je nejvyšší nezáporné číslo 65 535).",
                "Ostatní čísla se do tohoto intervalu převádějí transformací "
                "(mapováním na přirozená čísla z daného rozsahu).",
                NULL,
            },
        },
        {
            "2 / 2   •   Real", "Proměnná typu real (desetinná čísla)",
            "Tip: tvar = mantisa × základ^exponent",
            {
                "Desetinná čísla se ukládají v pohyblivé řádové čárce "
                "ve tvaru mantisa a exponent.",
                "Mantisa je normalizovaná tak, že první platná číslice "
                "je hned za desetinnou čárkou.",
                "Posun řádové čárky vyrovná odpovídající změna exponentu.",
                "Standardně zabírá 4 nebo 8 bytů v paměti.",
                "Více bytů se používá jen tehdy, když potřebujeme vyšší "
                "přesnost výpočtů.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[3], "hw_unit4", "hw_unit4_sub",
                               slides, HW4_SLIDES);
}

GtkWidget *build_hw_unit4_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak se ukládají celá čísla (integer)?",
         {"Jen jako text",
          "Binárně v pevné řádové čárce s pevným počtem bitů",
          "Jen jako obrázek",
          "Jen v pohyblivé řádové čárce bez exponentu"},
         4, 1},
        {"Kolik různých hodnot má 16bitový nezáporný integer?",
         {"256", "1024", "65 536 (0 až 65 535)", "4"},
         4, 2},
        {"Jak se ukládají desetinná čísla (real)?",
         {"Jen jako celá čísla bez tečky",
          "V pohyblivé řádové čárce jako mantisa a exponent",
          "Jen jako 1 bit",
          "Jen jako název souboru"},
         4, 1},
        {"Co znamená normalizace mantisy?",
         {"Že se číslo smaže",
          "Že první platná číslice je hned za desetinnou čárkou "
          "a exponent se upraví",
          "Že se použije jen 1 bit",
          "Že se číslo uloží jako text"},
         4, 1},
    };
    static const char *hints[] = {
        "Integer = pevná řádová čárka, pevný počet bitů (16/32…)",
        "16 bitů → 2¹⁶ = 65 536 hodnot (0–65 535 u nezáporných)",
        "Real = pohyblivá řádová čárka = mantisa + exponent",
        "Normalizace: 1. platná číslice hned za desetinnou čárkou",
    };
    return build_hw_mcq_page(3, "hwunit4", "hw_ex4_title", "hw_quiz4_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit5_page(void) {
    static const NetSlide slides[HW5_SLIDES] = {
        {
            "1 / 2   •   Signál", "Analogový a digitální zvuk",
            "Tip: analog je spojitý, digitál je posloupnost čísel.",
            {
                "Analogový zvuk je spojitý signál – v každém okamžiku "
                "má nějakou hodnotu.",
                "Digitální zvuk je posloupnost čísel, která ten signál "
                "popisují.",
                "Z mikrofonu vychází analogové napětí, do reproduktoru "
                "musí zase jít spojitý signál.",
                "Počítač umí uložit jen čísla, proto se zvuk digitalizuje.",
                NULL,
            },
        },
        {
            "2 / 2   •   Převod", "A/D a D/A převodník",
            "Tip: ADC dovnitř, DAC ven. Digitalizace = vzorkování + kvantování.",
            {
                "A/D převodník (ADC) převede analogový signál na čísla.",
                "D/A převodník (DAC) z čísel znovu složí spojitý signál.",
                "Digitalizace má dva kroky: vzorkování a kvantování.",
                "Vzorkování určí, kdy se hodnota změří.",
                "Kvantování určí, na kolik hladin se amplituda zaokrouhlí.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[4], "hw_unit5", "hw_unit5_sub",
                               slides, HW5_SLIDES);
}

GtkWidget *build_hw_unit5_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je analogový zvuk?",
         {"Posloupnost celých čísel",
          "Spojitý signál s hodnotou v každém okamžiku",
          "Jen název souboru WAV",
          "Jen bit v paměti"},
         4, 1},
        {"Co dělá A/D převodník (ADC)?",
         {"Převádí analogový signál na čísla",
          "Zesiluje jen reproduktor",
          "Maže vysoké tóny po uložení",
          "Počítá jen velikost souboru"},
         4, 0},
        {"Jaké jsou dva hlavní kroky digitalizace zvuku?",
         {"Komprese a tisk",
          "Vzorkování a kvantování",
          "Šifrování a záloha",
          "Násobení a dělení"},
         4, 1},
        {"Co dělá D/A převodník (DAC)?",
         {"Z čísel znovu skládá spojitý signál",
          "Měří jen vzorkovací frekvenci",
          "Ukládá zvuk jako text",
          "Zvyšuje počet bitů v souboru"},
         4, 0},
    };
    static const char *hints[] = {
        "Analog = spojitý signál",
        "ADC převádí analog na čísla",
        "Digitalizace = vzorkování + kvantování",
        "DAC skládá z čísel spojitý signál",
    };
    return build_hw_mcq_page(4, "hwunit5", "hw_ex5_title", "hw_quiz5_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit6_page(void) {
    static const NetSlide slides[HW6_SLIDES] = {
        {
            "1 / 2   •   Vzorky", "Vzorkování",
            "Tip: vzorkovací frekvence fs = počet vzorků za sekundu (Hz).",
            {
                "Vzorkování měří velikost signálu v pravidelných "
                "okamžicích.",
                "Mezi vzorky se hodnota neukládá.",
                "Vzorkovací frekvence fs je počet vzorků za sekundu.",
                "Jednotka je hertz (Hz). 1 kHz = 1000 vzorků za sekundu.",
                "Vyšší fs zachytí rychlejší změny, tedy vyšší tóny.",
                NULL,
            },
        },
        {
            "2 / 2   •   Nyquist", "Nyquistův–Shannonův teorém",
            "Tip: fs musí být vyšší než 2 × fmax. Nyquistova frekvence je fs / 2.",
            {
                "Pro věrnou rekonstrukci musí být fs vyšší než dvojnásobek "
                "nejvyšší frekvence ve signálu.",
                "Nyquistova frekvence je fs / 2 – vyšší tón už záznam neunese.",
                "Člověk slyší zhruba do 20 kHz, dvojnásobek je 40 kHz.",
                "CD proto používá 44,1 kHz: o něco víc než 40 kHz, "
                "aby zbyl prostor pro filtr.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[5], "hw_unit6", "hw_unit6_sub",
                               slides, HW6_SLIDES);
}

GtkWidget *build_hw_unit6_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je vzorkovací frekvence?",
         {"Počet bitů v jednom vzorku",
          "Počet vzorků za sekundu",
          "Délka skladby v minutách",
          "Počet kanálů (mono nebo stereo)"},
         4, 1},
        {"V jakých jednotkách se udává vzorkovací frekvence?",
         {"V bytech", "V hertzech", "Ve voltech", "V pixelech"},
         4, 1},
        {"Co říká Nyquistův–Shannonův teorém?",
         {"fs musí být vyšší než dvojnásobek nejvyšší frekvence",
          "fs musí být vždy 8 kHz",
          "Stačí jeden vzorek na celou skladbu",
          "Bitová hloubka musí být 1"},
         4, 0},
        {"Proč má audio CD vzorkovací frekvenci 44,1 kHz?",
         {"Protože byte má 8 bitů",
          "Sluch sahá asi do 20 kHz a fs musí být vyšší než dvojnásobek",
          "Protože stereo má dva kanály",
          "Protože minutu tvoří 60 sekund"},
         4, 1},
    };
    static const char *hints[] = {
        "fs = počet vzorků za sekundu",
        "Jednotka vzorkovací frekvence je hertz",
        "fs > 2 × fmax",
        "20 kHz × 2 = 40 kHz, CD má 44,1 kHz",
    };
    return build_hw_mcq_page(5, "hwunit6", "hw_ex6_title", "hw_quiz6_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit7_page(void) {
    static const NetSlide slides[HW7_SLIDES] = {
        {
            "1 / 2   •   Hladiny", "Kvantování",
            "Tip: n bitů → 2ⁿ hladin. 8 bitů = 256, 16 bitů = 65 536.",
            {
                "Kvantování zaokrouhlí amplitudu každého vzorku "
                "na nejbližší z konečného počtu hladin.",
                "Počet hladin je 2ⁿ, kde n je bitová hloubka "
                "(počet bitů na vzorek).",
                "8 bitů dává 256 hladin, 16 bitů dává 65 536 hladin.",
                "Vzorkování říká jak často, kvantování jak přesně.",
                NULL,
            },
        },
        {
            "2 / 2   •   Chyba", "Kvantizační chyba",
            "Tip: víc bitů = jemnější hladiny, menší šum a větší soubor.",
            {
                "Kvantizační chyba je rozdíl mezi skutečnou hodnotou "
                "a zvolenou hladinou.",
                "V záznamu se projeví jako kvantizační šum.",
                "Větší bitová hloubka hladiny zjemní a šum zmenší.",
                "Audio CD používá 16 bitů na vzorek, studio často 24 bitů.",
                "Vyšší hloubka také zvětší datový tok.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[6], "hw_unit7", "hw_unit7_sub",
                               slides, HW7_SLIDES);
}

GtkWidget *build_hw_unit7_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je kvantování?",
         {"Měření signálu v pravidelných okamžicích",
          "Zaokrouhlení amplitudy na konečný počet hladin",
          "Převod čísel zpět na analog",
          "Komprese souboru MP3"},
         4, 1},
        {"Kolik hladin má 8bitové kvantování?",
         {"8", "16", "256", "65 536"},
         4, 2},
        {"Kolik hladin má 16bitové kvantování?",
         {"16", "256", "1 024", "65 536"},
         4, 3},
        {"Co je kvantizační chyba?",
         {"Rozdíl mezi skutečnou hodnotou a zvolenou hladinou",
          "Příliš nízká vzorkovací frekvence",
          "Počet kanálů ve stereu",
          "Délka skladby v sekundách"},
         4, 0},
    };
    static const char *hints[] = {
        "Kvantování zaokrouhluje amplitudu na hladiny",
        "8 bitů → 2⁸ = 256 hladin",
        "16 bitů → 2¹⁶ = 65 536 hladin",
        "Kvantizační chyba = rozdíl proti skutečné hodnotě",
    };
    return build_hw_mcq_page(6, "hwunit7", "hw_ex7_title", "hw_quiz7_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit8_page(void) {
    static const NetSlide slides[HW8_SLIDES] = {
        {
            "1 / 2   •   PCM", "Pulzně kódová modulace",
            "Tip: tok = fs × bity na vzorek × počet kanálů.",
            {
                "PCM uloží každý vzorek jako binární číslo.",
                "Je to základní nekomprimovaný záznam (WAV, audio CD).",
                "Datový tok = vzorkovací frekvence × bitová hloubka "
                "× počet kanálů.",
                "Mono má jeden kanál, stereo dva (levý a pravý).",
                NULL,
            },
        },
        {
            "2 / 2   •   Velikost", "Kolik dat zabere CD",
            "Tip: komprese (MP3, AAC) soubor zmenší, ale část informace zahodí.",
            {
                "CD stereo: 44 100 Hz × 16 bit × 2 = 1 411 200 bit/s.",
                "To je 176 400 B/s.",
                "Za minutu je to 10 584 000 B, tedy přes 10 MB.",
                "Delší záznam, vyšší fs, více bitů nebo více kanálů "
                "soubor zvětší.",
                "MP3 a AAC soubor zmenší za cenu ztráty části informace.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[7], "hw_unit8", "hw_unit8_sub",
                               slides, HW8_SLIDES);
}

GtkWidget *build_hw_unit8_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je PCM?",
         {"Každý vzorek uložený jako binární číslo",
          "Jen komprimovaný formát MP3",
          "Filtr před mikrofonem",
          "Jednotka hlasitosti"},
         4, 0},
        {"Jak se spočítá datový tok nekomprimovaného zvuku?",
         {"fs + bity + kanály",
          "fs × bitová hloubka × počet kanálů",
          "jen délka skladby v minutách",
          "počet hladin děleno dvěma"},
         4, 1},
        {"Jaký je datový tok audio CD ve stereu?",
         {"44 100 bit/s", "256 bit/s", "1 411 200 bit/s", "8 bit/s"},
         4, 2},
        {"Co udělá komprese MP3 se záznamem?",
         {"Zvětší soubor a přidá vzorky",
          "Zmenší soubor a část informace zahodí",
          "Změní jen název souboru",
          "Převede digitál zpět na analog"},
         4, 1},
    };
    static const char *hints[] = {
        "PCM = vzorek jako binární číslo",
        "tok = fs × bity × kanály",
        "44 100 × 16 × 2 = 1 411 200 bit/s",
        "MP3 soubor zmenší a část informace zahodí",
    };
    return build_hw_mcq_page(7, "hwunit8", "hw_ex8_title", "hw_quiz8_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit9_page(void) {
    static const NetSlide slides[HW9_SLIDES] = {
        {
            "1 / 2   •   Aliasing", "Když je vzorkování moc řídké",
            "Tip: aliasing vznikne před uložením a z hotových vzorků "
            "už nejde spolehlivě odstranit.",
            {
                "Aliasing vznikne, když je fs příliš nízká a poruší se "
                "Nyquistovo pravidlo.",
                "Vysoká frekvence se v záznamu jeví jako nižší tón.",
                "Příklad: tón 6 kHz vzorkovaný 8 kHz se jeví jako 2 kHz.",
                "Antialiasingový filtr před převodníkem vysoké frekvence "
                "ořízne.",
                NULL,
            },
        },
        {
            "2 / 2   •   Formáty", "Běžné parametry záznamu",
            "Tip: vyšší fs a víc bitů znamená věrnější záznam a větší tok.",
            {
                "Audio CD: 44,1 kHz, 16 bitů, stereo.",
                "Klasický telefonní hovor: 8 kHz a 8 bitů – řeč se vejde "
                "do pásma do 4 kHz.",
                "Studio často používá 48 nebo 96 kHz a 24 bitů.",
                "Parametry se volí podle toho, co má záznam unést "
                "a jak velký smí být.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[8], "hw_unit9", "hw_unit9_sub",
                               slides, HW9_SLIDES);
}

GtkWidget *build_hw_unit9_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdy vzniká aliasing?",
         {"Když je vzorkovací frekvence příliš nízká",
          "Když má záznam 24 bitů",
          "Když je soubor ve stereu",
          "Když se použije D/A převodník"},
         4, 0},
        {"K čemu je antialiasingový filtr?",
         {"Zvětší bitovou hloubku po uložení",
          "Před vzorkováním ořízne frekvence nad Nyquistovu",
          "Spočítá datový tok",
          "Převede mono na stereo"},
         4, 1},
        {"Jaké parametry má audio CD?",
         {"8 kHz, 8 bitů, mono",
          "44,1 kHz, 16 bitů, stereo",
          "96 kHz, 8 bitů, mono",
          "1 kHz, 1 bit, stereo"},
         4, 1},
        {"Proč telefonnímu hovoru stačí vzorkování 8 kHz?",
         {"Řeč se vejde do pásma zhruba do 4 kHz",
          "Telefon nahrává na audio CD",
          "8 kHz je totéž co 44,1 kHz",
          "Řeč nemá žádnou frekvenci"},
         4, 0},
    };
    static const char *hints[] = {
        "Aliasing = příliš nízká fs",
        "Filtr ořízne frekvence nad fs / 2 ještě před vzorkováním",
        "CD = 44,1 kHz, 16 bit, stereo",
        "Pásmo řeči je do asi 4 kHz, proto stačí fs = 8 kHz",
    };
    return build_hw_mcq_page(8, "hwunit9", "hw_ex9_title", "hw_quiz9_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit10_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Účel", "Počítačová skříň",
            "Tip: skříň má být uzavřená, ale s větracími mřížkami.",
            {
                "Skříň zastřešuje a drží ostatní komponenty.",
                "Součásti se propojují kabely nebo přímo do základní desky.",
                "Při provozu vzniká teplo a skříň ho musí odvést.",
                "Uzavřená skříň s mřížkami udržuje správné proudění vzduchu "
                "(airflow).",
                NULL,
            },
        },
        {
            "2 / 2   •   Stěny", "Přední a zadní stěna, desktop a tower",
            "Tip: desktop leží, tower stojí.",
            {
                "Přední stěna: zapnutí, restart, zelená kontrolka chodu "
                "a červená kontrolka disku.",
                "Vpředu bývají i USB porty a audio vstupy a výstupy.",
                "Zadní stěna zpřístupňuje porty desky a přídavných karet.",
                "Volné otvory kryjí záslepky, aby se dovnitř neprášilo.",
                "Desktop je skříň naležato, monitor se na ni pokládá.",
                "Tower stojí nastojato vedle monitoru nebo pod stolem.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[9], "hw_unit10", "hw_unit10_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit10_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"K čemu slouží počítačová skříň?",
         {"Jen jako ozdoba monitoru",
          "Zastřešuje a uchycuje ostatní komponenty",
          "Převádí 230 V na 12 V",
          "Ukládá operační systém"},
         4, 1},
        {"Proč má být skříň uzavřená a mít větrací mřížky?",
         {"Aby se dovnitř vešel jen jeden disk",
          "Aby uvnitř správně proudil vzduch a odvádělo se teplo",
          "Aby nešel zapnout restart",
          "Aby odpadly kabely"},
         4, 1},
        {"Co typicky najdeme na přední stěně?",
         {"Tlačítka, kontrolky, USB a audio",
          "Jen patici procesoru",
          "Jen sloty ISA",
          "Jen plotny pevného disku"},
         4, 0},
        {"Jak se liší desktop a tower?",
         {"Desktop je bez zdroje, tower bez desky",
          "Desktop je jen pro notebooky",
          "Desktop leží, tower stojí",
          "Liší se jen barvou kontrolek"},
         4, 2},
    };
    static const char *hints[] = {
        "Skříň drží a chrání komponenty",
        "Airflow odvádí teplo",
        "Vpředu: power, reset, LED, USB, audio",
        "Desktop naležato, tower nastojato",
    };
    return build_hw_mcq_page(9, "hwunit10", "hw_ex10_title", "hw_quiz10_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit11_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Převod", "Napájecí zdroj",
            "Tip: ze sítě jde střídavých 230 V / 50 Hz.",
            {
                "Zdroj zásobuje všechny části počítače elektřinou.",
                "Převádí střídavé napětí 230 V, 50 Hz na stejnosměrné "
                "3,3 V, 5 V a 12 V.",
                "Nejrozšířenější je standard ATX, včetně variant TFX a SFX.",
                "ATX umí počítač vypnout softwarově přes základní desku.",
                "Výkon se vyrábí v řadě hodnot, třeba od 350 W po 700 W "
                "i víc.",
                NULL,
            },
        },
        {
            "2 / 2   •   Přívody", "Co se napájí přímo a kolik to bere",
            "Tip: spotřeba není stálá.",
            {
                "Přímo ze zdroje: deska, pevné disky, mechaniky, aktivní "
                "chladiče a grafické karty.",
                "Přes desku: rozšiřující karty, některé ventilátory, "
                "procesor, porty a LED kontrolky.",
                "V klidu bere sestava zhruba 100 W.",
                "V plné zátěži může spotřeba dosáhnout 500–600 W.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[10], "hw_unit11", "hw_unit11_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit11_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co zdroj udělá se síťovým napětím?",
         {"Nechá 230 V střídavých všude v počítači",
          "Převede 230 V střídavých na stejnosměrné 3,3 V, 5 V a 12 V",
          "Převede všechno na 230 V stejnosměrných",
          "Vyrábí jen 1,5 V pro USB"},
         4, 1},
        {"Co umožňuje standard ATX?",
         {"Softwarové vypnutí počítače přes základní desku",
          "Jen napájení disketové mechaniky",
          "Zrušení větracích mřížek",
          "Uložení BIOSu na pásku"},
         4, 0},
        {"Co se ze zdroje napájí přímo?",
         {"Jen LED na klávesnici",
          "Jen porty na zadní stěně",
          "Deska, disky, mechaniky, aktivní chladiče a grafické karty",
          "Jen operační paměť"},
         4, 2},
        {"Jak se mění spotřeba počítače?",
         {"Je pořád přesně 350 W",
          "V klidu asi 100 W, v plné zátěži 500–600 W",
          "V klidu 500 W a v zátěži 100 W",
          "Nezávisí na tom, co počítač dělá"},
         4, 1},
    };
    static const char *hints[] = {
        "230 V AC → 3,3 / 5 / 12 V DC",
        "ATX umí softwarové vypnutí",
        "Přímo: deska, disky, mechaniky, chladiče, GPU",
        "Klid cca 100 W, zátěž 500–600 W",
    };
    return build_hw_mcq_page(10, "hwunit11", "hw_ex11_title", "hw_quiz11_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit12_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Páteř", "Základní deska",
            "Tip: deska rozhoduje o kompatibilitě a o tom, co půjde upgradovat.",
            {
                "Je to vícevrstvý plošný spoj, fundament a páteř počítače.",
                "Propojuje procesor se všemi ostatními částmi.",
                "Čipset tvoří dva čipy: severní a jižní most.",
                "Sběrnice jsou měděné cestičky pro data a signály.",
                NULL,
            },
        },
        {
            "2 / 2   •   Čipy", "Patice, mosty a BIOS",
            "Tip: nastavení BIOSu drží čip CMOS na baterii.",
            {
                "Patice (socket) mechanicky drží procesor a elektricky "
                "ho propojí.",
                "Severní most je systémový řadič blízko CPU a řeší rychlé "
                "přesuny: FSB, AGP a paměťovou sběrnici.",
                "Jižní most připojuje pomalejší periferie, BIOS, disky, "
                "USB a porty.",
                "BIOS je mezistupeň mezi hardwarem a softwarem a nese "
                "instrukce pro zavedení operačního systému.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[11], "hw_unit12", "hw_unit12_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit12_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je základní deska?",
         {"Vícevrstvý plošný spoj, páteř počítače",
          "Jen kovový kryt zdroje",
          "Jen ventilátor na procesoru",
          "Externí disk"},
         4, 0},
        {"Z čeho se skládá čipset?",
         {"Ze tří grafických karet",
          "Ze severního a jižního mostu",
          "Jen z baterie CMOS",
          "Jen ze slotu ISA"},
         4, 1},
        {"Co řeší severní most?",
         {"Jen pomalé USB porty",
          "Jen zvukový výstup",
          "Rychlé přesuny dat: FSB, AGP a paměťovou sběrnici",
          "Jen tlačítko reset"},
         4, 2},
        {"Kde je uložené nastavení BIOSu?",
         {"V čipu CMOS napájeném baterií",
          "Jen na papírovém štítku skříně",
          "V mechanice CD",
          "V červeném vodiči zdroje"},
         4, 0},
    };
    static const char *hints[] = {
        "Deska je páteř a rozhoduje o kompatibilitě",
        "Čipset = severní + jižní most",
        "Severní most: FSB, AGP, paměť",
        "BIOS zavádí OS, nastavení drží CMOS",
    };
    return build_hw_mcq_page(11, "hwunit12", "hw_ex12_title", "hw_quiz12_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit13_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Konektory", "Napájení a datové kabely",
            "Tip: F_PANEL je svazek od předního panelu skříně.",
            {
                "Deska bere 24pinový napájecí konektor a procesor "
                "zvlášť 4pinový nebo 8pinový.",
                "Disky a mechaniky se připojují přes SATA.",
                "Starší rozhraní jsou IDE a FDD.",
                "F_PANEL připojuje Power, Reset, LED diody a PC speaker.",
                NULL,
            },
        },
        {
            "2 / 2   •   Sloty", "ISA, PCI, AGP a PCIe",
            "Tip: dnešní rychlé rozhraní je PCI Express.",
            {
                "ISA je zastaralý 16bitový slot.",
                "PCI je nezávislé na CPU, běží na 33 MHz a má propustnost "
                "264 MB/s.",
                "AGP vzniklo pro grafické karty a má přímé spojení s RAM.",
                "PCI Express je sériové, vysokorychlostní a umí Hot Plug.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[12], "hw_unit13", "hw_unit13_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit13_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jakými konektory se deska napájí?",
         {"Jen jedním USB",
          "24 piny pro desku a 4 nebo 8 piny pro CPU",
          "Jen konektorem FDD",
          "Jen slotem ISA"},
         4, 1},
        {"Co se připojuje na F_PANEL?",
         {"Power, Reset, LED a PC speaker",
          "Jen pevný disk",
          "Jen monitor",
          "Jen anténa Wi-Fi"},
         4, 0},
        {"Jaké má parametry sběrnice PCI?",
         {"Sériová, jen pro tiskárny",
          "16bitová a dnes jediná používaná",
          "Nezávislá na CPU, 33 MHz, 264 MB/s",
          "Jen pro procesor"},
         4, 2},
        {"K čemu vzniklo AGP?",
         {"Pro disketové mechaniky",
          "Speciálně pro grafické karty, s přímým spojením k RAM",
          "Jako náhrada napájecího zdroje",
          "Pro pomalé tiskárny"},
         4, 1},
    };
    static const char *hints[] = {
        "24 pinů deska, 4/8 pinů CPU",
        "F_PANEL = tlačítka, LED, speaker",
        "PCI: 33 MHz, 264 MB/s",
        "AGP je pro grafiku a sahá přímo na RAM",
    };
    return build_hw_mcq_page(12, "hwunit13", "hw_ex13_title", "hw_quiz13_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit14_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Dělení", "Vnitřní a vnější paměť",
            "Tip: RAM je rychlá, ale bez napájení data ztratí.",
            {
                "Vnitřní paměť je operační RAM: rychlá, pro rozpracovaná "
                "data, bez napájení se smaže.",
                "Vnější paměť jsou disky, USB a optická média.",
                "Vnější paměť data udrží i bez napájení.",
                "Je ale mnohem pomalejší než RAM.",
                NULL,
            },
        },
        {
            "2 / 2   •   Typy", "SRAM a DRAM",
            "Tip: SRAM je cache v procesoru, DRAM se musí obnovovat.",
            {
                "SRAM (statická) je velmi rychlá a drahá.",
                "Používá se jako cache v procesoru.",
                "DRAM (dynamická) je levnější a data se musí neustále "
                "obnovovat.",
                "Patří sem SDR a řada DDR až po DDR3.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[13], "hw_unit14", "hw_unit14_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit14_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"K čemu slouží operační RAM?",
         {"Pro rozpracovaná data; je rychlá a potřebuje napájení",
          "Pro trvalé uložení filmů bez elektřiny",
          "Jen jako náhrada zdroje",
          "Jen pro větrání skříně"},
         4, 0},
        {"Čím se vnější paměť liší od RAM?",
         {"Je rychlejší a maže se při vypnutí",
          "Drží data i bez napájení, ale je pomalejší",
          "Je vždy jen v procesoru",
          "Nepotřebuje žádný řadič"},
         4, 1},
        {"Kde se používá SRAM?",
         {"Jako jediný pevný disk",
          "Jen v optické mechanice",
          "Jako rychlá a drahá cache v procesoru",
          "Jen na předním panelu"},
         4, 2},
        {"Co platí o DRAM?",
         {"Je levnější a data se musí obnovovat (SDR, DDR až DDR3)",
          "Nikdy se neobnovuje a je dražší než SRAM",
          "Slouží jen jako baterie CMOS",
          "Je to jiný název pro zdroj ATX"},
         4, 0},
    };
    static const char *hints[] = {
        "RAM = rychlá vnitřní paměť, chce napájení",
        "Vnější paměť je stálá a pomalejší",
        "SRAM = cache CPU",
        "DRAM se obnovuje, SDR a DDR až DDR3",
    };
    return build_hw_mcq_page(13, "hwunit14", "hw_ex14_title", "hw_quiz14_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit15_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Mozek", "Procesor",
            "Tip: takt = základní hodiny 100 MHz × násobič.",
            {
                "CPU je hlavní výpočetní jednotka a řídí ostatní části.",
                "Je to křemíkový čip z miliard tranzistorů, které pracují "
                "jako spínače.",
                "Frekvence je počet cyklů za sekundu (Hz, v praxi GHz).",
                "Takt vznikne jako součin base clock (100 MHz) a násobiče.",
                NULL,
            },
        },
        {
            "2 / 3   •   Jádra", "Více jader, vlákna a cache",
            "Tip: cache L1, L2 a L3 zmenšují čekání na pomalejší paměť.",
            {
                "Více jader jsou samostatné jednotky pro souběžnou práci.",
                "Hyper-Threading / SMT nechá jedno jádro zpracovat dvě "
                "vlákna najednou.",
                "Cache je vyrovnávací paměť mezi rychlým jádrem a pomalejšími "
                "součástmi.",
                "Bývá ve třech úrovních: L1, L2 a L3.",
                NULL,
            },
        },
        {
            "3 / 3   •   Teplo", "Chlazení procesoru",
            "Tip: chlazení prodlužuje životnost a dovolí přetaktování.",
            {
                "Chladič odvádí teplo z čipu.",
                "Vzduchové chlazení je pasivní blok a ventilátor.",
                "Je levné a spolehlivé.",
                "Vodní all-in-one vede teplo kapalinou do radiátoru.",
                "U procesoru tak zabere méně místa.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[14], "hw_unit15", "hw_unit15_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit15_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak vzniká takt procesoru?",
         {"Jen z napětí 12 V",
          "Jako součin base clock 100 MHz a násobiče",
          "Jen z počtu USB portů",
          "Z rychlosti ventilátoru ve skříni"},
         4, 1},
        {"Co umí Hyper-Threading / SMT?",
         {"Jedno jádro zpracuje dvě vlákna současně",
          "Vypne chlazení",
          "Zdvojnásobí napětí zdroje",
          "Nahradí operační paměť diskem"},
         4, 0},
        {"K čemu je cache L1, L2 a L3?",
         {"K napájení grafické karty",
          "K uložení BIOSu na baterii",
          "Vyrovnává rychlostní rozdíl mezi procesorem a ostatními částmi",
          "K větrání zadní stěny"},
         4, 2},
        {"Čím se vyznačuje vzduchové chlazení CPU?",
         {"Jen kapalinou bez ventilátoru",
          "Pasivním blokem a ventilátorem; je levné a spolehlivé",
          "Tím, že procesor nehřeje",
          "Tím, že se montuje jen na disk"},
         4, 1},
    };
    static const char *hints[] = {
        "Takt = 100 MHz × násobič",
        "SMT = dvě vlákna na jedno jádro",
        "Cache vyrovnává rychlost",
        "Vzduch: blok + ventilátor",
    };
    return build_hw_mcq_page(14, "hwunit15", "hw_ex15_title", "hw_quiz15_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit16_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Karty", "Přídavné karty",
            "Tip: karta je plošný spoj, který počítači přidá schopnost.",
            {
                "Rozšiřující karta vylepší nebo rozšíří to, co počítač umí.",
                "Typické jsou grafické, zvukové, síťové a televizní karty.",
                "Sloty se vyvíjely, proto karta obvykle nejde zasunout "
                "do libovolného slotu.",
                NULL,
            },
        },
        {
            "2 / 2   •   PCIe", "Kam se karta dává",
            "Tip: grafika chce nejširší slot.",
            {
                "Grafické karty patří do PCI Express ×16.",
                "Ostatní karty se dnes dávají do PCI Express ×1 až ×4.",
                "Číslo za × říká, kolik linek slot kartě dá.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[15], "hw_unit16", "hw_unit16_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit16_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"K čemu je přídavná karta?",
         {"Rozšíří nebo vylepší schopnosti počítače",
          "Nahradí napájecí zdroj",
          "Jen uzavře skříň",
          "Ukládá BIOS"},
         4, 0},
        {"Do jakého slotu patří grafická karta?",
         {"Do ISA", "Do PCI Express ×16", "Do FDD", "Do F_PANEL"},
         4, 1},
        {"Kam se dnes dávají ostatní karty?",
         {"Jen do AGP",
          "Jen na přední stěnu skříně",
          "Do PCI Express ×1 až ×4",
          "Přímo do patice procesoru"},
         4, 2},
        {"Proč nejde karta zasunout do každého slotu?",
         {"Sloty se liší a karta sedí jen do odpovídajícího",
          "Protože všechny sloty jsou stejné",
          "Protože karty nemají kontakty",
          "Protože to zakazuje airflow"},
         4, 0},
    };
    static const char *hints[] = {
        "Karta rozšiřuje schopnosti PC",
        "GPU → PCIe ×16",
        "Ostatní karty → PCIe ×1 až ×4",
        "Slot a karta k sobě musí sedět",
    };
    return build_hw_mcq_page(15, "hwunit16", "hw_ex16_title", "hw_quiz16_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit17_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Obraz", "Z čeho se skládá obraz",
            "Tip: hry skládají scénu hlavně z trojúhelníků.",
            {
                "Digitalizace obrazu je vzorkování, kvantování a kódování.",
                "Jas může mít třeba 256 úrovní, nebo jen 2.",
                "Pixel je bod v obraze.",
                "Polygon je 2D mnohoúhelník; ve hrách často trojúhelník, "
                "protože ho GPU zpracuje nejjednodušeji.",
                "Vertex je vrchol, edge je hrana.",
                "Textura je bitmapa namapovaná na 3D model.",
                "Ray tracing je metoda sledování paprsku.",
                NULL,
            },
        },
        {
            "2 / 3   •   Typy", "Integrovaná a samostatná grafika",
            "Tip: integrovaný čip šetří peníze, samostatná karta přidá výkon.",
            {
                "Integrovaná grafika je čip přímo na základní desce.",
                "Bývá v levnějších a kancelářských sestavách i v noteboocích.",
                "Má nižší výkon a nižší cenu a může brzdit zbytek systému.",
                "Samostatná karta se dává do rozšiřujícího slotu.",
                "Míří na hráče a náročnější uživatele.",
                NULL,
            },
        },
        {
            "3 / 3   •   Cesta", "Co je na kartě a kam jde obraz",
            "Tip: obraz jde z CPU do grafické paměti, pak do GPU a dál na monitor.",
            {
                "Na kartě je GPU, RAMDAC, paměti, sběrnice, porty a často "
                "konektor přídavného napájení (Molex).",
                "GPU mívá aktivní chladič, okolo něj sedí paměťové čipy.",
                "Datový tok začíná v procesoru počítače a jde do grafické "
                "paměti.",
                "GPU data zpracuje a pošle je do převodníku signálu.",
                "Odtud obraz pokračuje do monitoru (framebuffer).",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[16], "hw_unit17", "hw_unit17_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit17_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je pixel?",
         {"Bod v obraze", "Hrana polygonu", "Napájecí konektor",
          "Typ pevného disku"},
         4, 0},
        {"Proč hry často používají trojúhelník?",
         {"Protože má víc hran než čtverec",
          "Pro grafickou kartu je nejjednodušší na zpracování",
          "Protože nepotřebuje texturu",
          "Protože nahradí monitor"},
         4, 1},
        {"Čím se vyznačuje integrovaná grafika?",
         {"Je vždy rychlejší než samostatná karta",
          "Sedí jen v mechanice DVD",
          "Čip je na desce, bývá levnější a má nižší výkon",
          "Nemá vliv na zbytek počítače"},
         4, 2},
        {"Jakou cestou jde obraz?",
         {"CPU → grafická paměť → GPU → převodník → monitor",
          "Monitor → disk → zdroj → skříň",
          "Jen z BIOSu přímo do reproduktoru",
          "Z F_PANEL rovnou na tiskárnu"},
         4, 0},
    };
    static const char *hints[] = {
        "Pixel = bod obrazu",
        "Trojúhelník je pro GPU nejjednodušší",
        "Integrovaná grafika je na desce",
        "CPU → VRAM → GPU → převodník → monitor",
    };
    return build_hw_mcq_page(16, "hwunit17", "hw_ex17_title", "hw_quiz17_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit18_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   GPU", "Grafický procesor",
            "Tip: pro výkon počítače je GPU stejně důležité jako CPU.",
            {
                "GPU přijímá data od CPU a počítá obraz.",
                "Je to složitý čip se stovkami milionů tranzistorů.",
                "Sleduje se takt v MHz, kapacita paměti a počet shaderů.",
                "Dřív se počítaly vertex a pixel shadery, dnes spíš "
                "unifikované, částečně programovatelné jednotky.",
                NULL,
            },
        },
        {
            "2 / 3   •   Programy", "Shader a renderování",
            "Tip: vertex shader posouvá vrcholy, ale nové nevytváří.",
            {
                "Shader je program, který řídí části grafického řetězce.",
                "Renderování tvoří obraz podle počítačového modelu, "
                "nejčastěji 3D.",
                "Render je vizualizace: model se přenese do 2D bitmapy.",
                "Softwarový render počítá procesor. Je přesnější, ale "
                "mnohonásobně pomalejší.",
                "Hardwarový render slouží pro náhledy a hry.",
                "Vertex shader transformuje vrcholy a nové nevytváří.",
                "Pixel shader potom mapuje texturu a přidává efekty, "
                "třeba ray tracing nebo bump mapping.",
                NULL,
            },
        },
        {
            "3 / 3   •   Výpočty", "CUDA a architektura GPU",
            "Tip: většinu čipu tvoří mnoho jednoduchých procesorů.",
            {
                "CUDA je paralelní platforma NVIDIA a nechá GPU počítat "
                "i jiné úlohy než obraz.",
                "Zrychluje třeba lineární algebru, obraz, video a "
                "hluboké učení.",
                "Obdoba u AMD se jmenuje FireStream.",
                "GPU vzniklo pro hry: miliony polygonů a velké textury.",
                "Je stavěné na tisíce vláken, hodně počítání a málo "
                "podmínek a na sekvenční čtení paměti.",
                "Jednoduché skalární procesory jsou sdružené do "
                "streaming multiprocesorů.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[17], "hw_unit18", "hw_unit18_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit18_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je shader?",
         {"Program řídící části grafického řetězce",
          "Napájecí konektor Molex",
          "Typ pevného disku",
          "Větrací mřížka"},
         4, 0},
        {"Co vertex shader nedělá?",
         {"Neposouvá vrcholy",
          "Nevytváří nové vertexy",
          "Nesahá na grafickou kartu",
          "Nepoužívá se ve 3D"},
         4, 1},
        {"Čím se liší softwarový render?",
         {"Je rychlejší, protože ho počítá jen GPU",
          "Umí jen černobíle",
          "Počítá ho CPU, je přesnější a mnohem pomalejší",
          "Nevytvoří bitmapu"},
         4, 2},
        {"Co je CUDA?",
         {"Platforma NVIDIA pro výpočty na GPU",
          "Standard napájecího zdroje",
          "Starý slot ISA",
          "Formát diskety"},
         4, 0},
    };
    static const char *hints[] = {
        "Shader řídí řetězec karty",
        "Vertex shader nové vrcholy nezakládá",
        "Softwarový render = CPU, přesný a pomalý",
        "CUDA je od NVIDIA, u AMD je FireStream",
    };
    return build_hw_mcq_page(17, "hwunit18", "hw_ex18_title", "hw_quiz18_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit19_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Převod", "RAMDAC a grafická paměť",
            "Tip: nižší CAS latency znamená kratší čekání.",
            {
                "RAMDAC převádí digitální data na analogová.",
                "U dnešních karet je uvnitř GPU a má přednost při čtení "
                "paměti.",
                "Okolo GPU jsou grafické paměti, často DDR nebo GDDR5.",
                "Hlavní parametry jsou kapacita a frekvence.",
                "CAS latency je čekání, než se po zadání adresy sloupce "
                "data objeví na pinech.",
                "Čím nižší latence, tím lépe. Časy paměti se udávají "
                "v nanosekundách.",
                NULL,
            },
        },
        {
            "2 / 3   •   Spojení", "Sběrnice grafické karty",
            "Tip: PCIe 4.0 má na jedné lince 2 GB/s.",
            {
                "Typická sběrnice karty je PCI Express ×16, dřív to bylo AGP.",
                "PCIe 4.0 zdvojnásobí propustnost proti předchozí verzi "
                "při stejném počtu linek.",
                "Rychlost jedné linky stoupla z 1 GB/s na 2 GB/s.",
                "Slot ×16 tak teoreticky dává 32 GB/s.",
                "M.2 se čtyřmi linkami dává 8 GB/s.",
                NULL,
            },
        },
        {
            "3 / 3   •   Výstupy", "Porty na kartě",
            "Tip: stejný obraz může odejít analogově i digitálně.",
            {
                "Běžné výstupy jsou D-Sub, DVI, HDMI a DisplayPort.",
                "Starší karty mají i TV výstup, S-Video nebo cinch.",
                "Výrobci parametry přikrášlují, proto se karty srovnávají "
                "benchmarkem.",
                "Známý test je 3DMark. Podíl má i procesor, rozhoduje ale "
                "grafická karta.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[18], "hw_unit19", "hw_unit19_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit19_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co dělá RAMDAC?",
         {"Převádí analogový signál na digitální",
          "Převádí digitální data na analogová a dnes je v GPU",
          "Počítá jen cache procesoru",
          "Formátuje pevný disk"},
         4, 1},
        {"Co znamená nižší CAS latency?",
         {"Data se objeví dřív, takže je to lepší",
          "Karta má míň portů",
          "Sběrnice je pomalejší",
          "Monitor má nižší rozlišení"},
         4, 0},
        {"Jakou teoretickou propustnost má PCIe 4.0 ×16?",
         {"2 GB/s", "8 GB/s", "32 GB/s", "264 MB/s"},
         4, 2},
        {"Co se stalo s rychlostí jedné linky u PCIe 4.0?",
         {"Klesla z 2 GB/s na 1 GB/s",
          "Stoupla z 1 GB/s na 2 GB/s",
          "Zůstala 133 MB/s",
          "Záleží jen na barvě slotu"},
         4, 1},
    };
    static const char *hints[] = {
        "RAMDAC = digital → analog, dnes v GPU",
        "Nižší CAS = kratší čekání",
        "PCIe 4.0 ×16 = 32 GB/s",
        "Jedna linka: 1 GB/s → 2 GB/s",
    };
    return build_hw_mcq_page(18, "hwunit19", "hw_ex19_title", "hw_quiz19_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit20_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Více karet", "SLI, CrossFire a režimy",
            "Tip: první režim IBM PC byl v roce 1982 MDA.",
            {
                "SLI od NVIDIA spojí dvě grafické karty do jednoho celku.",
                "Vychází levněji než jedna špičková karta.",
                "Obdoba u ATI se jmenuje CrossFire.",
                "Režim určuje rozlišení a barevnou hloubku.",
                "Dnešní karty zvládnou i 3200×2400 (QUXGA) při 32 bitech.",
                NULL,
            },
        },
        {
            "2 / 2   •   Názvy", "720p, 1080p, 4K a 8K",
            "Tip: 4K má čtyřikrát víc pixelů než 1080p, 8K šestnáctkrát.",
            {
                "720p je 1280×720, tedy HD nebo HD Ready.",
                "1080p je 1920×1080, tedy Full HD.",
                "1440p je 2560×1440, tedy QHD, čtyřnásobek 720p.",
                "4K (2160p) je 3840×2160, čtyřnásobek Full HD.",
                "8K (4320p) je 7680×4320, šestnáctinásobek Full HD.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[19], "hw_unit20", "hw_unit20_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit20_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je 1080p?",
         {"1920×1080, Full HD",
          "1280×720, HD Ready",
          "7680×4320, 8K",
          "3200×2400, QUXGA"},
         4, 0},
        {"Co je 4K?",
         {"1280×720",
          "3840×2160, čtyřnásobek pixelů Full HD",
          "2560×1440",
          "640×480"},
         4, 1},
        {"Kolikrát víc pixelů má 8K proti 1080p?",
         {"Dvakrát", "Čtyřikrát", "Šestnáctkrát", "Stejně"},
         4, 2},
        {"Co je SLI a co je CrossFire?",
         {"SLI spojuje karty NVIDIA, CrossFire je obdoba ATI",
          "Obojí je typ paměti DDR",
          "SLI je port HDMI, CrossFire je D-Sub",
          "Jsou to dva názvy stejné firmy"},
         4, 0},
    };
    static const char *hints[] = {
        "1080p = 1920×1080",
        "4K = 3840×2160",
        "8K má 16× víc pixelů než 1080p",
        "SLI = NVIDIA, CrossFire = ATI",
    };
    return build_hw_mcq_page(19, "hwunit20", "hw_ex20_title", "hw_quiz20_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit21_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Výstup", "Zvuková karta",
            "Tip: digitalizace zvuku je vzorkování, kvantování a kódování.",
            {
                "Karta zajistí zvukový výstup.",
                "Převede digitální signál na analogový, který zahraje "
                "reproduktor.",
                "Rozlišujeme integrované, interní a externí karty.",
                "CD má 44,1 kHz a bitovou hloubku 16 až 24 bitů.",
                NULL,
            },
        },
        {
            "2 / 3   •   Provedení", "Integrovaná, interní, externí a herní",
            "Tip: externí karta se připojuje přes USB a sedí mimo skříň.",
            {
                "Integrovaný zvuk je na téměř každé desce a na multimédia "
                "stačí.",
                "Má slabší zesílení a méně konektorů.",
                "Interní karta jde do PCIe a nabízí prostorový zvuk "
                "a software pro nastavení.",
                "Externí karta se hodí, když integrovaná nestačí, i do "
                "notebooku.",
                "Herní karta, interní nebo externí, drží prostorový zvuk, "
                "malou odezvu a přesnou reprodukci.",
                NULL,
            },
        },
        {
            "3 / 3   •   Čip", "DSP a nahrávání",
            "Tip: kondenzátorový mikrofon chce fantomové napájení 48 V.",
            {
                "Zvukový čip má jádra, D/A a A/D převodníky, sluchátkový "
                "zesilovač a vstupy i výstupy.",
                "DSP stojí na harvardské architektuře: paměť programu je "
                "oddělená od paměti dat, na rozdíl od von Neumannova modelu.",
                "Počet vstupů má odpovídat počtu nástrojů nahrávaných najednou.",
                "Na zpěv stačí jeden analogový vstup.",
                "Některé nástroje, třeba keyboard, chtějí vysokoimpedanční "
                "vstup Hi-Z.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[20], "hw_unit21", "hw_unit21_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit21_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co zvuková karta dělá se signálem?",
         {"Převede digitální signál na analogový pro reproduktor",
          "Převede jen obraz na texturu",
          "Nahradí procesor",
          "Ukládá data na pásku"},
         4, 0},
        {"Jak se připojuje interní zvuková karta?",
         {"Jen přes VGA", "Přes slot PCIe", "Jen přes FDD", "Přes baterii CMOS"},
         4, 1},
        {"Jak se připojuje externí zvuková karta?",
         {"Do patice procesoru", "Do slotu AGP", "Přes USB, mimo skříň",
          "Jen na přední LED"},
         4, 2},
        {"K čemu je fantomové napájení 48 V?",
         {"Pro kondenzátorové mikrofony",
          "Pro vypnutí zdroje ATX",
          "Pro chlazení procesoru",
          "Pro slot ISA"},
         4, 0},
    };
    static const char *hints[] = {
        "Digitál → analog pro reproduktor",
        "Interní karta sedí v PCIe",
        "Externí karta jde přes USB",
        "48 V je fantom pro kondenzátorový mikrofon",
    };
    return build_hw_mcq_page(20, "hwunit21", "hw_ex21_title", "hw_quiz21_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit22_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Síť", "Síťová karta",
            "Tip: dnes nemusí jít jen o kabel, umí i Wi-Fi.",
            {
                "Integrovaná karta má ethernetový konektor přímo na desce.",
                "Interní karta kombinuje kabel a Wi-Fi a jde do PCIe.",
                "Externí karta se strčí do USB.",
                "Wake on LAN umí počítač zapnout po datové síti.",
                "Externí anténa zvětší dosah.",
                "MIMO použije víc antén a zvedne propustnost.",
                "WPS zjednoduší nastavení bezdrátového spojení.",
                NULL,
            },
        },
        {
            "2 / 2   •   Televize", "Televizní karta",
            "Tip: tuner na základní desce integrovaný nebývá.",
            {
                "Televizní tuner na desce integrovaný není nikdy.",
                "Přidá počítači příjem televizního signálu.",
                "Umí signál i zaznamenat.",
                "Je to další rozšiřující karta, ne součást čipsetu.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[21], "hw_unit22", "hw_unit22_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit22_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kde je integrovaná síťová karta?",
         {"Ethernetový konektor přímo na desce",
          "Jen v optické mechanice",
          "Jen jako externí disk",
          "V patici procesoru"},
         4, 0},
        {"Co je Wake on LAN?",
         {"Vypnutí monitoru při nečinnosti",
          "Zapnutí počítače přes datovou síť",
          "Zrychlení pevného disku",
          "Jiný název pro BIOS"},
         4, 1},
        {"Co přináší MIMO?",
         {"Jednu anténu a nižší dosah",
          "Jen kabelový konektor",
          "Více antén a vyšší propustnost",
          "Zrušení Wi-Fi"},
         4, 2},
        {"Jak je to s televizní kartou na desce?",
         {"Není integrovaná nikdy",
          "Je na každé desce ATX",
          "Nahrazuje zvukovou kartu",
          "Je součást severního mostu"},
         4, 0},
    };
    static const char *hints[] = {
        "Integrovaná síťovka = LAN na desce",
        "Wake on LAN zapne počítač po síti",
        "MIMO = víc antén, vyšší propustnost",
        "TV tuner na desce integrovaný nebývá",
    };
    return build_hw_mcq_page(21, "hwunit22", "hw_ex22_title", "hw_quiz22_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit23_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Trvalá data", "Vnější paměti a HDD",
            "Tip: HDD je mechanický, otřes ho může poškodit.",
            {
                "Vnější paměť drží data dlouhodobě, má kapacitu v terabajtech "
                "a je energeticky nezávislá.",
                "Patří sem HDD, SSD a USB flash disky.",
                "Pevný disk zaznamenává data magneticky.",
                "Notebookové disky mají 2,5 palce, stolní 3,5 palce.",
                NULL,
            },
        },
        {
            "2 / 3   •   Čísla", "Parametry pevného disku",
            "Tip: běžné otáčky jsou 5400 a 7200 za minutu.",
            {
                "Kapacita se udává v terabajtech.",
                "Přístupová doba je v milisekundách.",
                "Přenosová rychlost je ve stovkách MB/s.",
                "Cache bývá třeba 256 MB.",
                NULL,
            },
        },
        {
            "3 / 3   •   Stopy", "Zápis, čtení a struktura",
            "Tip: hlavy se ploten nedotýkají, vznášejí se na vzduchu.",
            {
                "Zápis dělá cívka magnetickým polem.",
                "Čtení používá elektromagnetickou indukci.",
                "Disk je hermeticky uzavřený.",
                "Formátování vytvoří stopy a sektory.",
                "Cylindr jsou stejné stopy na všech plotnách.",
                "Defragmentace složí rozházená data k sobě, aby se četla "
                "rychleji.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[22], "hw_unit23", "hw_unit23_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit23_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Na jakém principu pracuje HDD?",
         {"Na magnetickém záznamu a je mechanický",
          "Jen na optickém laseru",
          "Nemá pohyblivé části",
          "Drží data jen při zapnutém napájení"},
         4, 0},
        {"Jaké rozměry mají pevné disky?",
         {"Jen 8 palců",
          "2,5\" do notebooku a 3,5\" do stolního počítače",
          "12 cm jako CD",
          "Jen formát microSD"},
         4, 1},
        {"Jaké otáčky se u HDD běžně uvádějí?",
         {"100 a 200 ot/min", "1000 ot/min", "5400 a 7200 ot/min",
          "48 000 ot/min"},
         4, 2},
        {"Jak disk zapisuje a čte?",
         {"Zápis cívkou, čtení elektromagnetickou indukcí",
          "Zápis laserem, čtení ventilátorem",
          "Obojí jen přes BIOS",
          "Zápis do CMOS, čtení z baterie"},
         4, 0},
    };
    static const char *hints[] = {
        "HDD = magnetický a mechanický",
        "2,5\" notebook, 3,5\" desktop",
        "5400 a 7200 ot/min",
        "Zápis = magnetické pole, čtení = indukce",
    };
    return build_hw_mcq_page(22, "hwunit23", "hw_ex23_title", "hw_quiz23_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit24_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Bez pohybu", "SSD",
            "Tip: fragmentace u SSD problém není.",
            {
                "SSD je polovodičový disk bez pohyblivých částí.",
                "Je odolnější, tiché, úspornější a rychlejší než HDD.",
                "Buňky se opotřebovávají, počet zápisů je omezený.",
                "Bývá dražší než plotnový disk stejné kapacity.",
                NULL,
            },
        },
        {
            "2 / 3   •   Buňky", "FTL, TRIM a záchrana",
            "Tip: smazání smaže metadata. U SSD TRIM záchranu ztěžuje.",
            {
                "FTL rovnoměrně rozkládá zápisy, aby se buňky sjížděly stejně.",
                "TRIM řekne disku, které bloky už nikdo nepoužívá, a zápis "
                "se tím zrychlí.",
                "Poškození může být hardwarové (mechanika, elektronika, "
                "oheň, voda) nebo softwarové (smazání, formát, virus).",
                "Smazání maže metadata, obsah na HDD často zůstane a dá "
                "se obnovit.",
                "U SSD je obnova kvůli TRIM obtížnější.",
                NULL,
            },
        },
        {
            "3 / 3   •   Přenos", "SSHD a USB flash",
            "Tip: konektor USB flash disku je počítaný asi na 1500 cyklů.",
            {
                "SSHD je pevný disk s malou SSD částí, zhruba 8 GB.",
                "Často používané soubory si přesune na rychlou část.",
                "Dává smysl, když chcete kapacitu i rychlost za rozumnou cenu.",
                "USB flash je malé médium na přenos dat po sběrnici USB.",
                "Životnost buněk záleží na typu SLC, MLC nebo TLC a na "
                "kvalitě hardwaru.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[23], "hw_unit24", "hw_unit24_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit24_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je hlavní výhoda SSD proti HDD?",
         {"Má plotny a je hlučnější",
          "Nemá pohyblivé části, je tiché a rychlejší",
          "Data ztratí při vypnutí",
          "Nejde ho použít jako systémový disk"},
         4, 1},
        {"Jaká je nevýhoda SSD?",
         {"Buňky se opotřebovávají a disk bývá dražší",
          "Musí se defragmentovat každý den",
          "Nepřežije přesun po stole",
          "Nemá TRIM ani FTL"},
         4, 0},
        {"Co dělá příkaz TRIM?",
         {"Zvýší otáčky ploten",
          "Smaže BIOS",
          "Oznámí SSD, které bloky už nejsou použité",
          "Zapne fantomové napájení"},
         4, 2},
        {"Co je SSHD?",
         {"Jen jiný název pro disketu",
          "HDD doplněné o malou SSD část, asi 8 GB",
          "Optický disk s modrým laserem",
          "Paměťová karta SD"},
         4, 1},
    };
    static const char *hints[] = {
        "SSD = bez pohyblivých částí",
        "Buňky se sjíždějí a cena je vyšší",
        "TRIM hlásí volné bloky",
        "SSHD = HDD + asi 8 GB SSD",
    };
    return build_hw_mcq_page(23, "hwunit24", "hw_ex24_title", "hw_quiz24_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit25_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Přehled", "Výměnná média",
            "Tip: páska se musí převinout, proto se hodí na archiv, ne na denní práci.",
            {
                "Mechanická média: děrné štítky a děrné pásky.",
                "Magnetická: pásky a diskety.",
                "Optická: CD, DVD a Blu-ray.",
                "Elektrická: paměťové karty a USB flash.",
                "K souboru na pásce nejde skočit, páska se na místo přetočí.",
                NULL,
            },
        },
        {
            "2 / 3   •   Pásky", "Magnetická páska",
            "Tip: páska pořád slouží velkým firmám na dlouhodobou zálohu.",
            {
                "Jedna raná páska unesla data asi deseti tisíc děrných štítků.",
                "Kapacita začínala u stovek kilobajtů, třeba 850 kB.",
                "Dlouho měla vyšší kapacitu než pevné disky; zlom přišel "
                "kolem roku 2010 s terabajtovými disky.",
                "Podle IBM ji v USA používá přímo nebo nepřímo až 82 % "
                "velkých firem.",
                "Odhad je stovky miliard gigabajtů dat na páskách po světě.",
                "Sklad CSCS v Curychu má 27 PB na 17 000 páskách LTO-5 "
                "po 1500 GB.",
                NULL,
            },
        },
        {
            "3 / 3   •   Diskety", "Z pásky na kotouč",
            "Tip: ikona ukládání je 3,5\" disketa.",
            {
                "Stopa se přesunula na otáčející se mylarový kotouč.",
                "Soustředné stopy umožní náhodný přístup, ne jen převíjení.",
                "8\" disketa (1971) měla 100 kB a zápis z jedné strany.",
                "5,25\" (1976) se dala poslat poštou; kapacita 80 až 1200 kB.",
                "3,5\" zavedla Sony v roce 1981.",
                "Standard 1990–1995 je 1,44 MB, pevný obal a dvířka, "
                "vloží se jen správně.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[24], "hw_unit25", "hw_unit25_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit25_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Proč se páska hodí na archiv a ne na běžnou práci?",
         {"K souboru se musí převinout, přístup není náhodný",
          "Nemá žádnou kapacitu",
          "Nejde ji vyjmout z mechaniky",
          "Maže se při každém čtení"},
         4, 0},
        {"Jaká byla raná 8\" disketa?",
         {"Z roku 1981 a měla 1,44 MB",
          "Z roku 1971 a měla 100 kB",
          "Z roku 2010 a měla 1 TB",
          "Byla optická"},
         4, 1},
        {"Co platí o 3,5\" disketě?",
         {"Zavedla ji IBM v roce 1960 a měla 850 kB",
          "Neměla pevný obal",
          "Sony, 1981, standardně 1,44 MB",
          "Šla vložit oběma směry"},
         4, 2},
        {"Čím disketa zrychlila přístup proti pásce?",
         {"Soustředné stopy dovolí náhodný přístup",
          "Má modrý laser",
          "Nemá magnetickou vrstvu",
          "Čte se jen od konce"},
         4, 0},
    };
    static const char *hints[] = {
        "Páska je sekvenční",
        "8\" = 1971, 100 kB",
        "3,5\" = Sony 1981, 1,44 MB",
        "Stopy na kotouči = náhodný přístup",
    };
    return build_hw_mcq_page(24, "hwunit25", "hw_ex25_title", "hw_quiz25_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit26_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Laser", "Jak se čte optický disk",
            "Tip: land odrazí paprsek (1), pit ho rozptýlí (0).",
            {
                "Kotouč má průměr 12 cm a tloušťku 1,2 mm.",
                "Data leží ve spirále od středu ke kraji.",
                "U 700MB CD je stopa dlouhá 6,244 km.",
                "Spirála střídá pity (prohlubně) a landy (plochy).",
                NULL,
            },
        },
        {
            "2 / 3   •   Generace", "CD, DVD a Blu-ray",
            "Tip: Blu-ray používá modrý laser 405 nm, DVD červený 650 nm.",
            {
                "CD má 650–870 MB. CD-R vypálí organické barvivo asi při "
                "290 °C a zápis je nevratný.",
                "CD-RW jde přepsat tisíckrát až stotisíckrát. Slitina "
                "AgInSbTe mění krystaly zahřátím a chlazením.",
                "DVD má 1,4 až 15,82 GB, stopu 11,84 km a umí dvě vrstvy "
                "i obě strany.",
                "Blu-ray má 25–128 GB. Kratší vlnová délka znamená hustší "
                "záznam.",
                "Optika ustoupila kvůli kapacitě, pomalému zápisu, "
                "spolehlivosti a tomu, že obsah je online.",
                NULL,
            },
        },
        {
            "3 / 3   •   Karty", "Flash karty",
            "Tip: NAND paměť vytlačila optické mechaniky.",
            {
                "Flash je statická vnější paměť: malá, s velkou kapacitou, "
                "odolná vůči vibracím a magnetickému poli.",
                "Buňky mají plovoucí hradlo: SLC, MLC, TLC nebo QLC.",
                "Karty: SD (i mini a micro), MMC, Memory Stick a "
                "CompactFlash.",
                "Čtečka na USB mívá víc slotů, aby vzala víc formátů.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[25], "hw_unit26", "hw_unit26_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit26_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co se stane, když laser trefí land a když trefí pit?",
         {"Obojí je signál 0",
          "Land se odrazí jako 1, pit se rozptýlí jako 0",
          "Land je 0 a pit je 1",
          "Laser se na disku nepoužívá"},
         4, 1},
        {"Jak se zapisuje CD-R?",
         {"Vypálením barviva, zápis je nevratný",
          "Magnetickou cívkou jako HDD",
          "Libovolně mnohokrát jako RAM",
          "Jen změnou názvu souboru"},
         4, 0},
        {"Čím se laser Blu-ray liší od DVD?",
         {"Má červený laser 650 nm a menší kapacitu",
          "Nemá spirálu",
          "Má modrý laser 405 nm a kapacitu 25–128 GB",
          "Má stejný laser jako CD"},
         4, 2},
        {"Které karty patří mezi flash média?",
         {"Jen děrné štítky",
          "SD, MMC, Memory Stick a CompactFlash",
          "Jen 8\" diskety",
          "Jen pásky LTO"},
         4, 1},
    };
    static const char *hints[] = {
        "Land = odraz = 1, pit = rozptyl = 0",
        "CD-R pálí barvivo a nejde vrátit",
        "Blu-ray: 405 nm, 25–128 GB",
        "SD, MMC, MS, CF",
    };
    return build_hw_mcq_page(25, "hwunit26", "hw_ex26_title", "hw_quiz26_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit27_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Vstup", "Polohovací zařízení a klávesnice",
            "Tip: klávesnice má šest částí.",
            {
                "Polohovací zařízení hýbe kurzorem a bývá drátové i bezdrátové.",
                "Hlavní písmenková část.",
                "Speciální a české znaky.",
                "Ovládací tlačítka a posunovací tlačítka.",
                "Numerická klávesnice a funkční klávesy.",
                NULL,
            },
        },
        {
            "2 / 2   •   Typy", "Membrána, guma, nůžky a laser",
            "Tip: scissor-switch je notebookový, mělký a tichý.",
            {
                "Membránová je nejčastější u stolních počítačů. Stisk "
                "propojí membránu a je velmi tichá.",
                "U vodivé gumy se guma prohne ke kontaktu a sepne obvod. "
                "Pod ní je izolační destička a pozlacený kontakt.",
                "Scissor-switch nemá gumovou membránu. Klávesa se propadne "
                "po nůžkovém mechanismu.",
                "Laserová klávesnice promítá rozložení na podložku a senzor "
                "snímá stisk. Hodí se k mobilu.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[26], "hw_unit27", "hw_unit27_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit27_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Z kolika částí se skládá běžná klávesnice?",
         {"Ze šesti", "Ze dvou", "Z dvanácti", "Jen z numerického bloku"},
         4, 0},
        {"Jaká je membránová klávesnice?",
         {"Nejhlučnější a jen pro notebooky",
          "Nejpoužívanější u stolních počítačů a velmi tichá",
          "Promítá se laserem",
          "Nemá žádný kontakt"},
         4, 1},
        {"Co je scissor-switch?",
         {"Gumová membrána ve stolním PC",
          "Laser na podložce",
          "Notebookový nůžkový mechanismus, mělký a tichý stisk",
          "Jen funkční klávesy"},
         4, 2},
        {"Jak pracuje laserová klávesnice?",
         {"Promítne rozložení a senzor snímá virtuální stisk",
          "Používá vodivou gumu",
          "Je jen mechanická s nejhlubším stiskem",
          "Čte jen numerický blok"},
         4, 0},
    };
    static const char *hints[] = {
        "Šest částí: písmena, znaky, ovládání, posun, numerická, funkce",
        "Membrána = nejčastější stolní, tichá",
        "Scissor = nůžky, notebook",
        "Laser promítá klávesy na podložku",
    };
    return build_hw_mcq_page(26, "hwunit27", "hw_ex27_title", "hw_quiz27_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit28_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Kulička", "Kuličková myš",
            "Tip: pohyb se měří prosvícením otvorů v kotoučcích.",
            {
                "Posuv otáčí kuličkou.",
                "Kulička točí válečky pro osy x a y.",
                "Kotoučky na válečcích mají otvory.",
                "Infračervené diody otvory prosvěcují.",
                "Senzory záblesky převedou na polohu.",
                NULL,
            },
        },
        {
            "2 / 3   •   Prostor", "Spaceball, trackball, tablet a digitizér",
            "Tip: tablet pozná tlak, až ve stovkách až 1024 úrovních.",
            {
                "Spaceball má pohyb a rotaci po třech osách. V CAD a "
                "grafice zvedá efektivitu zhruba o 30–50 %.",
                "Trackball je jako obrácená kuličková myš. Na rychlý "
                "a přesný pohyb se nehodí.",
                "Tablet má síť vodičů, která reaguje na pero. Hodí se "
                "do CAD a grafiky, kreslit jde i na LCD.",
                "Digitizér je speciální myš na přesnou polohu a chce "
                "speciální podložku. Měří odpor mezi ukazovátkem a podložkou.",
                NULL,
            },
        },
        {
            "3 / 3   •   Notebook", "Trackpoint a touchpad",
            "Tip: trackpoint sedí mezi klávesami G, H a B.",
            {
                "Trackpoint je malý joystick a šetří místo.",
                "Touchpad je nejčastější polohovací zařízení notebooku.",
                "Snímá elektrickou kapacitu prstu.",
                "Hotspoty na ploše mají zvláštní účel, třeba posuvník.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[27], "hw_unit28", "hw_unit28_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit28_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak kuličková myš pozná pohyb?",
         {"Laserem na podložce bez kuličky",
          "Kulička točí válečky a infračervené diody prosvěcují otvory",
          "Jen kapacitou prstu",
          "Měřením 48 V"},
         4, 1},
        {"Kde je trackpoint?",
         {"Mezi klávesami G, H a B",
          "Na zadní stěně skříně",
          "V optické mechanice",
          "Jen u stolní myši"},
         4, 0},
        {"Jak touchpad snímá prst?",
         {"Kuličkou a válečky",
          "Magnetickou páskou",
          "Elektrickou kapacitou prstu",
          "Fantomovým napájením"},
         4, 2},
        {"Co umí grafický tablet?",
         {"Jen zapnout počítač",
          "Reaguje na pero a tlak, až 1024 úrovní",
          "Nahradí napájecí zdroj",
          "Čte jen děrné štítky"},
         4, 1},
    };
    static const char *hints[] = {
        "Kulička, válečky, IR diody",
        "Trackpoint je mezi G, H a B",
        "Touchpad měří kapacitu",
        "Tablet: pero a tlak až 1024 úrovní",
    };
    return build_hw_mcq_page(27, "hwunit28", "hw_ex28_title", "hw_quiz28_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit29_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Hry", "Gamepad, joystick a volant",
            "Tip: force feedback přidá vibrace a nárazy.",
            {
                "Gamepad ovládá hry a mívá programovatelná tlačítka.",
                "DualShock přidává vibrace.",
                "Joystick je páka na základně. Dřív byl analogový, "
                "dnes digitální.",
                "Používá se i v letadlech nebo bagrech.",
                "Volant napodobuje řízení: pedály, řadící páka a "
                "programovatelná tlačítka.",
                NULL,
            },
        },
        {
            "2 / 3   •   Tělo", "Kinect",
            "Tip: sleduje celé tělo ve 3D, ne jen ovladač v ruce.",
            {
                "Kombinuje RGB kameru, hloubkové čidlo, mikrofony a "
                "vlastní procesor.",
                "Sleduje pohyb celého těla v prostoru.",
                "Reaguje na pokyny a pozná i změnu zabarvení hlasu a emoce.",
                NULL,
            },
        },
        {
            "3 / 3   •   Displej", "Odporová a kapacitní vrstva",
            "Tip: kapacitního displeje se musíte dotknout vodivě, třeba prstem.",
            {
                "Odporová vrstva má tvrzenou a vodivou vrstvu, odporové "
                "vrstvy, vymezovací bodovou síť a sklo.",
                "Dotyk vrstvy přitlačí k sobě a obvod se sepne.",
                "Kapacitní displej má dvě vodivé vrstvy pod napětím, "
                "oddělené mezerou.",
                "Chovají se jako kondenzátor a kolem sebe mají slabé pole.",
                "Změna kapacity prozradí místo dotyku.",
                "Funguje to s vodivým předmětem, ne s libovolným plastem.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[28], "hw_unit29", "hw_unit29_sub",
                               slides, G_N_ELEMENTS(slides));
}

GtkWidget *build_hw_unit29_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co přidává DualShock?",
         {"Vibrace gamepadu",
          "Optickou mechaniku",
          "Slot PCIe",
          "Magnetickou pásku"},
         4, 0},
        {"Co je force feedback?",
         {"Jen jiné rozlišení monitoru",
          "Vibrace a nárazy u joysticku a volantu",
          "Typ paměti DDR3",
          "Způsob formátování disku"},
         4, 1},
        {"Co Kinect sleduje?",
         {"Jen stisk jedné klávesy",
          "Jen otáčky pevného disku",
          "Pohyb celého těla ve 3D, pomocí kamery, hloubky a mikrofonů",
          "Jen napětí zdroje"},
         4, 2},
        {"Čím se musíte dotknout kapacitního displeje?",
         {"Vodivým předmětem, například prstem",
          "Jen nevodivým plastem",
          "Jen speciálním perem bez elektřiny",
          "Děrnou páskou"},
         4, 0},
    };
    static const char *hints[] = {
        "DualShock = vibrace",
        "Force feedback = vibrace a nárazy",
        "Kinect: RGB, hloubka, mikrofony, tělo ve 3D",
        "Kapacitní vrstva chce vodivý dotyk",
    };
    return build_hw_mcq_page(28, "hwunit29", "hw_ex29_title", "hw_quiz29_head",
                             qs, hints, 4);
}
