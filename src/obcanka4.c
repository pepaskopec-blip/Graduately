#include "graduately.h"

/* Year 4 of Občanská nauka.
 * The remaining units of the MŠMT model syllabus
 * (č.j. 18 396/2002-23) and the SOV civic framework: the economy,
 * Czechia in Europe and the world, and practical philosophy.
 * Schools place them in the fourth year. */

#define ON4_ENTRY(n) \
    { .n_slides = 2, .unit_page = "on4unit" #n, .ex_page = "on4ex" #n }

NetLesson on4_lessons[ON4_LESSONS] = {
    ON4_ENTRY(1),
    ON4_ENTRY(2),
    ON4_ENTRY(3),
    ON4_ENTRY(4),
    ON4_ENTRY(5),
    ON4_ENTRY(6),
    ON4_ENTRY(7),
    ON4_ENTRY(8),
    ON4_ENTRY(9),
    ON4_ENTRY(10),
    ON4_ENTRY(11),
    ON4_ENTRY(12),
    ON4_ENTRY(13),
    ON4_ENTRY(14),
    ON4_ENTRY(15),
    ON4_ENTRY(16),
};

#undef ON4_ENTRY

GtkWidget *on4_scroll;
GtkWidget *on4_fixed;
GtkWidget *on4_rail;
GtkWidget *on4_nodes[ON4_UNITS];
GtkWidget *on4_labels[ON4_UNITS];
double on4_cx[ON4_UNITS];
double on4_cy[ON4_UNITS];
int on4_cols = 1;
int on4_rows = 1;
int on4_cw = (int)(2.0 * ROAD_MX + (ON4_UNITS - 1) * PATH_SPAC);
int on4_ch = (int)(2.0 * ROAD_MY);
guint on4_idle;
double on4_last_avail = -1.0;
int on4_last_cols = -1;
int on4_last_rows = -1;
int on4_last_cw = -1;
int on4_last_ch = -1;
cairo_surface_t *on4_rail_cache;
int on4_cache_w;
int on4_cache_h;

static void on4_point(double t, double *ox, double *oy) {
    int k = (int)t;
    double u = t - (double)k;
    double u2, u3;
    double p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y;

    if (k < 0) { k = 0; u = 0.0; }
    if (k >= ON4_UNITS - 1) { k = ON4_UNITS - 2; u = 1.0; }

    p1x = on4_cx[k];     p1y = on4_cy[k];
    p2x = on4_cx[k + 1]; p2y = on4_cy[k + 1];
    if (k - 1 >= 0) { p0x = on4_cx[k - 1]; p0y = on4_cy[k - 1]; }
    else            { p0x = p1x - (p2x - p1x); p0y = p1y - (p2y - p1y); }
    if (k + 2 < ON4_UNITS) { p3x = on4_cx[k + 2]; p3y = on4_cy[k + 2]; }
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

static void on4_path(cairo_t *cr, double t0, double t1) {
    const double steps_per_unit = 24.0;
    int n = (int)((t1 - t0) * steps_per_unit);
    double x, y;
    int s;

    if (n < 1)
        n = 1;
    on4_point(t0, &x, &y);
    cairo_move_to(cr, x, y);
    for (s = 1; s <= n; s++) {
        on4_point(t0 + (t1 - t0) * (double)s / (double)n, &x, &y);
        cairo_line_to(cr, x, y);
    }
}

static void on4_geometry(double avail) {
    const double phase = 2.0 * G_PI * 1.7 / (double)(ON4_UNITS - 1);
    int rows;
    int r, c, i;

    for (rows = 1; rows <= ON4_UNITS; rows++) {
        int cols = (ON4_UNITS + rows - 1) / rows;
        if (2.0 * ROAD_MX + (cols - 1) * PATH_SPAC <= avail + 1.0)
            break;
    }
    if (rows > ON4_UNITS)
        rows = ON4_UNITS;
    on4_rows = rows;
    on4_cols = (ON4_UNITS + rows - 1) / rows;
    if (on4_cols < 1)
        on4_cols = 1;
    on4_cw = (int)(2.0 * ROAD_MX + (on4_cols - 1) * PATH_SPAC);
    on4_ch = (int)(2.0 * ROAD_MY + (rows - 1) * ROAD_GAP);

    for (r = 0; r < rows; r++) {
        int base = r * on4_cols;
        int len = MIN(on4_cols, ON4_UNITS - base);
        int fwd = (r % 2) == 0;
        for (c = 0; c < len; c++) {
            i = base + c;
            int cc = fwd ? c : (on4_cols - 1 - c);
            on4_cx[i] = ROAD_MX + cc * PATH_SPAC;
            on4_cy[i] = ROAD_MY + r * ROAD_GAP
                       + ROAD_WAVE * sin((double)i * phase);
        }
    }
}

static void on4_apply_layout(void) {
    if (!on4_fixed)
        return;

    if (on4_cols == on4_last_cols && on4_rows == on4_last_rows &&
        on4_cw == on4_last_cw && on4_ch == on4_last_ch)
        return;

    on4_last_cols = on4_cols;
    on4_last_rows = on4_rows;
    on4_last_cw = on4_cw;
    on4_last_ch = on4_ch;
    if (on4_rail_cache) {
        cairo_surface_destroy(on4_rail_cache);
        on4_rail_cache = NULL;
    }

    for (int i = 0; i < ON4_UNITS; i++) {
        if (!on4_nodes[i] || !on4_labels[i])
            continue;
        gtk_fixed_move(GTK_FIXED(on4_fixed), on4_nodes[i],
                       (int)(on4_cx[i] - NODE_SIZE / 2.0),
                       (int)(on4_cy[i] - NODE_SIZE / 2.0));
        gtk_fixed_move(GTK_FIXED(on4_fixed), on4_labels[i],
                       (int)(on4_cx[i] - (PATH_SPAC - 20.0) / 2.0),
                       (int)(on4_cy[i] + NODE_SIZE / 2.0 + 10.0));
    }

    gtk_widget_set_size_request(on4_fixed, on4_cw, on4_ch);
    gtk_widget_set_size_request(on4_rail, on4_cw, on4_ch);
    gtk_fixed_move(GTK_FIXED(on4_fixed), on4_rail, 0, 0);
    gtk_widget_queue_draw(on4_rail);
}

static void on4_relayout(void) {
    GtkAdjustment *hadj;
    double avail;

    if (!on4_scroll)
        return;
    hadj = gtk_scrolled_window_get_hadjustment(
        GTK_SCROLLED_WINDOW(on4_scroll));
    avail = gtk_adjustment_get_page_size(hadj);
    if (avail < 1.0)
        return;
    if (fabs(avail - on4_last_avail) < 1.0)
        return;
    on4_last_avail = avail;
    on4_geometry(avail);
    on4_apply_layout();
}

static gboolean on4_relayout_idle(gpointer data) {
    (void)data;
    on4_idle = 0;
    on4_relayout();
    return G_SOURCE_REMOVE;
}

static void on4_relayout_later(void) {
    if (on4_idle == 0)
        on4_idle = g_idle_add(on4_relayout_idle, NULL);
}

static void on4_adjust_notify(GtkAdjustment *adj, GParamSpec *ps,
                             gpointer data) {
    (void)adj;
    (void)ps;
    (void)data;
    on4_relayout_later();
}

static void on4_render_rail_to(cairo_t *cr) {
    const double t_end = (double)(ON4_UNITS - 1);
    const Rgb rail = color_from_hex(app_theme.rail);
    const Rgb muted = color_from_hex(app_theme.subtext);

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

    cairo_new_path(cr);
    on4_path(cr, 0.0, t_end);
    cairo_set_line_width(cr, 12);
    cairo_set_source_rgb(cr, rail.r, rail.g, rail.b);
    cairo_stroke(cr);

    cairo_set_source_rgba(cr, muted.r, muted.g, muted.b, 0.17);
    cairo_set_dash(cr, (double[]){2.0, 40.0}, 2, 0.0);
    cairo_new_path(cr);
    on4_path(cr, 0.0, t_end);
    cairo_stroke(cr);
    cairo_set_dash(cr, NULL, 0, 0.0);
}

static void on4_draw_rail(GtkDrawingArea *area, cairo_t *cr,
                         int width, int height, gpointer data) {
    (void)area;
    (void)width;
    (void)height;
    (void)data;

    if (!on4_rail_cache || on4_cache_w != on4_cw || on4_cache_h != on4_ch) {
        cairo_t *rcr;

        if (on4_rail_cache)
            cairo_surface_destroy(on4_rail_cache);
        on4_cache_w = on4_cw;
        on4_cache_h = on4_ch;
        if (on4_cache_w < 1)
            on4_cache_w = 1;
        if (on4_cache_h < 1)
            on4_cache_h = 1;
        on4_rail_cache = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                    on4_cache_w, on4_cache_h);
        rcr = cairo_create(on4_rail_cache);
        on4_render_rail_to(rcr);
        cairo_destroy(rcr);
    }

    cairo_set_source_surface(cr, on4_rail_cache, 0, 0);
    cairo_paint(cr);
}

void on4_rail_theme_reset(void) {
    if (on4_rail_cache) {
        cairo_surface_destroy(on4_rail_cache);
        on4_rail_cache = NULL;
    }
    if (on4_rail)
        gtk_widget_queue_draw(on4_rail);
}

static void on4_open_unit(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L)
        return;
    L->idx = 0;
    if (L->stack) {
        char name[16];

        g_snprintf(name, sizeof(name), "on4_slide%u", L->idx);
        gtk_stack_set_visible_child_name(GTK_STACK(L->stack), name);
        if (L->prev_btn)
            gtk_widget_set_sensitive(L->prev_btn, FALSE);
        if (L->next_btn)
            gtk_button_set_label(GTK_BUTTON(L->next_btn), tr("net_slide_next"));
    }
    gtk_stack_set_visible_child_name(main_stack, L->unit_page);
}

