#include "maturita.h"

/* ------------------------------------------------------------------ */
/* Roadmap page                                                       */
/* ------------------------------------------------------------------ */

void draw_node_halo(cairo_t *cr, double cx, double cy,
                           double radius, double r, double g, double b,
                           double alpha) {
    cairo_pattern_t *grad;
    grad = cairo_pattern_create_radial(cx, cy, 6.0, cx, cy, radius);
    cairo_pattern_add_color_stop_rgba(grad, 0.0, r, g, b, alpha);
    cairo_pattern_add_color_stop_rgba(grad, 0.55, r, g, b, alpha * 0.30);
    cairo_pattern_add_color_stop_rgba(grad, 1.0, r, g, b, 0.0);
    cairo_set_source(cr, grad);
    cairo_paint(cr);
    cairo_pattern_destroy(grad);
}

/* Draw the checkered "finish line" node: checkerboard behind the unit
 * number and the same lock glyph the locked units use. */
void draw_finish_cell(GtkDrawingArea *area, cairo_t *cr,
                             int width, int height, gpointer data) {
    const double s = 11.0;
    const Rgb gray = color_from_hex(app_theme.overlay);
    const Rgb fin_dark = color_from_hex(app_theme.finish_dark);
    const Rgb fin_light = color_from_hex(app_theme.finish_light);
    const Rgb fin_stroke = color_from_hex(app_theme.finish_stroke);
    int n = GPOINTER_TO_INT(data);
    double cx = width / 2.0;
    double cy = height / 2.0;
    double r = MIN(width, height) / 2.0 - 2.0;
    double start_x = cx - r;
    double start_y = cy - r;
    int cols = (int)ceil(2.0 * r / s);
    int rows = (int)ceil(2.0 * r / s);
    char buf[8];
    PangoLayout *layout;
    PangoFontDescription *fd;
    PangoRectangle ink;
    double num_cy;
    int ix, iy;

    (void)area;

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

    cairo_save(cr);
    cairo_new_path(cr);
    cairo_arc(cr, cx, cy, r, 0.0, 2.0 * G_PI);
    cairo_clip(cr);

    cairo_rectangle(cr, start_x, start_y, 2.0 * r, 2.0 * r);
    cairo_set_source_rgb(cr, fin_dark.r, fin_dark.g, fin_dark.b);
    cairo_fill(cr);

    for (iy = 0; iy < rows; iy++) {
        for (ix = 0; ix < cols; ix++) {
            if (((ix + iy) & 1) == 0)
                continue;
            cairo_rectangle(cr, start_x + ix * s, start_y + iy * s, s, s);
            cairo_set_source_rgb(cr, fin_light.r, fin_light.g, fin_light.b);
            cairo_fill(cr);
        }
    }

    /* unit number, styled like the other locked nodes */
    g_snprintf(buf, sizeof(buf), "%d", n);
    layout = pango_cairo_create_layout(cr);
    fd = pango_font_description_new();
    pango_font_description_set_family(fd, "sans");
    pango_font_description_set_weight(fd, PANGO_WEIGHT_ULTRABOLD);
    pango_font_description_set_size(fd, 22 * PANGO_SCALE);
    pango_layout_set_font_description(layout, fd);
    pango_layout_set_text(layout, buf, -1);
    pango_layout_get_pixel_extents(layout, &ink, NULL);

    cairo_set_source_rgb(cr, gray.r, gray.g, gray.b);
    num_cy = cy - 3.0;
    cairo_move_to(cr, cx - ink.width / 2.0 - ink.x,
                  num_cy - ink.height / 2.0 - ink.y);
    pango_cairo_show_layout(cr, layout);

    /* lock glyph below the number, as on the other locked units */
    lock_paint(cr, cx - 8.0, num_cy + ink.height / 2.0 + 6.0, 16.0, &gray);

    g_object_unref(layout);
    pango_font_description_free(fd);
    cairo_restore(cr);

    cairo_arc(cr, cx, cy, r, 0.0, 2.0 * G_PI);
    cairo_set_source_rgb(cr, fin_stroke.r, fin_stroke.g, fin_stroke.b);
    cairo_set_line_width(cr, 2.0);
    cairo_stroke(cr);
}

