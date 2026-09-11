#include "maturita.h"

/* ------------------------------------------------------------------ */
/* Unit 1 exercise path (roadmap-style serpentine)                    */
/* ------------------------------------------------------------------ */


/* Compute the serpentine exercise layout for the given available width. */
void ex_layout_geometry(UnitCtx *u, double avail) {
    const int n = u->n_ex;
    const double phase = 2.0 * G_PI * 1.7 / (double)(n > 1 ? n - 1 : 1);
    const double lift = u->has_branch ? EX_BRANCH_LIFT : 0.0;
    int rows;
    int r, c, i;

    if (n < 1)
        return;
    for (rows = 1; rows <= n; rows++) {
        int cols = (n + rows - 1) / rows;
        if (2.0 * EX_MX + (cols - 1) * EX_SPAC <= avail + 1.0)
            break;
    }
    if (rows > n)
        rows = n;
    u->ex_rows = rows;
    u->ex_cols = (n + rows - 1) / rows;
    if (u->ex_cols < 1)
        u->ex_cols = 1;
    u->ex_cw = (int)(2.0 * EX_MX + (u->ex_cols - 1) * EX_SPAC);
    u->ex_ch = (int)(2.0 * EX_MY + (rows - 1) * EX_GAP + lift);

    for (r = 0; r < rows; r++) {
        int base = r * u->ex_cols;
        int len = MIN(u->ex_cols, n - base);
        int fwd = (r % 2) == 0;
        for (c = 0; c < len; c++) {
            i = base + c;
            int cc = fwd ? c : (u->ex_cols - 1 - c);
            u->ex_cx[i] = EX_MX + cc * EX_SPAC;
            u->ex_cy[i] = EX_MY + lift + r * EX_GAP
                          + EX_WAVE * sin((double)i * phase);
        }
    }

    /* The branch hangs straight above exercise 2 (index 1). */
    if (u->has_branch && n > 1) {
        u->branch_cx = u->ex_cx[1];
        u->branch_cy = u->ex_cy[1] - EX_BRANCH_LIFT;
    }
}

void ex_point(UnitCtx *u, double t, double *ox, double *oy) {
    const int n = u->n_ex;
    int k = (int)t;
    double uu = t - (double)k;
    double u2, u3;
    double p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y;

    if (n < 2) { *ox = u->ex_cx[0]; *oy = u->ex_cy[0]; return; }
    if (k < 0) { k = 0; uu = 0.0; }
    if (k >= n - 1) { k = n - 2; uu = 1.0; }

    p1x = u->ex_cx[k];     p1y = u->ex_cy[k];
    p2x = u->ex_cx[k + 1]; p2y = u->ex_cy[k + 1];
    if (k - 1 >= 0) { p0x = u->ex_cx[k - 1]; p0y = u->ex_cy[k - 1]; }
    else            { p0x = p1x - (p2x - p1x); p0y = p1y - (p2y - p1y); }
    if (k + 2 < n) { p3x = u->ex_cx[k + 2]; p3y = u->ex_cy[k + 2]; }
    else           { p3x = p2x + (p2x - p1x); p3y = p2y + (p2y - p1y); }

    u2 = uu * uu;
    u3 = u2 * uu;
    *ox = 0.5 * (2.0 * p1x + (-p0x + p2x) * uu
                 + (2.0 * p0x - 5.0 * p1x + 4.0 * p2x - p3x) * u2
                 + (-p0x + 3.0 * p1x - 3.0 * p2x + p3x) * u3);
    *oy = 0.5 * (2.0 * p1y + (-p0y + p2y) * uu
                 + (2.0 * p0y - 5.0 * p1y + 4.0 * p2y - p3y) * u2
                 + (-p0y + 3.0 * p1y - 3.0 * p2y + p3y) * u3);
}

void ex_path(UnitCtx *u, cairo_t *cr, double t0, double t1) {
    const double steps_per_unit = 48.0;
    int n = (int)((t1 - t0) * steps_per_unit);
    double x, y;
    int s;

    if (n < 1)
        n = 1;
    ex_point(u, t0, &x, &y);
    cairo_move_to(cr, x, y);
    for (s = 1; s <= n; s++) {
        ex_point(u, t0 + (t1 - t0) * (double)s / (double)n, &x, &y);
        cairo_line_to(cr, x, y);
    }
}

