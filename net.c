#include "maturita.h"

/* ---- Computer networks: subject path, lesson & exercise ------------ */


NetLesson net_lessons[NET_LESSONS] = {
    /* UI unit N maps to lesson content: unit 1 = IP/VLSM (extra);
     * units 2–18 = source topics (Základní pojmy … switch podrobně). */
    { .n_slides = NET_SLIDES,   .unit_page = "netunit1",  .ex_page = "netex1"  },
    { .n_slides = NET2_SLIDES,  .unit_page = "netunit2",  .ex_page = "netex2"  },
    { .n_slides = NET3_SLIDES,  .unit_page = "netunit3",  .ex_page = "netex3"  },
    { .n_slides = NET4_SLIDES,  .unit_page = "netunit4",  .ex_page = "netex4"  },
    { .n_slides = NET5_SLIDES,  .unit_page = "netunit5",  .ex_page = "netex5"  },
    { .n_slides = NET6_SLIDES,  .unit_page = "netunit6",  .ex_page = "netex6"  },
    { .n_slides = NET7_SLIDES,  .unit_page = "netunit7",  .ex_page = "netex7"  },
    { .n_slides = NET8_SLIDES,  .unit_page = "netunit8",  .ex_page = "netex8"  },
    { .n_slides = NET9_SLIDES,  .unit_page = "netunit9",  .ex_page = "netex9"  },
    { .n_slides = NET10_SLIDES, .unit_page = "netunit10", .ex_page = "netex10" },
    { .n_slides = NET11_SLIDES, .unit_page = "netunit11", .ex_page = "netex11" },
    { .n_slides = NET12_SLIDES, .unit_page = "netunit12", .ex_page = "netex12" },
    { .n_slides = NET13_SLIDES, .unit_page = "netunit13", .ex_page = "netex13" },
    { .n_slides = NET14_SLIDES, .unit_page = "netunit14", .ex_page = "netex14" },
    { .n_slides = NET15_SLIDES, .unit_page = "netunit15", .ex_page = "netex15" },
    { .n_slides = NET16_SLIDES, .unit_page = "netunit16", .ex_page = "netex16" },
    { .n_slides = NET17_SLIDES, .unit_page = "netunit17", .ex_page = "netex17" },
    { .n_slides = NET18_SLIDES, .unit_page = "netunit18", .ex_page = "netex18" },
};

void net_paragraph(GtkWidget *box, const char *text) {
    GtkWidget *l = gtk_label_new(text);

    gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(l), TRUE);
    gtk_widget_add_css_class(l, "ex-prompt");
    gtk_box_append(GTK_BOX(box), l);
}

void net_heading(GtkWidget *box, const char *text) {
    GtkWidget *l = gtk_label_new(text);

    gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(l), TRUE);
    gtk_widget_set_margin_top(l, 6);
    gtk_widget_add_css_class(l, "ex-sub");
    gtk_box_append(GTK_BOX(box), l);
}

/* Note-card slides with text that scales with the window. */
GtkWidget *net_note_host;
int net_note_last_w = -1;
NetLesson *net_notes_target;

void net_lesson_notes_ensure(NetLesson *L) {
    if (L->note_kicks)
        return;
    L->note_kicks = g_array_new(FALSE, FALSE, sizeof(GtkWidget *));
    L->note_titles = g_array_new(FALSE, FALSE, sizeof(GtkWidget *));
    L->note_bodies = g_array_new(FALSE, FALSE, sizeof(GtkWidget *));
}

void net_note_font(GtkWidget *l, int px) {
    PangoAttrList *attrs = pango_attr_list_new();

    pango_attr_list_insert(attrs, pango_attr_size_new(px * PANGO_SCALE));
    gtk_label_set_attributes(GTK_LABEL(l), attrs);
    pango_attr_list_unref(attrs);
}

void net_rescale_lesson_notes(NetLesson *L, int body, int head, int kick) {
    if (!L || !L->note_kicks)
        return;
    for (guint i = 0; i < L->note_kicks->len; i++)
        net_note_font(g_array_index(L->note_kicks, GtkWidget *, i), kick);
    for (guint i = 0; i < L->note_titles->len; i++)
        net_note_font(g_array_index(L->note_titles, GtkWidget *, i), head);
    for (guint i = 0; i < L->note_bodies->len; i++)
        net_note_font(g_array_index(L->note_bodies, GtkWidget *, i), body);
}

void net_rescale_notes_at(int w) {
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

    for (int i = 0; i < NET_LESSONS; i++)
        net_rescale_lesson_notes(&net_lessons[i], body, head, kick);
}

void net_rescale_notes(void) {
    int w = net_note_host ? gtk_widget_get_allocated_width(net_note_host) : 0;

    net_rescale_notes_at(w);
}

gboolean net_note_tick(GtkWidget *w, GdkFrameClock *clock,
                              gpointer data) {
    int width = gtk_widget_get_allocated_width(w);

    (void)clock;
    (void)data;
    if (width != net_note_last_w) {
        net_note_last_w = width;
        net_rescale_notes_at(width);
    }
    return G_SOURCE_CONTINUE;
}

GtkWidget *net_slide_card(const char *cls) {
    GtkWidget *l = gtk_label_new(NULL);

    gtk_widget_add_css_class(l, cls);
    gtk_label_set_wrap(GTK_LABEL(l), TRUE);
    gtk_label_set_justify(GTK_LABEL(l), GTK_JUSTIFY_CENTER);
    gtk_widget_set_hexpand(l, TRUE);
    gtk_widget_set_halign(l, GTK_ALIGN_FILL);
    return l;
}

void net_note_kicker(GtkWidget *box, const char *text) {
    GtkWidget *l = net_slide_card("notes-kicker");

    gtk_label_set_text(GTK_LABEL(l), text);
    gtk_box_append(GTK_BOX(box), l);
    if (net_notes_target)
        g_array_append_val(net_notes_target->note_kicks, l);
}

void net_note_title(GtkWidget *box, const char *text) {
    GtkWidget *l = net_slide_card("notes-title");

    gtk_label_set_text(GTK_LABEL(l), text);
    gtk_box_append(GTK_BOX(box), l);
    if (net_notes_target)
        g_array_append_val(net_notes_target->note_titles, l);
}

void net_note_line(GtkWidget *box, const char *text, gboolean tip) {
    GtkWidget *l = net_slide_card(tip ? "notes-tip" : "notes-body");

    gtk_label_set_text(GTK_LABEL(l), text);
    gtk_widget_set_margin_top(l, 2);
    gtk_box_append(GTK_BOX(box), l);
    if (net_notes_target)
        g_array_append_val(net_notes_target->note_bodies, l);
}

void net_slide_apply(NetLesson *L) {
    char name[16];

    if (!L || !L->stack)
        return;

    g_snprintf(name, sizeof(name), "net_slide%u", L->idx);
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

void net_lessons_apply_lang(void) {
    for (int i = 0; i < NET_LESSONS; i++)
        net_slide_apply(&net_lessons[i]);
}

void net_open_unit(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L)
        return;
    L->idx = 0;
    net_slide_apply(L);
    gtk_stack_set_visible_child_name(main_stack, L->unit_page);
}

void net_slide_prev(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L)
        return;
    if (L->idx > 0) {
        L->idx--;
        net_slide_apply(L);
    }
}

void net_slide_next(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L)
        return;
    if (L->idx + 1 < L->n_slides) {
        L->idx++;
        net_slide_apply(L);
    } else {
        gtk_stack_set_visible_child_name(main_stack, L->ex_page);
    }
}

GtkWidget *net_scroll;
GtkWidget *net_fixed;
GtkWidget *net_rail;
GtkWidget *net_nodes[NET_UNITS];
GtkWidget *net_labels[NET_UNITS];
double net_cx[NET_UNITS];
double net_cy[NET_UNITS];
int net_cols = 1;
int net_rows = 1;
int net_cw = (int)(2.0 * ROAD_MX + (NET_UNITS - 1) * PATH_SPAC);
int net_ch = (int)(2.0 * ROAD_MY);
guint net_idle;
double net_last_avail = -1.0;
int net_last_cols = -1;
int net_last_rows = -1;
int net_last_cw = -1;
int net_last_ch = -1;
cairo_surface_t *net_rail_cache;
int net_cache_w;
int net_cache_h;

void net_point(double t, double *ox, double *oy) {
    int k = (int)t;
    double u = t - (double)k;
    double u2, u3;
    double p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y;

    if (k < 0) { k = 0; u = 0.0; }
    if (k >= NET_UNITS - 1) { k = NET_UNITS - 2; u = 1.0; }

    p1x = net_cx[k];     p1y = net_cy[k];
    p2x = net_cx[k + 1]; p2y = net_cy[k + 1];
    if (k - 1 >= 0) { p0x = net_cx[k - 1]; p0y = net_cy[k - 1]; }
    else            { p0x = p1x - (p2x - p1x); p0y = p1y - (p2y - p1y); }
    if (k + 2 < NET_UNITS) { p3x = net_cx[k + 2]; p3y = net_cy[k + 2]; }
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

void net_path(cairo_t *cr, double t0, double t1) {
    const double steps_per_unit = 24.0;
    int n = (int)((t1 - t0) * steps_per_unit);
    double x, y;
    int s;

    if (n < 1)
        n = 1;
    net_point(t0, &x, &y);
    cairo_move_to(cr, x, y);
    for (s = 1; s <= n; s++) {
        net_point(t0 + (t1 - t0) * (double)s / (double)n, &x, &y);
        cairo_line_to(cr, x, y);
    }
}

void net_geometry(double avail) {
    const double phase = 2.0 * G_PI * 1.7 / (double)(NET_UNITS - 1);
    int rows;
    int r, c, i;

    for (rows = 1; rows <= NET_UNITS; rows++) {
        int cols = (NET_UNITS + rows - 1) / rows;
        if (2.0 * ROAD_MX + (cols - 1) * PATH_SPAC <= avail + 1.0)
            break;
    }
    if (rows > NET_UNITS)
        rows = NET_UNITS;
    net_rows = rows;
    net_cols = (NET_UNITS + rows - 1) / rows;
    if (net_cols < 1)
        net_cols = 1;
    net_cw = (int)(2.0 * ROAD_MX + (net_cols - 1) * PATH_SPAC);
    net_ch = (int)(2.0 * ROAD_MY + (rows - 1) * ROAD_GAP);

    for (r = 0; r < rows; r++) {
        int base = r * net_cols;
        int len = MIN(net_cols, NET_UNITS - base);
        int fwd = (r % 2) == 0;
        for (c = 0; c < len; c++) {
            i = base + c;
            int cc = fwd ? c : (net_cols - 1 - c);
            net_cx[i] = ROAD_MX + cc * PATH_SPAC;
            net_cy[i] = ROAD_MY + r * ROAD_GAP
                        + ROAD_WAVE * sin((double)i * phase);
        }
    }
}

void net_apply_layout(void) {
    if (!net_fixed)
        return;

    if (net_cols == net_last_cols && net_rows == net_last_rows &&
        net_cw == net_last_cw && net_ch == net_last_ch)
        return;

    net_last_cols = net_cols;
    net_last_rows = net_rows;
    net_last_cw = net_cw;
    net_last_ch = net_ch;
    if (net_rail_cache) {
        cairo_surface_destroy(net_rail_cache);
        net_rail_cache = NULL;
    }

    for (int i = 0; i < NET_UNITS; i++) {
        if (!net_nodes[i] || !net_labels[i])
            continue;
        gtk_fixed_move(GTK_FIXED(net_fixed), net_nodes[i],
                       (int)(net_cx[i] - NODE_SIZE / 2.0),
                       (int)(net_cy[i] - NODE_SIZE / 2.0));
        gtk_fixed_move(GTK_FIXED(net_fixed), net_labels[i],
                       (int)(net_cx[i] - (PATH_SPAC - 20.0) / 2.0),
                       (int)(net_cy[i] + NODE_SIZE / 2.0 + 10.0));
    }

    gtk_widget_set_size_request(net_fixed, net_cw, net_ch);
    gtk_widget_set_size_request(net_rail, net_cw, net_ch);
    gtk_fixed_move(GTK_FIXED(net_fixed), net_rail, 0, 0);
    gtk_widget_queue_draw(net_rail);
}

void net_relayout(void) {
    GtkAdjustment *hadj;
    double avail;

    if (!net_scroll)
        return;
    hadj = gtk_scrolled_window_get_hadjustment(
        GTK_SCROLLED_WINDOW(net_scroll));
    avail = gtk_adjustment_get_page_size(hadj);
    if (avail < 1.0)
        return;
    if (fabs(avail - net_last_avail) < 1.0)
        return;
    net_last_avail = avail;
    net_geometry(avail);
    net_apply_layout();
}

gboolean net_relayout_idle(gpointer data) {
    (void)data;
    net_idle = 0;
    net_relayout();
    return G_SOURCE_REMOVE;
}

void net_relayout_later(void) {
    if (net_idle == 0)
        net_idle = g_idle_add(net_relayout_idle, NULL);
}

void net_adjust_notify(GtkAdjustment *adj, GParamSpec *ps,
                              gpointer data) {
    (void)adj;
    (void)ps;
    (void)data;
    net_relayout_later();
}

void net_render_rail_to(cairo_t *cr) {
    const double t_end = (double)(NET_UNITS - 1);
    const Rgb mauve = color_from_hex(app_theme.accent);
    const Rgb rail = color_from_hex(app_theme.rail);
    const Rgb muted = color_from_hex(app_theme.subtext);
    double hx, hy;

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

    net_point(0.0, &hx, &hy);
    draw_node_halo(cr, hx, hy, NODE_SIZE * 1.05,
                   mauve.r, mauve.g, mauve.b, 0.18);

    cairo_new_path(cr);
    net_path(cr, 0.0, t_end);
    cairo_set_line_width(cr, 16);
    cairo_set_source_rgb(cr, rail.r, rail.g, rail.b);
    cairo_stroke(cr);

    cairo_set_source_rgba(cr, muted.r, muted.g, muted.b, 0.17);
    cairo_set_dash(cr, (double[]){2.0, 40.0}, 2, 0.0);
    cairo_new_path(cr);
    net_path(cr, 0.0, t_end);
    cairo_stroke(cr);
    cairo_set_dash(cr, NULL, 0, 0.0);
}

void net_draw_rail(GtkDrawingArea *area, cairo_t *cr,
                          int width, int height, gpointer data) {
    (void)area;
    (void)width;
    (void)height;
    (void)data;

    if (!net_rail_cache || net_cache_w != net_cw || net_cache_h != net_ch) {
        cairo_t *rcr;

        if (net_rail_cache)
            cairo_surface_destroy(net_rail_cache);
        net_cache_w = net_cw;
        net_cache_h = net_ch;
        if (net_cache_w < 1)
            net_cache_w = 1;
        if (net_cache_h < 1)
            net_cache_h = 1;
        net_rail_cache = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                    net_cache_w, net_cache_h);
        rcr = cairo_create(net_rail_cache);
        net_render_rail_to(rcr);
        cairo_destroy(rcr);
    }

    cairo_set_source_surface(cr, net_rail_cache, 0, 0);
    cairo_paint(cr);
}

