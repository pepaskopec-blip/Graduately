#include "maturita.h"

/* ------------------------------------------------------------------ */
/* Subjects page — icon grid, one tile per subject                    */
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
int sub_cw;
int sub_ch;
guint sub_idle;

void sub_layout_geometry(double avail) {
    (void)avail;
}

void draw_sub_rail(GtkDrawingArea *area, cairo_t *cr,
                          int width, int height, gpointer user_data) {
    (void)area;
    (void)cr;
    (void)width;
    (void)height;
    (void)user_data;
}

void sub_apply_layout(void) {
}

void sub_relayout(void) {
}

gboolean sub_relayout_idle(gpointer data) {
    (void)data;
    sub_idle = 0;
    return G_SOURCE_REMOVE;
}

void sub_relayout_later(void) {
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

static gboolean subject_is_open(int i) {
    return i == 0 || i == NET_SUBJ || i == HW_SUBJ || i == CZ_SUBJ;
}

static GtkWidget *subject_icon(int i) {
    GtkWidget *area = gtk_drawing_area_new();

    gtk_widget_set_size_request(area, 48, 48);
    gtk_widget_set_halign(area, GTK_ALIGN_CENTER);
    if (i == 0)
        gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area),
                                       draw_german_flag, NULL, NULL);
    else if (i == NET_SUBJ)
        gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area),
                                       draw_wifi_icon, NULL, NULL);
    else if (i == HW_SUBJ)
        gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area),
                                       draw_chip_icon, NULL, NULL);
    else if (i == CZ_SUBJ)
        gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area),
                                       draw_czech_flag, NULL, NULL);
    else
        return icon_area_new(draw_lock_icon, 0.3451, 0.3569, 0.4392, 22);
    return area;
}

static const char *subject_target(int i) {
    if (i == 0)
        return "roadmap";
    if (i == NET_SUBJ)
        return "netyears";
    if (i == HW_SUBJ)
        return "hwyears";
    if (i == CZ_SUBJ)
        return "czechmap";
    return NULL;
}

GtkWidget *build_subjects_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *grid;
    int i;

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(page, "subjects-page");
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 40);
    gtk_widget_set_margin_end(page, 40);
    gtk_widget_set_margin_top(page, 16);
    gtk_widget_set_margin_bottom(page, 28);

    gtk_box_append(GTK_BOX(page),
                   top_bar("welcome", "subjects_title", "subjects_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 8);
    gtk_box_append(GTK_BOX(page), scroll);
    sub_scroll = scroll;

    grid = gtk_flow_box_new();
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(grid), 5);
    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(grid), 3);
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(grid), GTK_SELECTION_NONE);
    gtk_flow_box_set_homogeneous(GTK_FLOW_BOX(grid), TRUE);
    gtk_flow_box_set_column_spacing(GTK_FLOW_BOX(grid), 14);
    gtk_flow_box_set_row_spacing(GTK_FLOW_BOX(grid), 14);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_widget_add_css_class(grid, "subject-grid");
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), grid);
    sub_fixed = grid;

    for (i = 0; i < NUM_SUBJECTS; i++) {
        GtkWidget *card;
        GtkWidget *inner;
        GtkWidget *icon;
        GtkWidget *lbl;
        const char *target;
        gboolean open = subject_is_open(i);

        card = gtk_button_new();
        gtk_widget_add_css_class(card, "subject-tile");
        gtk_widget_set_can_focus(card, open);
        gtk_widget_set_hexpand(card, TRUE);
        gtk_widget_set_size_request(card, 180, 132);

        inner = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_widget_set_halign(inner, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(inner, GTK_ALIGN_CENTER);
        gtk_button_set_child(GTK_BUTTON(card), inner);

        icon = subject_icon(i);
        gtk_box_append(GTK_BOX(inner), icon);

        lbl = gtk_label_new(NULL);
        i18n_bind(lbl, sub_keys[i], 0);
        gtk_widget_set_halign(lbl, GTK_ALIGN_CENTER);
        gtk_label_set_justify(GTK_LABEL(lbl), GTK_JUSTIFY_CENTER);
        gtk_label_set_wrap(GTK_LABEL(lbl), TRUE);
        gtk_label_set_max_width_chars(GTK_LABEL(lbl), 16);
        gtk_widget_add_css_class(lbl, "subject-name");
        gtk_box_append(GTK_BOX(inner), lbl);

        if (open) {
            gtk_widget_add_css_class(card, "open");
            target = subject_target(i);
            g_object_set_data_full(G_OBJECT(card), "target",
                                   g_strdup(target), g_free);
            g_signal_connect(card, "clicked", G_CALLBACK(on_nav_clicked), NULL);
        } else {
            gtk_widget_add_css_class(card, "locked");
            gtk_widget_add_css_class(lbl, "unit-name-locked");
            gtk_widget_set_sensitive(card, FALSE);
        }

        sub_cells[i] = card;
        sub_labels[i] = lbl;
        gtk_flow_box_insert(GTK_FLOW_BOX(grid), card, -1);
    }

    return page;
}