void draw_ex_rail(GtkDrawingArea *area, cairo_t *cr,
                         int width, int height, gpointer user_data) {
    UnitCtx *u = user_data;
    const double t_end = (double)(u->n_ex - 1);
    const Rgb slate = color_from_hex(app_theme.surface1);
    const Rgb green = color_from_hex(app_theme.success);
    const Rgb rail = color_from_hex(app_theme.rail);

    (void)area;
    (void)width;
    (void)height;

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

    cairo_new_path(cr);
    ex_path(u, cr, 0.0, t_end);
    cairo_set_line_width(cr, 14);
    cairo_set_source_rgb(cr, rail.r, rail.g, rail.b);
    cairo_stroke(cr);

    cairo_new_path(cr);
    ex_path(u, cr, 0.0, t_end);
    cairo_set_line_width(cr, 9);
    cairo_set_source_rgb(cr, slate.r, slate.g, slate.b);
    cairo_stroke(cr);

    for (int k = 0; k < u->n_ex - 1; k++) {
        if (!(u->done[k + 1] || u->done[k + 2]))
            continue;
        cairo_new_path(cr);
        ex_path(u, cr, (double)k, (double)(k + 1));
        cairo_set_source_rgba(cr, green.r, green.g, green.b, 0.85);
        cairo_set_line_width(cr, 9);
        cairo_stroke(cr);
    }

    /* Off-path branch connector up from exercise 2. */
    if (u->has_branch && u->n_ex > 1) {
        gboolean bdone = u->done[u->branch_ex];

        cairo_new_path(cr);
        cairo_move_to(cr, u->ex_cx[1], u->ex_cy[1]);
        cairo_line_to(cr, u->branch_cx, u->branch_cy);
        cairo_set_line_width(cr, 14);
        cairo_set_source_rgb(cr, rail.r, rail.g, rail.b);
        cairo_stroke(cr);

        cairo_new_path(cr);
        cairo_move_to(cr, u->ex_cx[1], u->ex_cy[1]);
        cairo_line_to(cr, u->branch_cx, u->branch_cy);
        cairo_set_line_width(cr, 9);
        if (bdone)
            cairo_set_source_rgba(cr, green.r, green.g, green.b, 0.85);
        else
            cairo_set_source_rgb(cr, slate.r, slate.g, slate.b);
        cairo_stroke(cr);
    }
}

void ex_apply_layout(UnitCtx *u) {
    int i;

    if (!u->ex_fixed)
        return;

    for (i = 0; i < u->n_ex; i++) {
        if (u->ex_cells[i])
            gtk_fixed_move(GTK_FIXED(u->ex_fixed), u->ex_cells[i],
                           (int)(u->ex_cx[i] - EX_BUBBLE / 2.0),
                           (int)(u->ex_cy[i] - EX_BUBBLE / 2.0));
        if (u->ex_labels[i])
            gtk_fixed_move(GTK_FIXED(u->ex_fixed), u->ex_labels[i],
                           (int)(u->ex_cx[i] - (EX_SPAC - 20.0) / 2.0),
                           (int)(u->ex_cy[i] + EX_BUBBLE / 2.0 + 8.0));
    }

    if (u->has_branch && u->branch_cell) {
        gtk_fixed_move(GTK_FIXED(u->ex_fixed), u->branch_cell,
                       (int)(u->branch_cx - EX_BUBBLE / 2.0),
                       (int)(u->branch_cy - EX_BUBBLE / 2.0));
        if (u->branch_label)
            gtk_fixed_move(GTK_FIXED(u->ex_fixed), u->branch_label,
                           (int)(u->branch_cx + EX_BUBBLE / 2.0 + 10.0),
                           (int)(u->branch_cy - 14.0));
    }

    gtk_widget_set_size_request(u->ex_fixed, u->ex_cw, u->ex_ch);
    gtk_widget_set_size_request(u->ex_rail, u->ex_cw, u->ex_ch);
    gtk_fixed_move(GTK_FIXED(u->ex_fixed), u->ex_rail, 0, 0);
    gtk_widget_queue_draw(u->ex_rail);
}

void ex_relayout(UnitCtx *u) {
    GtkAdjustment *hadj;
    double avail;

    if (!u->ex_scroll)
        return;
    hadj = gtk_scrolled_window_get_hadjustment(
        GTK_SCROLLED_WINDOW(u->ex_scroll));
    avail = gtk_adjustment_get_page_size(hadj);
    if (avail < 1.0)
        return;
    ex_layout_geometry(u, avail);
    ex_apply_layout(u);
}

gboolean ex_relayout_idle(gpointer data) {
    UnitCtx *u = data;
    u->ex_idle = 0;
    ex_relayout(u);
    return G_SOURCE_REMOVE;
}

void ex_relayout_later(UnitCtx *u) {
    if (u->ex_idle == 0)
        u->ex_idle = g_idle_add(ex_relayout_idle, u);
}

void ex_adjust_notify(GtkAdjustment *adj, GParamSpec *ps,
                             gpointer data) {
    (void)adj;
    (void)ps;
    ex_relayout_later(data);
}