void net_rail_theme_reset(void) {
    if (net_rail_cache) {
        cairo_surface_destroy(net_rail_cache);
        net_rail_cache = NULL;
    }
    if (net_rail)
        gtk_widget_queue_draw(net_rail);
}

void net_add_node(GtkFixed *fixed, int index) {
    int num = index + 1;
    gboolean locked = index >= NET_LESSONS;
    GtkWidget *card;
    GtkWidget *vbox;
    GtkWidget *number;
    GtkWidget *icon;
    GtkWidget *name;
    char *text;
    static const char *unit_keys[NET_LESSONS] = {
        "net_unit1", "net_unit2", "net_unit3", "net_unit4", "net_unit5",
        "net_unit6", "net_unit7", "net_unit8", "net_unit9", "net_unit10",
        "net_unit11", "net_unit12", "net_unit13", "net_unit14", "net_unit15",
        "net_unit16", "net_unit17", "net_unit18"
    };

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
        net_lessons[index].done_icon = icon;
        g_signal_connect(card, "clicked", G_CALLBACK(net_open_unit),
                         &net_lessons[index]);
    }

    net_nodes[index] = card;
    gtk_fixed_put(fixed, card, 0, 0);

    name = gtk_label_new(NULL);
    gtk_widget_set_size_request(name, (int)(PATH_SPAC - 20.0), -1);
    gtk_widget_set_halign(name, GTK_ALIGN_CENTER);
    gtk_label_set_justify(GTK_LABEL(name), GTK_JUSTIFY_CENTER);
    gtk_label_set_wrap(GTK_LABEL(name), TRUE);
    gtk_widget_add_css_class(name, "unit-name");
    if (index < NET_LESSONS) {
        i18n_bind(name, unit_keys[index], 0);
    } else {
        gtk_widget_add_css_class(name, "unit-name-locked");
    }
    net_labels[index] = name;
    gtk_fixed_put(fixed, name, 0, 0);
}

GtkWidget *build_netmap_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *fixed;
    GtkWidget *rail;
    GtkAdjustment *ha;
    GtkAdjustment *va;

    net_geometry(1000.0);

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("subjects", "Správa počítačových sítí", "net_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);
    net_scroll = scroll;

    wrap = gtk_center_box_new();
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), wrap);

    fixed = gtk_fixed_new();
    gtk_widget_set_halign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(fixed, net_cw, net_ch);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), fixed);
    net_fixed = fixed;

    rail = gtk_drawing_area_new();
    gtk_widget_set_size_request(rail, net_cw, net_ch);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(rail), net_draw_rail,
                                   NULL, NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rail, 0, 0);
    net_rail = rail;

    for (int i = 0; i < NET_UNITS; i++)
        net_add_node(GTK_FIXED(fixed), i);

    ha = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scroll));
    va = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));
    g_signal_connect(ha, "notify::page-size",
                     G_CALLBACK(net_adjust_notify), NULL);
    g_signal_connect(va, "notify::page-size",
                     G_CALLBACK(net_adjust_notify), NULL);

    net_relayout_later();
    refresh_net_completion_ui();

    return page;
}

GtkWidget *build_net_unit_page(NetLesson *L, const char *title_key,
                               const char *sub_key, const NetSlide *slides,
                               guint n_slides) {
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

    /* sub_key must be a stable string (i18n_bind keeps the pointer). */
    gtk_box_append(GTK_BOX(page), top_bar("netmap", title_key, sub_key));

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
    net_note_host = L->stack;

    for (guint s = 0; s < n_slides; s++) {
        GtkWidget *stage = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        GtkWidget *card;
        char name[16];

        gtk_widget_set_vexpand(stage, TRUE);
        g_snprintf(name, sizeof(name), "net_slide%u", s);
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

    gtk_widget_add_tick_callback(L->stack, net_note_tick, NULL, NULL);
    net_note_last_w = -1;
    net_rescale_notes();
    net_notes_target = NULL;

    nav = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(nav), TRUE);
    gtk_widget_set_margin_top(nav, 10);
    gtk_box_append(GTK_BOX(page), nav);

    L->prev_btn = gtk_button_new_with_label(tr("net_slide_prev"));
    gtk_widget_add_css_class(L->prev_btn, "btn-primary");
    g_signal_connect(L->prev_btn, "clicked", G_CALLBACK(net_slide_prev), L);
    gtk_box_append(GTK_BOX(nav), L->prev_btn);

    L->next_btn = gtk_button_new_with_label(tr("net_slide_next"));
    gtk_widget_add_css_class(L->next_btn, "btn-primary");
    g_signal_connect(L->next_btn, "clicked", G_CALLBACK(net_slide_next), L);
    gtk_box_append(GTK_BOX(nav), L->next_btn);

    L->idx = 0;
    net_slide_apply(L);

    return page;
}

GtkWidget *build_net_unit1_page(void) {
    static const NetSlide slides[NET_SLIDES] = {
        {
            "1 / 4   •   VLSM", "Podsíťování sítí",
            "Tip: prefix /n = počet jedniček v masce.  /24 = 255.255.255.0",
            {
                "Cíl: rozdělit jednu IP síť na menší podsítě podle počtu uzlů.",
                "Pamatuj si tři role adres v každém bloku:",
                "síťová = první adresa bloku",
                "broadcast = poslední adresa bloku",
                "uzly = vše mezi nimi (síť + 1 až broadcast − 1)",
                NULL,
            },
        },
        {
            "2 / 4   •   Velikosti", "Jak velký blok potřebuji?",
            "Počet bitů pro uzly h = log₂(velikost),  prefix = 32 − h",
            {
                "Pro X uzlů potřebuješ blok, do kterého se vejde X + 2 adresy "
                "(síť a broadcast).",
                "Velikost bloku je vždy mocnina dvojky – vezmi nejmenší blok, "
                "který je větší nebo roven X + 2.",
                "60 uzlů → 62 → blok 64 → /26",
                "30 uzlů → 32 → blok 32 → /27",
                "14 uzlů → 16 → blok 16 → /28",
                "7 uzlů  → 9  → blok 16 → /28",
                NULL,
            },
        },
        {
            "3 / 4   •   Postup", "Jak na to krok za krokem",
            NULL,
            {
                "1. Seřaď podsítě od největší po nejmenší.",
                "2. Pro každou najdi potřebný prefix.",
                "3. Začni na síťové adrese ze zadání.",
                "4. Rozsah podsítě = od síťové adresy po broadcast.",
                "5. Rozsah uzlů = síťová + 1 až broadcast − 1.",
                "6. Posuň se hned za broadcast a pokračuj dál.",
                NULL,
            },
        },
        {
            "4 / 4   •   Příklad", "Ukázka: 10.0.0.0/24",
            "A: 60 uzlů, B: 30, C: 14, D: 7",
            {
                "A  → 10.0.0.0/26    uzly 10.0.0.1–10.0.0.62    "
                "broadcast 10.0.0.63",
                "B  → 10.0.0.64/27   uzly 10.0.0.65–10.0.0.94   "
                "broadcast 10.0.0.95",
                "C  → 10.0.0.96/28   uzly 10.0.0.97–10.0.0.110  "
                "broadcast 10.0.0.111",
                "D  → 10.0.0.112/28  uzly 10.0.0.113–10.0.0.126 "
                "broadcast 10.0.0.127",
                NULL,
            },
        },
    };

    return build_net_unit_page(&net_lessons[0], "net_unit1", "net_unit1_sub",
                               slides, NET_SLIDES);
}

GtkWidget *build_net_unit2_page(void) {
    static const NetSlide slides[NET2_SLIDES] = {
        {
            "1 / 5   •   Jednotky", "Bit, byte a počítačová síť",
            "Tip: 1 B = 8 b  →  2⁸ = 256 možných hodnot",
            {
                "Bit (1 b): základní jednotka informace – 0 nebo 1.",
                "0 = nevede proud, 1 = vede proud.",
                "Byte (1 B): 1 B = 8 b, nabývá 256 hodnot.",
                "Počítačová síť: propojení uzlů (PC a dalších prvků).",
                "Vyžaduje hardwarové i softwarové prostředky.",
                NULL,
            },
        },
        {
            "2 / 5   •   Propojení", "Trvalé, dočasné a důvody zapojení",
            NULL,
            {
                "Trvalé propojení: kabely – síť zůstává připojená i po přenosu.",
                "Dočasné propojení: modem, Wi‑Fi – po přenosu se odpojuje.",
                "Důvody zapojení do sítě:",
                "sdílení dat (nutná podpora aplikací)",
                "sdílení HW (tiskárny, disky) a SW (databáze)",
                "vyšší spolehlivost, bezpečnost (přístupová práva)",
                "komunikace (e‑mail, hlas)",
                NULL,
            },
        },
        {
            "3 / 5   •   Pojmy", "Terminologie sítí",
            "Architektura = topologie + standard",
            {
                "Uzel: jakékoli zařízení (stanice, server, aktivní prvek).",
                "Topologie: způsob propojení (hvězda, kruh, sběrnice).",
                "Standard: normy pro zpracování dat.",
                "Architektura: topologie + standard.",
                NULL,
            },
        },
        {
            "4 / 5   •   Rozlehlost", "WAN, MAN a LAN",
            NULL,
            {
                "WAN: rozsáhlé sítě (státy, kontinenty), např. internet.",
                "MAN: městské sítě – propojují lokální sítě.",
                "LAN: lokální sítě (budova), kabeláž v řádu km,",
                "jednotná architektura.",
                NULL,
            },
        },
        {
            "5 / 5   •   Uzly", "Server, stanice a peer‑to‑peer",
            NULL,
            {
                "Server: nadřazený, řídící, poskytuje služby, vyšší výkon.",
                "Pracovní stanice: pro uživatele, jednodušší konfigurace.",
                "Peer‑to‑peer: síť bez serverů – jen stanice,",
                "které sdílejí data a tiskárny.",
                NULL,
            },
        },
    };

    return build_net_unit_page(&net_lessons[1], "net_unit2", "net_unit2_sub",
                               slides, NET2_SLIDES);
}

GtkWidget *build_net_unit3_page(void) {
    static const NetSlide slides[NET3_SLIDES] = {
        {
            "1 / 4   •   Servery", "Role serverů a typy podle služby",
            NULL,
            {
                "Servery ověřují uživatele a povolují přístup.",
                "Souborový server (fileserver): sdílení souborů.",
                "Tiskový server: sdílení tiskáren.",
                "Poštovní server: e‑mailové služby.",
                "Databázový server: správa a přístup k databázím.",
                NULL,
            },
        },
        {
            "2 / 4   •   Rozložení", "Centralizovaný a distribuovaný server",
            NULL,
            {
                "Centralizovaný: jeden server v síti.",
                "Výpadek = nedostupnost služby.",
                "Distribuovaný: více serverů, každý obsluhuje část.",
                "Vyšší spolehlivost a rychlost.",
                NULL,
            },
        },
        {
            "3 / 4   •   Využití", "Vyhrazený, nevyhrazený a mainframe",
            NULL,
            {
                "Nevyhrazený: slouží i jako stanice (levnější).",
                "Vyhrazený: jen síťové služby – vysoký výkon a bezpečnost,",
                "typicky v racku.",
                "Mainframe: obří sálové počítače (IBM, Siemens),",
                "stovky procesorů, OS Unix (AIX, Solaris).",
                "Použití: banky a státní správa.",
                NULL,
            },
        },
        {
            "4 / 4   •   Cloud", "Cloudové služby",
            "Tip: SaaS = software jako služba",
            {
                "Cloud: globální síť vzdálených serverů propojených internetem.",
                "Fungují jako jeden systém: ukládání dat, SaaS,",
                "streamování, webová pošta.",
                "Přístup online z jakéhokoli zařízení.",
                NULL,
            },
        },
    };

    return build_net_unit_page(&net_lessons[2], "net_unit3", "net_unit3_sub",
                               slides, NET3_SLIDES);
}