static void on4_add_node(GtkFixed *fixed, int index) {
    int num = index + 1;
    gboolean locked = index >= ON4_LESSONS;
    GtkWidget *card;
    GtkWidget *vbox;
    GtkWidget *number;
    GtkWidget *icon;
    GtkWidget *name;
    char *text;
    static char unit_keys[ON4_LESSONS][16];
    static gboolean unit_keys_ready = FALSE;

    if (!unit_keys_ready) {
        for (int i = 0; i < ON4_LESSONS; i++)
            g_snprintf(unit_keys[i], sizeof unit_keys[i], "on4_unit%d", i + 1);
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
        on4_lessons[index].done_icon = icon;
        g_signal_connect(card, "clicked", G_CALLBACK(on4_open_unit),
                         &on4_lessons[index]);
    }

    on4_nodes[index] = card;
    gtk_fixed_put(fixed, card, 0, 0);

    name = gtk_label_new(NULL);
    gtk_widget_set_size_request(name, (int)(PATH_SPAC - 20.0), -1);
    gtk_widget_set_halign(name, GTK_ALIGN_CENTER);
    gtk_label_set_justify(GTK_LABEL(name), GTK_JUSTIFY_CENTER);
    gtk_label_set_wrap(GTK_LABEL(name), TRUE);
    gtk_widget_add_css_class(name, "unit-name");
    if (index < ON4_LESSONS) {
        i18n_bind(name, unit_keys[index], 0);
    } else {
        gtk_widget_add_css_class(name, "unit-name-locked");
    }
    on4_labels[index] = name;
    gtk_fixed_put(fixed, name, 0, 0);
}
GtkWidget *build_on4map_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *fixed;
    GtkWidget *rail;
    GtkAdjustment *ha;
    GtkAdjustment *va;

    on4_geometry(1000.0);

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("onyears", "on_year4", "on4_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);
    on4_scroll = scroll;

    wrap = gtk_center_box_new();
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), wrap);

    fixed = gtk_fixed_new();
    gtk_widget_set_halign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(fixed, on4_cw, on4_ch);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), fixed);
    on4_fixed = fixed;

    rail = gtk_drawing_area_new();
    gtk_widget_set_size_request(rail, on4_cw, on4_ch);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(rail), on4_draw_rail,
                                   NULL, NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rail, 0, 0);
    on4_rail = rail;

    for (int i = 0; i < ON4_UNITS; i++)
        on4_add_node(GTK_FIXED(fixed), i);

    ha = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scroll));
    va = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));
    g_signal_connect(ha, "notify::page-size",
                     G_CALLBACK(on4_adjust_notify), NULL);
    g_signal_connect(va, "notify::page-size",
                     G_CALLBACK(on4_adjust_notify), NULL);

    on4_apply_layout();
    on4_relayout_later();
    refresh_on4_completion_ui();

    return page;
}

static void on4_slide_apply(NetLesson *L);
static void on4_slide_prev(GtkButton *button, gpointer data);
static void on4_slide_next(GtkButton *button, gpointer data);

static int on4_note_last_w = -1;

static void on4_rescale_notes_at(int w) {
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
    for (int i = 0; i < ON4_LESSONS; i++)
        net_rescale_lesson_notes(&on4_lessons[i], body, head, kick);
}

static gboolean on4_note_tick(GtkWidget *w, GdkFrameClock *clock,
                             gpointer data) {
    int width = gtk_widget_get_allocated_width(w);

    (void)clock;
    (void)data;
    if (width != on4_note_last_w) {
        on4_note_last_w = width;
        on4_rescale_notes_at(width);
    }
    return G_SOURCE_CONTINUE;
}

static GtkWidget *build_on4_unit_page(NetLesson *L, const char *title_key,
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

    gtk_box_append(GTK_BOX(page), top_bar("on4map", title_key, sub_key));

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
        g_snprintf(name, sizeof(name), "on4_slide%u", s);
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

    gtk_widget_add_tick_callback(L->stack, on4_note_tick, NULL, NULL);
    on4_note_last_w = -1;
    on4_rescale_notes_at(0);
    net_notes_target = NULL;

    nav = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(nav), TRUE);
    gtk_widget_set_margin_top(nav, 10);
    gtk_box_append(GTK_BOX(page), nav);

    L->prev_btn = gtk_button_new_with_label(tr("net_slide_prev"));
    gtk_widget_add_css_class(L->prev_btn, "btn-primary");
    g_signal_connect(L->prev_btn, "clicked", G_CALLBACK(on4_slide_prev), L);
    gtk_box_append(GTK_BOX(nav), L->prev_btn);

    L->next_btn = gtk_button_new_with_label(tr("net_slide_next"));
    gtk_widget_add_css_class(L->next_btn, "btn-primary");
    g_signal_connect(L->next_btn, "clicked", G_CALLBACK(on4_slide_next), L);
    gtk_box_append(GTK_BOX(nav), L->next_btn);

    L->idx = 0;
    on4_slide_apply(L);
    return page;
}

