#include "maturita.h"

/* ---- Czech language and literature (Český jazyk a literatura) ----- */

#define CZ_NODES   3
#define CZ_BUBBLE  SUB_BUBBLE
#define CZ_LABEL_W 200.0
#define CZ_CW      600
#define CZ_CH      470

static const char *cz_keys[CZ_NODES] = {
    "Literatura", "Mluvnice", "Maturitní četba",
};
static const char *cz_initials[CZ_NODES] = {
    "L", "M", "Č",
};

static GtkWidget *cz_rail;
static double cz_cx[CZ_NODES];
static double cz_cy[CZ_NODES];

static void cz_geometry(void) {
    cz_cx[0] = 300.0; cz_cy[0] = 120.0;   /* Literatura           (top) */
    cz_cx[1] = 140.0; cz_cy[1] = 350.0;   /* Mluvnice   (bottom-left)   */
    cz_cx[2] = 460.0; cz_cy[2] = 350.0;   /* Maturitní četba (bottom-right) */
}

/* Three bubbles joined into a triangle by thick rails. */
static void draw_cz_rail(GtkDrawingArea *area, cairo_t *cr,
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
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

    cairo_new_path(cr);
    cairo_move_to(cr, cz_cx[0], cz_cy[0]);
    cairo_line_to(cr, cz_cx[1], cz_cy[1]);
    cairo_line_to(cr, cz_cx[2], cz_cy[2]);
    cairo_close_path(cr);

    cairo_set_line_width(cr, 14);
    cairo_set_source_rgb(cr, rail.r, rail.g, rail.b);
    cairo_stroke_preserve(cr);

    cairo_set_line_width(cr, 9);
    cairo_set_source_rgb(cr, slate.r, slate.g, slate.b);
    cairo_stroke(cr);
}

void czech_rail_theme_reset(void) {
    if (cz_rail)
        gtk_widget_queue_draw(cz_rail);
}

static GtkWidget *cz_make_node(int index) {
    GtkWidget *btn;
    GtkWidget *label;

    btn = gtk_button_new();
    gtk_widget_add_css_class(btn, "unit-node");
    gtk_widget_add_css_class(btn, "current");
    gtk_widget_set_can_focus(btn, FALSE);
    gtk_widget_set_size_request(btn, (int)CZ_BUBBLE, (int)CZ_BUBBLE);

    label = gtk_label_new(cz_initials[index]);
    gtk_widget_add_css_class(label, "unit-number");
    gtk_button_set_child(GTK_BUTTON(btn), label);

    return btn;
}

GtkWidget *build_czechmap_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *fixed;
    GtkWidget *rail;

    cz_geometry();

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("subjects", "Český jazyk a literatura",
                           "czech_sub"));

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
    gtk_widget_set_size_request(fixed, CZ_CW, CZ_CH);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), fixed);

    rail = gtk_drawing_area_new();
    gtk_widget_set_size_request(rail, CZ_CW, CZ_CH);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(rail), draw_cz_rail,
                                   NULL, NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rail, 0, 0);
    cz_rail = rail;

    for (int i = 0; i < CZ_NODES; i++) {
        GtkWidget *btn = cz_make_node(i);
        GtkWidget *name = gtk_label_new(NULL);

        if (i == 2) {
            g_object_set_data_full(G_OBJECT(btn), "target",
                                   g_strdup("readinglist"), g_free);
            g_signal_connect(btn, "clicked", G_CALLBACK(on_nav_clicked), NULL);
        }

        i18n_bind(name, cz_keys[i], 0);
        gtk_widget_set_size_request(name, (int)CZ_LABEL_W, -1);
        gtk_widget_set_halign(name, GTK_ALIGN_CENTER);
        gtk_label_set_justify(GTK_LABEL(name), GTK_JUSTIFY_CENTER);
        gtk_label_set_wrap(GTK_LABEL(name), TRUE);
        gtk_widget_add_css_class(name, "unit-name");
        gtk_widget_add_css_class(name, "cz-label");

        gtk_fixed_put(GTK_FIXED(fixed), btn, 0, 0);
        gtk_fixed_put(GTK_FIXED(fixed), name, 0, 0);

        gtk_fixed_move(GTK_FIXED(fixed), btn,
                       (int)(cz_cx[i] - CZ_BUBBLE / 2.0),
                       (int)(cz_cy[i] - CZ_BUBBLE / 2.0));
        gtk_fixed_move(GTK_FIXED(fixed), name,
                       (int)(cz_cx[i] - CZ_LABEL_W / 2.0),
                       (int)(cz_cy[i] + CZ_BUBBLE / 2.0 + 10.0));
    }

    return page;
}