GtkWidget *build_net_unit4_page(void) {
    static const NetSlide slides[NET4_SLIDES] = {
        {
            "1 / 4   •   Přenos", "Paralelní a sériový přenos",
            NULL,
            {
                "Paralelní: více bitů současně po více vodičích.",
                "Problém: přeslechy (rušení) při delších kabelech",
                "a vysokých frekvencích.",
                "Sériový: bit po bitu po jednom vodiči.",
                "Umožňuje vyšší frekvence i délky – celkově rychlejší.",
                NULL,
            },
        },
        {
            "2 / 4   •   Časování", "Asynchronní přenos",
            "Tip: Start-bit = 0, Stop-bit = 1",
            {
                "Data se posílají v blocích (znaky 5–8 bitů).",
                "Start-bit (0) synchronizuje začátek znaku.",
                "Stop-bit (1) znak ukončuje.",
                NULL,
            },
        },
        {
            "3 / 4   •   Časování", "Synchronní přenos",
            NULL,
            {
                "Řízeno společným hodinovým signálem.",
                "Přesné časování mezi vysílačem a přijímačem.",
                NULL,
            },
        },
        {
            "4 / 4   •   Zabezpečení", "Parita, checksum a CRC",
            "Tip: CRC je nejbezpečnější z těchto tří metod",
            {
                "Parita: nejslabší – přidá bit pro sudý/lichý počet jedniček.",
                "Checksum: součet znaků jako dvojkových čísel.",
                "CRC (cyklické kódy): nejbezpečnější,",
                "počítá se z jednotlivých bitů v bloku.",
                NULL,
            },
        },
    };

    return build_net_unit_page(&net_lessons[3], "net_unit4", "net_unit4_sub",
                               slides, NET4_SLIDES);
}

GtkWidget *build_net_unit5_page(void) {
    static const NetSlide slides[NET5_SLIDES] = {
        {
            "1 / 4   •   Signál", "Analogový a digitální signál",
            "Tip: f = 1/T",
            {
                "Analogový: spojitý, nabývá libovolných hodnot.",
                "Digitální: diskrétní, hodnoty 0 a 1.",
                "Parametry signálu:",
                "perioda T, frekvence f = 1/T,",
                "amplituda A, fázový posun Φ.",
                NULL,
            },
        },
        {
            "2 / 4   •   Modulace", "Základní typy modulace",
            "Modulace = přenos v přeloženém pásmu",
            {
                "AM – amplitudová modulace.",
                "FM – frekvenční modulace.",
                "PM – fázová modulace.",
                NULL,
            },
        },
        {
            "3 / 4   •   Kombinované", "QPSK a 256-QAM",
            NULL,
            {
                "QPSK: 2 bity v jednom prvku (symbolu).",
                "256-QAM: 8 bitů v jednom prvku.",
                NULL,
            },
        },
        {
            "4 / 4   •   Rychlosti", "Modulační, přenosová a šířka pásma",
            NULL,
            {
                "Modulační rychlost: počet změn signálu za sekundu [Baud, Bd/s].",
                "Přenosová rychlost: velikost přenesené informace [bit/s, bps].",
                "Šířka pásma: rozsah frekvencí –",
                "určuje maximální možnou rychlost.",
                NULL,
            },
        },
    };

    return build_net_unit_page(&net_lessons[4], "net_unit5", "net_unit5_sub",
                               slides, NET5_SLIDES);
}

GtkWidget *build_net_unit6_page(void) {
    static const NetSlide slides[NET6_SLIDES] = {
        {
            "1 / 4   •   Spoj", "Datový spoj",
            NULL,
            {
                "Datový spoj umožňuje výměnu informací.",
                "Může být dvoubodový nebo vícebodový.",
                NULL,
            },
        },
        {
            "2 / 4   •   Cesta", "Přenosová cesta",
            NULL,
            {
                "Přenosová cesta je fyzické médium.",
                "Příklady: metalické nebo optické kabely,",
                "mikrovlny, družice.",
                NULL,
            },
        },
        {
            "3 / 4   •   Kanál a okruh", "Přenosový kanál a okruh",
            "Tip: okruh = dva kanály → obousměrný přenos",
            {
                "Přenosový kanál: jednosměrný souhrn prostředků pro spojení.",
                "Okruh: tvořen dvěma kanály – umožňuje obousměrný přenos.",
                NULL,
            },
        },
        {
            "4 / 4   •   Trunking", "Spojování cest",
            NULL,
            {
                "Trunking spojuje více cest do jednoho okruhu.",
                "Cíl: zvýšení šířky pásma.",
                NULL,
            },
        },
    };

    return build_net_unit_page(&net_lessons[5], "net_unit6", "net_unit6_sub",
                               slides, NET6_SLIDES);
}

GtkWidget *build_net_unit7_page(void) {
    static const NetSlide slides[NET7_SLIDES] = {
        {
            "1 / 4   •   Multiplex", "Rozdělení kanálu",
            NULL,
            {
                "Multiplex: rozdělení jednoho kanálu",
                "na více logických podkanálů.",
                NULL,
            },
        },
        {
            "2 / 4   •   Typy", "FDM, TDM a STDM",
            NULL,
            {
                "FDM (frekvenční): každému kanálu část frekvenčního pásma.",
                "TDM (časový): každému kanálu vyhrazený časový slot.",
                "STDM (statistický): kapacita dle potřeby (paketový přenos),",
                "negarantuje 100% dostupnost.",
                NULL,
            },
        },
        {
            "3 / 4   •   Směry", "Simplex, poloduplex a duplex",
            NULL,
            {
                "Simplex: jen jeden směr (např. optické vlákno).",
                "Poloduplex (half-duplex): oběma směry, ne současně.",
                "Duplex (full-duplex): oběma směry současně.",
                NULL,
            },
        },
        {
            "4 / 4   •   Agregace", "Sdílení kapacity",
            NULL,
            {
                "Agregace: sdílení kapacity kanálu více uživateli.",
                NULL,
            },
        },
    };

    return build_net_unit_page(&net_lessons[6], "net_unit7", "net_unit7_sub",
                               slides, NET7_SLIDES);
}

GtkWidget *build_net_unit8_page(void) {
    static const NetSlide slides[NET8_SLIDES] = {
        {
            "1 / 4   •   Okruhy", "Přepojování okruhů",
            "Tip: spojovaná služba – jako telefon",
            {
                "Cesta se vytýčí předem (spojovaná služba).",
                "Kapacita je garantována.",
                "Přenos probíhá v reálném čase.",
                NULL,
            },
        },
        {
            "2 / 4   •   Pakety", "Přepojování paketů",
            NULL,
            {
                "Data se dělí na pakety s adresami.",
                "Každý paket může jít jinou cestou.",
                NULL,
            },
        },
        {
            "3 / 4   •   Hodnocení", "Výhody a nevýhody paketů",
            NULL,
            {
                "Výhody: reakce na zátěž, lepší využití kapacity.",
                "Nevýhody: negarantuje pořadí ani plynulost.",
                "Nevhodné pro video v reálném čase.",
                NULL,
            },
        },
        {
            "4 / 4   •   Virtuální", "Virtuální spoje",
            NULL,
            {
                "Cesta je vytýčena předem,",
                "ale prostředky se využívají jen při průchodu paketu.",
                "Výsledek: zrychlení datagramů.",
                NULL,
            },
        },
    };

    return build_net_unit_page(&net_lessons[7], "net_unit8", "net_unit8_sub",
                               slides, NET8_SLIDES);
}

GtkWidget *build_net_unit9_page(void) {
    static const NetSlide slides[NET9_SLIDES] = {
        {
            "1 / 4   •   Proč", "Standardizace a dekompozice",
            NULL,
            {
                "Standardizace zajišťuje kompatibilitu mezi výrobci.",
                "Dekompozice: rozklad složitého problému přenosu na vrstvy.",
                NULL,
            },
        },
        {
            "2 / 4   •   Vrstvy", "Komunikace mezi vrstvami",
            "Tip: SAP = Service Access Point",
            {
                "Vrstvy komunikují jen se sousedy přes rozhraní (SAP).",
                NULL,
            },
        },
        {
            "3 / 4   •   Protokol", "Pravidla komunikace",
            NULL,
            {
                "Protokol: pravidla komunikace mezi stejnými vrstvami",
                "na různých uzlech.",
                NULL,
            },
        },
        {
            "4 / 4   •   Jednotky", "Hlavičky a PDU",
            NULL,
            {
                "Každá vrstva přidá k datům hlavičku (header).",
                "Rámec – linková vrstva.",
                "Paket – síťová vrstva.",
                "Segment – transportní vrstva.",
                NULL,
            },
        },
    };

    return build_net_unit_page(&net_lessons[8], "net_unit9", "net_unit9_sub",
                               slides, NET9_SLIDES);
}

GtkWidget *build_net_unit10_page(void) {
    static const NetSlide slides[NET10_SLIDES] = {
        {
            "1 / 3   •   ISO/OSI", "Sedmivrstvý model",
            NULL,
            {
                "ISO/OSI: 7vrstvý model.",
                "Je univerzální – popisuje komunikaci obecně.",
                "V praxi je ale pomalejší: každá vrstva řešila spolehlivost.",
                NULL,
            },
        },
        {
            "2 / 3   •   TCP/IP", "Praktická architektura",
            NULL,
            {
                "TCP/IP: architektura, která zvítězila v praxi.",
                "Vycházela ze skutečných potřeb sítí.",
                "Spolehlivost řeší až vyšší vrstvy.",
                NULL,
            },
        },
        {
            "3 / 3   •   Srovnání", "Proč zvítězil TCP/IP",
            "Tip: méně režie na nižších vrstvách = rychlejší provoz",
            {
                "ISO/OSI: teoretický, univerzální, těžší a pomalejší.",
                "TCP/IP: praktický, jednodušší dělení odpovědností.",
                "Proto se TCP/IP stal základem dnešního internetu.",
                NULL,
            },
        },
    };

    return build_net_unit_page(&net_lessons[9], "net_unit10", "net_unit10_sub",
                               slides, NET10_SLIDES);
}

GtkWidget *build_net_unit11_page(void) {
    static const NetSlide slides[NET11_SLIDES] = {
        {
            "1 / 8   •   Přehled", "Tři skupiny vrstev",
            "Tip: přenos → přizpůsobení → aplikace",
            {
                "Vrstvy orientované na přenos: fyzická, spojová, síťová.",
                "Přizpůsobovací vrstva: transportní.",
                "Vrstvy orientované na aplikace: relační, prezentační,",
                "aplikační.",
                NULL,
            },
        },
        {
            "2 / 8   •   Fyzická", "Přenos bitů",
            NULL,
            {
                "Přenos bitů po médiu.",
                "Definuje napětí, kabely, konektory, kódování a modulaci.",
                "Řeší také duplex / simplex.",
                NULL,
            },
        },
        {
            "3 / 8   •   Spojová", "Rámce a MAC",
            "Tip: podvrstvy MAC (přístup k médiu) a LLC (logické řízení)",
            {
                "Tvorba rámců a fyzické (MAC) adresy.",
                "Kontrola chyb (CRC) a řízení toku mezi sousedními uzly.",
                "Podvrstvy: MAC a LLC.",
                NULL,
            },
        },
        {
            "4 / 8   •   Síťová", "Směrování paketů",
            NULL,
            {
                "Směrování (routing) paketů.",
                "Funguje i v sítích bez přímého spojení mezi uzly.",
                NULL,
            },
        },
        {
            "5 / 8   •   Transportní", "Segmenty end-to-end",
            "Tip: v routerech transportní vrstva není",
            {
                "Rozklad zpráv na segmenty a jejich zpětné složení.",
                "Kontrola pořadí a náprava chyb.",
                "Existuje jen v koncových uzlech – není v routerech.",
                NULL,
            },
        },
        {
            "6 / 8   •   Relační", "Relace a dialog",
            NULL,
            {
                "Navazuje, udržuje a ukončuje relace.",
                "Řídí dialog („neskákat si do řeči“).",
                "Synchronizace – navázání po přerušení.",
                NULL,
            },
        },
        {
            "7 / 8   •   Prezentační", "Formát dat",
            NULL,
            {
                "Formátování dat (např. ASCII).",
                "Komprese a šifrování.",
                "Převod mezi různými standardy.",
                NULL,
            },
        },
        {
            "8 / 8   •   Aplikační", "Rozhraní pro programy",
            NULL,
            {
                "Rozhraní pro aplikace a služby.",
                "Například e-mail, HTTP, FTP nebo terminál.",
                NULL,
            },
        },
    };

    return build_net_unit_page(&net_lessons[10], "net_unit11", "net_unit11_sub",
                               slides, NET11_SLIDES);
}

