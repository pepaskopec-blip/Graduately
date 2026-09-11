#include "maturita.h"

/* ---- Computer networks: subject path, lesson & exercise ------------ */


NetLesson net_lessons[NET_LESSONS] = {
    { .n_slides = NET_SLIDES,  .unit_page = "netunit1", .ex_page = "netex"  },
    { .n_slides = NET2_SLIDES, .unit_page = "netunit2", .ex_page = "netex2" },
    { .n_slides = NET3_SLIDES, .unit_page = "netunit3", .ex_page = "netex3" },
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
GArray *net_note_kicks;
GArray *net_note_titles;
GArray *net_note_bodies;
int net_note_last_w = -1;

void net_notes_ensure(void) {
    if (net_note_kicks)
        return;
    net_note_kicks = g_array_new(FALSE, FALSE, sizeof(GtkWidget *));
    net_note_titles = g_array_new(FALSE, FALSE, sizeof(GtkWidget *));
    net_note_bodies = g_array_new(FALSE, FALSE, sizeof(GtkWidget *));
}

void net_note_font(GtkWidget *l, int px) {
    PangoAttrList *attrs = pango_attr_list_new();

    pango_attr_list_insert(attrs, pango_attr_size_new(px * PANGO_SCALE));
    gtk_label_set_attributes(GTK_LABEL(l), attrs);
    pango_attr_list_unref(attrs);
}

void net_rescale_notes(void) {
    int w = net_note_host ? gtk_widget_get_allocated_width(net_note_host) : 0;
    int body, head, kick;

    if (!net_note_kicks)
        return;
    if (w < 160)
        w = 160;
    body = w / 46;
    if (body < 14) body = 14;
    if (body > 26) body = 26;
    head = body + 14;
    if (head > 46) head = 46;
    kick = body - 3;
    if (kick < 11) kick = 11;

    for (guint i = 0; i < net_note_kicks->len; i++) {
        net_note_font(g_array_index(net_note_kicks, GtkWidget *, i), kick);
        net_note_font(g_array_index(net_note_titles, GtkWidget *, i), head);
        net_note_font(g_array_index(net_note_bodies, GtkWidget *, i), body);
    }
}

gboolean net_note_tick(GtkWidget *w, GdkFrameClock *clock,
                              gpointer data) {
    int width = gtk_widget_get_allocated_width(w);

    (void)clock;
    (void)data;
    if (width != net_note_last_w) {
        net_note_last_w = width;
        net_rescale_notes();
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
    g_array_append_val(net_note_kicks, l);
}

void net_note_title(GtkWidget *box, const char *text) {
    GtkWidget *l = net_slide_card("notes-title");

    gtk_label_set_text(GTK_LABEL(l), text);
    gtk_box_append(GTK_BOX(box), l);
    g_array_append_val(net_note_titles, l);
}

void net_note_line(GtkWidget *box, const char *text, gboolean tip) {
    GtkWidget *l = net_slide_card(tip ? "notes-tip" : "notes-body");

    gtk_label_set_text(GTK_LABEL(l), text);
    gtk_widget_set_margin_top(l, 2);
    gtk_box_append(GTK_BOX(box), l);
    g_array_append_val(net_note_bodies, l);
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
        "net_unit1", "net_unit2", "net_unit3"
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

    return page;
}

GtkWidget *build_net_unit_page(NetLesson *L, const char *title_key,
                               const NetSlide *slides, guint n_slides) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *nav;

    net_notes_ensure();
    L->n_slides = n_slides;

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(page, 28);
    gtk_widget_set_margin_end(page, 28);
    gtk_widget_set_margin_top(page, 20);
    gtk_widget_set_margin_bottom(page, 20);

    gtk_box_append(GTK_BOX(page), top_bar("netmap", title_key, NULL));

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

    return build_net_unit_page(&net_lessons[0], "net_unit1", slides, NET_SLIDES);
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

    return build_net_unit_page(&net_lessons[1], "net_unit2", slides, NET2_SLIDES);
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

    return build_net_unit_page(&net_lessons[2], "net_unit3", slides, NET3_SLIDES);
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

    if (ok == 4)
        set_feedback(q->feed, TRUE, tr("feedback_ok"));
    else
        set_feedback(q->feed, FALSE, tr("feedback_retry"));
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

/* ---- Unit 2: multiple-choice quiz on basic networking concepts ---- */

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

    if (ok == ctx->n)
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
    else
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry_short"));
}

GtkWidget *build_net_unit2_exercise_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *body;
    GtkWidget *check;
    GtkWidget *btns;
    NetMcqCtx *ctx = g_new0(NetMcqCtx, 1);
    int n = (int)G_N_ELEMENTS(net2_qs);
    int n_opts = net2_qs[0].n_options;

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("netunit2", "net_ex2_title", NULL));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 12);
    gtk_box_append(GTK_BOX(page), scroll);

    body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), body);

    net_heading(body, "Kvíz k základním pojmům sítí");
    net_paragraph(body,
                  "Vyberte u každé otázky jednu správnou odpověď "
                  "a stiskněte Zkontrolovat.");

    ctx->qs = net2_qs;
    ctx->n = n;
    ctx->n_opts = n_opts;
    ctx->toggles = g_new0(GtkToggleButton *, n * n_opts);
    ctx->hints = g_new0(GtkWidget *, n);

    for (int i = 0; i < n; i++) {
        GtkWidget *prompt;
        GtkWidget *row;
        GtkToggleButton *first = NULL;
        char *qtext;

        qtext = g_strdup_printf("%d.) %s", i + 1, net2_qs[i].prompt);
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
            GtkWidget *tb =
                gtk_toggle_button_new_with_label(net2_qs[i].options[o]);

            gtk_widget_add_css_class(tb, "pill");
            gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(tb), first);
            if (!first)
                first = GTK_TOGGLE_BUTTON(tb);
            gtk_flow_box_append(GTK_FLOW_BOX(row), tb);
            ctx->toggles[i * n_opts + o] = GTK_TOGGLE_BUTTON(tb);
        }

        ctx->hints[i] = meaning_add(body, net2_hints[i]);
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

