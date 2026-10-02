#include "graduately.h"

/* Year 2 of Občanská nauka.
 * Thematic unit "Člověk jako občan v demokratickém státě" from the MŠMT
 * model syllabus (č.j. 18 396/2002-23) and the SOV civic framework, which
 * schools place in the second year. Law, the economy and philosophy stay
 * in later years. */

#define ON2_ENTRY(n) \
    { .n_slides = 2, .unit_page = "on2unit" #n, .ex_page = "on2ex" #n }

NetLesson on2_lessons[ON2_LESSONS] = {
    ON2_ENTRY(1),
    ON2_ENTRY(2),
    ON2_ENTRY(3),
    ON2_ENTRY(4),
    ON2_ENTRY(5),
    ON2_ENTRY(6),
    ON2_ENTRY(7),
    ON2_ENTRY(8),
    ON2_ENTRY(9),
    ON2_ENTRY(10),
    ON2_ENTRY(11),
    ON2_ENTRY(12),
    ON2_ENTRY(13),
    ON2_ENTRY(14),
    ON2_ENTRY(15),
    ON2_ENTRY(16),
};

#undef ON2_ENTRY

GtkWidget *on2_scroll;
GtkWidget *on2_fixed;
GtkWidget *on2_rail;
GtkWidget *on2_nodes[ON2_UNITS];
GtkWidget *on2_labels[ON2_UNITS];
double on2_cx[ON2_UNITS];
double on2_cy[ON2_UNITS];
int on2_cols = 1;
int on2_rows = 1;
int on2_cw = (int)(2.0 * ROAD_MX + (ON2_UNITS - 1) * PATH_SPAC);
int on2_ch = (int)(2.0 * ROAD_MY);
guint on2_idle;
double on2_last_avail = -1.0;
int on2_last_cols = -1;
int on2_last_rows = -1;
int on2_last_cw = -1;
int on2_last_ch = -1;
cairo_surface_t *on2_rail_cache;
int on2_cache_w;
int on2_cache_h;

static void on2_point(double t, double *ox, double *oy) {
    int k = (int)t;
    double u = t - (double)k;
    double u2, u3;
    double p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y;

    if (k < 0) { k = 0; u = 0.0; }
    if (k >= ON2_UNITS - 1) { k = ON2_UNITS - 2; u = 1.0; }

    p1x = on2_cx[k];     p1y = on2_cy[k];
    p2x = on2_cx[k + 1]; p2y = on2_cy[k + 1];
    if (k - 1 >= 0) { p0x = on2_cx[k - 1]; p0y = on2_cy[k - 1]; }
    else            { p0x = p1x - (p2x - p1x); p0y = p1y - (p2y - p1y); }
    if (k + 2 < ON2_UNITS) { p3x = on2_cx[k + 2]; p3y = on2_cy[k + 2]; }
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

static void on2_path(cairo_t *cr, double t0, double t1) {
    const double steps_per_unit = 24.0;
    int n = (int)((t1 - t0) * steps_per_unit);
    double x, y;
    int s;

    if (n < 1)
        n = 1;
    on2_point(t0, &x, &y);
    cairo_move_to(cr, x, y);
    for (s = 1; s <= n; s++) {
        on2_point(t0 + (t1 - t0) * (double)s / (double)n, &x, &y);
        cairo_line_to(cr, x, y);
    }
}

static void on2_geometry(double avail) {
    const double phase = 2.0 * G_PI * 1.7 / (double)(ON2_UNITS - 1);
    int rows;
    int r, c, i;

    for (rows = 1; rows <= ON2_UNITS; rows++) {
        int cols = (ON2_UNITS + rows - 1) / rows;
        if (2.0 * ROAD_MX + (cols - 1) * PATH_SPAC <= avail + 1.0)
            break;
    }
    if (rows > ON2_UNITS)
        rows = ON2_UNITS;
    on2_rows = rows;
    on2_cols = (ON2_UNITS + rows - 1) / rows;
    if (on2_cols < 1)
        on2_cols = 1;
    on2_cw = (int)(2.0 * ROAD_MX + (on2_cols - 1) * PATH_SPAC);
    on2_ch = (int)(2.0 * ROAD_MY + (rows - 1) * ROAD_GAP);

    for (r = 0; r < rows; r++) {
        int base = r * on2_cols;
        int len = MIN(on2_cols, ON2_UNITS - base);
        int fwd = (r % 2) == 0;
        for (c = 0; c < len; c++) {
            i = base + c;
            int cc = fwd ? c : (on2_cols - 1 - c);
            on2_cx[i] = ROAD_MX + cc * PATH_SPAC;
            on2_cy[i] = ROAD_MY + r * ROAD_GAP
                       + ROAD_WAVE * sin((double)i * phase);
        }
    }
}

static void on2_apply_layout(void) {
    if (!on2_fixed)
        return;

    if (on2_cols == on2_last_cols && on2_rows == on2_last_rows &&
        on2_cw == on2_last_cw && on2_ch == on2_last_ch)
        return;

    on2_last_cols = on2_cols;
    on2_last_rows = on2_rows;
    on2_last_cw = on2_cw;
    on2_last_ch = on2_ch;
    if (on2_rail_cache) {
        cairo_surface_destroy(on2_rail_cache);
        on2_rail_cache = NULL;
    }

    for (int i = 0; i < ON2_UNITS; i++) {
        if (!on2_nodes[i] || !on2_labels[i])
            continue;
        gtk_fixed_move(GTK_FIXED(on2_fixed), on2_nodes[i],
                       (int)(on2_cx[i] - NODE_SIZE / 2.0),
                       (int)(on2_cy[i] - NODE_SIZE / 2.0));
        gtk_fixed_move(GTK_FIXED(on2_fixed), on2_labels[i],
                       (int)(on2_cx[i] - (PATH_SPAC - 20.0) / 2.0),
                       (int)(on2_cy[i] + NODE_SIZE / 2.0 + 10.0));
    }

    gtk_widget_set_size_request(on2_fixed, on2_cw, on2_ch);
    gtk_widget_set_size_request(on2_rail, on2_cw, on2_ch);
    gtk_fixed_move(GTK_FIXED(on2_fixed), on2_rail, 0, 0);
    gtk_widget_queue_draw(on2_rail);
}

static void on2_relayout(void) {
    GtkAdjustment *hadj;
    double avail;

    if (!on2_scroll)
        return;
    hadj = gtk_scrolled_window_get_hadjustment(
        GTK_SCROLLED_WINDOW(on2_scroll));
    avail = gtk_adjustment_get_page_size(hadj);
    if (avail < 1.0)
        return;
    if (fabs(avail - on2_last_avail) < 1.0)
        return;
    on2_last_avail = avail;
    on2_geometry(avail);
    on2_apply_layout();
}

static gboolean on2_relayout_idle(gpointer data) {
    (void)data;
    on2_idle = 0;
    on2_relayout();
    return G_SOURCE_REMOVE;
}

static void on2_relayout_later(void) {
    if (on2_idle == 0)
        on2_idle = g_idle_add(on2_relayout_idle, NULL);
}

static void on2_adjust_notify(GtkAdjustment *adj, GParamSpec *ps,
                             gpointer data) {
    (void)adj;
    (void)ps;
    (void)data;
    on2_relayout_later();
}

static void on2_render_rail_to(cairo_t *cr) {
    const double t_end = (double)(ON2_UNITS - 1);
    const Rgb rail = color_from_hex(app_theme.rail);
    const Rgb muted = color_from_hex(app_theme.subtext);

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

    cairo_new_path(cr);
    on2_path(cr, 0.0, t_end);
    cairo_set_line_width(cr, 12);
    cairo_set_source_rgb(cr, rail.r, rail.g, rail.b);
    cairo_stroke(cr);

    cairo_set_source_rgba(cr, muted.r, muted.g, muted.b, 0.17);
    cairo_set_dash(cr, (double[]){2.0, 40.0}, 2, 0.0);
    cairo_new_path(cr);
    on2_path(cr, 0.0, t_end);
    cairo_stroke(cr);
    cairo_set_dash(cr, NULL, 0, 0.0);
}

static void on2_draw_rail(GtkDrawingArea *area, cairo_t *cr,
                         int width, int height, gpointer data) {
    (void)area;
    (void)width;
    (void)height;
    (void)data;

    if (!on2_rail_cache || on2_cache_w != on2_cw || on2_cache_h != on2_ch) {
        cairo_t *rcr;

        if (on2_rail_cache)
            cairo_surface_destroy(on2_rail_cache);
        on2_cache_w = on2_cw;
        on2_cache_h = on2_ch;
        if (on2_cache_w < 1)
            on2_cache_w = 1;
        if (on2_cache_h < 1)
            on2_cache_h = 1;
        on2_rail_cache = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                    on2_cache_w, on2_cache_h);
        rcr = cairo_create(on2_rail_cache);
        on2_render_rail_to(rcr);
        cairo_destroy(rcr);
    }

    cairo_set_source_surface(cr, on2_rail_cache, 0, 0);
    cairo_paint(cr);
}

void on2_rail_theme_reset(void) {
    if (on2_rail_cache) {
        cairo_surface_destroy(on2_rail_cache);
        on2_rail_cache = NULL;
    }
    if (on2_rail)
        gtk_widget_queue_draw(on2_rail);
}

static void on2_open_unit(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L)
        return;
    L->idx = 0;
    if (L->stack) {
        char name[16];

        g_snprintf(name, sizeof(name), "on2_slide%u", L->idx);
        gtk_stack_set_visible_child_name(GTK_STACK(L->stack), name);
        if (L->prev_btn)
            gtk_widget_set_sensitive(L->prev_btn, FALSE);
        if (L->next_btn)
            gtk_button_set_label(GTK_BUTTON(L->next_btn), tr("net_slide_next"));
    }
    gtk_stack_set_visible_child_name(main_stack, L->unit_page);
}