GtkWidget *build_net_unit12_page(void) {
    static const NetSlide slides[NET12_SLIDES] = {
        {
            "1 / 6   •   Mapování", "TCP/IP vs ISO/OSI",
            NULL,
            {
                "Aplikační TCP/IP = aplikační + prezentační + relační ISO/OSI.",
                "Transportní TCP/IP = transportní ISO/OSI.",
                "Síťová (internetová) = síťová ISO/OSI.",
                "Vrstva síťového rozhraní = linková + fyzická ISO/OSI.",
                NULL,
            },
        },
        {
            "2 / 6   •   Vznik", "Nejprve protokoly",
            "Tip: opačný postup než u ISO/OSI",
            {
                "Nejprve vznikly protokoly pro Internet.",
                "Až poté byl definován model TCP/IP.",
                "U ISO/OSI to bylo naopak: nejdřív model, pak protokoly.",
                NULL,
            },
        },
        {
            "3 / 6   •   Aplikace", "Aplikační protokoly",
            NULL,
            {
                "HTTP, HTTPS, FTP, DHCP, DNS.",
                "Pošta: POP3, IMAP, SMTP.",
                NULL,
            },
        },
        {
            "4 / 6   •   Transport", "TCP a UDP",
            NULL,
            {
                "TCP: spolehlivý, spojovaný.",
                "UDP: nespolehlivý, rychlý.",
                NULL,
            },
        },
        {
            "5 / 6   •   Síť", "Internetové protokoly",
            NULL,
            {
                "IP – základní přenos paketů.",
                "ICMP, ARP.",
                "Směrování: RIP, OSPF.",
                NULL,
            },
        },
        {
            "6 / 6   •   Rozhraní", "Síťové rozhraní",
            NULL,
            {
                "Ethernet, Token Ring, PPP, ATM.",
                "Odpovídá linkové a fyzické vrstvě ISO/OSI.",
                NULL,
            },
        },
    };

    return build_net_unit_page(&net_lessons[11], "net_unit12", "net_unit12_sub",
                               slides, NET12_SLIDES);
}

GtkWidget *build_net_unit13_page(void) {
    static const NetSlide slides[NET13_SLIDES] = {
        {
            "1 / 4   •   Přehled", "Topologie sítě",
            NULL,
            {
                "Topologie popisuje, jak jsou uzly propojené.",
                "Základní typy: sběrnice, hvězda a kruh.",
                NULL,
            },
        },
        {
            "2 / 4   •   Sběrnice", "Bus",
            "Tip: dříve typicky koaxiální kabel",
            {
                "Médium sdílí všichni – jednoduchá a levná topologie.",
                "Malá délka kabelů.",
                "Nevýhody: nízká bezpečnost (všichni slyší vše).",
                "Přerušení kabelu nebo terminátoru vyřadí celou síť.",
                NULL,
            },
        },
        {
            "3 / 4   •   Hvězda", "Star",
            "Tip: kroucená dvoulinka nebo optika",
            {
                "Uzly jdou přes centrální prvek (hub nebo switch).",
                "Výhody: odolnost proti poruchám kabelů k uzlům,",
                "snadná diagnostika.",
                "Nevýhody: aktivní prvky a více kabeláže.",
                NULL,
            },
        },
        {
            "4 / 4   •   Kruh", "Ring",
            "Tip: často zdvojený kruh pro odolnost",
            {
                "Kabely tvoří souvislý kruh.",
                "Zprávy obíhají, dokud nenajdou adresáta.",
                NULL,
            },
        },
    };

    return build_net_unit_page(&net_lessons[12], "net_unit13", "net_unit13_sub",
                               slides, NET13_SLIDES);
}

GtkWidget *build_net_unit14_page(void) {
    static const NetSlide slides[NET14_SLIDES] = {
        {
            "1 / 5   •   Princip", "Světlo místo proudu",
            "Tip: obousměrný provoz = dvě vlákna",
            {
                "Elektrické impulsy se převádí na světlo (LED nebo laser).",
                "Zpět je zachytí fotodioda.",
                "Přenos jedním vláknem je jednosměrný.",
                NULL,
            },
        },
        {
            "2 / 5   •   Výhody", "Proč optika",
            NULL,
            {
                "Imunita vůči elektromagnetickému rušení.",
                "Obrovská šířka pásma a dosah až kolem 100 km.",
                "Lepší bezpečnost proti odposlechu.",
                "Malý průměr a nízká váha.",
                NULL,
            },
        },
        {
            "3 / 5   •   Nevýhody", "Cena a montáž",
            NULL,
            {
                "Náročná montáž.",
                "Dražší aktivní prvky a konektory.",
                NULL,
            },
        },
        {
            "4 / 5   •   MM", "Mnohovidová vlákna",
            NULL,
            {
                "Větší jádro: 50 / 62,5 µm.",
                "Více cest světla (vidů).",
                "Levnější zdroje – typicky LED.",
                NULL,
            },
        },
        {
            "5 / 5   •   SM", "Jednovidová vlákna",
            NULL,
            {
                "Malé jádro: 7–10 µm.",
                "Jen jeden paprsek osou vlákna.",
                "Velký dosah, vyžaduje laser.",
                NULL,
            },
        },
    };

    return build_net_unit_page(&net_lessons[13], "net_unit14", "net_unit14_sub",
                               slides, NET14_SLIDES);
}

GtkWidget *build_net_unit15_page(void) {
    static const NetSlide slides[NET15_SLIDES] = {
        {
            "1 / 3   •   Primární", "Ochrana 250 µm",
            NULL,
            {
                "Primární ochrana má průměr 250 µm.",
                "Chrání samotné vlákno hned po výrobě.",
                NULL,
            },
        },
        {
            "2 / 3   •   Sekundární", "Ochrana 900 µm",
            "Tip: těsná pro svislé, volná pro vodorovné instalace",
            {
                "Sekundární ochrana má průměr 900 µm.",
                "Těsná varianta: vhodné pro svislé instalace.",
                "Volná varianta: vhodné pro vodorovné instalace.",
                NULL,
            },
        },
        {
            "3 / 3   •   Montáž", "Lámání a sváření",
            "Tip: kolmost lomu do 0,5°",
            {
                "Vyžaduje precizní lámání lámačkou.",
                "Spojování se dělá svářečkou optických vláken.",
                NULL,
            },
        },
    };

    return build_net_unit_page(&net_lessons[14], "net_unit15", "net_unit15_sub",
                               slides, NET15_SLIDES);
}

GtkWidget *build_net_unit16_page(void) {
    static const NetSlide slides[NET16_SLIDES] = {
        {
            "1 / 4   •   Kolize", "Sdílené médium",
            NULL,
            {
                "Kolize nastane, když na sdíleném médiu vysílá více uzlů najednou.",
                NULL,
            },
        },
        {
            "2 / 4   •   Aloha", "Vysílej kdykoliv",
            "Tip: časté kolize, primitivní metoda",
            {
                "Uzel vysílá kdykoliv chce.",
                "Když nepřijde potvrzení, pošle data znovu.",
                "Kolize jsou časté.",
                NULL,
            },
        },
        {
            "3 / 4   •   CSMA/CD", "Ethernet",
            "Tip: Carrier Sense + Collision Detection",
            {
                "Nejdřív se poslouchá nosná (Carrier Sense).",
                "Je-li ticho, uzel vysílá.",
                "Při kolizi se vysílání zastaví a zkusí se znovu",
                "po náhodném čase.",
                NULL,
            },
        },
        {
            "4 / 4   •   CSMA/CA", "Wi-Fi",
            "Tip: Collision Avoidance přes RTS/CTS",
            {
                "Předchází kolizím (Collision Avoidance).",
                "RTS (Request to Send) a CTS (Clear to Send)",
                "rezervují médium před vysíláním.",
                NULL,
            },
        },
    };

    return build_net_unit_page(&net_lessons[15], "net_unit16", "net_unit16_sub",
                               slides, NET16_SLIDES);
}

GtkWidget *build_net_unit17_page(void) {
    static const NetSlide slides[NET17_SLIDES] = {
        {
            "1 / 7   •   Přehled", "Aktivní prvky",
            NULL,
            {
                "Aktivní prvky zpracovávají nebo převádí signál / data.",
                "Liší se podle vrstvy ISO/OSI, na které pracují.",
                NULL,
            },
        },
        {
            "2 / 7   •   Repeater", "Opakovač",
            "Tip: fyzická vrstva",
            {
                "Zesiluje signál a tím zvyšuje dosah.",
                NULL,
            },
        },
        {
            "3 / 7   •   Transceiver", "Konvertor",
            "Tip: fyzická vrstva",
            {
                "Převádí signál mezi různými médii.",
                "Například metaliku na optiku.",
                NULL,
            },
        },
        {
            "4 / 7   •   Hub", "Rozbočovač",
            "Tip: fyzická vrstva – „hloupý“ prvek",
            {
                "Co přijme na jednom portu, rozešle do všech ostatních.",
                NULL,
            },
        },
        {
            "5 / 7   •   Bridge/Switch", "Most a přepínač",
            "Tip: linková vrstva – rámce a MAC",
            {
                "Pracují s rámci a MAC adresami.",
                "Switch dělí síť na kolizní domény.",
                NULL,
            },
        },
        {
            "6 / 7   •   Router", "Směrovač",
            "Tip: síťová vrstva – pakety a IP",
            {
                "Pracuje s pakety a IP adresami.",
                "Propojuje různé sítě.",
                NULL,
            },
        },
        {
            "7 / 7   •   Gateway", "Brána",
            "Tip: vyšší vrstvy",
            {
                "Převádí mezi odlišnými architekturami nebo protokoly.",
                NULL,
            },
        },
    };

    return build_net_unit_page(&net_lessons[16], "net_unit17", "net_unit17_sub",
                               slides, NET17_SLIDES);
}

GtkWidget *build_net_unit18_page(void) {
    static const NetSlide slides[NET18_SLIDES] = {
        {
            "1 / 5   •   Funkce", "Učení MAC adres",
            "Tip: CAM tabulka = adresa → port",
            {
                "Switch se učí MAC adresy odesílatelů.",
                "Ukládá je do CAM tabulky.",
                "Rámce pak posílá jen na konkrétní port.",
                NULL,
            },
        },
        {
            "2 / 5   •   Metody", "Tři způsoby přepínání",
            NULL,
            {
                "Cut-through, store-and-forward a fragment-free.",
                "Liší se rychlostí a kontrolou chyb.",
                NULL,
            },
        },
        {
            "3 / 5   •   Cut-through", "Hned po adrese",
            NULL,
            {
                "Čte jen adresu příjemce a hned posílá dál.",
                "Nejrychlejší metoda.",
                "Nevýhoda: posílá i chybné rámce.",
                NULL,
            },
        },
        {
            "4 / 5   •   Store-and-forward", "Celý rámec + CRC",
            NULL,
            {
                "Přijme celý rámec a zkontroluje CRC.",
                "Teprve pak rámec pošle.",
                "Nejpomalejší, ale nejbezpečnější metoda.",
                NULL,
            },
        },
        {
            "5 / 5   •   Fragment-free", "Modifikovaný cut-through",
            "Tip: prvních 64 bytů = detekce kolizí",
            {
                "Čte prvních 64 bytů rámce.",
                "To stačí k odhalení kolizních fragmentů.",
                "Kompromis mezi rychlostí a bezpečností.",
                NULL,
            },
        },
    };

    return build_net_unit_page(&net_lessons[17], "net_unit18", "net_unit18_sub",
                               slides, NET18_SLIDES);
}

/* Solutions below are the canonical "largest subnet first, in order A–D"
 * allocation. Scenario 3 does not fit into a /24 as written, which is why
 * its "solution" explains that instead. */

const NetTask net_tasks[] = {
    {
        "1.) Síť má adresu 10.0.0.0/24. Rozdělte ji do podsítí s následujícími "
        "požadavky:\n\n"
        "Podsíť A: 60 uzlů\nPodsíť B: 30 uzlů\nPodsíť C: 14 uzlů\n"
        "Podsíť D: 7 uzlů",
        "A: 10.0.0.0/26   síť 10.0.0.0   uzly 10.0.0.1–10.0.0.62   "
        "broadcast 10.0.0.63\n"
        "B: 10.0.0.64/27  síť 10.0.0.64  uzly 10.0.0.65–10.0.0.94   "
        "broadcast 10.0.0.95\n"
        "C: 10.0.0.96/28  síť 10.0.0.96  uzly 10.0.0.97–10.0.0.110  "
        "broadcast 10.0.0.111\n"
        "D: 10.0.0.112/28 síť 10.0.0.112 uzly 10.0.0.113–10.0.0.126  "
        "broadcast 10.0.0.127",
    },
    {
        "2.) Síť má adresu 172.16.0.0/16. Rozdělte ji do podsítí s "
        "následujícími požadavky:\n\n"
        "Podsíť A: 500 uzlů\nPodsíť B: 200 uzlů\nPodsíť C: 100 uzlů\n"
        "Podsíť D: 50 uzlů",
        "A: 172.16.0.0/23    síť 172.16.0.0   uzly 172.16.0.1–172.16.1.254  "
        "broadcast 172.16.1.255\n"
        "B: 172.16.2.0/24    síť 172.16.2.0   uzly 172.16.2.1–172.16.2.254  "
        "broadcast 172.16.2.255\n"
        "C: 172.16.3.0/25    síť 172.16.3.0   uzly 172.16.3.1–172.16.3.126  "
        "broadcast 172.16.3.127\n"
        "D: 172.16.3.128/26  síť 172.16.3.128 uzly 172.16.3.129–172.16.3.190 "
        "broadcast 172.16.3.191",
    },
    {
        "3.) Síť má adresu 192.168.10.0/24. Rozdělte ji do podsítí s "
        "následujícími požadavky:\n\n"
        "Podsíť A: 120 uzlů\nPodsíť B: 70 uzlů\nPodsíť C: 35 uzlů\n"
        "Podsíť D: 15 uzlů",
        "Do sítě /24 se tyto požadavky nevejdou: A i B potřebují /25 "
        "(2 × 128 = 256 adres), na C a D už nezbude žádný blok. "
        "Při zachování velikostí by bylo nutné větší síť (např. /23).",
    },
    {
        "4.) Síť má adresu 192.168.2.0/25. Rozdělte ji do podsítí s "
        "následujícími požadavky:\n\n"
        "Podsíť A: 50 uzlů\nPodsíť B: 25 uzlů\nPodsíť C: 12 uzlů\n"
        "Podsíť D: 5 uzlů",
        "A: 192.168.2.0/26    síť 192.168.2.0    uzly 192.168.2.1–192.168.2.62 "
        "broadcast 192.168.2.63\n"
        "B: 192.168.2.64/27   síť 192.168.2.64   "
        "uzly 192.168.2.65–192.168.2.94   broadcast 192.168.2.95\n"
        "C: 192.168.2.96/28   síť 192.168.2.96   "
        "uzly 192.168.2.97–192.168.2.110  broadcast 192.168.2.111\n"
        "D: 192.168.2.112/28  síť 192.168.2.112  "
        "uzly 192.168.2.113–192.168.2.126  broadcast 192.168.2.127",
    },
};