/* ---- Unit 3: client–server and cloud quiz --------------------------- */

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
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *body;
    GtkWidget *check;
    GtkWidget *btns;
    NetMcqCtx *ctx = g_new0(NetMcqCtx, 1);
    int n = (int)G_N_ELEMENTS(net3_qs);
    int n_opts = net3_qs[0].n_options;

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("netunit3", "net_ex3_title", NULL));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 12);
    gtk_box_append(GTK_BOX(page), scroll);

    body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), body);

    net_heading(body, "Kvíz ke klient–server a cloudu");
    net_paragraph(body,
                  "Vyberte u každé otázky jednu správnou odpověď "
                  "a stiskněte Zkontrolovat.");

    ctx->qs = net3_qs;
    ctx->n = n;
    ctx->n_opts = n_opts;
    ctx->toggles = g_new0(GtkToggleButton *, n * n_opts);
    ctx->hints = g_new0(GtkWidget *, n);

    for (int i = 0; i < n; i++) {
        GtkWidget *prompt;
        GtkWidget *row;
        GtkToggleButton *first = NULL;
        char *qtext;

        qtext = g_strdup_printf("%d.) %s", i + 1, net3_qs[i].prompt);
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
            GtkWidget *tb =
                gtk_toggle_button_new_with_label(net3_qs[i].options[o]);

            gtk_widget_add_css_class(tb, "pill");
            gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(tb), first);
            if (!first)
                first = GTK_TOGGLE_BUTTON(tb);
            gtk_flow_box_append(GTK_FLOW_BOX(row), tb);
            ctx->toggles[i * n_opts + o] = GTK_TOGGLE_BUTTON(tb);
        }

        ctx->hints[i] = meaning_add(body, net3_hints[i]);
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