static void on4_slide_apply(NetLesson *L) {
    char name[16];

    if (!L || !L->stack)
        return;
    g_snprintf(name, sizeof(name), "on4_slide%u", L->idx);
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

static void on4_slide_prev(GtkButton *button, gpointer data) {
    NetLesson *L = data;
    (void)button;
    if (!L) return;
    if (L->idx > 0) {
        L->idx--;
        on4_slide_apply(L);
    }
}

static void on4_slide_next(GtkButton *button, gpointer data) {
    NetLesson *L = data;
    (void)button;
    if (!L) return;
    if (L->idx + 1 < L->n_slides) {
        L->idx++;
        on4_slide_apply(L);
    } else {
        gtk_stack_set_visible_child_name(main_stack, L->ex_page);
    }
}

void on4_lessons_apply_lang(void) {
    for (int i = 0; i < ON4_LESSONS; i++)
        on4_slide_apply(&on4_lessons[i]);
}

static void on4_mcq_check(GtkButton *button, gpointer data) {
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
        mark_on4_done(ctx->lesson_id);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry_short"));
    }
}

static GtkWidget *build_on4_mcq_page(int lesson_id, const char *back_page,
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
    g_signal_connect(check, "clicked", G_CALLBACK(on4_mcq_check), ctx);

    ctx->feedback = gtk_label_new("");
    gtk_widget_set_halign(ctx->feedback, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(body), ctx->feedback);

    return page;
}
static GtkWidget *build_on4_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Cena", "Nabídka a poptávka",
            "Tip: cena není rozkaz, je to signál.",
            {
                "Trh je místo, kde se potkává nabídka s poptávkou.",
                "Poptávka je ochota kupujících vzít zboží za určitou cenu.",
                "Nabídka je ochota prodávajících zboží prodat.",
                "Když cena roste, poptávka obvykle klesá a nabídka obvykle roste.",
                NULL,
            },
        },
        {
            "2 / 2   •   Soutěž", "Konkurence a monopol",
            "Tip: čtvrtý ročník bere hospodářství, svět a filozofii.",
            {
                "Rovnovážná cena je tam, kde se nabídka s poptávkou potkají.",
                "Konkurence nutí prodávající hlídat cenu i kvalitu.",
                "Monopol má na trhu jediný prodávající a cenu si snáze diktuje.",
                "Česko má tržní hospodářství. Stát stanoví pravidla, nenahrazuje každý obchod.",
                NULL,
            },
        },
    };

    return build_on4_unit_page(&on4_lessons[0], "on4_unit1", "on4_unit1_sub",
                               slides, G_N_ELEMENTS(slides));
}
static GtkWidget *build_on4_unit1_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kde se potkává nabídka s poptávkou?",
         {
          "Jen v parlamentu",
          "Na trhu",
          "Jen u notáře",
          "Jen v jedné rodině",
         },
         4, 1},
        {"Co se s poptávkou obvykle stane, když cena roste?",
         {
          "Vždycky roste",
          "Obvykle klesá",
          "Zruší se zákon",
          "Zůstane vždy stejná",
         },
         4, 1},
        {"Kdo je monopol?",
         {
          "Jediný prodávající s velkou mocí nad cenou",
          "Každý malý obchod",
          "Stát, který vybírá daně",
          "Kupující, který srovnává ceny",
         },
         4, 0},
        {"K čemu konkurence prodávající tlačí?",
         {
          "K libovolně vysoké ceně",
          "Aby trh zavřeli",
          "Aby hlídali cenu i kvalitu",
          "Aby přestali prodávat",
         },
         4, 2},
    };
    static const char *hints[] = {
        "Trh není jen kamenný krám",
        "Vyšší cena část kupujících odradí",
        "Konkurence monopolu chybí",
        "Kupující si může vybrat jinde",
    };
    return build_on4_mcq_page(0, "on4unit1", "on4_ex1_title",
                             "on4_quiz1_head", qs, hints, 4);
}

static GtkWidget *build_on4_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Měřítko", "Co HDP počítá",
            "Tip: HDP není vysvědčení štěstí.",
            {
                "Hrubý domácí produkt je tržní hodnota konečných statků a služeb.",
                "Počítá se, co se na území státu vytvoří za určité období.",
                "Meziprodukt, který už je v ceně hotového výrobku, se znovu nepřičítá.",
                "HDP neříká, jak jsou lidé šťastní, a nepočítá neplacenou péči.",
                NULL,
            },
        },
        {
            "2 / 2   •   Vlny", "Růst a recese",
            "Tip: pokles výroby se lidí dotkne dřív než tabulka.",
            {
                "Růst znamená, že se za období vytvoří víc.",
                "Recese je období, kdy hospodářství klesá.",
                "V poklesu obvykle roste nezaměstnanost a firmy odkládají investice.",
                "HDP na osobu pomáhá státy srovnat, ale nezměří spravedlnost.",
                NULL,
            },
        },
    };

    return build_on4_unit_page(&on4_lessons[1], "on4_unit2", "on4_unit2_sub",
                               slides, G_N_ELEMENTS(slides));
}
static GtkWidget *build_on4_unit2_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je hrubý domácí produkt?",
         {
          "Tržní hodnota konečných statků a služeb vytvořených na území státu",
          "Součet poslaneckých platů",
          "Počet nezaměstnaných",
          "Státní dluh",
         },
         4, 0},
        {"Počítá se meziprodukt do HDP znovu?",
         {
          "Ano, čím víckrát, tím líp",
          "Ne, aby se výroba nepočítala dvakrát",
          "Jen o víkendu",
          "Jen když jde o dovoz",
         },
         4, 1},
        {"Co je recese?",
         {
          "Nejrychlejší růst hospodářství",
          "Den, kdy se platí daně",
          "Období poklesu hospodářství",
          "Růst všech mezd ze zákona",
         },
         4, 2},
        {"Měří HDP štěstí lidí?",
         {
          "Ano, je to měřítko štěstí",
          "Ano, počítá i neplacenou péči",
          "Ne, měří výrobu, ne štěstí",
          "Ano, vyšší HDP znamená spravedlivější stát",
         },
         4, 2},
    };
    static const char *hints[] = {
        "Jde o výrobu za určité období",
        "V ceně hotového výrobku už je",
        "Opak růstu",
        "Neplacená péče v něm není",
    };
    return build_on4_mcq_page(1, "on4unit2", "on4_ex2_title",
                             "on4_quiz2_head", qs, hints, 4);
}