static void on2_add_node(GtkFixed *fixed, int index) {
    int num = index + 1;
    gboolean locked = index >= ON2_LESSONS;
    GtkWidget *card;
    GtkWidget *vbox;
    GtkWidget *number;
    GtkWidget *icon;
    GtkWidget *name;
    char *text;
    static char unit_keys[ON2_LESSONS][16];
    static gboolean unit_keys_ready = FALSE;

    if (!unit_keys_ready) {
        for (int i = 0; i < ON2_LESSONS; i++)
            g_snprintf(unit_keys[i], sizeof unit_keys[i], "on2_unit%d", i + 1);
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
        on2_lessons[index].done_icon = icon;
        g_signal_connect(card, "clicked", G_CALLBACK(on2_open_unit),
                         &on2_lessons[index]);
    }

    on2_nodes[index] = card;
    gtk_fixed_put(fixed, card, 0, 0);

    name = gtk_label_new(NULL);
    gtk_widget_set_size_request(name, (int)(PATH_SPAC - 20.0), -1);
    gtk_widget_set_halign(name, GTK_ALIGN_CENTER);
    gtk_label_set_justify(GTK_LABEL(name), GTK_JUSTIFY_CENTER);
    gtk_label_set_wrap(GTK_LABEL(name), TRUE);
    gtk_widget_add_css_class(name, "unit-name");
    if (index < ON2_LESSONS) {
        i18n_bind(name, unit_keys[index], 0);
    } else {
        gtk_widget_add_css_class(name, "unit-name-locked");
    }
    on2_labels[index] = name;
    gtk_fixed_put(fixed, name, 0, 0);
}
GtkWidget *build_on2map_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *fixed;
    GtkWidget *rail;
    GtkAdjustment *ha;
    GtkAdjustment *va;

    on2_geometry(1000.0);

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("onyears", "on_year2", "on2_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);
    on2_scroll = scroll;

    wrap = gtk_center_box_new();
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), wrap);

    fixed = gtk_fixed_new();
    gtk_widget_set_halign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(fixed, on2_cw, on2_ch);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), fixed);
    on2_fixed = fixed;

    rail = gtk_drawing_area_new();
    gtk_widget_set_size_request(rail, on2_cw, on2_ch);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(rail), on2_draw_rail,
                                   NULL, NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rail, 0, 0);
    on2_rail = rail;

    for (int i = 0; i < ON2_UNITS; i++)
        on2_add_node(GTK_FIXED(fixed), i);

    ha = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scroll));
    va = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));
    g_signal_connect(ha, "notify::page-size",
                     G_CALLBACK(on2_adjust_notify), NULL);
    g_signal_connect(va, "notify::page-size",
                     G_CALLBACK(on2_adjust_notify), NULL);

    on2_apply_layout();
    on2_relayout_later();
    refresh_on2_completion_ui();

    return page;
}

static void on2_slide_apply(NetLesson *L);
static void on2_slide_prev(GtkButton *button, gpointer data);
static void on2_slide_next(GtkButton *button, gpointer data);

static int on2_note_last_w = -1;

static void on2_rescale_notes_at(int w) {
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
    for (int i = 0; i < ON2_LESSONS; i++)
        net_rescale_lesson_notes(&on2_lessons[i], body, head, kick);
}

static gboolean on2_note_tick(GtkWidget *w, GdkFrameClock *clock,
                             gpointer data) {
    int width = gtk_widget_get_allocated_width(w);

    (void)clock;
    (void)data;
    if (width != on2_note_last_w) {
        on2_note_last_w = width;
        on2_rescale_notes_at(width);
    }
    return G_SOURCE_CONTINUE;
}

