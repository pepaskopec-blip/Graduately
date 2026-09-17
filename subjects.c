#include "maturita.h"

/* ------------------------------------------------------------------ */
/* Subjects page ("Předměty") - serpentine subject map                */
/* ------------------------------------------------------------------ */


/* Index 0 (Deutsch), NET_SUBJ (networks), HW_SUBJ (hardware) and CZ_SUBJ
 * (Czech) are unlocked. */
const char *sub_keys[NUM_SUBJECTS] = {
    "Deutsch",
    "Správa počítačových sítí",
    "Technické vybavení",
    "Český jazyk a literatura",
    "Občanská nauka",
    "English",
    "Matematika",
    "Fyzika",
    "Základy Přírodopisných věd",
    "Technická grafika",
    "Prezentační grafika",
    "Programování",
    "Programové vybavení",
};

/* Slice of the global `units[]` table that each subject owns. A start of -1
 * (or a count of 0) means the subject has no content yet; such subjects show
 * as locked on the statistics page until their units are added. */
const int sub_unit_start[NUM_SUBJECTS] = {
    0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};
const int sub_unit_count[NUM_SUBJECTS] = {
    NUM_UNITS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

GtkWidget *sub_scroll;
GtkWidget *sub_fixed;
GtkWidget *sub_rail;
GtkWidget *sub_cells[NUM_SUBJECTS];
GtkWidget *sub_labels[NUM_SUBJECTS];
double sub_cx[NUM_SUBJECTS];
double sub_cy[NUM_SUBJECTS];
int sub_rows = 1;
int sub_cols = NUM_SUBJECTS;
int sub_cw = (int)(2.0 * SUB_MX + (NUM_SUBJECTS - 1) * SUB_SPAC);
int sub_ch = (int)(2.0 * SUB_MY);
guint sub_idle;

void sub_layout_geometry(double avail) {
    const double phase = 2.0 * G_PI * 1.7 / (double)(NUM_SUBJECTS - 1);
    int rows;
    int r, c, i;

    for (rows = 1; rows <= NUM_SUBJECTS; rows++) {
        int cols = (NUM_SUBJECTS + rows - 1) / rows;
        if (2.0 * SUB_MX + (cols - 1) * SUB_SPAC <= avail + 1.0)
            break;
    }
    if (rows > NUM_SUBJECTS)
        rows = NUM_SUBJECTS;
    sub_rows = rows;
    sub_cols = (NUM_SUBJECTS + rows - 1) / rows;
    if (sub_cols < 1)
        sub_cols = 1;
    sub_cw = (int)(2.0 * SUB_MX + (sub_cols - 1) * SUB_SPAC);
    sub_ch = (int)(2.0 * SUB_MY + (rows - 1) * SUB_GAP);

    for (r = 0; r < rows; r++) {
        int base = r * sub_cols;
        int len = MIN(sub_cols, NUM_SUBJECTS - base);
        int fwd = (r % 2) == 0;
        for (c = 0; c < len; c++) {
            i = base + c;
            int cc = fwd ? c : (sub_cols - 1 - c);
            sub_cx[i] = SUB_MX + cc * SUB_SPAC;
            sub_cy[i] = SUB_MY + r * SUB_GAP
                        + SUB_WAVE * sin((double)i * phase);
        }
    }
}

void sub_point(double t, double *ox, double *oy) {
    int k = (int)t;
    double u = t - (double)k;
    double u2, u3;
    double p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y;

    if (k < 0) { k = 0; u = 0.0; }
    if (k >= NUM_SUBJECTS - 1) { k = NUM_SUBJECTS - 2; u = 1.0; }

    p1x = sub_cx[k];     p1y = sub_cy[k];
    p2x = sub_cx[k + 1]; p2y = sub_cy[k + 1];
    if (k - 1 >= 0) { p0x = sub_cx[k - 1]; p0y = sub_cy[k - 1]; }
    else            { p0x = p1x - (p2x - p1x); p0y = p1y - (p2y - p1y); }
    if (k + 2 < NUM_SUBJECTS) { p3x = sub_cx[k + 2]; p3y = sub_cy[k + 2]; }
    else                      { p3x = p2x + (p2x - p1x); p3y = p2y + (p2y - p1y); }

    u2 = u * u;
    u3 = u2 * u;
    *ox = 0.5 * (2.0 * p1x + (-p0x + p2x) * u
                 + (2.0 * p0x - 5.0 * p1x + 4.0 * p2x - p3x) * u2
                 + (-p0x + 3.0 * p1x - 3.0 * p2x + p3x) * u3);
    *oy = 0.5 * (2.0 * p1y + (-p0y + p2y) * u
                 + (2.0 * p0y - 5.0 * p1y + 4.0 * p2y - p3y) * u2
                 + (-p0y + 3.0 * p1y - 3.0 * p2y + p3y) * u3);
}

void sub_path(cairo_t *cr, double t0, double t1) {
    const double steps_per_unit = 48.0;
    int n = (int)((t1 - t0) * steps_per_unit);
    double x, y;
    int s;

    if (n < 1)
        n = 1;
    sub_point(t0, &x, &y);
    cairo_move_to(cr, x, y);
    for (s = 1; s <= n; s++) {
        sub_point(t0 + (t1 - t0) * (double)s / (double)n, &x, &y);
        cairo_line_to(cr, x, y);
    }
}

void draw_sub_rail(GtkDrawingArea *area, cairo_t *cr,
                          int width, int height, gpointer user_data) {
    const double t_end = (double)(NUM_SUBJECTS - 1);
    const Rgb rail = color_from_hex(app_theme.rail);

    (void)area;
    (void)width;
    (void)height;
    (void)user_data;

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

    cairo_new_path(cr);
    sub_path(cr, 0.0, t_end);
    cairo_set_line_width(cr, 12);
    cairo_set_source_rgb(cr, rail.r, rail.g, rail.b);
    cairo_stroke(cr);
}

void sub_apply_layout(void) {
    int i;

    if (!sub_fixed)
        return;

    for (i = 0; i < NUM_SUBJECTS; i++) {
        if (sub_cells[i])
            gtk_fixed_move(GTK_FIXED(sub_fixed), sub_cells[i],
                           (int)(sub_cx[i] - SUB_BUBBLE / 2.0),
                           (int)(sub_cy[i] - SUB_BUBBLE / 2.0));
        if (sub_labels[i])
            gtk_fixed_move(GTK_FIXED(sub_fixed), sub_labels[i],
                           (int)(sub_cx[i] - (SUB_SPAC - 24.0) / 2.0),
                           (int)(sub_cy[i] + SUB_BUBBLE / 2.0 + 10.0));
    }

    gtk_widget_set_size_request(sub_fixed, sub_cw, sub_ch);
    gtk_widget_set_size_request(sub_rail, sub_cw, sub_ch);
    gtk_fixed_move(GTK_FIXED(sub_fixed), sub_rail, 0, 0);
    gtk_widget_queue_draw(sub_rail);
}

void sub_relayout(void) {
    GtkAdjustment *hadj;
    double avail;

    if (!sub_scroll)
        return;
    hadj = gtk_scrolled_window_get_hadjustment(
        GTK_SCROLLED_WINDOW(sub_scroll));
    avail = gtk_adjustment_get_page_size(hadj);
    if (avail < 1.0)
        return;
    sub_layout_geometry(avail);
    sub_apply_layout();
}

gboolean sub_relayout_idle(gpointer data) {
    (void)data;
    sub_idle = 0;
    sub_relayout();
    return G_SOURCE_REMOVE;
}

void sub_relayout_later(void) {
    if (sub_idle == 0)
        sub_idle = g_idle_add(sub_relayout_idle, NULL);
}

void sub_adjust_notify(GtkAdjustment *adj, GParamSpec *ps,
                              gpointer data) {
    (void)adj;
    (void)ps;
    (void)data;
    sub_relayout_later();
}

/* German flag (black / red / gold) drawn in a clipped circle. */
void draw_german_flag(GtkDrawingArea *area, cairo_t *cr,
                             int width, int height, gpointer data) {
    double cx = width / 2.0;
    double cy = height / 2.0;
    double r = MIN(width, height) / 2.0 - 2.0;
    double band = (double)height / 3.0;

    (void)area;
    (void)data;

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

    cairo_save(cr);
    cairo_new_path(cr);
    cairo_arc(cr, cx, cy, r, 0.0, 2.0 * G_PI);
    cairo_clip(cr);

    cairo_new_path(cr);
    cairo_rectangle(cr, 0.0, 0.0, (double)width, band);
    cairo_set_source_rgb(cr, 0.18, 0.18, 0.19);
    cairo_fill(cr);

    cairo_new_path(cr);
    cairo_rectangle(cr, 0.0, band, (double)width, band);
    cairo_set_source_rgb(cr, 0.72, 0.27, 0.24);
    cairo_fill(cr);

    cairo_new_path(cr);
    cairo_rectangle(cr, 0.0, 2.0 * band, (double)width, band);
    cairo_set_source_rgb(cr, 0.84, 0.70, 0.32);
    cairo_fill(cr);

    cairo_restore(cr);
}

/* Czech flag (white / red with a blue triangle) drawn in a clipped circle. */
void draw_czech_flag(GtkDrawingArea *area, cairo_t *cr,
                            int width, int height, gpointer data) {
    double cx = width / 2.0;
    double cy = height / 2.0;
    double r = MIN(width, height) / 2.0 - 2.0;

    (void)area;
    (void)data;

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

    cairo_save(cr);
    cairo_new_path(cr);
    cairo_arc(cr, cx, cy, r, 0.0, 2.0 * G_PI);
    cairo_clip(cr);

    cairo_new_path(cr);
    cairo_rectangle(cr, 0.0, 0.0, (double)width, (double)height);
    cairo_set_source_rgb(cr, 0.98, 0.98, 0.98);
    cairo_fill(cr);

    cairo_new_path(cr);
    cairo_rectangle(cr, 0.0, (double)height / 2.0,
                    (double)width, (double)height / 2.0);
    cairo_set_source_rgb(cr, 0.83, 0.16, 0.18);
    cairo_fill(cr);

    cairo_new_path(cr);
    cairo_move_to(cr, 0.0, 0.0);
    cairo_line_to(cr, (double)width * 0.55, (double)height / 2.0);
    cairo_line_to(cr, 0.0, (double)height);
    cairo_close_path(cr);
    cairo_set_source_rgb(cr, 0.07, 0.24, 0.55);
    cairo_fill(cr);

    cairo_restore(cr);
}

GtkWidget *build_subjects_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *fixed;
    GtkWidget *rail;
    GtkAdjustment *ha;
    GtkAdjustment *va;
    int i;

    sub_layout_geometry(1000.0);

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("welcome", "subjects_title", NULL));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);
    sub_scroll = scroll;

    wrap = gtk_center_box_new();
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), wrap);

    fixed = gtk_fixed_new();
    gtk_widget_set_halign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(fixed, sub_cw, sub_ch);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), fixed);
    sub_fixed = fixed;

    rail = gtk_drawing_area_new();
    gtk_widget_set_size_request(rail, sub_cw, sub_ch);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(rail), draw_sub_rail,
                                   NULL, NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rail, 0, 0);
    sub_rail = rail;

    for (i = 0; i < NUM_SUBJECTS; i++) {
        GtkWidget *card;
        GtkWidget *lbl;

        card = gtk_button_new();
        gtk_widget_add_css_class(card, "unit-node");
        gtk_widget_set_can_focus(card, FALSE);
        gtk_widget_set_size_request(card, (int)SUB_BUBBLE, (int)SUB_BUBBLE);

        if (i == 0) {
            GtkWidget *flag = gtk_drawing_area_new();
            gtk_widget_add_css_class(card, "current");
            gtk_widget_set_size_request(flag, (int)SUB_BUBBLE,
                                        (int)SUB_BUBBLE);
            gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(flag),
                                           draw_german_flag, NULL, NULL);
            gtk_button_set_child(GTK_BUTTON(card), flag);
            g_object_set_data_full(G_OBJECT(card), "target",
                                   g_strdup("roadmap"), g_free);
            g_signal_connect(card, "clicked", G_CALLBACK(on_nav_clicked), NULL);
        } else if (i == NET_SUBJ) {
            GtkWidget *wifi = gtk_drawing_area_new();
            gtk_widget_add_css_class(card, "current");
            gtk_widget_set_size_request(wifi, (int)SUB_BUBBLE,
                                        (int)SUB_BUBBLE);
            gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(wifi),
                                           draw_wifi_icon, NULL, NULL);
            gtk_button_set_child(GTK_BUTTON(card), wifi);
            g_object_set_data_full(G_OBJECT(card), "target",
                                   g_strdup("netyears"), g_free);
            g_signal_connect(card, "clicked", G_CALLBACK(on_nav_clicked), NULL);
        } else if (i == HW_SUBJ) {
            GtkWidget *chip = gtk_drawing_area_new();
            gtk_widget_add_css_class(card, "current");
            gtk_widget_set_size_request(chip, (int)SUB_BUBBLE,
                                        (int)SUB_BUBBLE);
            gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(chip),
                                           draw_chip_icon, NULL, NULL);
            gtk_button_set_child(GTK_BUTTON(card), chip);
            g_object_set_data_full(G_OBJECT(card), "target",
                                   g_strdup("hwmap"), g_free);
            g_signal_connect(card, "clicked", G_CALLBACK(on_nav_clicked), NULL);
        } else if (i == CZ_SUBJ) {
            GtkWidget *flag = gtk_drawing_area_new();
            gtk_widget_add_css_class(card, "current");
            gtk_widget_set_size_request(flag, (int)SUB_BUBBLE,
                                        (int)SUB_BUBBLE);
            gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(flag),
                                           draw_czech_flag, NULL, NULL);
            gtk_button_set_child(GTK_BUTTON(card), flag);
            g_object_set_data_full(G_OBJECT(card), "target",
                                   g_strdup("czechmap"), g_free);
            g_signal_connect(card, "clicked", G_CALLBACK(on_nav_clicked), NULL);
        } else {
            GtkWidget *lock;
            gtk_widget_add_css_class(card, "locked");
            gtk_widget_set_sensitive(card, FALSE);
            lock = icon_area_new(draw_lock_icon,
                                 0.3451, 0.3569, 0.4392, 26);
            gtk_button_set_child(GTK_BUTTON(card), lock);
        }

        sub_cells[i] = card;
        gtk_fixed_put(GTK_FIXED(fixed), card, 0, 0);

        lbl = gtk_label_new(NULL);
        i18n_bind(lbl, sub_keys[i], 0);
        gtk_widget_set_size_request(lbl, (int)(SUB_SPAC - 24.0), -1);
        gtk_widget_set_halign(lbl, GTK_ALIGN_CENTER);
        gtk_label_set_justify(GTK_LABEL(lbl), GTK_JUSTIFY_CENTER);
        gtk_label_set_wrap(GTK_LABEL(lbl), TRUE);
        gtk_widget_add_css_class(lbl, "unit-name");
        if (i != 0 && i != NET_SUBJ && i != HW_SUBJ && i != CZ_SUBJ)
            gtk_widget_add_css_class(lbl, "unit-name-locked");
        sub_labels[i] = lbl;
        gtk_fixed_put(GTK_FIXED(fixed), lbl, 0, 0);
    }

    ha = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scroll));
    va = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));
    g_signal_connect(ha, "notify::page-size",
                     G_CALLBACK(sub_adjust_notify), NULL);
    g_signal_connect(va, "notify::page-size",
                     G_CALLBACK(sub_adjust_notify), NULL);

    sub_apply_layout();
    sub_relayout_later();

    return page;
}