static GtkWidget *build_on4_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Směna", "K čemu jsou peníze",
            "Tip: eurozóna není totéž co Evropská unie.",
            {
                "Peníze jsou prostředek směny a společné měřítko cen.",
                "Dá se v nich také uložit hodnota na později.",
                "Měnou České republiky je koruna.",
                "Česko je v Evropské unii, ale eurem se zde neplatí.",
                NULL,
            },
        },
        {
            "2 / 2   •   Ceny", "Inflace",
            "Tip: zdražení jedné věci ještě není inflace.",
            {
                "Inflace je všeobecný růst cenové hladiny.",
                "Měří ji Český statistický úřad jako růst spotřebitelských cen.",
                "Česká národní banka usiluje o inflaci poblíž 2 procent.",
                "Vysoká inflace znehodnocuje úspory a pevný příjem koupí míň.",
                NULL,
            },
        },
    };

    return build_on4_unit_page(&on4_lessons[2], "on4_unit3", "on4_unit3_sub",
                               slides, G_N_ELEMENTS(slides));
}
static GtkWidget *build_on4_unit3_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"K čemu peníze slouží?",
         {
          "Jsou prostředek směny a společné měřítko cen",
          "Nahrazují zákon",
          "Jsou totéž co státní dluh",
          "Platí jen v jednom obchodě",
         },
         4, 0},
        {"Jaká je měna České republiky?",
         {
          "Euro",
          "Koruna",
          "Dolar",
          "Měna celé eurozóny",
         },
         4, 1},
        {"Co je inflace?",
         {
          "Zdražení jedné značky",
          "Pokles všech mezd ze zákona",
          "Všeobecný růst cenové hladiny",
          "Schodek státního rozpočtu",
         },
         4, 2},
        {"O jakou inflaci usiluje Česká národní banka?",
         {
          "Poblíž nuly za každou cenu",
          "Poblíž 10 procent",
          "Poblíž 20 procent",
          "Poblíž 2 procent",
         },
         4, 3},
    };
    static const char *hints[] = {
        "Bez nich se směna srovnává hůř",
        "Euro se v Česku nepoužívá",
        "Nejde o jednu věc v regálu",
        "Cíl je nízká a stabilní inflace",
    };
    return build_on4_mcq_page(2, "on4unit3", "on4_ex3_title",
                             "on4_quiz3_head", qs, hints, 4);
}

static GtkWidget *build_on4_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Stát", "Česká národní banka",
            "Tip: ČNB není obchodní banka s pobočkami na náměstí.",
            {
                "Česká národní banka je ústřední banka a vydává korunu.",
                "Je nezávislá. Vláda jí nesmí diktovat úrokové sazby.",
                "Hlavním nástrojem měnové politiky jsou právě úrokové sazby.",
                "Dohlíží na banky a na další části finančního trhu.",
                NULL,
            },
        },
        {
            "2 / 2   •   Pobočka", "Obchodní banka a vklady",
            "Tip: limit ochrany vkladů se počítá na osobu a jednu banku.",
            {
                "Obchodní banka přijímá vklady a poskytuje úvěry.",
                "Peníze na účtu nejsou totéž co hotovost v trezoru banky.",
                "Vklady chrání garanční systém.",
                "Ochrana je zpravidla do ekvivalentu 100 000 eur na osobu a banku.",
                NULL,
            },
        },
    };

    return build_on4_unit_page(&on4_lessons[3], "on4_unit4", "on4_unit4_sub",
                               slides, G_N_ELEMENTS(slides));
}
static GtkWidget *build_on4_unit4_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je Česká národní banka?",
         {
          "Obchodní banka s nejvíce pobočkami",
          "Ústřední banka, která vydává korunu",
          "Úřad práce",
          "Ministerstvo financí",
         },
         4, 1},
        {"Čím ČNB hlavně řídí měnovou politiku?",
         {
          "Školním řádem",
          "Počtem poslanců",
          "Úrokovými sazbami",
          "Sazbou DPH",
         },
         4, 2},
        {"Co dělá obchodní banka?",
         {
          "Schvaluje zákony",
          "Přijímá vklady a poskytuje úvěry",
          "Ukládá tresty",
          "Stanovuje inflační cíl",
         },
         4, 1},
        {"Jak jsou vklady v bance chráněné?",
         {
          "Nejsou chráněné vůbec",
          "Zpravidla do ekvivalentu 100 000 eur na osobu a banku",
          "Jen do 1 000 korun",
          "Bez limitu u všech bank dohromady",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Neslouží jako běžný účet pro veřejnost",
        "Sazby ovlivňují cenu peněz",
        "Inflační cíl je věc ČNB",
        "Limit se nepočítá na celou zemi najednou",
    };
    return build_on4_mcq_page(3, "on4unit4", "on4_ex4_title",
                             "on4_quiz4_head", qs, hints, 4);
}

static GtkWidget *build_on4_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Výběr", "Přímé a nepřímé daně",
            "Tip: DPH je v ceně nákupu, i když ji odvádí prodejce.",
            {
                "Přímá daň se platí z příjmu nebo z majetku.",
                "Nepřímou daň člověk platí v ceně. Taková je daň z přidané hodnoty.",
                "Základní sazba DPH je 21 procent, snížená 12 procent.",
                "Vybrané knihy jsou od DPH osvobozené. Není to třetí sazba.",
                NULL,
            },
        },
        {
            "2 / 2   •   Rok", "Rozpočet, schodek a dluh",
            "Tip: schodek je jeden rok, dluh se táhne dál.",
            {
                "Daně financují školy, zdravotnictví, obranu a další veřejné výdaje.",
                "Státní rozpočet srovnává příjmy a výdaje státu na jeden rok.",
                "Schodek vznikne, když stát v tom roce utratí víc, než vybere.",
                "Dluh je to, co z minulých schodků ještě není splacené. Rozpočet schvaluje Poslanecká sněmovna.",
                NULL,
            },
        },
    };

    return build_on4_unit_page(&on4_lessons[4], "on4_unit5", "on4_unit5_sub",
                               slides, G_N_ELEMENTS(slides));
}
static GtkWidget *build_on4_unit5_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Čím se DPH liší od daně z příjmu?",
         {
          "DPH je v ceně nákupu, daň z příjmu se platí z příjmu",
          "Jsou totéž",
          "DPH lidé v ceně nikdy neplatí",
          "Daň z příjmu je přirážka na rohlíku",
         },
         4, 0},
        {"Jaká je základní sazba DPH?",
         {
          "12 procent",
          "21 procent",
          "5 procent",
          "10 procent",
         },
         4, 1},
        {"Co je schodek státního rozpočtu?",
         {
          "Přebytek na účtu domácnosti",
          "Počet bank",
          "Výdaje státu v daném roce převýší příjmy",
          "Inflační cíl ČNB",
         },
         4, 2},
        {"Kdo schvaluje státní rozpočet?",
         {
          "Česká národní banka",
          "Obecní úřad",
          "Každý živnostník",
          "Poslanecká sněmovna",
         },
         4, 3},
    };
    static const char *hints[] = {
        "Jedna je nepřímá, druhá přímá",
        "Snížená sazba je 12 procent",
        "Jde o jeden rozpočtový rok",
        "Rozpočet je zákon",
    };
    return build_on4_mcq_page(4, "on4unit5", "on4_ex5_title",
                             "on4_quiz5_head", qs, hints, 4);
}