static GtkWidget *build_on2_unit_page(NetLesson *L, const char *title_key,
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

    gtk_box_append(GTK_BOX(page), top_bar("on2map", title_key, sub_key));

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
        g_snprintf(name, sizeof(name), "on2_slide%u", s);
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

    gtk_widget_add_tick_callback(L->stack, on2_note_tick, NULL, NULL);
    on2_note_last_w = -1;
    on2_rescale_notes_at(0);
    net_notes_target = NULL;

    nav = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(nav), TRUE);
    gtk_widget_set_margin_top(nav, 10);
    gtk_box_append(GTK_BOX(page), nav);

    L->prev_btn = gtk_button_new_with_label(tr("net_slide_prev"));
    gtk_widget_add_css_class(L->prev_btn, "btn-primary");
    g_signal_connect(L->prev_btn, "clicked", G_CALLBACK(on2_slide_prev), L);
    gtk_box_append(GTK_BOX(nav), L->prev_btn);

    L->next_btn = gtk_button_new_with_label(tr("net_slide_next"));
    gtk_widget_add_css_class(L->next_btn, "btn-primary");
    g_signal_connect(L->next_btn, "clicked", G_CALLBACK(on2_slide_next), L);
    gtk_box_append(GTK_BOX(nav), L->next_btn);

    L->idx = 0;
    on2_slide_apply(L);
    return page;
}

static void on2_slide_apply(NetLesson *L) {
    char name[16];

    if (!L || !L->stack)
        return;
    g_snprintf(name, sizeof(name), "on2_slide%u", L->idx);
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

static void on2_slide_prev(GtkButton *button, gpointer data) {
    NetLesson *L = data;
    (void)button;
    if (!L) return;
    if (L->idx > 0) {
        L->idx--;
        on2_slide_apply(L);
    }
}

static void on2_slide_next(GtkButton *button, gpointer data) {
    NetLesson *L = data;
    (void)button;
    if (!L) return;
    if (L->idx + 1 < L->n_slides) {
        L->idx++;
        on2_slide_apply(L);
    } else {
        gtk_stack_set_visible_child_name(main_stack, L->ex_page);
    }
}

void on2_lessons_apply_lang(void) {
    for (int i = 0; i < ON2_LESSONS; i++)
        on2_slide_apply(&on2_lessons[i]);
}

static void on2_mcq_check(GtkButton *button, gpointer data) {
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
        mark_on2_done(ctx->lesson_id);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry_short"));
    }
}