/* Interactive subnet calculator for the practice sheet: the student
 * chooses a prefix and types the network address, broadcast and the
 * usable host range. */

const NetAns net_ans[4][4] = {
    { /* 10.0.0.0/24 */
        {26, "10.0.0.0",   "10.0.0.63",  "10.0.0.1",  "10.0.0.62"},
        {27, "10.0.0.64",  "10.0.0.95",  "10.0.0.65", "10.0.0.94"},
        {28, "10.0.0.96",  "10.0.0.111", "10.0.0.97", "10.0.0.110"},
        {28, "10.0.0.112", "10.0.0.127", "10.0.0.113", "10.0.0.126"},
    },
    { /* 172.16.0.0/16 */
        {23, "172.16.0.0",   "172.16.1.255", "172.16.0.1",   "172.16.1.254"},
        {24, "172.16.2.0",   "172.16.2.255", "172.16.2.1",   "172.16.2.254"},
        {25, "172.16.3.0",   "172.16.3.127", "172.16.3.1",   "172.16.3.126"},
        {26, "172.16.3.128", "172.16.3.191", "172.16.3.129", "172.16.3.190"},
    },
    { /* does not fit into a /24 */
        {-1, NULL, NULL, NULL, NULL},
        {-1, NULL, NULL, NULL, NULL},
        {-1, NULL, NULL, NULL, NULL},
        {-1, NULL, NULL, NULL, NULL},
    },
    { /* 192.168.2.0/25 */
        {26, "192.168.2.0",   "192.168.2.63",  "192.168.2.1",  "192.168.2.62"},
        {27, "192.168.2.64",  "192.168.2.95",  "192.168.2.65", "192.168.2.94"},
        {28, "192.168.2.96",  "192.168.2.111", "192.168.2.97", "192.168.2.110"},
        {28, "192.168.2.112", "192.168.2.127", "192.168.2.113", "192.168.2.126"},
    },
};


GtkWidget *net_qz_combo(void) {
    GtkWidget *c = gtk_combo_box_text_new();

    for (int p = 8; p <= 30; p++) {
        char buf[8];

        g_snprintf(buf, sizeof(buf), "/%d", p);
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(c), NULL, buf);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(c), -1);
    return c;
}

void net_qz_set_prefix(GtkComboBoxText *c, int pfx) {
    if (pfx >= 8 && pfx <= 30)
        gtk_combo_box_set_active(GTK_COMBO_BOX(c), pfx - 8);
    else
        gtk_combo_box_set_active(GTK_COMBO_BOX(c), -1);
}

int net_qz_prefix(GtkComboBoxText *c) {
    gchar *txt = gtk_combo_box_text_get_active_text(c);
    int p = txt ? atoi(txt + (txt[0] == '/' ? 1 : 0)) : -1;

    g_free(txt);
    return p;
}

/* Case- and space-insensitive comparison of typed address answers. */
gboolean net_txt_eq(const char *a, const char *b) {
    const char *x, *y;

    if (!a || !b)
        return a == b;
    x = a;
    y = b;
    while (*x && *y) {
        while (*x == ' ' || *x == '\t')
            x++;
        while (*y == ' ' || *y == '\t')
            y++;
        if (g_ascii_tolower((guchar)*x) != g_ascii_tolower((guchar)*y))
            return FALSE;
        if (*x)
            x++;
        if (*y)
            y++;
    }
    while (*x == ' ' || *x == '\t')
        x++;
    while (*y == ' ' || *y == '\t')
        y++;
    return *x == '\0' && *y == '\0';
}

gboolean net_qz_entry_ok(GtkWidget *entry, const char *want) {
    const gchar *got = gtk_editable_get_text(GTK_EDITABLE(entry));

    return got && net_txt_eq(got, want);
}

static gboolean net_u1_task_ok[4];

void net_qz_check(GtkButton *button, gpointer data) {
    NetQz *q = data;
    int ok = 0;

    (void)button;

    if (q->infeasible) {
        set_feedback(q->feed, FALSE,
                     "Toto zadání se do sítě /24 nevejde – vyzkoušej "
                     "„Ukázat řešení“ a přečti si proč.");
        return;
    }

    for (int i = 0; i < 4; i++) {
        const NetAns *a = &net_ans[q->id][i];
        gboolean p_ok = a->pfx > 0 &&
                        net_qz_prefix(GTK_COMBO_BOX_TEXT(q->combo[i])) == a->pfx;
        gboolean net_ok = net_qz_entry_ok(q->net[i], a->net);
        gboolean bc_ok = net_qz_entry_ok(q->bcast[i], a->bcast);
        gboolean lo_ok = net_qz_entry_ok(q->lo[i], a->lo);
        gboolean hi_ok = net_qz_entry_ok(q->hi[i], a->hi);

        answer_mark(q->combo[i], p_ok);
        answer_mark(q->net[i], net_ok);
        answer_mark(q->bcast[i], bc_ok);
        answer_mark(q->lo[i], lo_ok);
        answer_mark(q->hi[i], hi_ok);
        if (p_ok && net_ok && bc_ok && lo_ok && hi_ok)
            ok++;
    }

    if (ok == 4) {
        set_feedback(q->feed, TRUE, tr("feedback_ok"));
        net_u1_task_ok[q->id] = TRUE;
        if (net_u1_task_ok[0] && net_u1_task_ok[1] && net_u1_task_ok[2] &&
            net_u1_task_ok[3])
            mark_net_done(0);
    } else {
        set_feedback(q->feed, FALSE, tr("feedback_retry"));
    }
}

void net_qz_fill(GtkButton *button, gpointer data) {
    NetQz *q = data;

    (void)button;
    for (int i = 0; i < 4; i++) {
        const NetAns *a = &net_ans[q->id][i];

        if (a->pfx <= 0)
            continue;
        net_qz_set_prefix(GTK_COMBO_BOX_TEXT(q->combo[i]), a->pfx);
        gtk_editable_set_text(GTK_EDITABLE(q->net[i]), a->net);
        gtk_editable_set_text(GTK_EDITABLE(q->bcast[i]), a->bcast);
        gtk_editable_set_text(GTK_EDITABLE(q->lo[i]), a->lo);
        gtk_editable_set_text(GTK_EDITABLE(q->hi[i]), a->hi);
    }
    if (q->note)
        gtk_widget_set_visible(q->note, TRUE);
    if (q->infeasible) {
        net_u1_task_ok[q->id] = TRUE;
        if (net_u1_task_ok[0] && net_u1_task_ok[1] && net_u1_task_ok[2] &&
            net_u1_task_ok[3])
            mark_net_done(0);
    }
}

void net_qz_header(GtkWidget *box, const char *text) {
    GtkWidget *l = gtk_label_new(text);

    gtk_widget_add_css_class(l, "meaning");
    gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(box), l);
}

GtkWidget *net_qz_make_entry(void) {
    GtkWidget *e = gtk_entry_new();

    gtk_widget_set_hexpand(e, TRUE);
    gtk_editable_set_width_chars(GTK_EDITABLE(e), 15);
    return e;
}

GtkWidget *build_net_exercise_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *body;

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("netunit1", "net_ex_title", NULL));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 12);
    gtk_box_append(GTK_BOX(page), scroll);

    body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), body);

    net_heading(body,
                "Zadání pro výpočet IP adres – k procvičení");
    net_paragraph(body,
                  "Určete pro každou podsíť:\n"
                  "• Maska podsítě (prefix)\n"
                  "• Rozsah IP adres podsítě\n"
                  "• Rozsah IP adres pro uzly\n"
                  "• Síťovou adresu\n"
                  "• Broadcast adresu");

    for (guint t = 0; t < G_N_ELEMENTS(net_tasks); t++) {
        GtkWidget *task = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
        GtkWidget *table;
        GtkWidget *btns;
        GtkWidget *reveal;
        GtkWidget *check;
        GtkWidget *sol;
        NetQz *q = g_new0(NetQz, 1);

        q->infeasible = (t == 2);
        q->id = (int)t;
        gtk_widget_set_margin_top(task, 10);
        gtk_box_append(GTK_BOX(body), task);

        net_paragraph(task, net_tasks[t].prompt);

        if (q->infeasible) {
            net_qz_header(task, net_tasks[t].solution);
        } else {
            static const char *cols[] = {"Prefix", "Síťová adresa",
                                         "Broadcast", "Uzly od", "Uzly do"};
            table = gtk_grid_new();
            gtk_grid_set_row_spacing(GTK_GRID(table), 6);
            gtk_grid_set_column_spacing(GTK_GRID(table), 8);
            gtk_widget_set_margin_top(table, 6);
            gtk_box_append(GTK_BOX(task), table);

            for (guint c = 1; c < 6; c++) {
                GtkWidget *h = gtk_label_new(cols[c - 1]);
                gtk_widget_add_css_class(h, "meaning");
                gtk_grid_attach(GTK_GRID(table), h, (int)c, 0, 1, 1);
            }

            for (int i = 0; i < 4; i++) {
                char letter[8];
                GtkWidget *lab = gtk_label_new(NULL);
                int row = i + 1;

                g_snprintf(letter, sizeof(letter), "%c", 'A' + i);
                gtk_label_set_text(GTK_LABEL(lab), letter);
                gtk_widget_add_css_class(lab, "ex-prompt");
                gtk_grid_attach(GTK_GRID(table), lab, 0, row, 1, 1);

                q->combo[i] = net_qz_combo();
                gtk_grid_attach(GTK_GRID(table), q->combo[i], 1, row, 1, 1);
                q->net[i] = net_qz_make_entry();
                gtk_grid_attach(GTK_GRID(table), q->net[i], 2, row, 1, 1);
                q->bcast[i] = net_qz_make_entry();
                gtk_grid_attach(GTK_GRID(table), q->bcast[i], 3, row, 1, 1);
                q->lo[i] = net_qz_make_entry();
                gtk_grid_attach(GTK_GRID(table), q->lo[i], 4, row, 1, 1);
                q->hi[i] = net_qz_make_entry();
                gtk_grid_attach(GTK_GRID(table), q->hi[i], 5, row, 1, 1);
            }
        }

        btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        gtk_box_append(GTK_BOX(task), btns);

        if (!q->infeasible) {
            check = gtk_button_new();
            i18n_bind(check, "check", 1);
            gtk_widget_add_css_class(check, "pill");
            gtk_widget_set_halign(check, GTK_ALIGN_START);
            gtk_box_append(GTK_BOX(btns), check);
            g_signal_connect(check, "clicked", G_CALLBACK(net_qz_check), q);
        }

        reveal = gtk_button_new();
        i18n_bind(reveal, "net_solution", 1);
        gtk_widget_add_css_class(reveal, "pill");
        gtk_widget_set_halign(reveal, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(btns), reveal);
        g_signal_connect(reveal, "clicked", G_CALLBACK(net_qz_fill), q);

        q->feed = gtk_label_new("");
        gtk_widget_set_halign(q->feed, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(task), q->feed);

        sol = gtk_label_new(net_tasks[t].solution);
        gtk_widget_set_halign(sol, GTK_ALIGN_START);
        gtk_label_set_xalign(GTK_LABEL(sol), 0.0);
        gtk_label_set_wrap(GTK_LABEL(sol), TRUE);
        gtk_widget_set_margin_start(sol, 8);
        gtk_widget_add_css_class(sol, "meaning");
        gtk_widget_set_visible(sol, FALSE);
        gtk_box_append(GTK_BOX(task), sol);
        q->note = sol;
    }

    return page;
}

/* ---- Units 2–10: shared multiple-choice quizzes -------------------- */

void net_mcq_check(GtkButton *button, gpointer data) {
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
        mark_net_done(ctx->lesson_id);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry_short"));
    }
}