static GtkWidget *build_on4_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Účet", "Kdo je podnikatel",
            "Tip: ne každá výdělečná činnost je živnost.",
            {
                "Podnikatel jedná soustavně, na vlastní účet a odpovědnost a za účelem zisku.",
                "Častou cestou drobného podnikání je živnost.",
                "Ohlašovací živnost se ohlásí. Koncesovaná navíc potřebuje povolení.",
                "OSVČ podniká jako fyzická osoba. Společnost s ručením omezeným je osoba právnická.",
                NULL,
            },
        },
        {
            "2 / 2   •   Ručení", "s.r.o. a akciová společnost",
            "Tip: společnost ručí svým majetkem, společník jen omezeně.",
            {
                "Společník s.r.o. ručí jen do výše nesplaceného vkladu.",
                "Minimální vklad společníka s.r.o. je 1 koruna.",
                "Akciová společnost má akcie a základní kapitál nejméně 2 miliony korun.",
                "Podnikatel platí daně a odvody. Ohlášení živnosti není návod, jak se povinnostem vyhnout.",
                NULL,
            },
        },
    };

    return build_on4_unit_page(&on4_lessons[5], "on4_unit6", "on4_unit6_sub",
                               slides, G_N_ELEMENTS(slides));
}
static GtkWidget *build_on4_unit6_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co vystihuje podnikání?",
         {
          "Soustavná činnost na vlastní účet a odpovědnost za účelem zisku",
          "Jakákoli brigáda bez jakékoli povinnosti",
          "Jen práce pro stát",
          "Povinná školní docházka",
         },
         4, 0},
        {"Čím se liší koncesovaná živnost?",
         {
          "Ničím, stačí stejné ohlášení",
          "Potřebuje povolení, nestačí jen ohlášení",
          "Smí ji mít jen akciová společnost",
          "Je vždy bez podmínek",
         },
         4, 1},
        {"Jaký je minimální vklad společníka s.r.o.?",
         {
          "2 miliony korun",
          "100 000 eur",
          "1 koruna",
          "Kapitál se skládat nesmí",
         },
         4, 2},
        {"Čím ručí společník s.r.o. za dluhy společnosti?",
         {
          "Celým osobním majetkem bez omezení",
          "Jen do výše nesplaceného vkladu",
          "Za dluhy státu",
          "Společnost svým majetkem neručí vůbec",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Zisk je záměr, ne záruka",
        "Ohlašovací živnost se jen ohlašuje",
        "Akciová společnost má hranici mnohem výš",
        "Majetkem ručí samotná společnost",
    };
    return build_on4_mcq_page(5, "on4unit6", "on4_ex6_title",
                             "on4_quiz6_head", qs, hints, 4);
}

static GtkWidget *build_on4_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Mzda", "Kdo práci hledá",
            "Tip: nezaměstnaný není každý, kdo zrovna nesedí v práci.",
            {
                "Trh práce spojuje zaměstnavatele a lidi, kteří práci hledají.",
                "Mzda závisí na tom, co člověk umí, na poptávce a na dohodě.",
                "Nezaměstnaný je, kdo práci nemá, chce pracovat a hledá ji.",
                "Kdo práci nehledá, není v tomto smyslu nezaměstnaný.",
                NULL,
            },
        },
        {
            "2 / 2   •   Úřad", "Pomoc při ztrátě práce",
            "Tip: smlouvu a výpověď bere třetí ročník.",
            {
                "Úřad práce eviduje uchazeče o zaměstnání a pomáhá místo hledat.",
                "Podporu v nezaměstnanosti lze dostat jen při splnění zákonných podmínek.",
                "Vzdělání a dovednosti šanci na práci zvyšují.",
                "Pracovní smlouva, výpověď a zkušební doba jsou učivo třetího ročníku.",
                NULL,
            },
        },
    };

    return build_on4_unit_page(&on4_lessons[6], "on4_unit7", "on4_unit7_sub",
                               slides, G_N_ELEMENTS(slides));
}
static GtkWidget *build_on4_unit7_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo je nezaměstnaný?",
         {
          "Každý, kdo zrovna nemá směnu",
          "Kdo práci nemá, chce ji a hledá ji",
          "Každý student",
          "Kdo práci má, ale nelíbí se mu",
         },
         4, 1},
        {"Kdo eviduje uchazeče o zaměstnání?",
         {
          "Česká národní banka",
          "Úřad práce",
          "Ústavní soud",
          "Notář",
         },
         4, 1},
        {"Dostane podporu v nezaměstnanosti každý bez práce?",
         {
          "Ano, hned a bez podmínek",
          "Ano, vyplácí ji škola",
          "Ne, jen při splnění zákonných podmínek",
          "Ano, je to totéž co mzda",
         },
         4, 2},
        {"Kde se bere pracovní smlouva a výpověď?",
         {
          "U daní ve čtvrtém ročníku",
          "U voleb ve druhém ročníku",
          "U rodiny v prvním ročníku",
          "U pracovního práva ve třetím ročníku",
         },
         4, 3},
    };
    static const char *hints[] = {
        "Patří k tomu aktivní hledání",
        "Není to banka ani soud",
        "Podpora není automatická",
        "Čtvrtý ročník smlouvu neopakuje",
    };
    return build_on4_mcq_page(6, "on4unit7", "on4_ex7_title",
                             "on4_quiz7_head", qs, hints, 4);
}

static GtkWidget *build_on4_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Měsíc", "Příjmy a výdaje",
            "Tip: rozpočet je porovnání, ne přání.",
            {
                "Rozpočet domácnosti srovnává, co přijde a co odejde.",
                "Příjem je mzda, dávka, kapesné nebo jiný pravidelný přísun.",
                "Nejdřív patří bydlení, jídlo, doprava a další nutné výdaje.",
                "Až potom zbývá místo na věci, bez kterých se domácnost obejde.",
                NULL,
            },
        },
        {
            "2 / 2   •   Zásoba", "Rezerva a schodek",
            "Tip: rezerva není totéž co půjčka.",
            {
                "Rezerva kryje nečekaný výdaj, třeba opravu nebo výpadek příjmu.",
                "Krátký schodek se dá vyrovnat z rezervy.",
                "Dlouhý schodek znamená škrtat, nebo se zadlužovat.",
                "Rozpočet má smysl, jen když se podle něj domácnost opravdu řídí.",
                NULL,
            },
        },
    };

    return build_on4_unit_page(&on4_lessons[7], "on4_unit8", "on4_unit8_sub",
                               slides, G_N_ELEMENTS(slides));
}
static GtkWidget *build_on4_unit8_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co srovnává rozpočet domácnosti?",
         {
          "Jen přání",
          "Příjmy a výdaje",
          "Počet poslanců",
          "Inflaci a státní dluh",
         },
         4, 1},
        {"Co má v rozpočtu přednost?",
         {
          "Nejdřív bydlení, jídlo a další nutné výdaje",
          "Nejdřív všechno z reklamy",
          "Nejdřív půjčka na dovolenou",
          "Rozpočet se nesestavuje",
         },
         4, 0},
        {"K čemu je rezerva?",
         {
          "Aby smazala státní dluh",
          "Na nečekaný výdaj",
          "Nahrazuje zdravotní pojištění",
          "Je totéž co daň",
         },
         4, 1},
        {"Co znamená dlouhý schodek domácnosti?",
         {
          "Domácnost šetří víc, než musí",
          "Příjmy jsou vyšší než výdaje",
          "Výdaje převyšují příjmy a je třeba škrtat, nebo se domácnost zadluží",
          "Inflace je nulová",
         },
         4, 2},
    };
    static const char *hints[] = {
        "Obě strany musí být vidět",
        "Nutné výdaje jsou dřív než přání",
        "Patří na výpadek, ne na stát",
        "Schodek není přebytek",
    };
    return build_on4_mcq_page(7, "on4unit8", "on4_ex8_title",
                             "on4_quiz8_head", qs, hints, 4);
}