void road_point(double t, double *ox, double *oy) {
    int k = (int)t;
    double u = t - (double)k;
    double u2, u3;
    double p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y;

    if (k < 0) { k = 0; u = 0.0; }
    if (k >= NUM_UNITS - 1) { k = NUM_UNITS - 2; u = 1.0; }

    p1x = road_cx[k];     p1y = road_cy[k];
    p2x = road_cx[k + 1]; p2y = road_cy[k + 1];
    if (k - 1 >= 0) { p0x = road_cx[k - 1]; p0y = road_cy[k - 1]; }
    else            { p0x = p1x - (p2x - p1x); p0y = p1y - (p2y - p1y); }
    if (k + 2 < NUM_UNITS) { p3x = road_cx[k + 2]; p3y = road_cy[k + 2]; }
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

void road_path(cairo_t *cr, double t0, double t1) {
    const double steps_per_unit = 48.0;
    int n = (int)((t1 - t0) * steps_per_unit);
    double x, y;
    int s;

    if (n < 1)
        n = 1;
    road_point(t0, &x, &y);
    cairo_move_to(cr, x, y);
    for (s = 1; s <= n; s++) {
        road_point(t0 + (t1 - t0) * (double)s / (double)n, &x, &y);
        cairo_line_to(cr, x, y);
    }
}

/* Compute the serpentine node layout for the given available width. */
void roadmap_geometry(double avail) {
    const double phase = 2.0 * G_PI * 1.7 / (double)(NUM_UNITS - 1);
    int rows;
    int r, c, i;

    for (rows = 1; rows <= NUM_UNITS; rows++) {
        int cols = (NUM_UNITS + rows - 1) / rows;
        if (2.0 * ROAD_MX + (cols - 1) * PATH_SPAC <= avail + 1.0)
            break;
    }
    if (rows > NUM_UNITS)
        rows = NUM_UNITS;
    road_rows = rows;
    road_cols = (NUM_UNITS + rows - 1) / rows;
    if (road_cols < 1)
        road_cols = 1;
    road_cw = (int)(2.0 * ROAD_MX + (road_cols - 1) * PATH_SPAC);
    road_ch = (int)(2.0 * ROAD_MY + (rows - 1) * ROAD_GAP);

    for (r = 0; r < rows; r++) {
        int base = r * road_cols;
        int len = MIN(road_cols, NUM_UNITS - base);
        int fwd = (r % 2) == 0;
        for (c = 0; c < len; c++) {
            i = base + c;
            int cc = fwd ? c : (road_cols - 1 - c);
            road_cx[i] = ROAD_MX + cc * PATH_SPAC;
            road_cy[i] = ROAD_MY + r * ROAD_GAP
                         + ROAD_WAVE * sin((double)i * phase);
        }
    }
}

void road_apply_layout(void) {
    int i;

    if (!road_fixed)
        return;

    for (i = 0; i < NUM_UNITS; i++) {
        if (!road_nodes[i] || !road_labels[i])
            continue;
        gtk_fixed_move(GTK_FIXED(road_fixed), road_nodes[i],
                       (int)(road_cx[i] - NODE_SIZE / 2.0),
                       (int)(road_cy[i] - NODE_SIZE / 2.0));
        gtk_fixed_move(GTK_FIXED(road_fixed), road_labels[i],
                       (int)(road_cx[i] - (PATH_SPAC - 20.0) / 2.0),
                       (int)(road_cy[i] + NODE_SIZE / 2.0 + 10.0));
    }

    gtk_widget_set_size_request(road_fixed, road_cw, road_ch);
    gtk_widget_set_size_request(road_rail, road_cw, road_ch);
    gtk_fixed_move(GTK_FIXED(road_fixed), road_rail, 0, 0);
    gtk_widget_queue_draw(road_rail);
}

void road_relayout(void) {
    GtkAdjustment *hadj;
    double avail;

    if (!road_scroll)
        return;
    hadj = gtk_scrolled_window_get_hadjustment(
        GTK_SCROLLED_WINDOW(road_scroll));
    avail = gtk_adjustment_get_page_size(hadj);
    if (avail < 1.0)
        return;
    roadmap_geometry(avail);
    road_apply_layout();
}

guint road_idle;

gboolean road_relayout_idle(gpointer data) {
    (void)data;
    road_idle = 0;
    road_relayout();
    return G_SOURCE_REMOVE;
}

void road_relayout_later(void) {
    if (road_idle == 0)
        road_idle = g_idle_add(road_relayout_idle, NULL);
}

void road_adjust_notify(GtkAdjustment *adj, GParamSpec *ps,
                               gpointer data) {
    (void)adj;
    (void)ps;
    (void)data;
    road_relayout_later();
}

void draw_rail(GtkDrawingArea *area, cairo_t *cr,
                      int width, int height, gpointer user_data) {
    const double t_lit = (double)(NUM_UNLOCKED - 1);
    const double t_end = (double)(NUM_UNITS - 1);
    const double x0 = ROAD_MX;
    const double x1 = (double)road_cw - ROAD_MX;
    const Rgb mauve = color_from_hex(app_theme.accent);
    const Rgb blue = color_from_hex(app_theme.accent3);
    const Rgb rail = color_from_hex(app_theme.rail);
    const Rgb muted = color_from_hex(app_theme.subtext);
    double hx, hy;

    (void)area;
    (void)width;
    (void)height;
    (void)user_data;

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

    road_point((double)(NUM_UNLOCKED - 1), &hx, &hy);
    draw_node_halo(cr, hx, hy, NODE_SIZE * 1.05,
                   mauve.r, mauve.g, mauve.b, 0.18);

    cairo_new_path(cr);
    road_path(cr, 0.0, t_end);
    cairo_set_line_width(cr, 16);
    cairo_set_source_rgb(cr, rail.r, rail.g, rail.b);
    cairo_stroke(cr);

    if (t_lit > 0.0) {
        cairo_pattern_t *grad;

        cairo_new_path(cr);
        road_path(cr, 0.0, t_lit);

        grad = cairo_pattern_create_linear(x0, 0.0, x1, 0.0);
        cairo_pattern_add_color_stop_rgba(grad, 0.0,
                                          mauve.r, mauve.g, mauve.b, 0.16);
        cairo_pattern_add_color_stop_rgba(grad, 1.0,
                                          blue.r, blue.g, blue.b, 0.16);
        cairo_set_source(cr, grad);
        cairo_set_line_width(cr, 34);
        cairo_stroke_preserve(cr);
        cairo_pattern_destroy(grad);

        grad = cairo_pattern_create_linear(x0, 0.0, x1, 0.0);
        cairo_pattern_add_color_stop_rgba(grad, 0.0,
                                          mauve.r, mauve.g, mauve.b, 1.0);
        cairo_pattern_add_color_stop_rgba(grad, 1.0,
                                          blue.r, blue.g, blue.b, 1.0);
        cairo_set_source(cr, grad);
        cairo_set_line_width(cr, 16);
        cairo_stroke_preserve(cr);
        cairo_pattern_destroy(grad);
    }

    {
        double start_t = t_lit > 0.0 ? t_lit : 0.0;

        cairo_set_source_rgba(cr, muted.r, muted.g, muted.b, 0.17);
        cairo_set_dash(cr, (double[]){1.0, 26.0}, 2, 0.0);
        cairo_new_path(cr);
        road_path(cr, start_t, t_end);
        cairo_stroke(cr);
        cairo_set_dash(cr, NULL, 0, 0.0);
    }
}

void add_path_node(GtkFixed *fixed, int index) {
    int num = index + 1;
    UnitCtx *u = &units[index];
    gboolean locked = !u->unlocked;
    GtkWidget *card;
    GtkWidget *name;
    char *text;

    card = gtk_button_new();
    gtk_widget_add_css_class(card, "unit-node");
    gtk_widget_set_can_focus(card, FALSE);
    gtk_widget_set_sensitive(card, !locked);
    gtk_widget_set_size_request(card, (int)NODE_SIZE, (int)NODE_SIZE);

    if (index == NUM_UNITS - 1) {
        GtkWidget *fin = gtk_drawing_area_new();

        gtk_widget_add_css_class(card, "finish");
        gtk_widget_add_css_class(card, "locked");
        gtk_widget_set_size_request(fin, (int)NODE_SIZE, (int)NODE_SIZE);
        gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(fin),
                                       draw_finish_cell,
                                       GINT_TO_POINTER(num), NULL);
        gtk_button_set_child(GTK_BUTTON(card), fin);
    } else {
        GtkWidget *vbox;
        GtkWidget *number;
        GtkWidget *icon;

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
            icon = icon_area_new(draw_lock_icon,
                                 0.3451, 0.3569, 0.4392, 16);
        } else {
            icon = icon_area_new(draw_check_icon,
                                 0.1176, 0.1176, 0.1804, 18);
            gtk_widget_set_visible(icon, FALSE);
            u->node = card;
            u->node_done_icon = icon;
            g_object_set_data_full(G_OBJECT(card), "target",
                                   g_strdup(u->page), g_free);
            g_signal_connect(card, "clicked", G_CALLBACK(on_nav_clicked), NULL);
        }
        gtk_box_append(GTK_BOX(vbox), icon);
    }

    road_nodes[index] = card;
    gtk_fixed_put(fixed, card, 0, 0);

    name = gtk_label_new(u->title);
    gtk_widget_set_size_request(name, (int)(PATH_SPAC - 20.0), -1);
    gtk_widget_set_halign(name, GTK_ALIGN_CENTER);
    gtk_label_set_justify(GTK_LABEL(name), GTK_JUSTIFY_CENTER);
    gtk_label_set_wrap(GTK_LABEL(name), TRUE);
    gtk_widget_add_css_class(name, "unit-name");
    if (locked)
        gtk_widget_add_css_class(name, "unit-name-locked");
    road_labels[index] = name;
    gtk_fixed_put(fixed, name, 0, 0);
}

GtkWidget *build_roadmap_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *fixed;
    GtkWidget *rail;
    GtkAdjustment *ha;
    GtkAdjustment *va;
    int i;

    roadmap_geometry(1000.0);

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("subjects", "roadmap_title", "roadmap_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);
    road_scroll = scroll;

    wrap = gtk_center_box_new();
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), wrap);

    fixed = gtk_fixed_new();
    gtk_widget_set_halign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(fixed, road_cw, road_ch);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), fixed);
    road_fixed = fixed;

    rail = gtk_drawing_area_new();
    gtk_widget_set_size_request(rail, road_cw, road_ch);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(rail), draw_rail, NULL, NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rail, 0, 0);
    road_rail = rail;

    for (i = 0; i < NUM_UNITS; i++)
        add_path_node(GTK_FIXED(fixed), i);

    ha = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scroll));
    va = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));
    g_signal_connect(ha, "notify::page-size",
                     G_CALLBACK(road_adjust_notify), NULL);
    g_signal_connect(va, "notify::page-size",
                     G_CALLBACK(road_adjust_notify), NULL);

    road_relayout_later();

    return page;
}