GtkWidget *build_net_mcq_page(int lesson_id, const char *back_page,
                              const char *title_key, const char *heading_key,
                              const ChoiceQ *qs, const char **hints, int n) {
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

    body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), body);

    head = gtk_label_new(NULL);
    i18n_bind(head, heading_key, 0);
    gtk_widget_set_halign(head, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(head), TRUE);
    gtk_widget_set_margin_top(head, 6);
    gtk_widget_add_css_class(head, "ex-sub");
    gtk_box_append(GTK_BOX(body), head);

    intro = gtk_label_new(NULL);
    i18n_bind(intro, "net_quiz_intro", 0);
    gtk_widget_set_halign(intro, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(intro), TRUE);
    gtk_widget_add_css_class(intro, "ex-prompt");
    gtk_box_append(GTK_BOX(body), intro);

    ctx->qs = qs;
    ctx->n = n;
    ctx->n_opts = n_opts;
    ctx->lesson_id = lesson_id;
    ctx->toggles = g_new0(GtkToggleButton *, n * n_opts);
    ctx->hints = g_new0(GtkWidget *, n);

    for (int i = 0; i < n; i++) {
        GtkWidget *prompt;
        GtkWidget *row;
        GtkToggleButton *first = NULL;
        char *qtext;

        qtext = g_strdup_printf("%d.) %s", i + 1, qs[i].prompt);
        prompt = gtk_label_new(qtext);
        g_free(qtext);
        gtk_widget_set_halign(prompt, GTK_ALIGN_START);
        gtk_label_set_wrap(GTK_LABEL(prompt), TRUE);
        gtk_widget_add_css_class(prompt, "ex-prompt");
        gtk_widget_set_margin_top(prompt, 8);
        gtk_box_append(GTK_BOX(body), prompt);

        row = gtk_flow_box_new();
        gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(row), GTK_SELECTION_NONE);
        gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(row), n_opts);
        gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(row), n_opts);
        gtk_widget_set_halign(row, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(body), row);

        for (int o = 0; o < n_opts; o++) {
            GtkWidget *tb = gtk_toggle_button_new_with_label(qs[i].options[o]);

            gtk_widget_add_css_class(tb, "pill");
            gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(tb), first);
            if (!first)
                first = GTK_TOGGLE_BUTTON(tb);
            gtk_flow_box_append(GTK_FLOW_BOX(row), tb);
            ctx->toggles[i * n_opts + o] = GTK_TOGGLE_BUTTON(tb);
        }

        ctx->hints[i] = meaning_add(body, hints[i]);
    }

    btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_margin_top(btns, 12);
    gtk_box_append(GTK_BOX(body), btns);

    check = gtk_button_new();
    i18n_bind(check, "check", 1);
    gtk_widget_add_css_class(check, "pill");
    gtk_widget_set_halign(check, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(btns), check);
    g_signal_connect(check, "clicked", G_CALLBACK(net_mcq_check), ctx);

    ctx->feedback = gtk_label_new("");
    gtk_widget_set_halign(ctx->feedback, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(body), ctx->feedback);

    return page;
}

const ChoiceQ net2_qs[] = {
    {"Kolik bitů má jeden byte?",
     {"4", "8", "16", "32"}, 4, 1},
    {"Kolik různých hodnot nabývá jeden byte?",
     {"128", "255", "256", "512"}, 4, 2},
    {"Co je trvalé propojení sítě?",
     {"Modem", "Wi‑Fi", "Kabely", "Bluetooth"}, 4, 2},
    {"Co označuje zkratka WAN?",
     {"Lokální síť v budově", "Městskou síť",
      "Rozsáhlou síť (státy, kontinenty)", "Bezdrátovou síť"}, 4, 2},
    {"Co je topologie sítě?",
     {"Norma pro zpracování dat", "Způsob propojení uzlů",
      "Název serveru", "Rychlost přenosu"}, 4, 1},
    {"Architektura sítě je:",
     {"Jen topologie", "Jen standard", "Topologie + standard",
      "Pouze typ kabelu"}, 4, 2},
    {"Peer‑to‑peer síť znamená:",
     {"Síť s jedním výkonným serverem",
      "Síť bez serverů – stanice sdílejí data",
      "Jen internetovou síť", "Síť pouze s tiskárnami"}, 4, 1},
    {"Server v síti je:",
     {"Jednoduchá stanice pro uživatele",
      "Nadřazený uzel poskytující služby",
      "Jen aktivní prvek kabeláže", "Dočasné Wi‑Fi připojení"}, 4, 1},
};

const char *net2_hints[] = {
    "1 B = 8 b",
    "2⁸ = 256 hodnot (0–255)",
    "Kabely – síť zůstává připojená i po přenosu",
    "WAN = Wide Area Network, např. internet",
    "Např. hvězda, kruh, sběrnice",
    "Architektura = topologie + standard",
    "Bez centrálního serveru – stanice si sdílejí data a tiskárny",
    "Server řídí a poskytuje služby, má vyšší výkon",
};

GtkWidget *build_net_unit2_exercise_page(void) {
    return build_net_mcq_page(1, "netunit2", "net_ex2_title", "net_quiz2_head",
                              net2_qs, net2_hints,
                              (int)G_N_ELEMENTS(net2_qs));
}

const ChoiceQ net3_qs[] = {
    {"Co typicky dělají servery v síti?",
     {"Jen tisknou dokumenty", "Ověřují uživatele a povolují přístup",
      "Nahrazují kabely", "Slouží jen jako Wi‑Fi"}, 4, 1},
    {"Fileserver je:",
     {"Tiskový server", "Poštovní server", "Souborový server",
      "Databázový server"}, 4, 2},
    {"Centralizovaný server znamená:",
     {"Více serverů v síti", "Jeden server – výpadek = nedostupnost služby",
      "Server jen pro tisk", "Cloud bez internetu"}, 4, 1},
    {"Distribuovaný model serverů nabízí hlavně:",
     {"Nižší spolehlivost", "Vyšší spolehlivost a rychlost",
      "Jen levnější kabely", "Odstranění uživatelů"}, 4, 1},
    {"Nevyhrazený server:",
     {"Slouží jen síťovým službám", "Slouží i jako pracovní stanice",
      "Je vždy mainframe", "Nemůže ověřovat uživatele"}, 4, 1},
    {"Vyhrazený server je určen:",
     {"Jen jako stanice uživatele", "Jen pro síťové služby",
      "Jen pro Bluetooth", "Jen pro dočasné Wi‑Fi"}, 4, 1},
    {"Mainframe se typicky používá:",
     {"Jen v domácnostech", "V bankách a státní správě",
      "Jen pro streamování hudby", "Jen jako tiskárna"}, 4, 1},
    {"Cloud a SaaS znamenají především:",
     {"Lokální síť bez internetu",
      "Vzdálené servery a software jako služba online",
      "Jen jeden kabel v budově", "Peer‑to‑peer bez serverů"}, 4, 1},
};

const char *net3_hints[] = {
    "Ověřují uživatele a povolují přístup ke službám",
    "Fileserver = souborový server",
    "Jeden server – při výpadku je služba nedostupná",
    "Více serverů, každý obsluhuje část – vyšší spolehlivost",
    "Slouží zároveň i jako pracovní stanice",
    "Jen síťové služby – vysoký výkon a bezpečnost, často v racku",
    "Obří sálové počítače (IBM, Siemens) pro banky a státní správu",
    "SaaS = software jako služba, přístup online z libovolného zařízení",
};

GtkWidget *build_net_unit3_exercise_page(void) {
    return build_net_mcq_page(2, "netunit3", "net_ex3_title", "net_quiz3_head",
                              net3_qs, net3_hints,
                              (int)G_N_ELEMENTS(net3_qs));
}

const ChoiceQ net4_qs[] = {
    {"Paralelní přenos znamená:",
     {"Bit po bitu po jednom vodiči",
      "Více bitů současně po více vodičích",
      "Jen Wi‑Fi bez kabelů", "Jen cloudové ukládání"}, 4, 1},
    {"Hlavní problém paralelního přenosu při delších kabelech je:",
     {"Absence serveru", "Přeslechy (rušení)", "Chybějící SaaS",
      "Peer‑to‑peer režim"}, 4, 1},
    {"Sériový přenos je oproti paralelnímu celkově:",
     {"Vždy pomalejší", "Rychlejší díky vyšším frekvencím a délkám",
      "Jen pro tiskárny", "Bez jakéhokoli kabelu"}, 4, 1},
    {"V asynchronním přenosu start-bit má hodnotu:",
     {"1", "0", "8", "255"}, 4, 1},
    {"Synchronní přenos je řízen:",
     {"Jen stop-bitem", "Společným hodinovým signálem",
      "Jen paritou", "Jen checksumem"}, 4, 1},
    {"Nejslabší metoda zabezpečení z uvedených je:",
     {"CRC", "Checksum", "Parita", "Mainframe"}, 4, 2},
    {"Checksum spočívá v:",
     {"Přidání start-bitu", "Součtu znaků jako dvojkových čísel",
      "Jen Wi‑Fi šifrování", "Výměně kabelů"}, 4, 1},
    {"CRC (cyklické kódy) je z uvedených metod:",
     {"Nejslabší", "Stejně slabá jako parita", "Nejbezpečnější",
      "Jen pro paralelní přenos"}, 4, 2},
};

const char *net4_hints[] = {
    "Více bitů najednou po více vodičích; problém jsou přeslechy",
    "Přeslechy vznikají při delších kabelech a vysokých frekvencích",
    "Bit po bitu po jednom vodiči – vyšší frekvence i délky",
    "Start-bit (0) synchronizuje, Stop-bit (1) ukončuje znak",
    "Řízeno společným hodinovým signálem a přesným časováním",
    "Nejslabší – přidá bit pro sudý/lichý počet jedniček",
    "Součet znaků jako dvojkových čísel",
    "Nejbezpečnější – počítá se z jednotlivých bitů v bloku",
};

GtkWidget *build_net_unit4_exercise_page(void) {
    return build_net_mcq_page(3, "netunit4", "net_ex4_title", "net_quiz4_head",
                              net4_qs, net4_hints,
                              (int)G_N_ELEMENTS(net4_qs));
}

const ChoiceQ net5_qs[] = {
    {"Digitální signál nabývá:",
     {"Libovolných spojitých hodnot", "Diskrétních hodnot 0 a 1",
      "Jen frekvence FM", "Jen šířky pásma"}, 4, 1},
    {"Frekvence f je:",
     {"Stejná jako amplituda", "f = 1/T", "Jen fázový posun",
      "Počet bitů v QPSK"}, 4, 1},
    {"AM, FM a PM jsou:",
     {"Typy kabelů", "Základní typy modulace", "Jen CRC metody",
      "Typy serverů"}, 4, 1},
    {"QPSK přenáší v jednom prvku:",
     {"1 bit", "2 bity", "8 bitů", "256 bitů"}, 4, 1},
    {"256-QAM přenáší v jednom prvku:",
     {"2 bity", "4 bity", "8 bitů", "16 bitů"}, 4, 2},
    {"Modulační rychlost se udává v:",
     {"Jen bit/s", "Baud (Bd/s)", "Jen metrech", "Jen hertzech amplitudy"}, 4, 1},
    {"Přenosová rychlost vyjadřuje:",
     {"Počet změn signálu za sekundu", "Velikost přenesené informace [bit/s]",
      "Jen fázový posun", "Jen periodu T"}, 4, 1},
    {"Šířka pásma určuje:",
     {"Jen barvu kabelu", "Maximální možnou přenosovou rychlost",
      "Jen počet serverů", "Jen start-bit"}, 4, 1},
};

const char *net5_hints[] = {
    "Analogový je spojitý, digitální má diskrétní hodnoty 0 a 1",
    "Frekvence f = 1/T, kde T je perioda",
    "AM mění amplitudu, FM frekvenci, PM fázi",
    "QPSK nese 2 bity v prvku, 256-QAM nese 8 bitů",
    "QPSK nese 2 bity v prvku, 256-QAM nese 8 bitů",
    "Modulační rychlost = počet změn signálu za sekundu [Bd/s]",
    "Přenosová rychlost = množství informace za sekundu [bit/s]",
    "Šířka pásma určuje maximální možnou přenosovou rychlost",
};

GtkWidget *build_net_unit5_exercise_page(void) {
    return build_net_mcq_page(4, "netunit5", "net_ex5_title", "net_quiz5_head",
                              net5_qs, net5_hints,
                              (int)G_N_ELEMENTS(net5_qs));
}

const ChoiceQ net6_qs[] = {
    {"Datový spoj slouží k:",
     {"Jen měření amplitudy", "Výměně informací", "Jen FM modulaci",
      "Jen tisku dokumentů"}, 4, 1},
    {"Přenosová cesta je:",
     {"Jen softwarová služba", "Fyzické médium přenosu",
      "Jen CRC kód", "Jen peer‑to‑peer účet"}, 4, 1},
    {"Mezi přenosová média patří:",
     {"Jen parita", "Kabely, mikrovlny a družice", "Jen Baud",
      "Jen start-bit"}, 4, 1},
    {"Přenosový kanál je:",
     {"Obousměrný vždy", "Jednosměrný souhrn prostředků pro spojení",
      "Jen název serveru", "Jen typ QPSK"}, 4, 1},
    {"Okruh vzniká:",
     {"Z jednoho kanálu", "Ze dvou kanálů pro obousměrný přenos",
      "Jen z parity", "Jen z Wi‑Fi hesla"}, 4, 1},
    {"Trunking znamená:",
     {"Odpojení všech kabelů", "Spojení více cest do jednoho okruhu",
      "Jen asynchronní stop-bit", "Jen digitální 0 a 1"}, 4, 1},
    {"Hlavní cíl trunkingu je:",
     {"Snížit šířku pásma", "Zvýšit šířku pásma", "Zrušit kanály",
      "Nahradit mainframe"}, 4, 1},
};

const char *net6_hints[] = {
    "Datový spoj umožňuje výměnu informací mezi uzly",
    "Fyzické médium: kabely, mikrovlny nebo družice",
    "Fyzické médium: kabely, mikrovlny nebo družice",
    "Kanál je jednosměrný souhrn prostředků pro spojení",
    "Okruh tvoří dva kanály pro obousměrný přenos",
    "Trunking spojuje více cest do jednoho okruhu",
    "Trunking zvyšuje šířku pásma spojením cest",
};