static GtkWidget *build_on4_unit9_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Cena", "Spoření a úvěr",
            "Tip: vyšší slíbený výnos obvykle znamená vyšší riziko.",
            {
                "Úspora je odložená spotřeba. Úvěr jsou půjčené peníze.",
                "Úrok je cena půjčených peněz a úvěr se vrací i s ním.",
                "RPSN ukazuje roční náklady spotřebitelského úvěru včetně poplatků.",
                "Nabídky se porovnávají. Půjčit si lze jen tolik, kolik se dá vracet.",
                NULL,
            },
        },
        {
            "2 / 2   •   Krytí", "Pojištění a tíseň",
            "Tip: nabídku, která těží z tísně, nepodepisujte.",
            {
                "Veřejné zdravotní pojištění je v České republice povinné.",
                "Sociální pojištění se týká důchodu, nemoci a ztráty zaměstnání, vždy podle zákona.",
                "Pojištění bytu nebo odpovědnosti je dobrovolné a kryje jen dohodnuté riziko.",
                "Nabídku, která těží z něčí tísně, odmítněte a řekněte o ní dospělému.",
                NULL,
            },
        },
    };

    return build_on4_unit_page(&on4_lessons[8], "on4_unit9", "on4_unit9_sub",
                               slides, G_N_ELEMENTS(slides));
}
static GtkWidget *build_on4_unit9_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je úrok?",
         {
          "Cena půjčených peněz",
          "Daň z přidané hodnoty",
          "Pokuta za hluk",
          "Státní dluh",
         },
         4, 0},
        {"Co ukazuje RPSN?",
         {
          "Jen barvu platební karty",
          "Roční náklady spotřebitelského úvěru včetně poplatků",
          "Výši HDP",
          "Počet poboček banky",
         },
         4, 1},
        {"Kolik si má domácnost půjčit?",
         {
          "Co nejvíc, výnos je jistý",
          "Tolik, kolik slíbí první reklama",
          "Jen tolik, kolik zvládne vracet",
          "Libovolně, úvěr se vracet nemusí",
         },
         4, 2},
        {"Jaké pojištění je v České republice povinné?",
         {
          "Pojištění dovolené",
          "Pojištění mobilu",
          "Pojištění bytu",
          "Veřejné zdravotní pojištění",
         },
         4, 3},
    };
    static const char *hints[] = {
        "Úvěr není zdarma",
        "Poplatky do ní patří",
        "Jistý vysoký zisk nikdo seriózní neslíbí",
        "Pojištění bytu je dobrovolné",
    };
    return build_on4_mcq_page(8, "on4unit9", "on4_ex9_title",
                             "on4_quiz9_head", qs, hints, 4);
}

static GtkWidget *build_on4_unit10_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Členství", "Unie, Schengen a euro",
            "Tip: členství v Unii ještě neznamená euro.",
            {
                "Česká republika je členským státem Evropské unie od 1. května 2004.",
                "Unie má 27 států. Společný trh je volný pohyb zboží, služeb, kapitálu a osob.",
                "Česko je v Schengenu. Ne každý stát Unie v něm je a některé státy Schengenu v Unii nejsou.",
                "Eurozóna je jen část Unie. V Česku se platí korunou.",
                NULL,
            },
        },
        {
            "2 / 2   •   Orgány", "Kdo v Unii rozhoduje",
            "Tip: Evropská rada není Rada Evropské unie.",
            {
                "Evropský parlament volí občané přímo.",
                "Evropská komise navrhuje unijní předpisy a dohlíží, jak se provádějí.",
                "Rada Evropské unie sdružuje ministry členských států.",
                "Evropská rada je setkání hlav států a vlád a určuje směr.",
                NULL,
            },
        },
    };

    return build_on4_unit_page(&on4_lessons[9], "on4_unit10", "on4_unit10_sub",
                               slides, G_N_ELEMENTS(slides));
}
static GtkWidget *build_on4_unit10_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Od kdy je Česká republika v Evropské unii?",
         {
          "Od roku 1993",
          "Od 1. května 2004",
          "Od 12. března 1999",
          "Od roku 1989",
         },
         4, 1},
        {"Platí Česko eurem?",
         {
          "Ano, od vstupu do Unie",
          "Ano, koruna zanikla",
          "Ne, je v Unii, ale ne v eurozóně",
          "Euro je měna Severoatlantické aliance",
         },
         4, 2},
        {"Je každý stát Evropské unie v Schengenu?",
         {
          "Ano, je to totéž",
          "Ne",
          "Ano, a Schengen je totéž co euro",
          "Ne, a Česko v něm také není",
         },
         4, 1},
        {"Kterou unijní instituci volí občané přímo?",
         {
          "Evropskou komisi",
          "Evropskou radu",
          "Radu Evropské unie",
          "Evropský parlament",
         },
         4, 3},
    };
    static const char *hints[] = {
        "Rok 1999 patří k NATO",
        "Eurozóna je užší než Unie",
        "Česko v Schengenu je",
        "Ministři v Radě EU voleni přímo nejsou",
    };
    return build_on4_mcq_page(9, "on4unit10", "on4_ex10_title",
                             "on4_quiz10_head", qs, hints, 4);
}

static GtkWidget *build_on4_unit11_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Aliance", "Česko v NATO",
            "Tip: společná obrana neruší suverenitu státu.",
            {
                "Česká republika je svrchovaný stát.",
                "NATO je obranná aliance. Česko do něj vstoupilo 12. března 1999.",
                "Smlouva bere ozbrojený útok na jednoho člena jako útok na všechny.",
                "Ostatní členové pak poskytnou pomoc. Aliance není obchodní smlouva ani soud.",
                NULL,
            },
        },
        {
            "2 / 2   •   Mír", "Organizace spojených národů",
            "Tip: OSN není totéž co Evropská unie.",
            {
                "OSN vznikla roku 1945, aby státy chránily mír a řešily spory jednáním.",
                "Valné shromáždění sdružuje členské státy.",
                "Rada bezpečnosti se zabývá ohrožením míru.",
                "Česká republika je členem OSN, stejně jako je členem Evropské unie a NATO.",
                NULL,
            },
        },
    };

    return build_on4_unit_page(&on4_lessons[10], "on4_unit11", "on4_unit11_sub",
                               slides, G_N_ELEMENTS(slides));
}
static GtkWidget *build_on4_unit11_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdy Česko vstoupilo do NATO?",
         {
          "1. května 2004",
          "12. března 1999",
          "Roku 1945",
          "Roku 1918",
         },
         4, 1},
        {"Co znamená zásada společné obrany?",
         {
          "Ozbrojený útok na jednoho člena se bere jako útok na všechny",
          "Aliance ruší suverenitu členů",
          "Členové si nesmějí pomáhat",
          "Jde o smlouvu o sazbě DPH",
         },
         4, 0},
        {"K čemu vznikla OSN?",
         {
          "Aby vydávala korunu",
          "Aby státy chránily mír a řešily spory jednáním",
          "Aby vedla živnostenský rejstřík",
          "Aby stanovila českou DPH",
         },
         4, 1},
        {"Je Česká republika členem OSN?",
         {
          "Ne",
          "Jen jako pozorovatel bez členství",
          "Ano",
          "Byla, ale roku 2004 vystoupila",
         },
         4, 2},
    };
    static const char *hints[] = {
        "Rok 2004 je vstup do Evropské unie",
        "Pomoc není daňový předpis",
        "Vznikla roku 1945",
        "Členství v Unii členství v OSN neruší",
    };
    return build_on4_mcq_page(10, "on4unit11", "on4_ex11_title",
                             "on4_quiz11_head", qs, hints, 4);
}

