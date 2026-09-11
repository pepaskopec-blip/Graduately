#include "maturita.h"

/* Vector icons drawn with cairo so they never depend on the system
 * icon theme (which is missing on some installs). */

void draw_check_icon(GtkDrawingArea *area, cairo_t *cr,
                            int width, int height, gpointer data) {
    Rgb c = color_from_hex(app_theme.on_accent);

    (void)area;
    (void)data;
    cairo_set_source_rgb(cr, c.r, c.g, c.b);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
    cairo_set_line_width(cr, width * 0.16);
    cairo_move_to(cr, width * 0.24, height * 0.56);
    cairo_line_to(cr, width * 0.43, height * 0.74);
    cairo_line_to(cr, width * 0.78, height * 0.30);
    cairo_stroke(cr);
}

/* Paint a padlock centred in the size x size box with top-left (x0,y0). */
void lock_paint(cairo_t *cr, double x0, double y0, double size,
                       const Rgb *c) {
    double sl = MAX(1.6, size * 0.13);   /* shackle stroke width          */
    double bw = size * 0.60;             /* body width                    */
    double bh = size * 0.44;             /* body height                   */
    double cx = x0 + size / 2.0;
    double body_top = y0 + size - bh - size * 0.04;
    double bx = cx - bw / 2.0;
    double r = bw / 2.0 - sl / 2.0;      /* shackle arc radius            */

    cairo_set_source_rgb(cr, c->r, c->g, c->b);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_width(cr, sl);

    cairo_new_path(cr);
    cairo_arc(cr, cx, body_top, r, G_PI, 2.0 * G_PI);
    cairo_stroke(cr);

    cairo_new_path(cr);
    cairo_rectangle(cr, bx, body_top, bw, bh);
    cairo_fill(cr);

    cairo_new_path(cr);
    cairo_arc(cr, cx, body_top + bh * 0.42, size * 0.07, 0.0, 2.0 * G_PI);
    {
        Rgb hole = color_from_hex(app_theme.crust);
        cairo_set_source_rgb(cr, hole.r, hole.g, hole.b);
    }
    cairo_fill(cr);
}

void draw_lock_icon(GtkDrawingArea *area, cairo_t *cr,
                           int width, int height, gpointer data) {
    Rgb c = color_from_hex(app_theme.overlay);
    (void)area;
    (void)data;
    lock_paint(cr, 0.0, 0.0, MIN(width, height), &c);
}

/* Wireless icon: three upward arcs that fan out from a base dot. */
void draw_wifi_icon(GtkDrawingArea *area, cairo_t *cr,
                           int width, int height, gpointer data) {
    const double s = MIN(width, height);
    const double cx = width / 2.0;
    const double cy = height * 0.68;
    const double dot_r = s * 0.06;
    const double radii[3] = {s * 0.15, s * 0.27, s * 0.39};
    const double phi = 0.75;
    /* Keep the icon legible on the accent-gradient bubble while giving it a
     * hint of the active theme: blend the contrasting on-accent colour a
     * little way towards the palette's middle accent. */
    const Rgb c = mix_rgb(color_from_hex(app_theme.on_accent),
                          color_from_hex(app_theme.accent2), 0.22);

    (void)area;
    (void)data;

    cairo_set_source_rgb(cr, c.r, c.g, c.b);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_width(cr, MAX(1.5, s * 0.07));

    for (int i = 0; i < 3; i++) {
        double r = radii[i];

        cairo_new_path(cr);
        cairo_arc(cr, cx, cy, r, -G_PI / 2.0 - phi, -G_PI / 2.0 + phi);
        cairo_stroke(cr);
    }

    cairo_new_path(cr);
    cairo_arc(cr, cx, cy, dot_r, 0.0, 2.0 * G_PI);
    cairo_fill(cr);
}

/* CPU / chip icon for Technické vybavení. */
void draw_chip_icon(GtkDrawingArea *area, cairo_t *cr,
                           int width, int height, gpointer data) {
    const double s = MIN(width, height);
    const double cx = width / 2.0;
    const double cy = height / 2.0;
    const double body = s * 0.42;
    const double pin_len = s * 0.12;
    const double pin_w = s * 0.055;
    const Rgb c = mix_rgb(color_from_hex(app_theme.on_accent),
                          color_from_hex(app_theme.accent2), 0.22);

    (void)area;
    (void)data;

    cairo_set_source_rgb(cr, c.r, c.g, c.b);

    cairo_rectangle(cr, cx - body / 2.0, cy - body / 2.0, body, body);
    cairo_fill(cr);

    cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);
    cairo_set_line_width(cr, pin_w);
    for (int i = 0; i < 4; i++) {
        double t = (i + 0.5) / 4.0;
        double x = cx - body / 2.0 + t * body;
        double y = cy - body / 2.0 + t * body;

        cairo_move_to(cr, x, cy - body / 2.0);
        cairo_line_to(cr, x, cy - body / 2.0 - pin_len);
        cairo_move_to(cr, x, cy + body / 2.0);
        cairo_line_to(cr, x, cy + body / 2.0 + pin_len);
        cairo_move_to(cr, cx - body / 2.0, y);
        cairo_line_to(cr, cx - body / 2.0 - pin_len, y);
        cairo_move_to(cr, cx + body / 2.0, y);
        cairo_line_to(cr, cx + body / 2.0 + pin_len, y);
    }
    cairo_stroke(cr);

    {
        Rgb hole = color_from_hex(app_theme.crust);
        double core = body * 0.34;

        cairo_set_source_rgb(cr, hole.r, hole.g, hole.b);
        cairo_rectangle(cr, cx - core / 2.0, cy - core / 2.0, core, core);
        cairo_fill(cr);
    }
}