GtkWidget *build_net_unit6_exercise_page(void) {
    return build_net_mcq_page(5, "netunit6", "net_ex6_title", "net_quiz6_head",
                              net6_qs, net6_hints,
                              (int)G_N_ELEMENTS(net6_qs));
}

const ChoiceQ net7_qs[] = {
    {"Multiplex znamená:",
     {"Jen jeden uživatel na kanál",
      "Rozdělení jednoho kanálu na více logických podkanálů",
      "Jen CRC kontrolu", "Jen digitální 0 a 1"}, 4, 1},
    {"FDM přiděluje každému kanálu:",
     {"Časový slot", "Část frekvenčního pásma", "Jen start-bit",
      "Jen mainframe"}, 4, 1},
    {"TDM přiděluje každému kanálu:",
     {"Část frekvenčního pásma", "Vyhrazený časový slot",
      "Jen SaaS účet", "Jen paritu"}, 4, 1},
    {"STDM:",
     {"Garantuje vždy 100% dostupnost",
      "Přiděluje kapacitu dle potřeby a negarantuje 100%",
      "Je jen simplex", "Je jen trunking kabelů"}, 4, 1},
    {"Simplex umožňuje přenos:",
     {"Oběma směry současně", "Jen jedním směrem",
      "Oběma směry, ale ne současně", "Jen přes CRC"}, 4, 1},
    {"Half-duplex znamená:",
     {"Jen jeden směr", "Oběma směry, ne současně",
      "Oběma směry současně", "Jen FDM"}, 4, 1},
    {"Full-duplex znamená:",
     {"Jen jeden směr", "Oběma směry, ne současně",
      "Oběma směry současně", "Jen STDM"}, 4, 2},
    {"Agregace je:",
     {"Sdílení kapacity kanálu více uživateli", "Jen fázová modulace",
      "Jen dvoubodový spoj", "Jen stop-bit"}, 4, 0},
};

const char *net7_hints[] = {
    "Multiplex dělí jeden kanál na více logických podkanálů",
    "FDM: každému kanálu část frekvenčního pásma",
    "TDM: každému kanálu vyhrazený časový slot",
    "STDM přiděluje kapacitu dle potřeby, negarantuje 100 %",
    "Simplex = jen jeden směr přenosu",
    "Half-duplex = oběma směry, ale ne současně",
    "Full-duplex = oběma směry současně",
    "Agregace = sdílení kapacity kanálu více uživateli",
};

GtkWidget *build_net_unit7_exercise_page(void) {
    return build_net_mcq_page(6, "netunit7", "net_ex7_title", "net_quiz7_head",
                              net7_qs, net7_hints,
                              (int)G_N_ELEMENTS(net7_qs));
}

const ChoiceQ net8_qs[] = {
    {"Přepojování okruhů znamená:",
     {"Pakety jdou vždy jinou cestou",
      "Cesta se vytýčí předem a kapacita je garantována",
      "Jen FDM multiplex", "Jen half-duplex"}, 4, 1},
    {"Přepojování okruhů je vhodné pro:",
     {"Jen náhodné datagramy bez cesty", "Přenos v reálném čase (jako telefon)",
      "Jen CRC", "Jen peer‑to‑peer tisk"}, 4, 1},
    {"Při přepojování paketů:",
     {"Vždy existuje jedna pevná cesta",
      "Každý paket může jít jinou cestou",
      "Kapacita je vždy garantována", "Neexistují adresy"}, 4, 1},
    {"Výhoda přepojování paketů je:",
     {"Garantované pořadí vždy", "Reakce na zátěž a využití kapacity",
      "Jen simplex", "Jen trunking"}, 4, 1},
    {"Nevýhoda přepojování paketů je:",
     {"Nereaguje na zátěž", "Negarantuje pořadí ani plynulost",
      "Nelze použít adresy", "Vždy blokuje kanál jako telefon"}, 4, 1},
    {"Pro video v reálném čase je přepojování paketů:",
     {"Ideální vždy", "Spíše nevhodné kvůli plynulosti",
      "Jediná možná metoda", "Stejné jako AM modulace"}, 4, 1},
    {"Virtuální spoje:",
     {"Nemají předem žádnou cestu",
      "Mají cestu předem, prostředky jen při průchodu paketu",
      "Jsou jen FDM", "Jsou jen parita"}, 4, 1},
    {"Virtuální spoje slouží hlavně k:",
     {"Zrušení paketů", "Zrychlení datagramů", "Jen simplexu",
      "Jen analogovému signálu"}, 4, 1},
};

const char *net8_hints[] = {
    "Cesta se vytýčí předem, kapacita je garantována",
    "Cesta se vytýčí předem, kapacita je garantována",
    "Data se dělí na pakety, každý může jít jinou cestou",
    "Výhody: reakce na zátěž a lepší využití kapacity",
    "Negarantuje pořadí ani plynulost – nevhodné pro živé video",
    "Negarantuje pořadí ani plynulost – nevhodné pro živé video",
    "Cesta je předem, prostředky se použijí jen při průchodu paketu",
    "Virtuální spoje zrychlují přenos datagramů",
};

GtkWidget *build_net_unit8_exercise_page(void) {
    return build_net_mcq_page(7, "netunit8", "net_ex8_title", "net_quiz8_head",
                              net8_qs, net8_hints,
                              (int)G_N_ELEMENTS(net8_qs));
}

const ChoiceQ net9_qs[] = {
    {"Standardizace v sítích především:",
     {"Zvyšuje nekompatibilitu", "Zajišťuje kompatibilitu mezi výrobci",
      "Ruší protokoly", "Nahrazuje kabely"}, 4, 1},
    {"Dekompozice znamená:",
     {"Spojení všeho do jedné vrstvy",
      "Rozklad složitého přenosu na vrstvy",
      "Jen FDM", "Jen CRC"}, 4, 1},
    {"Vrstvy komunikují:",
     {"Se všemi vrstvami najednou", "Jen se sousedy přes rozhraní (SAP)",
      "Jen přes satelit", "Jen bez protokolu"}, 4, 1},
    {"Protokol je:",
     {"Fyzický kabel", "Pravidla komunikace mezi stejnými vrstvami",
      "Jen amplituda signálu", "Jen tiskový server"}, 4, 1},
    {"Každá vrstva k datům typicky přidá:",
     {"Jen stop-bit", "Hlavičku (header)", "Jen šířku pásma",
      "Jen mainframe"}, 4, 1},
    {"Jednotka linkové vrstvy se nazývá:",
     {"Segment", "Paket", "Rámec", "Baud"}, 4, 2},
    {"Jednotka síťové vrstvy se nazývá:",
     {"Rámec", "Paket", "Segment", "Simplex"}, 4, 1},
    {"Jednotka transportní vrstvy se nazývá:",
     {"Rámec", "Paket", "Segment", "Trunk"}, 4, 2},
};

const char *net9_hints[] = {
    "Standardizace zajišťuje kompatibilitu mezi výrobci",
    "Dekompozice rozkládá přenos na jednotlivé vrstvy",
    "Vrstvy komunikují jen se sousedy přes rozhraní SAP",
    "Protokol = pravidla mezi stejnými vrstvami na různých uzlech",
    "Každá vrstva přidává k datům hlavičku (header)",
    "Rámec = linková, paket = síťová, segment = transportní",
    "Rámec = linková, paket = síťová, segment = transportní",
    "Rámec = linková, paket = síťová, segment = transportní",
};

GtkWidget *build_net_unit9_exercise_page(void) {
    return build_net_mcq_page(8, "netunit9", "net_ex9_title", "net_quiz9_head",
                              net9_qs, net9_hints,
                              (int)G_N_ELEMENTS(net9_qs));
}

const ChoiceQ net10_qs[] = {
    {"Model ISO/OSI má:",
     {"4 vrstvy", "5 vrstev", "7 vrstev", "10 vrstev"}, 4, 2},
    {"ISO/OSI je především:",
     {"Jen typ kabelu", "Univerzální referenční model",
      "Jen tiskový server", "Jen FDM"}, 4, 1},
    {"Proč je ISO/OSI v praxi pomalejší?",
     {"Nemá žádné vrstvy", "Každá vrstva řešila spolehlivost",
      "Nepoužívá protokoly", "Nemá SAP"}, 4, 1},
    {"TCP/IP v praxi:",
     {"Prohrál proti ISO/OSI", "Zvítězil jako praktická architektura",
      "Je jen analogový signál", "Je jen half-duplex"}, 4, 1},
    {"TCP/IP vycházel z:",
     {"Jen teoretických schémat bez praxe", "Skutečných potřeb sítí",
      "Jen parity", "Jen trunkingu"}, 4, 1},
    {"V TCP/IP spolehlivost řeší:",
     {"Vždy jen fyzická vrstva", "Až vyšší vrstvy",
      "Jen metalické kabely", "Jen mainframe"}, 4, 1},
};

const char *net10_hints[] = {
    "ISO/OSI má 7 vrstev a je univerzální model",
    "ISO/OSI má 7 vrstev a je univerzální model",
    "V praxi je ISO/OSI pomalejší – spolehlivost řeší každá vrstva",
    "TCP/IP zvítězil, protože vycházel ze skutečných potřeb",
    "TCP/IP zvítězil, protože vycházel ze skutečných potřeb",
    "V TCP/IP řeší spolehlivost až vyšší vrstvy",
};

GtkWidget *build_net_unit10_exercise_page(void) {
    return build_net_mcq_page(9, "netunit10", "net_ex10_title", "net_quiz10_head",
                              net10_qs, net10_hints,
                              (int)G_N_ELEMENTS(net10_qs));
}

const ChoiceQ net11_qs[] = {
    {"Fyzická vrstva se stará o:",
     {"Přenos bitů", "Směrování paketů", "Navazování relací",
      "Formátování ASCII"}, 4, 0},
    {"MAC a LLC jsou podvrstvy:",
     {"Síťové vrstvy", "Spojové (linkové) vrstvy", "Transportní vrstvy",
      "Aplikační vrstvy"}, 4, 1},
    {"Směrování paketů v sítích bez přímého spojení řeší:",
     {"Fyzická vrstva", "Relační vrstva", "Síťová vrstva",
      "Prezentační vrstva"}, 4, 2},
    {"Transportní vrstva existuje:",
     {"Jen v koncových uzlech", "I v routerech", "Jen ve switchech",
      "Jen na hubu"}, 4, 0},
    {"Relační vrstva především:",
     {"Definuje napětí a kabely", "Navazuje, udržuje a ukončuje relace",
      "Přidává MAC adresy", "Směruje pakety"}, 4, 1},
    {"Komprese a šifrování patří do vrstvy:",
     {"Fyzické", "Spojové", "Prezentační", "Síťové"}, 4, 2},
    {"Aplikační vrstva poskytuje:",
     {"CRC mezi sousedy", "Rozhraní pro programy (HTTP, FTP, e-mail…)",
      "Jen duplex/simplex", "Jen směrování"}, 4, 1},
};

const char *net11_hints[] = {
    "Fyzická vrstva přenáší bity a řeší médium",
    "Spojová vrstva má podvrstvy MAC a LLC",
    "Síťová vrstva směruje pakety i bez přímého spojení",
    "Transportní vrstva je jen v koncových uzlech, ne v routerech",
    "Relační vrstva řídí relace, dialog a synchronizaci",
    "Prezentační vrstva formátuje data, komprimuje a šifruje",
    "Aplikační vrstva je rozhraní pro programy a služby",
};

GtkWidget *build_net_unit11_exercise_page(void) {
    return build_net_mcq_page(10, "netunit11", "net_ex11_title", "net_quiz11_head",
                              net11_qs, net11_hints,
                              (int)G_N_ELEMENTS(net11_qs));
}

const ChoiceQ net12_qs[] = {
    {"Aplikační vrstva TCP/IP odpovídá v ISO/OSI:",
     {"Jen aplikační vrstvě", "Aplikační + prezentační + relační",
      "Jen fyzické vrstvě", "Linkové + síťové"}, 4, 1},
    {"Vrstva síťového rozhraní TCP/IP odpovídá:",
     {"Jen síťové ISO/OSI", "Linkové + fyzické ISO/OSI",
      "Jen transportní ISO/OSI", "Jen aplikační ISO/OSI"}, 4, 1},
    {"Model TCP/IP vznikl:",
     {"Nejdřív model, pak protokoly", "Nejdřív protokoly, pak model",
      "Současně s Token Ring", "Jen jako náhrada za ATM"}, 4, 1},
    {"TCP je:",
     {"Nespolehlivý a rychlý", "Spolehlivý a spojovaný",
      "Jen směrovací protokol", "Jen fyzické kódování"}, 4, 1},
    {"UDP je:",
     {"Spolehlivý a spojovaný", "Nespolehlivý a rychlý",
      "Jen e-mailový protokol", "Jen ARP"}, 4, 1},
    {"HTTP, DNS a SMTP patří do vrstvy:",
     {"Aplikační", "Transportní", "Síťové", "Síťového rozhraní"}, 4, 0},
    {"IP, ICMP a OSPF patří do vrstvy:",
     {"Aplikační", "Transportní", "Síťové (internetové)",
      "Síťového rozhraní"}, 4, 2},
    {"Ethernet a PPP patří do vrstvy:",
     {"Aplikační", "Transportní", "Síťové", "Síťového rozhraní"}, 4, 3},
};