static GtkWidget *build_on4_unit12_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Hranice", "Propojený svět",
            "Tip: propojení není jen výhoda.",
            {
                "Globalizace je rostoucí propojení ekonomik, informací a kultur přes hranice.",
                "Přináší obchod, cestování a rychlejší šíření poznatků.",
                "Nese i závislost na jiných státech, nerovnost a tlak na životní prostředí.",
                "Globální problém, třeba změna klimatu nebo pandemie, se na hranici nezastaví.",
                NULL,
            },
        },
        {
            "2 / 2   •   Odpověď", "Spolupráce a vlastní volba",
            "Tip: sledovat původ zboží je taky rozhodnutí.",
            {
                "Krize v jedné zemi se může přelít do jiných.",
                "Státy proto spolupracují v organizacích a ve smlouvách.",
                "Člověk může sledovat, odkud zboží je, a rozhodovat se s rozmyslem.",
                "Hlas ve volbách je způsob, jak se k těm otázkám postavit.",
                NULL,
            },
        },
    };

    return build_on4_unit_page(&on4_lessons[11], "on4_unit12", "on4_unit12_sub",
                               slides, G_N_ELEMENTS(slides));
}
static GtkWidget *build_on4_unit12_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je globalizace?",
         {
          "Zákaz obchodu přes hranice",
          "Rostoucí propojení ekonomik, informací a kultur",
          "Totéž co státní rozpočet",
          "Zánik všech států",
         },
         4, 1},
        {"Co globalizace může přinést?",
         {
          "Obchod, cestování a rychlejší šíření poznatků",
          "Jen izolaci",
          "Zrušení všech měn",
          "Konec odpovědnosti",
         },
         4, 0},
        {"Co je globální problém?",
         {
          "Jen hádka dvou sousedů",
          "Problém, který se nezastaví na hranici jednoho státu",
          "Schodek jedné domácnosti",
          "Místní školní řád",
         },
         4, 1},
        {"Proč státy u takových otázek spolupracují?",
         {
          "Protože krize se může přelít přes hranice",
          "Protože hranice už neexistují",
          "Protože OSN vybírá české daně",
          "Spolupráce není k ničemu",
         },
         4, 0},
    };
    static const char *hints[] = {
        "Státy globalizací nemizejí",
        "Přínos nevylučuje riziko",
        "Klima a pandemie jsou příklady",
        "Jedna země to sama neuzavře",
    };
    return build_on4_mcq_page(11, "on4unit12", "on4_ex12_title",
                             "on4_quiz12_head", qs, hints, 4);
}

static GtkWidget *build_on4_unit13_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Otázka", "Co filozofie dělá",
            "Tip: filozofie není seznam hotových odpovědí.",
            {
                "Filozofie se ptá, co je skutečné, co můžeme poznat a jak máme žít.",
                "Nestačí jí příkaz ani pouhý zvyk. Chce důvody.",
                "Rozlišuje pojmy, aby se lidé nebavili o různých věcech stejným slovem.",
                "Nemá jeden školní klíč, který by disputaci ukončil.",
                NULL,
            },
        },
        {
            "2 / 2   •   Antika", "Sókratés, Platón a Aristotelés",
            "Tip: Sókratés se nevydával za majitele hotových pravd.",
            {
                "Sókratés zkoumal, co lidé pokládají za jisté.",
                "Sám se za vlastníka hotových pravd nevydával.",
                "Platón hledal trvalé ideje za proměnlivým světem.",
                "Aristotelés vycházel ze zkušenosti a třídil, co lze poznat.",
                NULL,
            },
        },
    };

    return build_on4_unit_page(&on4_lessons[12], "on4_unit13", "on4_unit13_sub",
                               slides, G_N_ELEMENTS(slides));
}
static GtkWidget *build_on4_unit13_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Na co se filozofie ptá?",
         {
          "Jen na výši DPH",
          "Co je skutečné, co lze poznat a jak žít",
          "Kolik je poslanců",
          "Jak se vyplní daňové přiznání",
         },
         4, 1},
        {"Stačí filozofii pouhý příkaz?",
         {
          "Ano, příkaz je důkaz",
          "Ne, chce důvody",
          "Ano, zvyk se nezkoumá",
          "Je totéž co zákoník",
         },
         4, 1},
        {"Co Sókratés odmítal o sobě tvrdit?",
         {
          "Že už vlastní hotové pravdy",
          "Že se má ptát",
          "Že rozhovor má smysl",
          "Že člověk může nevědět",
         },
         4, 0},
        {"Čím se Aristotelés ve škole často liší od Platóna?",
         {
          "Popíral, že lze cokoli poznat",
          "Sepsal zákoník práce",
          "Vycházel ze zkušenosti",
          "Odmítal třídit pojmy",
         },
         4, 2},
    };
    static const char *hints[] = {
        "Jde o otázky, které tabulka neuzavře",
        "Bez důvodů to není zkoumání",
        "Zkoumal jistoty druhých",
        "Platón hledal trvalé ideje",
    };
    return build_on4_mcq_page(12, "on4unit13", "on4_ex13_title",
                             "on4_quiz13_head", qs, hints, 4);
}

static GtkWidget *build_on4_unit14_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Správné", "Norma a svědomí",
            "Tip: svědomí může mýlit, proto patří i důvody.",
            {
                "Etika se ptá, co je správné dělat a proč.",
                "Mravní norma není totéž co zákon. Zákon bere třetí ročník.",
                "Svědomí je vnitřní smysl pro správné a špatné.",
                "Může se mýlit. Proto nestačí pocit bez důvodu.",
                NULL,
            },
        },
        {
            "2 / 2   •   Volba", "Svoboda a důstojnost",
            "Tip: výhodné jednání ještě nemusí být správné.",
            {
                "Svoboda znamená, že člověk volí. Odpovědnost znamená, že za volbu stojí.",
                "Svoboda bez odpovědnosti je svévole.",
                "Lidská důstojnost patří každému, ne jen úspěšným nebo užitečným.",
                "Dobré jednání se nepozná jen podle toho, že se vyplatí.",
                NULL,
            },
        },
    };

    return build_on4_unit_page(&on4_lessons[13], "on4_unit14", "on4_unit14_sub",
                               slides, G_N_ELEMENTS(slides));
}
static GtkWidget *build_on4_unit14_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co zkoumá etika?",
         {
          "Výši úrokových sazeb",
          "Co je správné dělat a proč",
          "Soustavu soudů",
          "Sazbu DPH",
         },
         4, 1},
        {"Je mravní norma totéž co zákon?",
         {
          "Ano",
          "Ne",
          "Každá neslušnost je hned trestný čin",
          "Morálku vynucuje jen soud",
         },
         4, 1},
        {"Je svědomí neomylné?",
         {
          "Ano, proto důvody nepotřebujeme",
          "Ano, nahrazuje zákon",
          "Ne, může se mýlit",
          "Svědomí mají jen věřící",
         },
         4, 2},
        {"Komu patří lidská důstojnost?",
         {
          "Jen úspěšným",
          "Jen užitečným",
          "Každému člověku",
          "Jen těm, kdo se nikdy nezmýlí",
         },
         4, 2},
    };
    static const char *hints[] = {
        "Ptá se po důvodech jednání",
        "Zákon a mrav se často kryjí, ale nejsou totéž",
        "Pocit se má zkoumat",
        "Důstojnost se nekupuje výkonem",
    };
    return build_on4_mcq_page(13, "on4unit14", "on4_ex14_title",
                             "on4_quiz14_head", qs, hints, 4);
}