static GtkWidget *build_on2_mcq_page(int lesson_id, const char *back_page,
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
    g_signal_connect(check, "clicked", G_CALLBACK(on2_mcq_check), ctx);

    ctx->feedback = gtk_label_new("");
    gtk_widget_set_halign(ctx->feedback, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(body), ctx->feedback);

    return page;
}
static GtkWidget *build_on2_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Stát", "Co dělá stát státem",
            "Tip: bydliště ještě není občanství.",
            {
                "Stát je organizace veřejné moci na určitém území a nad lidmi, kteří tam žijí.",
                "Má území, obyvatelstvo, veřejnou moc a právní řád.",
                "Občan má ke státu trvalý právní svazek, kterému se říká státní občanství.",
                "Cizinec může na území žít, občanem se tím sám nestává.",
                NULL,
            },
        },
        {
            "2 / 2   •   Podíl", "Práva a povinnosti občana",
            "Tip: demokratický stát má občanovi sloužit.",
            {
                "Z občanství plynou práva, třeba volit do Parlamentu, a také povinnosti.",
                "Povinností je dodržovat zákony, platit daně a respektovat práva druhých.",
                "Moc ve státě má být kontrolovatelná, ne neomezená.",
                "Občan se podílí volbami, kontrolou moci a slušným jednáním.",
                NULL,
            },
        },
    };

    return build_on2_unit_page(&on2_lessons[0], "on2_unit1", "on2_unit1_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on2_unit1_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je stát?",
         {
          "Jen mapa a hymna",
          "Organizace veřejné moci na území s obyvatelstvem a právním řádem",
          "Každá skupina lidí",
          "Pouze vláda",
         },
         4, 1},
        {"Čím se občan liší od cizince?",
         {
          "Má trvalý právní svazek se státem",
          "Bydlí v hlavním městě",
          "Mluví úředním jazykem",
          "Platí jízdné",
         },
         4, 0},
        {"Co z občanství plyne?",
         {
          "Jen povinnost volit",
          "Jen právo na pas",
          "Práva i povinnosti",
          "Nic, dokud člověk nepracuje",
         },
         4, 2},
        {"Jak se občan na demokratickém státě podílí?",
         {
          "Jen poslušností bez ptaní",
          "Volbami, kontrolou moci a dodržováním zákonů",
          "Tím, že moc nechá bez dozoru",
          "Jen placením pokut",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Stát má území, obyvatele, moc a právo",
        "Občanství je právní svazek, ne jen pobyt",
        "Občan má práva a také povinnosti",
        "Podíl je volba, kontrola i respekt k právu",
    };
    return build_on2_mcq_page(0, "on2unit1", "on2_ex1_title",
                             "on2_quiz1_head", qs, hints, 4);
}
static GtkWidget *build_on2_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Nabytí", "Jak se české občanství získává",
            "Tip: narození na území ČR samo o sobě občanství nedává.",
            {
                "Dítě ho nabývá narozením, je-li alespoň jeden rodič občanem České republiky.",
                "Dále ho lze nabýt určením otcovství, osvojením nebo nalezením dítěte, jehož totožnost se nezjistí.",
                "Dospělý cizinec ho může získat udělením, v některých případech prohlášením.",
                "Podrobnosti určuje zákon o státním občanství.",
                NULL,
            },
        },
        {
            "2 / 2   •   Udělení", "Když o občanství žádá cizinec",
            "Tip: na udělení není právní nárok.",
            {
                "Udělení je rozhodnutí státu, ne automatický nárok žadatele.",
                "Žadatel musí splnit podmínky, mimo jiné pobyt, bezúhonnost a znalost češtiny.",
                "Patří sem i základní znalost ústavního systému a plnění povinností vůči státu.",
                "Občan České republiky může mít i jiné občanství, dovoluje-li to zákon.",
                NULL,
            },
        },
    };

    return build_on2_unit_page(&on2_lessons[1], "on2_unit2", "on2_unit2_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on2_unit2_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdy dítě nabývá české občanství narozením?",
         {
          "Když se narodí v Praze",
          "Když je alespoň jeden rodič občanem ČR",
          "Když chodí do české školy",
          "Když mu je osmnáct",
         },
         4, 1},
        {"Dává samotné narození v ČR občanství?",
         {
          "Ano, každému",
          "Ne, kromě zvláštních případů, třeba nalezeného dítěte",
          "Ano, pokud rodiče platí daň",
          "Ano, po roce pobytu",
         },
         4, 1},
        {"Je na udělení občanství právní nárok?",
         {
          "Ano, po týdnu pobytu",
          "Ne, stát ho uděluje po splnění podmínek",
          "Ano, každému obyvateli EU",
          "Ano, kdo složí maturitu",
         },
         4, 1},
        {"Co se při udělení obvykle požaduje?",
         {
          "Jen fotografii",
          "Pobyt, bezúhonnost a znalost češtiny",
          "Vlastnictví bytu",
          "Členství v politické straně",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Rozhoduje občanství rodiče, ne samo místo narození",
        "Platí původ po rodičích, ne místo narození",
        "Udělení je rozhodnutí státu",
        "Podmínky chrání, aby šlo o skutečný vztah ke státu",
    };
    return build_on2_mcq_page(1, "on2unit2", "on2_ex2_title",
                             "on2_quiz2_head", qs, hints, 4);
}
static GtkWidget *build_on2_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Výčet", "Které symboly Česká republika má",
            "Tip: hymnou je oficiálně jen první sloka.",
            {
                "Zákon uvádí velký a malý státní znak, státní barvy, vlajku a vlajku prezidenta.",
                "Patří k nim také státní pečeť a státní hymna.",
                "Státní barvy jsou bílá, červená a modrá.",
                "Hymnou je první sloka písně Kde domov můj.",
                NULL,
            },
        },
        {
            "2 / 2   •   Úcta", "K čemu symboly jsou",
            "Tip: velký znak spojuje Čechy, Moravu a Slezsko.",
            {
                "Symboly představují stát a lidé jim mají prokazovat úctu.",
                "Velký znak nese českého lva a moravskou a slezskou orlici.",
                "Vlajka je bílá a červená s modrým klínem u žerdi.",
                "Užívají se podle zákona, ne jako libovolná ozdoba.",
                NULL,
            },
        },
    };

    return build_on2_unit_page(&on2_lessons[2], "on2_unit3", "on2_unit3_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on2_unit3_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Která píseň je státní hymna?",
         {
          "První sloka Kde domov můj",
          "Celá píseň včetně druhé sloky jako povinný text",
          "Hej, Slované",
          "Ódu na radost",
         },
         4, 0},
        {"Jaké jsou státní barvy?",
         {
          "Červená, bílá a zelená",
          "Bílá, červená a modrá",
          "Modrá a žlutá",
          "Černá a zlatá",
         },
         4, 1},
        {"Co spojuje velký státní znak?",
         {
          "Jen Prahu a Brno",
          "Znaky Čech, Moravy a Slezska",
          "Vlajky sousedních států",
          "Znak Evropské unie",
         },
         4, 1},
        {"Patří státní pečeť mezi státní symboly?",
         {
          "Ne, je to jen razítko úřadu",
          "Ano",
          "Jen když ji použije obec",
          "Ne, zrušila ji ústava",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Zákon stanoví první sloku",
        "Bílá, červená a modrá",
        "Lev a dvě orlice",
        "Pečeť je v zákonném výčtu symbolů",
    };
    return build_on2_mcq_page(2, "on2unit3", "on2_ex3_title",
                             "on2_quiz3_head", qs, hints, 4);
}
static GtkWidget *build_on2_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Pořádek", "Nejvyšší pravidla státu",
            "Tip: Listina je součást ústavního pořádku, ne obyčejný zákon.",
            {
                "Ústava je základní zákon a běžný zákon jí nesmí odporovat.",
                "Ústavní pořádek tvoří Ústava, Listina základních práv a svobod a další ústavní zákony.",
                "Ústava České republiky platí od 1. ledna 1993.",
                "Popisuje území, moc a základní pravidla politického systému.",
                NULL,
            },
        },
        {
            "2 / 2   •   Změna", "Proč se ústava mění obtížně",
            "Tip: rigidní ústava chrání pravidla před chvilkovou většinou.",
            {
                "Ústava se mění obtížněji než obyčejný zákon.",
                "Ústavní zákon potřebuje tři pětiny všech poslanců.",
                "V Senátu potřebuje tři pětiny přítomných senátorů.",
                "Listina vedle ústavy chrání práva člověka a občana.",
                NULL,
            },
        },
    };

    return build_on2_unit_page(&on2_lessons[3], "on2_unit4", "on2_unit4_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on2_unit4_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je ústava?",
         {
          "Volební program vlády",
          "Základní zákon, kterému nesmí odporovat běžné zákony",
          "Jednací řád školy",
          "Rozpočet na jeden rok",
         },
         4, 1},
        {"Je Listina základních práv a svobod součást ústavního pořádku?",
         {
          "Ne, je to doporučení",
          "Ano",
          "Jen pro cizince",
          "Jen do roku 1993",
         },
         4, 1},
        {"Jaká většina je potřeba k ústavnímu zákonu?",
         {
          "Nadpoloviční většina přítomných poslanců",
          "Tři pětiny všech poslanců a tři pětiny přítomných senátorů",
          "Souhlas prezidenta stačí",
          "Jednomyslnost vlády",
         },
         4, 1},
        {"Od kdy ústava České republiky platí?",
         {
          "Od roku 1918",
          "Od 1. ledna 1993",
          "Od vstupu do Evropské unie",
          "Od roku 1960",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Ústava stojí nad obyčejnými zákony",
        "Listina má sílu ústavního zákona",
        "Ústava je rigidní",
        "Platí od vzniku samostatné České republiky",
    };
    return build_on2_mcq_page(3, "on2unit4", "on2_ex4_title",
                             "on2_quiz4_head", qs, hints, 4);
}
static GtkWidget *build_on2_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Složky", "Tři moci vedle sebe",
            "Tip: soudce nemá být podřízený ministrovi.",
            {
                "Moc se dělí, aby ji jeden orgán nemohl užívat bez kontroly.",
                "Zákonodárná moc dává zákony a v České republice ji má Parlament.",
                "Výkonná moc zákony provádí: patří k ní vláda a prezident.",
                "Soudní moc rozhoduje spory a je na ostatních složkách nezávislá.",
                NULL,
            },
        },
        {
            "2 / 2   •   Rovnováha", "Brzdy mezi složkami",
            "Tip: kontrola moci je součást demokracie, ne překážka.",
            {
                "Složky se navzájem omezují, tomu se říká brzdy a rovnováhy.",
                "Prezident může zákon vrátit a Sněmovna může vyslovit vládě nedůvěru.",
                "Ústavní soud hlídá, aby právo drželo ústavní pořádek.",
                "Nejvyšší kontrolní úřad kontroluje hospodaření se státním majetkem a Česká národní banka pečuje o cenovou stabilitu.",
                NULL,
            },
        },
    };

    return build_on2_unit_page(&on2_lessons[4], "on2_unit5", "on2_unit5_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on2_unit5_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Proč se státní moc dělí?",
         {
          "Aby bylo více ministerstev",
          "Aby ji nikdo neužíval bez kontroly",
          "Aby soudil prezident",
          "Aby zákony psala armáda",
         },
         4, 1},
        {"Kdo vykonává zákonodárnou moc?",
         {
          "Vláda",
          "Parlament",
          "Ústavní soud",
          "Hejtmani",
         },
         4, 1},
        {"Kdo patří k výkonné moci?",
         {
          "Jen Senát",
          "Vláda a prezident",
          "Jen obecní zastupitelstva",
          "Jen advokáti",
         },
         4, 1},
        {"Co dělá Ústavní soud?",
         {
          "Schvaluje státní rozpočet",
          "Hlídá soulad s ústavním pořádkem",
          "Řídí policii",
          "Jmenuje starosty",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Dělba brání soustředění moci",
        "Zákony dává Parlament",
        "Výkonná moc zákony provádí",
        "Ústavní soud není další komora Parlamentu",
    };
    return build_on2_mcq_page(4, "on2unit5", "on2_ex5_title",
                             "on2_quiz5_head", qs, hints, 4);
}
static GtkWidget *build_on2_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Komory", "Sněmovna a Senát",
            "Tip: senátor má delší mandát než poslanec.",
            {
                "Parlament tvoří Poslanecká sněmovna a Senát.",
                "Sněmovna má 200 poslanců volených na čtyři roky.",
                "Senát má 81 senátorů volených na šest let a každé dva roky se obmění třetina.",
                "Poslancem lze být od 21 let, senátorem od 40 let.",
                NULL,
            },
        },
        {
            "2 / 2   •   Zákon", "Jak vzniká obyčejný zákon",
            "Tip: kraj smí podat návrh zákona.",
            {
                "Návrh může podat poslanec, Senát, vláda nebo zastupitelstvo kraje.",
                "Návrh schvaluje Poslanecká sněmovna.",
                "Senát ho může vrátit nebo zamítnout, Sněmovna může senátní nesouhlas přehlasovat.",
                "Prezident zákon podepíše, nebo ho vrátí; i jeho veto může Sněmovna přehlasovat.",
                NULL,
            },
        },
    };

    return build_on2_unit_page(&on2_lessons[5], "on2_unit6", "on2_unit6_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on2_unit6_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kolik poslanců má Poslanecká sněmovna?",
         {
          "81",
          "200",
          "150",
          "26",
         },
         4, 1},
        {"Jak dlouhý je mandát senátora?",
         {
          "Dva roky",
          "Čtyři roky",
          "Šest let",
          "Do odvolání vládou",
         },
         4, 2},
        {"Od kolika let lze být zvolen poslancem?",
         {
          "Od 18 let",
          "Od 21 let",
          "Od 40 let",
          "Od 35 let",
         },
         4, 1},
        {"Kdo smí podat návrh zákona?",
         {
          "Jen prezident",
          "Poslanec, Senát, vláda nebo kraj",
          "Jen Ústavní soud",
          "Každý občan přímo do Sbírky",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Sněmovna má dvě stě poslanců",
        "Senát se obměňuje po třetinách",
        "Do Sněmovny od 21, do Senátu od 40",
        "Návrh zákona je v ústavě vyhrazen určeným subjektům",
    };
    return build_on2_mcq_page(5, "on2unit6", "on2_ex6_title",
                             "on2_quiz6_head", qs, hints, 4);
}
static GtkWidget *build_on2_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Hlava státu", "Prezident republiky",
            "Tip: prezident se volí přímo.",
            {
                "Prezident je hlava státu a volí se přímo na pět let.",
                "Stejný člověk může úřad vykonávat nejvýše dvě období po sobě.",
                "Kandidát musí mít nejméně 40 let.",
                "Zastupuje stát navenek, jmenuje vládu, je vrchním velitelem ozbrojených sil a může udělit milost.",
                NULL,
            },
        },
        {
            "2 / 2   •   Vláda", "Vrchol výkonné moci",
            "Tip: vládě předsedá premiér, ne prezident.",
            {
                "Vládu tvoří předseda, místopředsedové a ministři.",
                "Je vrcholným orgánem výkonné moci.",
                "Odpovídá Poslanecké sněmovně, která jí může vyslovit nedůvěru.",
                "Po jmenování žádá Sněmovnu o důvěru a řídí ministerstva.",
                NULL,
            },
        },
    };

    return build_on2_unit_page(&on2_lessons[6], "on2_unit7", "on2_unit7_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on2_unit7_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak se volí prezident republiky?",
         {
          "Parlamentem na doživotí",
          "Přímo občany, na pět let",
          "Vládou na dva roky",
          "Senátem na deset let",
         },
         4, 1},
        {"Kolikrát za sebou může být člověk prezidentem?",
         {
          "Jen jednou",
          "Nejvýše dvakrát",
          "Bez omezení",
          "Třikrát",
         },
         4, 1},
        {"Komu je odpovědná vláda?",
         {
          "Senátu",
          "Poslanecké sněmovně",
          "Ústavnímu soudu",
          "Hejtmanům",
         },
         4, 1},
        {"Kdo vládě předsedá?",
         {
          "Prezident",
          "Předseda vlády",
          "Předseda Senátu",
          "Guvernér České národní banky",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Přímá volba od roku 2013, mandát pět let",
        "Ústava zakazuje třetí období v řadě",
        "Sněmovna může vyslovit nedůvěru",
        "Premiér není totéž co hlava státu",
    };
    return build_on2_mcq_page(6, "on2unit7", "on2_ex7_title",
                             "on2_quiz7_head", qs, hints, 4);
}
static GtkWidget *build_on2_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Správa", "Stát a územní celky",
            "Tip: samospráva není totéž co ministerstvo.",
            {
                "Veřejná správa je správa veřejných záležitostí.",
                "Státní správu vykonávají jménem státu třeba ministerstva a další úřady.",
                "Samospráva patří obcím a krajům: rozhodují o svých věcech.",
                "Obec a kraj mají vlastní působnost a plní i úkoly, které jim svěří stát.",
                NULL,
            },
        },
        {
            "2 / 2   •   Obec a kraj", "Kdo stojí v čele",
            "Tip: hejtman stojí v čele kraje, starosta v čele obce.",
            {
                "V obci je volené zastupitelstvo, rada a starosta.",
                "V kraji je zastupitelstvo, rada a hejtman.",
                "Zastupitelstvo je nejvyšší orgán územní samosprávy.",
                "Praha je hlavní město a má postavení obce i kraje.",
                NULL,
            },
        },
    };

    return build_on2_unit_page(&on2_lessons[7], "on2_unit8", "on2_unit8_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on2_unit8_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je územní samospráva?",
         {
          "Řízení ministerstva z Prahy",
          "Rozhodování obce a kraje o vlastních záležitostech",
          "Jen práce policie",
          "Jen soudní řízení",
         },
         4, 1},
        {"Kdo stojí v čele obce?",
         {
          "Hejtman",
          "Starosta",
          "Ministr vnitra",
          "Senátor za okres",
         },
         4, 1},
        {"Kdo stojí v čele kraje?",
         {
          "Starosta",
          "Hejtman",
          "Prezident",
          "Ředitel školy",
         },
         4, 1},
        {"Jaké postavení má Praha?",
         {
          "Je jen městská část",
          "Je obec a zároveň kraj",
          "Není samospráva",
          "Řídí ji přímo vláda bez zastupitelstva",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Samospráva patří obci a kraji",
        "Obec vede starosta",
        "Kraj vede hejtman",
        "Hlavní město je obec i kraj",
    };
    return build_on2_mcq_page(7, "on2unit8", "on2_ex8_title",
                             "on2_quiz8_head", qs, hints, 4);
}
static GtkWidget *build_on2_unit9_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Hodnoty", "Od čeho se demokracie pozná",
            "Tip: většina nesmí menšině vzít základní práva.",
            {
                "V demokracii veřejná moc pochází od lidu.",
                "Patří k ní svobodné volby, vláda většiny a ochrana menšiny.",
                "Dál právní stát, dělba moci a soutěž více politických stran.",
                "Svoboda člověka jde ruku v ruce s odpovědností vůči druhým.",
                NULL,
            },
        },
        {
            "2 / 2   •   Ohrožení", "Co demokracii podrývá",
            "Tip: korupce je zneužití postavení k soukromému prospěchu.",
            {
                "Škodí jí korupce, násilí a neodpovědnost.",
                "Škodí i snaha zrušit kontrolu moci a pravidla soutěže.",
                "Demokracie není vláda bez pravidel.",
                "Občan ji chrání účastí, ne lhostejností.",
                NULL,
            },
        },
    };

    return build_on2_unit_page(&on2_lessons[8], "on2_unit9", "on2_unit9_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on2_unit9_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Od koho pochází veřejná moc v demokracii?",
         {
          "Od armády",
          "Od lidu",
          "Od nejbohatších firem",
          "Od jednoho vůdce",
         },
         4, 1},
        {"Chrání demokracie jen vítěznou většinu?",
         {
          "Ano, menšina nemá práva",
          "Ne, chrání i základní práva menšiny",
          "Menšinu chrání jen v obci",
          "Práva platí jen ve volební den",
         },
         4, 1},
        {"Co je korupce?",
         {
          "Svobodná volba",
          "Zneužití postavení k soukromému prospěchu",
          "Kritika vlády",
          "Práce zastupitelstva",
         },
         4, 1},
        {"Co k demokracii patří?",
         {
          "Jedna povolená strana",
          "Svobodné volby a právní stát",
          "Moc bez soudů",
          "Zákaz menšinových názorů",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Lid je zdroj moci",
        "Vláda většiny má mez v právech menšiny",
        "Korupce demokracii ohrožuje",
        "Volby a právo drží demokracii pohromadě",
    };
    return build_on2_mcq_page(8, "on2unit9", "on2_ex9_title",
                             "on2_quiz9_head", qs, hints, 4);
}
static GtkWidget *build_on2_unit10_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Listina", "Práva člověka a občana",
            "Tip: stát práva nedaruje podle nálady.",
            {
                "Lidská práva má člověk proto, že je člověk.",
                "Listina chrání základní svobody, politická práva, práva menšin i hospodářská, sociální a kulturní práva.",
                "Patří sem třeba život, osobní svoboda, vlastnictví, volební právo a vzdělání.",
                "Samostatná hlava chrání právo domáhat se svých práv u nezávislého soudu.",
                NULL,
            },
        },
        {
            "2 / 2   •   Meze", "Ochrana a hranice svobody",
            "Tip: svoboda projevu nechrání pomluvu ani navádění k násilí.",
            {
                "Práva lze omezit jen zákonem a jen z vážných důvodů, třeba ochrany druhých.",
                "Ohrožená práva se hájí u soudu a lze se obrátit i na Ústavní soud.",
                "Ve světě je připomíná Všeobecná deklarace lidských práv a evropská úmluva.",
                "Právo jednoho končí tam, kde začíná stejně vážené právo druhého.",
                NULL,
            },
        },
    };

    return build_on2_unit_page(&on2_lessons[9], "on2_unit10", "on2_unit10_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on2_unit10_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Proč má člověk lidská práva?",
         {
          "Protože je člověk",
          "Jen když vyhraje volby",
          "Jen jako odměnu státu",
          "Jen do 18 let",
         },
         4, 0},
        {"Kde jsou v České republice ústavně zakotvena?",
         {
          "Jen v jednacím řádu Sněmovny",
          "V Listině základních práv a svobod",
          "Jen v občanském zákoníku",
          "V obecní vyhlášce",
         },
         4, 1},
        {"Lze lidská práva omezit libovolně?",
         {
          "Ano, rozhodnutím starosty",
          "Ne, jen zákonem a z vážných důvodů",
          "Ano, většinou v anketě",
          "Ne, nelze je omezit nikdy a v ničem",
         },
         4, 1},
        {"Kam se obrátit, jsou-li práva ohrožena?",
         {
          "Na nikoho, práva se nehájí",
          "Třeba k soudu",
          "Jen na politickou stranu",
          "Jen do novin",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Práva nejsou milost úřadu",
        "Listina je součást ústavního pořádku",
        "Omezení má zákonný důvod",
        "Soudní ochrana je součást Listiny",
    };
    return build_on2_mcq_page(9, "on2unit10", "on2_ex10_title",
                             "on2_quiz10_head", qs, hints, 4);
}
static GtkWidget *build_on2_unit11_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Dítě", "Úmluva o právech dítěte",
            "Tip: dítětem je člověk mladší 18 let.",
            {
                "Dítě má stejnou důstojnost jako dospělý.",
                "Úmluva zdůrazňuje zájem dítěte, právo na život, vzdělání a ochranu před násilím.",
                "Dítě má právo být slyšeno ve věcech, které se ho týkají.",
                "Rodiče i stát mají dítě chránit.",
                NULL,
            },
        },
        {
            "2 / 2   •   Ochránce", "Veřejný ochránce práv",
            "Tip: ombudsman není soudce a neruší rozsudky.",
            {
                "Ombudsman chrání lidi před pochybením a nečinností úřadů.",
                "Může věc prošetřit a žádat nápravu.",
                "Nerozhoduje místo soudu a neukládá tresty.",
                "Obrátit se na něj může člověk, který si neví rady s úřadem.",
                NULL,
            },
        },
    };

    return build_on2_unit_page(&on2_lessons[10], "on2_unit11", "on2_unit11_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on2_unit11_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo je podle Úmluvy o právech dítěte dítě?",
         {
          "Každý žák střední školy do maturity",
          "Člověk mladší 18 let",
          "Jen občan České republiky",
          "Jen ten, kdo žije s rodiči",
         },
         4, 1},
        {"Co úmluva zvlášť zdůrazňuje?",
         {
          "Povinnost dítěte volit",
          "Život, vzdělání a ochranu před násilím",
          "Právo řídit obec",
          "Povinnou práci od 14 let",
         },
         4, 1},
        {"Co dělá veřejný ochránce práv?",
         {
          "Prošetřuje pochybení úřadů",
          "Ruší rozsudky trestních soudů",
          "Jmenuje ministry",
          "Schvaluje zákony",
         },
         4, 0},
        {"Může ombudsman uložit trest?",
         {
          "Ano, jako soudce",
          "Ne",
          "Ano, pokutu do milionu",
          "Ano, pokud jde o dítě",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Hranicí je 18 let",
        "Zájem dítěte a ochrana",
        "Ombudsman není soud",
        "Tresty ukládají soudy a správní orgány, ne ombudsman",
    };
    return build_on2_mcq_page(10, "on2unit11", "on2_ex11_title",
                             "on2_quiz11_head", qs, hints, 4);
}
static GtkWidget *build_on2_unit12_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Směry", "Čtyři klasické postoje",
            "Tip: ideologie není totéž co jedna konkrétní strana.",
            {
                "Ideologie je soustava názorů na to, jak má vypadat společnost a stát.",
                "Liberalismus staví na svobodě jednotlivce a na omezené moci státu.",
                "Konzervatismus brání osvědčené pořádky a změny chce postupné.",
                "Socialismus zdůrazňuje rovnost a silnější roli státu v hospodářství.",
                NULL,
            },
        },
        {
            "2 / 2   •   Mez", "Další směry a co demokracie nepřipouští",
            "Tip: fašismus a nacismus jsou s ústavním pořádkem neslučitelné.",
            {
                "Nacionalismus staví národ na první místo a může být umírněný, nebo nepřátelský k jiným.",
                "Environmentalismus klade důraz na ochranu přírody, feminismus na rovnost žen a mužů.",
                "Anarchismus odmítá stát jako takový.",
                "Fašismus a nacismus popírají demokracii a rovnost lidí.",
                NULL,
            },
        },
    };

    return build_on2_unit_page(&on2_lessons[11], "on2_unit12", "on2_unit12_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on2_unit12_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co zdůrazňuje liberalismus?",
         {
          "Vládu jednoho vůdce",
          "Svobodu jednotlivce",
          "Zrušení voleb",
          "Národ jako jedinou hodnotu",
         },
         4, 1},
        {"Co je typické pro konzervatismus?",
         {
          "Změna za každou cenu",
          "Postupná změna a úcta k osvědčeným pořádkům",
          "Odmítnutí státu",
          "Zákaz tradice",
         },
         4, 1},
        {"Jsou fašismus a nacismus slučitelné s demokracií?",
         {
          "Ano, jsou jen další strany",
          "Ne, popírají rovnost a demokratická pravidla",
          "Ano, pokud vyhrají volby",
          "Záleží na vlajce",
         },
         4, 1},
        {"Co je politická ideologie?",
         {
          "Jednací řád úřadu",
          "Soustava názorů na uspořádání společnosti",
          "Státní symbol",
          "Druh daně",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Liberalismus hlídá svobodu a meze státu",
        "Konzervatismus nespěchá se zlomy",
        "Ústavní pořádek je nepřipouští",
        "Ideologie je názorový celek, ne úřad",
    };
    return build_on2_mcq_page(11, "on2unit12", "on2_ex12_title",
                             "on2_quiz12_head", qs, hints, 4);
}
static GtkWidget *build_on2_unit13_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Strany", "K čemu je politická strana",
            "Tip: občan se dívá na program, ne jen na heslo.",
            {
                "Strana sdružuje lidi s podobným programem a uchází se o hlasy.",
                "V demokracii soutěží více stran a žádná nemá moc natrvalo zaručenou.",
                "Strany se v čase mění, proto je důležitější program než naučený seznam názvů.",
                "Jedna strana potřebuje ke vstupu do Sněmovny alespoň 5 procent hlasů.",
                NULL,
            },
        },
        {
            "2 / 2   •   Volby", "Právo volit a být volen",
            "Tip: kdo nevolí, nechává rozhodnutí na ostatních.",
            {
                "Aktivní volební právo je právo volit, pasivní je právo být volen.",
                "Do Sněmovny se volí poměrným systémem, do Senátu většinovým ve dvou kolech.",
                "Prezident se volí přímo, nejvýše ve dvou kolech.",
                "Do Parlamentu volí občané České republiky od 18 let; do obecního zastupitelstva mohou volit i občané EU s trvalým pobytem v obci.",
                NULL,
            },
        },
    };

    return build_on2_unit_page(&on2_lessons[12], "on2_unit13", "on2_unit13_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on2_unit13_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"K čemu je politická strana?",
         {
          "Nahrazuje soud",
          "Sdružuje lidi s programem a uchází se o hlasy",
          "Vybírá daně",
          "Jmenuje soudce",
         },
         4, 1},
        {"Jak se volí Poslanecká sněmovna?",
         {
          "Losem",
          "Poměrným systémem",
          "Jen většinově v jednom kole jako Senát",
          "Vládou",
         },
         4, 1},
        {"Jak se volí Senát?",
         {
          "Poměrně jednou kandidátkou",
          "Většinově ve dvou kolech",
          "Prezidentem",
          "Stejně jako evropský parlament",
         },
         4, 1},
        {"Od kolika let smí občan ČR volit do Parlamentu?",
         {
          "Od 15 let",
          "Od 18 let",
          "Od 21 let",
          "Od 40 let",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Strana soutěží ve volbách",
        "Hlasy se ve Sněmovně rozdělují poměrně",
        "Senátní obvody jsou většinové",
        "Aktivní právo je od 18 let",
    };
    return build_on2_mcq_page(12, "on2unit13", "on2_ex13_title",
                             "on2_quiz13_head", qs, hints, 4);
}
static GtkWidget *build_on2_unit14_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Spolky", "Lidé vedle státu",
            "Tip: spolek není úřad.",
            {
                "Občanská společnost jsou lidé, kteří se sdružují nezávisle na státu.",
                "Patří sem spolky, odbory, nadační organizace i místní iniciativy.",
                "Není to úřad a není to totéž co politická strana, i když s nimi může jednat.",
                "Hlídá moc, pomáhá a přináší témata, která stát sám nevidí.",
                NULL,
            },
        },
        {
            "2 / 2   •   Ctnosti", "Co demokracie potřebuje od lidí",
            "Tip: ctnost není slepá poslušnost.",
            {
                "Patří sem odpovědnost, tolerance a respekt k právu.",
                "Patří sem i ochota domluvit se a solidarita.",
                "Občanská ctnost demokracii nepodrývá.",
                "Angažovat se lze ve spolku, v obci i tím, že člověk neničí společné věci.",
                NULL,
            },
        },
    };

    return build_on2_unit_page(&on2_lessons[13], "on2_unit14", "on2_unit14_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on2_unit14_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je občanská společnost?",
         {
          "Soubor ministerstev",
          "Sdružování lidí nezávisle na státu",
          "Jen Poslanecká sněmovna",
          "Armáda",
         },
         4, 1},
        {"Je spolek státní úřad?",
         {
          "Ano",
          "Ne",
          "Ano, pokud má stanovy",
          "Jen v kraji",
         },
         4, 1},
        {"Která vlastnost demokracii pomáhá?",
         {
          "Lhostejnost",
          "Odpovědnost a tolerance",
          "Pohrdání právem",
          "Nenávist k menšině",
         },
         4, 1},
        {"Je slepá poslušnost občanská ctnost?",
         {
          "Ano, občan se neptá",
          "Ne",
          "Ano, pokud to řekne úřad",
          "Jen ve škole",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Žije vedle státu, ne místo úřadů",
        "Spolek je soukromé sdružení",
        "Ctnosti drží soužití",
        "Ctnost zahrnuje úsudek a respekt k právu",
    };
    return build_on2_mcq_page(13, "on2unit14", "on2_ex14_title",
                             "on2_quiz14_head", qs, hints, 4);
}
static GtkWidget *build_on2_unit15_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Pojmy", "Není to totéž",
            "Tip: kritika vlády ještě není extremismus.",
            {
                "Radikální názor chce výraznou změnu a sám o sobě ještě nemusí rušit demokracii.",
                "Extremismus popírá rovnost lidí nebo demokratická pravidla a často sahá k nenávisti.",
                "Terorismus je násilí proti nezúčastněným, aby společnost dostala strach a politicky ustoupila.",
                "Nesouhlas s vládou se v demokracii řeší argumentem a volbami.",
                NULL,
            },
        },
        {
            "2 / 2   •   Mez", "Proč to demokracii ohrožuje",
            "Tip: symbol nenávistného hnutí není nevinný vtip.",
            {
                "Extremismus bere části lidí bezpečí a rovné postavení.",
                "Propagace hnutí, které směřuje k potlačení práv a svobod, je nepřijatelná a může být trestná.",
                "Extremistická symbolika na takové hnutí odkazuje.",
                "Když parta tlačí k nenávisti, pomoc je u dospělého, ve škole nebo u policie.",
                NULL,
            },
        },
    };

    return build_on2_unit_page(&on2_lessons[14], "on2_unit15", "on2_unit15_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on2_unit15_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Je každá ostrá kritika vlády extremismus?",
         {
          "Ano",
          "Ne",
          "Ano, pokud je v novinách",
          "Ano, pokud kritik nevolil",
         },
         4, 1},
        {"Co je terorismus?",
         {
          "Pokojná demonstrace",
          "Násilí proti nezúčastněným s cílem nahnat strach",
          "Volební kampaň",
          "Žaloba k soudu",
         },
         4, 1},
        {"Proč není extremistická symbolika nevinná?",
         {
          "Protože je drahá",
          "Odkazuje na hnutí, která popírají práva a demokracii",
          "Protože ji zakazuje škola bez zákona",
          "Protože nepatří do módy",
         },
         4, 1},
        {"Kam se obrátit, když skupina tlačí k nenávisti?",
         {
          "Hlouběji do té skupiny",
          "Na dospělého, školu nebo policii",
          "Nikam, je to soukromá věc strany",
          "Na autora symbolu s prosbou o radu",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Kritika patří k demokracii",
        "Teror cílí na strach veřejnosti",
        "Symbol nese program nenávisti",
        "Pomoc je mimo tu skupinu",
    };
    return build_on2_mcq_page(14, "on2unit15", "on2_ex15_title",
                             "on2_quiz15_head", qs, hints, 4);
}
static GtkWidget *build_on2_unit16_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Úloha", "Tisk, vysílání a internet",
            "Tip: bez informací se moc špatně kontroluje.",
            {
                "Média jsou tisk, televize, rozhlas i internet.",
                "Mají informovat, kontrolovat moc, komentovat a také bavit.",
                "Svobodný přístup k informacím je pro demokracii nutný.",
                "Veřejnoprávní médium má sloužit veřejnosti, soukromé žije i z reklamy a rozhodnutí vlastníků.",
                NULL,
            },
        },
        {
            "2 / 2   •   Kritika", "Jak se nenechat zmást",
            "Tip: jedna sdílená zpráva ještě není důkaz.",
            {
                "Zpráva, komentář a reklama nejsou totéž.",
                "Ptáme se, kdo informaci vydal a jaký k ní má zájem.",
                "Ověření znamená podívat se i do dalšího zdroje.",
                "Svoboda projevu má meze: pomluvu a navádění k násilí nechrání.",
                NULL,
            },
        },
    };

    return build_on2_unit_page(&on2_lessons[15], "on2_unit16", "on2_unit16_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on2_unit16_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"K čemu jsou média v demokracii?",
         {
          "Jen k reklamě",
          "Informovat a pomáhat kontrolovat moc",
          "Nahrazovat soudy",
          "Schvalovat zákony",
         },
         4, 1},
        {"Je komentář totéž co zpráva?",
         {
          "Ano",
          "Ne, komentář je názor",
          "Ano, pokud má titulek",
          "Zpráva je vždy reklama",
         },
         4, 1},
        {"Jak si ověřit informaci?",
         {
          "Stačí počet sdílení",
          "Podívat se na zdroj, zájem autora a další zdroj",
          "Věřit nejhlasitějšímu titulku",
          "Zeptat se jen toho, kdo ji poslal",
         },
         4, 1},
        {"Chrání svoboda projevu navádění k násilí?",
         {
          "Ano, bez výjimky",
          "Ne",
          "Ano, na internetu",
          "Ano, pokud jde o politiku",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Informace a kontrola",
        "Fakt a názor se rozlišují",
        "Kritický přístup srovnává zdroje",
        "Svoboda projevu má zákonné meze",
    };
    return build_on2_mcq_page(15, "on2unit16", "on2_ex16_title",
                             "on2_quiz16_head", qs, hints, 4);
}