void draw_back_icon(GtkDrawingArea *area, cairo_t *cr,
                           int width, int height, gpointer data) {
    const double size = 15.0;      /* fixed logical icon size            */
    const double stroke = 1.8;
    double ox = (width - size) / 2.0;
    double oy = (height - size) / 2.0;
    GtkWidget *parent = gtk_widget_get_parent(GTK_WIDGET(area));
    GdkRGBA color;

    (void)data;
    gtk_style_context_get_color(gtk_widget_get_style_context(parent), &color);
    cairo_set_source_rgb(cr, color.red, color.green, color.blue);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
    cairo_set_line_width(cr, stroke);
    cairo_move_to(cr, ox + size * 0.72, oy + size * 0.24);
    cairo_line_to(cr, ox + size * 0.30, oy + size * 0.50);
    cairo_line_to(cr, ox + size * 0.72, oy + size * 0.76);
    cairo_stroke(cr);
}

void draw_settings_icon(GtkDrawingArea *area, cairo_t *cr,
                               int width, int height, gpointer data) {
    const int teeth = 8;
    const double size = 16.0;
    double cx = width / 2.0;
    double cy = height / 2.0;
    double r_outer = size * 0.48;
    double r_inner = size * 0.34;
    double r_hole = size * 0.16;
    double tooth_half = G_PI / (double)teeth;
    GtkWidget *color_w = data ? GTK_WIDGET(data)
                              : gtk_widget_get_parent(GTK_WIDGET(area));
    GdkRGBA color;
    int i;

    gtk_style_context_get_color(gtk_widget_get_style_context(color_w), &color);
    cairo_set_source_rgb(cr, color.red, color.green, color.blue);
    cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);

    cairo_new_path(cr);
    for (i = 0; i < teeth; i++) {
        double a = -G_PI / 2.0 + (2.0 * G_PI * (double)i) / (double)teeth;
        double a0 = a - tooth_half * 0.55;
        double a1 = a - tooth_half * 0.28;
        double a2 = a + tooth_half * 0.28;
        double a3 = a + tooth_half * 0.55;

        if (i == 0)
            cairo_move_to(cr, cx + cos(a0) * r_inner, cy + sin(a0) * r_inner);
        else
            cairo_line_to(cr, cx + cos(a0) * r_inner, cy + sin(a0) * r_inner);

        cairo_line_to(cr, cx + cos(a1) * r_outer, cy + sin(a1) * r_outer);
        cairo_line_to(cr, cx + cos(a2) * r_outer, cy + sin(a2) * r_outer);
        cairo_line_to(cr, cx + cos(a3) * r_inner, cy + sin(a3) * r_inner);
    }
    cairo_close_path(cr);
    cairo_new_sub_path(cr);
    cairo_arc(cr, cx, cy, r_hole, 0.0, 2.0 * G_PI);
    cairo_fill(cr);
}

void draw_stats_icon(GtkDrawingArea *area, cairo_t *cr,
                            int width, int height, gpointer data) {
    GtkWidget *color_w = data ? GTK_WIDGET(data)
                              : gtk_widget_get_parent(GTK_WIDGET(area));
    GdkRGBA color;
    const double bar_w = 3.2;
    const double gap = 2.6;
    const double total_w = 3.0 * bar_w + 2.0 * gap;
    const double heights[3] = {0.42, 0.72, 1.0};
    double base_x = (width - total_w) / 2.0;
    double base_y = height * 0.82;
    double max_h = height * 0.58;
    int i;

    (void)area;
    gtk_style_context_get_color(gtk_widget_get_style_context(color_w), &color);
    cairo_set_source_rgb(cr, color.red, color.green, color.blue);
    cairo_set_line_width(cr, 1.5);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

    for (i = 0; i < 3; i++) {
        double x = base_x + (double)i * (bar_w + gap);
        double h = max_h * heights[i];
        double y = base_y - h;
        double r = 1.2;

        cairo_new_sub_path(cr);
        cairo_arc(cr, x + r, y + r, r, G_PI, 1.5 * G_PI);
        cairo_arc(cr, x + bar_w - r, y + r, r, 1.5 * G_PI, 2.0 * G_PI);
        cairo_line_to(cr, x + bar_w, base_y);
        cairo_line_to(cr, x, base_y);
        cairo_close_path(cr);
        cairo_fill(cr);
    }
}

GtkWidget *icon_area_new(GtkDrawingAreaDrawFunc fn,
                                double r, double g, double b, int px) {
    Rgb *col = g_new(Rgb, 1);
    GtkWidget *d = gtk_drawing_area_new();

    col->r = r;
    col->g = g;
    col->b = b;
    gtk_widget_set_size_request(d, px, px);
    gtk_widget_set_halign(d, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(d, GTK_ALIGN_CENTER);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(d), fn,
                                   col, (GDestroyNotify)g_free);
    return d;
}