GtkWidget *make_bubble(UnitCtx *u, int n) {
    GtkWidget *btn;
    GtkWidget *vbox;
    GtkWidget *num_label;
    GtkWidget *icon;
    char *num;
    char *target;

    btn = gtk_button_new();
    gtk_widget_add_css_class(btn, "ex-bubble");
    gtk_widget_set_size_request(btn, (int)EX_BUBBLE, (int)EX_BUBBLE);
    gtk_widget_set_can_focus(btn, FALSE);
    if (u->done[n])
        gtk_widget_add_css_class(btn, "done");

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
    gtk_button_set_child(GTK_BUTTON(btn), vbox);

    num = g_strdup_printf("%d", n);
    num_label = gtk_label_new(num);
    g_free(num);
    gtk_widget_set_halign(num_label, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(num_label, "bubble-number");
    gtk_box_append(GTK_BOX(vbox), num_label);

    icon = icon_area_new(draw_check_icon, 0.1176, 0.1176, 0.1804, 16);
    gtk_widget_set_visible(icon, u->done[n]);
    gtk_box_append(GTK_BOX(vbox), icon);

    u->ex_cells[n - 1] = btn;
    u->ex_icons[n] = icon;

    target = g_strdup_printf("%se%d", u->ex_tag, n);
    g_object_set_data_full(G_OBJECT(btn), "target", target, g_free);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_nav_clicked), NULL);

    return btn;
}

static GtkWidget *make_branch_bubble(UnitCtx *u) {
    GtkWidget *btn;
    GtkWidget *vbox;
    GtkWidget *star;
    GtkWidget *icon;

    btn = gtk_button_new();
    gtk_widget_add_css_class(btn, "ex-bubble");
    gtk_widget_add_css_class(btn, "ex-branch");
    gtk_widget_set_size_request(btn, (int)EX_BUBBLE, (int)EX_BUBBLE);
    gtk_widget_set_can_focus(btn, FALSE);
    if (u->done[u->branch_ex])
        gtk_widget_add_css_class(btn, "done");

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
    gtk_button_set_child(GTK_BUTTON(btn), vbox);

    star = gtk_label_new("★");
    gtk_widget_set_halign(star, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(star, "bubble-number");
    gtk_box_append(GTK_BOX(vbox), star);

    icon = icon_area_new(draw_check_icon, 0.1176, 0.1176, 0.1804, 16);
    gtk_widget_set_visible(icon, u->done[u->branch_ex]);
    gtk_box_append(GTK_BOX(vbox), icon);

    u->branch_cell = btn;
    u->branch_icon = icon;

    g_object_set_data_full(G_OBJECT(btn), "target",
                           g_strdup(u->branch_target), g_free);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_nav_clicked), NULL);
    return btn;
}

GtkWidget *build_unit_page(UnitCtx *u) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *fixed;
    GtkWidget *rail;
    GtkAdjustment *ha;
    GtkAdjustment *va;
    int i;

    if (u->n_ex < 1)
        return NULL;
    ex_layout_geometry(u, 1000.0);

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("roadmap", u->title, u->sub_key));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);
    u->ex_scroll = scroll;

    wrap = gtk_center_box_new();
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), wrap);

    fixed = gtk_fixed_new();
    gtk_widget_set_halign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(fixed, u->ex_cw, u->ex_ch);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), fixed);
    u->ex_fixed = fixed;

    rail = gtk_drawing_area_new();
    gtk_widget_set_size_request(rail, u->ex_cw, u->ex_ch);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(rail), draw_ex_rail,
                                   u, NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rail, 0, 0);
    u->ex_rail = rail;

    for (i = 0; i < u->n_ex; i++) {
        GtkWidget *btn = make_bubble(u, i + 1);
        GtkWidget *lbl = gtk_label_new(u->ex_names[i + 1]);

        u->ex_labels[i] = lbl;

        gtk_widget_set_size_request(lbl, (int)(EX_SPAC - 20.0), -1);
        gtk_widget_set_halign(lbl, GTK_ALIGN_CENTER);
        gtk_label_set_justify(GTK_LABEL(lbl), GTK_JUSTIFY_CENTER);
        gtk_label_set_wrap(GTK_LABEL(lbl), TRUE);
        gtk_widget_add_css_class(lbl, "ex-label");

        gtk_fixed_put(GTK_FIXED(fixed), btn, 0, 0);
        gtk_fixed_put(GTK_FIXED(fixed), lbl, 0, 0);
    }

    if (u->has_branch && u->branch_target) {
        GtkWidget *btn = make_branch_bubble(u);
        GtkWidget *lbl = gtk_label_new(u->branch_name);

        u->branch_label = lbl;
        gtk_widget_set_halign(lbl, GTK_ALIGN_START);
        gtk_widget_add_css_class(lbl, "ex-label");
        gtk_widget_set_visible(lbl, TRUE);

        gtk_fixed_put(GTK_FIXED(fixed), btn, 0, 0);
        gtk_fixed_put(GTK_FIXED(fixed), lbl, 0, 0);
    }

    ha = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scroll));
    va = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));
    g_signal_connect(ha, "notify::page-size",
                     G_CALLBACK(ex_adjust_notify), u);
    g_signal_connect(va, "notify::page-size",
                     G_CALLBACK(ex_adjust_notify), u);

    ex_apply_layout(u);
    ex_relayout_later(u);

    return page;
}