void add_on2_pages(GtkStack *stack) {
    typedef GtkWidget *(*OnBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        OnBuilder build_unit;
        OnBuilder build_ex;
    } pages[] = {
        {"on2unit1", "on2ex1", build_on2_unit1_page, build_on2_unit1_exercise_page},
        {"on2unit2", "on2ex2", build_on2_unit2_page, build_on2_unit2_exercise_page},
        {"on2unit3", "on2ex3", build_on2_unit3_page, build_on2_unit3_exercise_page},
        {"on2unit4", "on2ex4", build_on2_unit4_page, build_on2_unit4_exercise_page},
        {"on2unit5", "on2ex5", build_on2_unit5_page, build_on2_unit5_exercise_page},
        {"on2unit6", "on2ex6", build_on2_unit6_page, build_on2_unit6_exercise_page},
        {"on2unit7", "on2ex7", build_on2_unit7_page, build_on2_unit7_exercise_page},
        {"on2unit8", "on2ex8", build_on2_unit8_page, build_on2_unit8_exercise_page},
        {"on2unit9", "on2ex9", build_on2_unit9_page, build_on2_unit9_exercise_page},
        {"on2unit10", "on2ex10", build_on2_unit10_page, build_on2_unit10_exercise_page},
        {"on2unit11", "on2ex11", build_on2_unit11_page, build_on2_unit11_exercise_page},
        {"on2unit12", "on2ex12", build_on2_unit12_page, build_on2_unit12_exercise_page},
        {"on2unit13", "on2ex13", build_on2_unit13_page, build_on2_unit13_exercise_page},
        {"on2unit14", "on2ex14", build_on2_unit14_page, build_on2_unit14_exercise_page},
        {"on2unit15", "on2ex15", build_on2_unit15_page, build_on2_unit15_exercise_page},
        {"on2unit16", "on2ex16", build_on2_unit16_page, build_on2_unit16_exercise_page}
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