const char *net12_hints[] = {
    "Aplikační TCP/IP = aplikační + prezentační + relační ISO/OSI",
    "Síťové rozhraní TCP/IP = linková + fyzická ISO/OSI",
    "U TCP/IP vznikly nejdřív protokoly, až pak model",
    "TCP je spolehlivý a spojovaný",
    "UDP je nespolehlivý a rychlý",
    "HTTP, DNS a SMTP jsou aplikační protokoly",
    "IP, ICMP a OSPF patří na síťovou (internetovou) vrstvu",
    "Ethernet a PPP patří na vrstvu síťového rozhraní",
};

GtkWidget *build_net_unit12_exercise_page(void) {
    return build_net_mcq_page(11, "netunit12", "net_ex12_title", "net_quiz12_head",
                              net12_qs, net12_hints,
                              (int)G_N_ELEMENTS(net12_qs));
}

const ChoiceQ net13_qs[] = {
    {"Topologie sběrnice (bus) znamená, že:",
     {"Každý uzel má vlastní centrální switch", "Médium sdílí všichni",
      "Kabely tvoří jen zdvojený kruh", "Používá jen OSPF"}, 4, 1},
    {"Nevýhoda sběrnice je hlavně:",
     {"Že potřebuje hub u každého uzlu",
      "Nízká bezpečnost a náchylnost na poruchu kabelu",
      "Že nelze použít koaxiál", "Že zprávy neobíhají"}, 4, 1},
    {"Topologie hvězda propojuje uzly přes:",
     {"Jen jeden společný koaxiál bez centra",
      "Centrální prvek (hub nebo switch)",
      "Jen Token Ring", "Jen ATM"}, 4, 1},
    {"Výhoda hvězdy je:",
     {"Že přerušení jednoho kabelu k uzlu nevyřadí celou síť",
      "Že všichni slyší vše", "Že nepotřebuje žádnou kabeláž",
      "Že nemá aktivní prvky"}, 4, 0},
    {"Hvězda typicky používá:",
     {"Jen koaxiální kabel", "Kroucenou dvoulinku nebo optiku",
      "Jen simplexní rádio", "Jen FDM"}, 4, 1},
    {"V topologii kruh (ring):",
     {"Zprávy obíhají, dokud nenajdou adresáta",
      "Každý paket jde jen přes DNS",
      "Médium sdílí hub bez kabelů",
      "Neexistuje žádná kabeláž"}, 4, 0},
};

const char *net13_hints[] = {
    "Sběrnice: médium sdílí všichni",
    "Sběrnice: nízká bezpečnost a porucha kabelu vyřadí síť",
    "Hvězda: uzly jdou přes hub nebo switch",
    "Hvězda: porucha kabelu k uzlu nevyřadí celou síť",
    "Hvězda používá kroucenou dvoulinku nebo optiku",
    "V kruhu zprávy obíhají, dokud nenajdou adresáta",
};

GtkWidget *build_net_unit13_exercise_page(void) {
    return build_net_mcq_page(12, "netunit13", "net_ex13_title", "net_quiz13_head",
                              net13_qs, net13_hints,
                              (int)G_N_ELEMENTS(net13_qs));
}

const ChoiceQ net14_qs[] = {
    {"Princip optiky je:",
     {"Převod el. impulsů na světlo a zpět fotodiodou",
      "Jen směrování paketů OSPF", "Jen sdílená sběrnice",
      "Jen UDP spojení"}, 4, 0},
    {"Jedním vláknem je přenos:",
     {"Vždy plně duplexní bez výjimky", "Jednosměrný (obousměrně dvě vlákna)",
      "Jen poloviční kruh", "Jen Token Ring"}, 4, 1},
    {"Výhoda optiky není:",
     {"Imunita vůči el.mag. rušení", "Velký dosah a šířka pásma",
      "Levná a snadná montáž konektorů", "Lepší ochrana proti odposlechu"}, 4, 2},
    {"Mnohovidové vlákno (MM) má:",
     {"Jádro 7–10 µm a vyžaduje laser", "Větší jádro 50/62,5 µm a více vidů",
      "Jen koaxiální stínění", "Jen jednu MAC adresu"}, 4, 1},
    {"Jednovidové vlákno (SM):",
     {"Má větší jádro a LED zdroj", "Má malé jádro, jeden paprsek a laser",
      "Nepřenáší světlo", "Funguje jen jako hub"}, 4, 1},
    {"Levnější zdroje (LED) typicky patří k:",
     {"Jednovidovým vláknům", "Mnohovidovým vláknům",
      "Jen kruhové topologii", "Jen ATM"}, 4, 1},
};

const char *net14_hints[] = {
    "Optika: el. impulsy → světlo (LED/laser) → fotodioda",
    "Jedno vlákno = jednosměrný přenos; obousměrně dvě vlákna",
    "Nevýhody: náročná montáž a cena prvků/konektorů",
    "MM: jádro 50/62,5 µm a více vidů",
    "SM: jádro 7–10 µm, jeden paprsek, laser",
    "MM typicky používá levnější LED zdroje",
};

GtkWidget *build_net_unit14_exercise_page(void) {
    return build_net_mcq_page(13, "netunit14", "net_ex14_title", "net_quiz14_head",
                              net14_qs, net14_hints,
                              (int)G_N_ELEMENTS(net14_qs));
}

const ChoiceQ net15_qs[] = {
    {"Primární ochrana optického vlákna má průměr:",
     {"900 µm", "250 µm", "50 µm", "7–10 µm"}, 4, 1},
    {"Sekundární ochrana má průměr:",
     {"250 µm", "900 µm", "62,5 µm", "1 km"}, 4, 1},
    {"Těsná sekundární ochrana je vhodná hlavně:",
     {"Pro svislé instalace", "Jen pro kruhovou topologii",
      "Jen pro UDP", "Jen pro hub"}, 4, 0},
    {"Volná sekundární ochrana je vhodná hlavně:",
     {"Pro vodorovné instalace", "Jen pro ATM",
      "Jen pro LED bez vlákna", "Jen pro sběrnici"}, 4, 0},
    {"Montáž optiky vyžaduje:",
     {"Jen nůžky na koaxiál", "Precizní lámání a svářečku vláken",
      "Jen hub bez konektorů", "Jen Token Ring"}, 4, 1},
    {"Při lámání se požaduje kolmost zhruba:",
     {"Do 0,5°", "Do 45°", "Přesně 90° bez tolerance", "Bez významu"}, 4, 0},
};

const char *net15_hints[] = {
    "Primární ochrana: 250 µm",
    "Sekundární ochrana: 900 µm",
    "Těsná sekundární ochrana: svislé instalace",
    "Volná sekundární ochrana: vodorovné instalace",
    "Montáž: lámačka a svářečka optických vláken",
    "Lámání vyžaduje kolmost asi 0,5°",
};

GtkWidget *build_net_unit15_exercise_page(void) {
    return build_net_mcq_page(14, "netunit15", "net_ex15_title", "net_quiz15_head",
                              net15_qs, net15_hints,
                              (int)G_N_ELEMENTS(net15_qs));
}

const ChoiceQ net16_qs[] = {
    {"Kolize nastane, když:",
     {"Na sdíleném médiu vysílá více uzlů najednou",
      "Router vypne OSPF", "Vlákno má ochranu 250 µm",
      "Použijeme jen TCP"}, 4, 0},
    {"Metoda Aloha znamená především:",
     {"Vysílej kdykoliv; bez potvrzení pošli znovu",
      "Vždy rezervuj médium RTS/CTS",
      "Nikdy nevysílej při tichu",
      "Jen směrování RIP"}, 4, 0},
    {"CSMA/CD se typicky pojí s:",
     {"Wi-Fi", "Ethernetem", "Jen SMTP", "Jen ATM"}, 4, 1},
    {"Při CSMA/CD uzel před vysíláním:",
     {"Ignoruje médium", "Monitoruje nosnou (Carrier Sense)",
      "Vždy čeká přesně 1 hodinu", "Použije jen laser"}, 4, 1},
    {"Když CSMA/CD detekuje kolizi:",
     {"Pokračuje vysíláním bez změny",
      "Zastaví vysílání a zkusí to po náhodném čase",
      "Přepne síť na sběrnici",
      "Smaže IP adresu"}, 4, 1},
    {"CSMA/CA na Wi-Fi předchází kolizím pomocí:",
     {"RTS a CTS", "Jen CRC bez nosné", "Jen hubu", "Jen koaxiálu"}, 4, 0},
};

const char *net16_hints[] = {
    "Kolize = více uzlů vysílá na sdíleném médiu najednou",
    "Aloha: vysílej kdykoliv, při chybě znovu",
    "CSMA/CD patří k Ethernetu",
    "CSMA/CD nejdřív poslouchá nosnou (Carrier Sense)",
    "Po kolizi CSMA/CD čeká náhodný čas a zkusí znovu",
    "CSMA/CA rezervuje médium přes RTS/CTS",
};

GtkWidget *build_net_unit16_exercise_page(void) {
    return build_net_mcq_page(15, "netunit16", "net_ex16_title", "net_quiz16_head",
                              net16_qs, net16_hints,
                              (int)G_N_ELEMENTS(net16_qs));
}

const ChoiceQ net17_qs[] = {
    {"Repeater (opakovač) hlavně:",
     {"Zesiluje signál a zvyšuje dosah", "Směruje IP pakety",
      "Převádí mezi protokoly vyšších vrstev", "Rezervuje médium RTS/CTS"}, 4, 0},
    {"Transceiver typicky:",
     {"Převádí signál mezi různými médii", "Dělí síť jen podle DNS",
      "Je vždy jen brána", "Pracuje jen s UDP"}, 4, 0},
    {"Hub (rozbočovač) je:",
     {"„Hloupý“ prvek – data z jednoho portu pošle všem ostatním",
      "Směrovač na síťové vrstvě", "Jen laserový zdroj",
      "Jen MAC filtr bez portů"}, 4, 0},
    {"Bridge a switch pracují na vrstvě:",
     {"Fyzické", "Linkové (rámce a MAC)", "Aplikační", "Jen transportní"}, 4, 1},
    {"Switch mimo jiné:",
     {"Dělí síť na kolizní domény", "Nikdy nepoužívá MAC adresy",
      "Funguje jen jako hub bez portů", "Převádí jen ASCII"}, 4, 0},
    {"Router pracuje s:",
     {"Pakety a IP adresami a propojuje sítě", "Jen koaxiálem bez IP",
      "Jen huby bez směrování", "Jen RTS/CTS"}, 4, 0},
    {"Gateway (brána) především:",
     {"Převádí mezi odlišnými architekturami/protokoly",
      "Jen zesiluje bitový signál", "Jen tvoří kolize",
      "Jen lámání vlákna"}, 4, 0},
};

const char *net17_hints[] = {
    "Repeater zesiluje signál na fyzické vrstvě",
    "Transceiver převádí mezi médii (např. metalika ↔ optika)",
    "Hub rozešle přijatá data do všech ostatních portů",
    "Bridge/switch = linková vrstva, rámce a MAC",
    "Switch dělí síť na kolizní domény",
    "Router = síťová vrstva, pakety a IP, propojuje sítě",
    "Gateway převádí mezi odlišnými architekturami/protokoly",
};

GtkWidget *build_net_unit17_exercise_page(void) {
    return build_net_mcq_page(16, "netunit17", "net_ex17_title", "net_quiz17_head",
                              net17_qs, net17_hints,
                              (int)G_N_ELEMENTS(net17_qs));
}

const ChoiceQ net18_qs[] = {
    {"Switch se učí MAC adresy a ukládá je do:",
     {"CAM tabulky", "Jen DNS cache", "Jen ARP bez portů", "Jen OSPF"}, 4, 0},
    {"Díky CAM tabulce switch posílá rámce:",
     {"Jen na konkrétní port", "Vždy na všechny porty jako hub",
      "Jen na routery", "Jen přes laser"}, 4, 0},
    {"Cut-through přepínání:",
     {"Čte jen adresu příjemce a hned posílá",
      "Vždy čeká na celé CRC", "Nikdy neposílá rámce",
      "Pracuje jen s IP směrováním"}, 4, 0},
    {"Nevýhoda cut-through je:",
     {"Že posílá i chybné rámce", "Že je nejpomalejší",
      "Že nemá MAC adresy", "Že vyžaduje gateway"}, 4, 0},
    {"Store-and-forward:",
     {"Přijme celý rámec, zkontroluje CRC a pak pošle",
      "Čte jen první 2 byty", "Nikdy nekontroluje chyby",
      "Funguje jen jako Aloha"}, 4, 0},
    {"Fragment-free (modifikovaný cut-through) čte:",
     {"Prvních 64 bytů (detekce kolizí)", "Jen poslední byte",
      "Celý paket IP včetně směrování", "Jen SMTP hlavičku"}, 4, 0},
};

const char *net18_hints[] = {
    "MAC adresy switch ukládá do CAM tabulky",
    "Podle CAM tabulky posílá rámec jen na cílový port",
    "Cut-through čte adresu příjemce a hned přeposílá",
    "Cut-through je rychlý, ale propustí i chyby",
    "Store-and-forward kontroluje celý rámec včetně CRC",
    "Fragment-free čte prvních 64 bytů kvůli kolizím",
};

GtkWidget *build_net_unit18_exercise_page(void) {
    return build_net_mcq_page(17, "netunit18", "net_ex18_title", "net_quiz18_head",
                              net18_qs, net18_hints,
                              (int)G_N_ELEMENTS(net18_qs));
}