static GtkWidget *build_on4_unit15_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Smysl", "Víra i její odmítnutí",
            "Tip: stát nesmí víru vnutit ani zakázat.",
            {
                "Lidé hledají smysl ve víře, ve filozofii i v pohledu bez náboženství.",
                "Křesťanství dlouho utvářelo českou a evropskou kulturu.",
                "Vedle něj studenti potkávají judaismus, islám a další tradice.",
                "Ateismus a agnosticismus jsou postoje, které svoboda svědomí taky chrání.",
                NULL,
            },
        },
        {
            "2 / 2   •   Hranice", "Respekt není totéž co pravda",
            "Tip: nesouhlasit lze bez pohrdání.",
            {
                "Člověk smí věřit, víru změnit, nebo nevěřit.",
                "Respekt k člověku neznamená, že každý názor je pravdivý.",
                "Náboženství a věda odpovídají na jiný druh otázek.",
                "Svoboda svědomí chrání i toho, kdo se víry zřekl.",
                NULL,
            },
        },
    };

    return build_on4_unit_page(&on4_lessons[14], "on4_unit15", "on4_unit15_sub",
                               slides, G_N_ELEMENTS(slides));
}
static GtkWidget *build_on4_unit15_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Smí stát vnutit víru?",
         {
          "Ano, tu většinovou",
          "Ne",
          "Ano, ve škole povinně jednu",
          "Ano, nevěřící ztrácí práva",
         },
         4, 1},
        {"Chrání svoboda svědomí i ateismus?",
         {
          "Ne",
          "Ano, člověk smí nevěřit",
          "Jen mimo Evropu",
          "Ateismus je státní náboženství",
         },
         4, 1},
        {"Znamená respekt, že každý názor je pravdivý?",
         {
          "Ano",
          "Ne, nesouhlasit lze slušně",
          "Pravda je to, co říká hlasitější",
          "Názory se nesmějí vůbec porovnávat",
         },
         4, 1},
        {"Je náboženství totéž co věda?",
         {
          "Ano",
          "Ano, obojí měří inflaci",
          "Ne, odpovídají na jiný druh otázek",
          "Věda ukládá povinnou víru",
         },
         4, 2},
    };
    static const char *hints[] = {
        "Svoboda svědomí nutit nedovolí",
        "Patří k ní i odmítnutí víry",
        "Respekt patří člověku",
        "Každé se ptá jinak",
    };
    return build_on4_mcq_page(14, "on4unit15", "on4_ex15_title",
                             "on4_quiz15_head", qs, hints, 4);
}

static GtkWidget *build_on4_unit16_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Měřítko", "Podle čeho volíme",
            "Tip: praktická filozofie chce důvody, ne heslo.",
            {
                "Hodnoty jsou měřítka volby: důstojnost, pravda, spravedlnost, svoboda, solidarita.",
                "Dobrý život není totéž co pohodlí nebo vysoký příjem.",
                "Člověk má zkoumat vlastní důvody, ne jen opakovat větu.",
                "Společenství, občan, právo a hospodářství k sobě v občance patří.",
                NULL,
            },
        },
        {
            "2 / 2   •   Druzí", "Pravda, solidarita, svoboda",
            "Tip: svoboda druhého je hranice mé svobody.",
            {
                "Pravda se nepozná podle toho, kdo křičí hlasitěji.",
                "Spravedlnost chce podobným případům podobný přístup.",
                "Solidarita je ochota nést část nákladu s druhými, ne jen brát výhody.",
                "Svoboda druhého je hranice mé svobody.",
                NULL,
            },
        },
    };

    return build_on4_unit_page(&on4_lessons[15], "on4_unit16", "on4_unit16_sub",
                               slides, G_N_ELEMENTS(slides));
}
static GtkWidget *build_on4_unit16_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"K čemu jsou hodnoty?",
         {
          "Nahrazují domácí rozpočet",
          "Jsou měřítka, podle kterých volíme",
          "Ruší odpovědnost",
          "Jsou totéž co sazba DPH",
         },
         4, 1},
        {"Je dobrý život totéž co vysoký příjem?",
         {
          "Ano",
          "Ne",
          "Ano, HDP ho měří přímo",
          "Ano, důstojnost se kupuje",
         },
         4, 1},
        {"Co praktická filozofie od člověka chce?",
         {
          "Aby opakoval heslo",
          "Aby zkoumal vlastní důvody",
          "Aby se vzdal svobody",
          "Aby nečetl námitky",
         },
         4, 1},
        {"Co ohraničuje mou svobodu?",
         {
          "Nic",
          "Jen výše mzdy",
          "Svoboda druhého",
          "Počet bank v ulici",
         },
         4, 2},
    };
    static const char *hints[] = {
        "Bez nich je volba nahodilá",
        "Příjem je prostředek, ne celý život",
        "Heslo bez důvodu nestačí",
        "Svoboda není svévole",
    };
    return build_on4_mcq_page(15, "on4unit16", "on4_ex16_title",
                             "on4_quiz16_head", qs, hints, 4);
}
void add_on4_pages(GtkStack *stack) {
    typedef GtkWidget *(*OnBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        OnBuilder build_unit;
        OnBuilder build_ex;
    } pages[] = {
        {"on4unit1", "on4ex1", build_on4_unit1_page, build_on4_unit1_exercise_page},
        {"on4unit2", "on4ex2", build_on4_unit2_page, build_on4_unit2_exercise_page},
        {"on4unit3", "on4ex3", build_on4_unit3_page, build_on4_unit3_exercise_page},
        {"on4unit4", "on4ex4", build_on4_unit4_page, build_on4_unit4_exercise_page},
        {"on4unit5", "on4ex5", build_on4_unit5_page, build_on4_unit5_exercise_page},
        {"on4unit6", "on4ex6", build_on4_unit6_page, build_on4_unit6_exercise_page},
        {"on4unit7", "on4ex7", build_on4_unit7_page, build_on4_unit7_exercise_page},
        {"on4unit8", "on4ex8", build_on4_unit8_page, build_on4_unit8_exercise_page},
        {"on4unit9", "on4ex9", build_on4_unit9_page, build_on4_unit9_exercise_page},
        {"on4unit10", "on4ex10", build_on4_unit10_page, build_on4_unit10_exercise_page},
        {"on4unit11", "on4ex11", build_on4_unit11_page, build_on4_unit11_exercise_page},
        {"on4unit12", "on4ex12", build_on4_unit12_page, build_on4_unit12_exercise_page},
        {"on4unit13", "on4ex13", build_on4_unit13_page, build_on4_unit13_exercise_page},
        {"on4unit14", "on4ex14", build_on4_unit14_page, build_on4_unit14_exercise_page},
        {"on4unit15", "on4ex15", build_on4_unit15_page, build_on4_unit15_exercise_page},
        {"on4unit16", "on4ex16", build_on4_unit16_page, build_on4_unit16_exercise_page}
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
