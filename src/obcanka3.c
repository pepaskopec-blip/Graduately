#include "graduately.h"

/* Year 3 of Občanská nauka.
 * Thematic unit "Člověk a právo" from the MŠMT model syllabus
 * (č.j. 18 396/2002-23) and the SOV civic framework, which schools place
 * in the third year. The economy, the world and philosophy are year 4. */

#define ON3_ENTRY(n) \
    { .n_slides = 2, .unit_page = "on3unit" #n, .ex_page = "on3ex" #n }

NetLesson on3_lessons[ON3_LESSONS] = {
    ON3_ENTRY(1),
    ON3_ENTRY(2),
    ON3_ENTRY(3),
    ON3_ENTRY(4),
    ON3_ENTRY(5),
    ON3_ENTRY(6),
    ON3_ENTRY(7),
    ON3_ENTRY(8),
    ON3_ENTRY(9),
    ON3_ENTRY(10),
    ON3_ENTRY(11),
    ON3_ENTRY(12),
    ON3_ENTRY(13),
    ON3_ENTRY(14),
    ON3_ENTRY(15),
    ON3_ENTRY(16),
};

#undef ON3_ENTRY

GtkWidget *on3_scroll;
GtkWidget *on3_fixed;
GtkWidget *on3_rail;
GtkWidget *on3_nodes[ON3_UNITS];
GtkWidget *on3_labels[ON3_UNITS];
double on3_cx[ON3_UNITS];
double on3_cy[ON3_UNITS];
int on3_cols = 1;
int on3_rows = 1;
int on3_cw = (int)(2.0 * ROAD_MX + (ON3_UNITS - 1) * PATH_SPAC);
int on3_ch = (int)(2.0 * ROAD_MY);
guint on3_idle;
double on3_last_avail = -1.0;
int on3_last_cols = -1;
int on3_last_rows = -1;
int on3_last_cw = -1;
int on3_last_ch = -1;
cairo_surface_t *on3_rail_cache;
int on3_cache_w;
int on3_cache_h;

static void on3_point(double t, double *ox, double *oy) {
    int k = (int)t;
    double u = t - (double)k;
    double u2, u3;
    double p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y;

    if (k < 0) { k = 0; u = 0.0; }
    if (k >= ON3_UNITS - 1) { k = ON3_UNITS - 2; u = 1.0; }

    p1x = on3_cx[k];     p1y = on3_cy[k];
    p2x = on3_cx[k + 1]; p2y = on3_cy[k + 1];
    if (k - 1 >= 0) { p0x = on3_cx[k - 1]; p0y = on3_cy[k - 1]; }
    else            { p0x = p1x - (p2x - p1x); p0y = p1y - (p2y - p1y); }
    if (k + 2 < ON3_UNITS) { p3x = on3_cx[k + 2]; p3y = on3_cy[k + 2]; }
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

static void on3_path(cairo_t *cr, double t0, double t1) {
    const double steps_per_unit = 24.0;
    int n = (int)((t1 - t0) * steps_per_unit);
    double x, y;
    int s;

    if (n < 1)
        n = 1;
    on3_point(t0, &x, &y);
    cairo_move_to(cr, x, y);
    for (s = 1; s <= n; s++) {
        on3_point(t0 + (t1 - t0) * (double)s / (double)n, &x, &y);
        cairo_line_to(cr, x, y);
    }
}

static void on3_geometry(double avail) {
    const double phase = 2.0 * G_PI * 1.7 / (double)(ON3_UNITS - 1);
    int rows;
    int r, c, i;

    for (rows = 1; rows <= ON3_UNITS; rows++) {
        int cols = (ON3_UNITS + rows - 1) / rows;
        if (2.0 * ROAD_MX + (cols - 1) * PATH_SPAC <= avail + 1.0)
            break;
    }
    if (rows > ON3_UNITS)
        rows = ON3_UNITS;
    on3_rows = rows;
    on3_cols = (ON3_UNITS + rows - 1) / rows;
    if (on3_cols < 1)
        on3_cols = 1;
    on3_cw = (int)(2.0 * ROAD_MX + (on3_cols - 1) * PATH_SPAC);
    on3_ch = (int)(2.0 * ROAD_MY + (rows - 1) * ROAD_GAP);

    for (r = 0; r < rows; r++) {
        int base = r * on3_cols;
        int len = MIN(on3_cols, ON3_UNITS - base);
        int fwd = (r % 2) == 0;
        for (c = 0; c < len; c++) {
            i = base + c;
            int cc = fwd ? c : (on3_cols - 1 - c);
            on3_cx[i] = ROAD_MX + cc * PATH_SPAC;
            on3_cy[i] = ROAD_MY + r * ROAD_GAP
                       + ROAD_WAVE * sin((double)i * phase);
        }
    }
}

static void on3_apply_layout(void) {
    if (!on3_fixed)
        return;

    if (on3_cols == on3_last_cols && on3_rows == on3_last_rows &&
        on3_cw == on3_last_cw && on3_ch == on3_last_ch)
        return;

    on3_last_cols = on3_cols;
    on3_last_rows = on3_rows;
    on3_last_cw = on3_cw;
    on3_last_ch = on3_ch;
    if (on3_rail_cache) {
        cairo_surface_destroy(on3_rail_cache);
        on3_rail_cache = NULL;
    }

    for (int i = 0; i < ON3_UNITS; i++) {
        if (!on3_nodes[i] || !on3_labels[i])
            continue;
        gtk_fixed_move(GTK_FIXED(on3_fixed), on3_nodes[i],
                       (int)(on3_cx[i] - NODE_SIZE / 2.0),
                       (int)(on3_cy[i] - NODE_SIZE / 2.0));
        gtk_fixed_move(GTK_FIXED(on3_fixed), on3_labels[i],
                       (int)(on3_cx[i] - (PATH_SPAC - 20.0) / 2.0),
                       (int)(on3_cy[i] + NODE_SIZE / 2.0 + 10.0));
    }

    gtk_widget_set_size_request(on3_fixed, on3_cw, on3_ch);
    gtk_widget_set_size_request(on3_rail, on3_cw, on3_ch);
    gtk_fixed_move(GTK_FIXED(on3_fixed), on3_rail, 0, 0);
    gtk_widget_queue_draw(on3_rail);
}

static void on3_relayout(void) {
    GtkAdjustment *hadj;
    double avail;

    if (!on3_scroll)
        return;
    hadj = gtk_scrolled_window_get_hadjustment(
        GTK_SCROLLED_WINDOW(on3_scroll));
    avail = gtk_adjustment_get_page_size(hadj);
    if (avail < 1.0)
        return;
    if (fabs(avail - on3_last_avail) < 1.0)
        return;
    on3_last_avail = avail;
    on3_geometry(avail);
    on3_apply_layout();
}

static gboolean on3_relayout_idle(gpointer data) {
    (void)data;
    on3_idle = 0;
    on3_relayout();
    return G_SOURCE_REMOVE;
}

static void on3_relayout_later(void) {
    if (on3_idle == 0)
        on3_idle = g_idle_add(on3_relayout_idle, NULL);
}

static void on3_adjust_notify(GtkAdjustment *adj, GParamSpec *ps,
                             gpointer data) {
    (void)adj;
    (void)ps;
    (void)data;
    on3_relayout_later();
}

static void on3_render_rail_to(cairo_t *cr) {
    const double t_end = (double)(ON3_UNITS - 1);
    const Rgb rail = color_from_hex(app_theme.rail);
    const Rgb muted = color_from_hex(app_theme.subtext);

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

    cairo_new_path(cr);
    on3_path(cr, 0.0, t_end);
    cairo_set_line_width(cr, 12);
    cairo_set_source_rgb(cr, rail.r, rail.g, rail.b);
    cairo_stroke(cr);

    cairo_set_source_rgba(cr, muted.r, muted.g, muted.b, 0.17);
    cairo_set_dash(cr, (double[]){2.0, 40.0}, 2, 0.0);
    cairo_new_path(cr);
    on3_path(cr, 0.0, t_end);
    cairo_stroke(cr);
    cairo_set_dash(cr, NULL, 0, 0.0);
}

static void on3_draw_rail(GtkDrawingArea *area, cairo_t *cr,
                         int width, int height, gpointer data) {
    (void)area;
    (void)width;
    (void)height;
    (void)data;

    if (!on3_rail_cache || on3_cache_w != on3_cw || on3_cache_h != on3_ch) {
        cairo_t *rcr;

        if (on3_rail_cache)
            cairo_surface_destroy(on3_rail_cache);
        on3_cache_w = on3_cw;
        on3_cache_h = on3_ch;
        if (on3_cache_w < 1)
            on3_cache_w = 1;
        if (on3_cache_h < 1)
            on3_cache_h = 1;
        on3_rail_cache = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                    on3_cache_w, on3_cache_h);
        rcr = cairo_create(on3_rail_cache);
        on3_render_rail_to(rcr);
        cairo_destroy(rcr);
    }

    cairo_set_source_surface(cr, on3_rail_cache, 0, 0);
    cairo_paint(cr);
}

void on3_rail_theme_reset(void) {
    if (on3_rail_cache) {
        cairo_surface_destroy(on3_rail_cache);
        on3_rail_cache = NULL;
    }
    if (on3_rail)
        gtk_widget_queue_draw(on3_rail);
}

static void on3_open_unit(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L)
        return;
    L->idx = 0;
    if (L->stack) {
        char name[16];

        g_snprintf(name, sizeof(name), "on3_slide%u", L->idx);
        gtk_stack_set_visible_child_name(GTK_STACK(L->stack), name);
        if (L->prev_btn)
            gtk_widget_set_sensitive(L->prev_btn, FALSE);
        if (L->next_btn)
            gtk_button_set_label(GTK_BUTTON(L->next_btn), tr("net_slide_next"));
    }
    show_page(L->unit_page, 1);
}

static void on3_add_node(GtkFixed *fixed, int index) {
    int num = index + 1;
    gboolean locked = index >= ON3_LESSONS;
    GtkWidget *card;
    GtkWidget *vbox;
    GtkWidget *number;
    GtkWidget *icon;
    GtkWidget *name;
    char *text;
    static char unit_keys[ON3_LESSONS][16];
    static gboolean unit_keys_ready = FALSE;

    if (!unit_keys_ready) {
        for (int i = 0; i < ON3_LESSONS; i++)
            g_snprintf(unit_keys[i], sizeof unit_keys[i], "on3_unit%d", i + 1);
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
        on3_lessons[index].done_icon = icon;
        g_signal_connect(card, "clicked", G_CALLBACK(on3_open_unit),
                         &on3_lessons[index]);
    }

    on3_nodes[index] = card;
    gtk_fixed_put(fixed, card, 0, 0);

    name = gtk_label_new(NULL);
    gtk_widget_set_size_request(name, (int)(PATH_SPAC - 20.0), -1);
    gtk_widget_set_halign(name, GTK_ALIGN_CENTER);
    gtk_label_set_justify(GTK_LABEL(name), GTK_JUSTIFY_CENTER);
    gtk_label_set_wrap(GTK_LABEL(name), TRUE);
    gtk_widget_add_css_class(name, "unit-name");
    if (index < ON3_LESSONS) {
        i18n_bind(name, unit_keys[index], 0);
    } else {
        gtk_widget_add_css_class(name, "unit-name-locked");
    }
    on3_labels[index] = name;
    gtk_fixed_put(fixed, name, 0, 0);
}
GtkWidget *build_on3map_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *fixed;
    GtkWidget *rail;
    GtkAdjustment *ha;
    GtkAdjustment *va;

    on3_geometry(1000.0);

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("onyears", "on_year3", "on3_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);
    on3_scroll = scroll;

    wrap = gtk_center_box_new();
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), wrap);

    fixed = gtk_fixed_new();
    gtk_widget_set_halign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(fixed, on3_cw, on3_ch);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), fixed);
    on3_fixed = fixed;

    rail = gtk_drawing_area_new();
    gtk_widget_set_size_request(rail, on3_cw, on3_ch);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(rail), on3_draw_rail,
                                   NULL, NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rail, 0, 0);
    on3_rail = rail;

    for (int i = 0; i < ON3_UNITS; i++)
        on3_add_node(GTK_FIXED(fixed), i);

    ha = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scroll));
    va = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));
    g_signal_connect(ha, "notify::page-size",
                     G_CALLBACK(on3_adjust_notify), NULL);
    g_signal_connect(va, "notify::page-size",
                     G_CALLBACK(on3_adjust_notify), NULL);

    on3_apply_layout();
    on3_relayout_later();
    refresh_on3_completion_ui();

    return page;
}

static void on3_slide_apply(NetLesson *L);
static void on3_slide_prev(GtkButton *button, gpointer data);
static void on3_slide_next(GtkButton *button, gpointer data);

static int on3_note_last_w = -1;

static void on3_rescale_notes_at(int w) {
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
    for (int i = 0; i < ON3_LESSONS; i++)
        net_rescale_lesson_notes(&on3_lessons[i], body, head, kick);
}

static gboolean on3_note_tick(GtkWidget *w, GdkFrameClock *clock,
                             gpointer data) {
    int width = gtk_widget_get_allocated_width(w);

    (void)clock;
    (void)data;
    if (width != on3_note_last_w) {
        on3_note_last_w = width;
        on3_rescale_notes_at(width);
    }
    return G_SOURCE_CONTINUE;
}

static GtkWidget *build_on3_unit_page(NetLesson *L, const char *title_key,
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

    gtk_box_append(GTK_BOX(page), top_bar("on3map", title_key, sub_key));

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
        g_snprintf(name, sizeof(name), "on3_slide%u", s);
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

    gtk_widget_add_tick_callback(L->stack, on3_note_tick, NULL, NULL);
    on3_note_last_w = -1;
    on3_rescale_notes_at(0);
    net_notes_target = NULL;

    nav = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(nav), TRUE);
    gtk_widget_set_margin_top(nav, 10);
    gtk_box_append(GTK_BOX(page), nav);

    L->prev_btn = gtk_button_new_with_label(tr("net_slide_prev"));
    gtk_widget_add_css_class(L->prev_btn, "btn-primary");
    g_signal_connect(L->prev_btn, "clicked", G_CALLBACK(on3_slide_prev), L);
    gtk_box_append(GTK_BOX(nav), L->prev_btn);

    L->next_btn = gtk_button_new_with_label(tr("net_slide_next"));
    gtk_widget_add_css_class(L->next_btn, "btn-primary");
    g_signal_connect(L->next_btn, "clicked", G_CALLBACK(on3_slide_next), L);
    gtk_box_append(GTK_BOX(nav), L->next_btn);

    L->idx = 0;
    on3_slide_apply(L);
    return page;
}

static void on3_slide_apply(NetLesson *L) {
    char name[16];

    if (!L || !L->stack)
        return;
    g_snprintf(name, sizeof(name), "on3_slide%u", L->idx);
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

static void on3_slide_prev(GtkButton *button, gpointer data) {
    NetLesson *L = data;
    (void)button;
    if (!L) return;
    if (L->idx > 0) {
        L->idx--;
        on3_slide_apply(L);
    }
}

static void on3_slide_next(GtkButton *button, gpointer data) {
    NetLesson *L = data;
    (void)button;
    if (!L) return;
    if (L->idx + 1 < L->n_slides) {
        L->idx++;
        on3_slide_apply(L);
    } else {
        show_page(L->ex_page, 1);
    }
}

void on3_lessons_apply_lang(void) {
    for (int i = 0; i < ON3_LESSONS; i++)
        on3_slide_apply(&on3_lessons[i]);
}

static void on3_mcq_check(GtkButton *button, gpointer data) {
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
        mark_on3_done(ctx->lesson_id);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry_short"));
    }
}

static GtkWidget *build_on3_mcq_page(int lesson_id, const char *back_page,
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
    g_signal_connect(check, "clicked", G_CALLBACK(on3_mcq_check), ctx);

    ctx->feedback = gtk_label_new("");
    gtk_widget_set_halign(ctx->feedback, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(body), ctx->feedback);

    return page;
}
static GtkWidget *build_on3_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Právo", "Pravidla, která stát vynutí",
            "Tip: ne každé neslušné jednání je nezákonné.",
            {
                "Právo je soubor pravidel, která může stát vynutit.",
                "Morálka říká, co je slušné. Právo říká, co je závazné.",
                "Obojí se často kryje, ale není to totéž.",
                "Bez vynutitelného práva by rozhodovala síla, ne pravidlo.",
                NULL,
            },
        },
        {
            "2 / 2   •   Spravedlnost", "Podobné případy podobně",
            "Tip: spravedlnost je cíl práva, ne jeho automatický výsledek.",
            {
                "Spravedlnost chce, aby se podobné případy posuzovaly podobně.",
                "Právo má chránit slabšího před svévolí silnějšího.",
                "Sankce následuje, když někdo pravidlo poruší.",
                "Třetí ročník bere právní řád, soudy, smlouvy, rodinu, práci a tresty.",
                NULL,
            },
        },
    };

    return build_on3_unit_page(&on3_lessons[0], "on3_unit1", "on3_unit1_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on3_unit1_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je právo?",
         {
          "Jen dobré mravy",
          "Soubor pravidel, která může stát vynutit",
          "Názor většiny na sociálních sítích",
          "Zvyk jedné rodiny",
         },
         4, 1},
        {"Je morálka totéž co právo?",
         {
          "Ano",
          "Ne, často se kryjí, ale nejsou totéž",
          "Morálka je vždy přísnější zákon",
          "Právo je jen náboženství",
         },
         4, 1},
        {"K čemu spravedlnost v právu směřuje?",
         {
          "Aby vyhrál hlasitější",
          "Aby se podobné případy posuzovaly podobně",
          "Aby trest byl vždy nejvyšší",
          "Aby soudil ten, kdo je u moci",
         },
         4, 1},
        {"Co by bez vynutitelného práva rozhodovalo?",
         {
          "Síla",
          "Los",
          "Školní řád",
          "Anketa",
         },
         4, 0},
    };
    static const char *hints[] = {
        "Právo je vynutitelné státní mocí",
        "Slušnost a zákon se nemusí krýt",
        "Stejné se má měřit stejným metrem",
        "Právo má nahradit právo silnějšího",
    };
    return build_on3_mcq_page(0, "on3unit1", "on3_ex1_title",
                             "on3_quiz1_head", qs, hints, 4);
}
static GtkWidget *build_on3_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Vázanost", "I stát má meze",
            "Tip: úřad smí jen to, co mu dovoluje zákon.",
            {
                "V právním státě je právem vázán i stát, ne jen občan.",
                "Úřad smí zasahovat do práv člověka jen tam, kde mu to zákon dovolí.",
                "Moc se dělí, aby ji nikdo neužíval bez kontroly.",
                "Soudy mají být nezávislé na vládě a na politicích.",
                NULL,
            },
        },
        {
            "2 / 2   •   Ochrana", "Kam se člověk obrátí",
            "Tip: právní stát stojí na tom, že se práva dá dovolat.",
            {
                "Člověk se může domáhat ochrany u nezávislého soudu.",
                "Rozhodnutí úřadu má být přezkoumatelné.",
                "Nikdo nesmí být trestán bez zákona, který čin už předtím zakazoval.",
                "Právní stát není stát bez pravidel. Je to stát, který svá pravidla sám dodržuje.",
                NULL,
            },
        },
    };

    return build_on3_unit_page(&on3_lessons[1], "on3_unit2", "on3_unit2_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on3_unit2_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo je v právním státě vázán právem?",
         {
          "Jen občané",
          "I stát",
          "Jen cizinci",
          "Jen firmy",
         },
         4, 1},
        {"Kdy smí úřad zasáhnout do práv člověka?",
         {
          "Kdykoli se mu to hodí",
          "Jen když mu to dovoluje zákon",
          "Když o to požádá soused",
          "Když to schválí starosta ústně",
         },
         4, 1},
        {"Jakou roli mají soudy?",
         {
          "Plnit pokyny ministra",
          "Rozhodovat nezávisle",
          "Psát zákony místo Parlamentu",
          "Vybírat daně",
         },
         4, 1},
        {"Lze trestat čin, který v době spáchání zákon nezakazoval?",
         {
          "Ano, zpětně",
          "Ne",
          "Ano, pokud šlo o přestupek školy",
          "Ano, rozhodnutím vlády",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Právo váže i veřejnou moc",
        "Veřejná moc jedná podle zákona",
        "Nezávislost soudů je součást právního státu",
        "Nullum crimen sine lege",
    };
    return build_on3_mcq_page(1, "on3unit2", "on3_ex2_title",
                             "on3_quiz2_head", qs, hints, 4);
}
static GtkWidget *build_on3_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Pyramida", "Co stojí nad čím",
            "Tip: nižší předpis nesmí odporovat vyššímu.",
            {
                "Nejvýš stojí ústavní zákony, včetně Ústavy a Listiny.",
                "Pod nimi jsou zákony, pak nařízení vlády a vyhlášky ministerstev.",
                "Obec a kraj mohou vydávat vyhlášky jen v mezích zákona.",
                "Platné předpisy se vyhlašují ve Sbírce zákonů.",
                NULL,
            },
        },
        {
            "2 / 2   •   Odvětví", "Veřejné a soukromé právo",
            "Tip: trestní právo je veřejné, kupní smlouva je soukromé.",
            {
                "Veřejné právo chrání zájem celku. Patří sem třeba trestní a správní právo.",
                "Soukromé právo upravuje vztahy mezi lidmi. Patří sem občanské, rodinné a pracovní právo.",
                "Když si předpisy odporují, použije se ten vyšší.",
                "Neznalost zákona neomlouvá, proto se předpisy zveřejňují.",
                NULL,
            },
        },
    };

    return build_on3_unit_page(&on3_lessons[2], "on3_unit3", "on3_unit3_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on3_unit3_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co stojí v právním řádu nejvýš?",
         {
          "Obecní vyhláška",
          "Ústavní zákony",
          "Nařízení vlády",
          "Vnitřní pokyn úřadu",
         },
         4, 1},
        {"Smí vyhláška ministerstva zrušit zákon?",
         {
          "Ano",
          "Ne, nižší předpis nesmí odporovat vyššímu",
          "Ano, pokud ji podepíše ministr",
          "Ano, na jeden rok",
         },
         4, 1},
        {"Kam patří trestní právo?",
         {
          "Do soukromého práva",
          "Do veřejného práva",
          "Mimo právní řád",
          "Jen do školního řádu",
         },
         4, 1},
        {"Kde se právní předpisy vyhlašují?",
         {
          "Ve Sbírce zákonů",
          "Jen na nástěnce obce",
          "Jen v televizi",
          "V notářském zápisu",
         },
         4, 0},
    };
    static const char *hints[] = {
        "Ústava a Listina jsou nad zákony",
        "Pyramida předpisů se nepřeskakuje",
        "Trestní právo chrání společnost",
        "Bez vyhlášení předpis nezavazuje",
    };
    return build_on3_mcq_page(2, "on3unit3", "on3_ex3_title",
                             "on3_quiz3_head", qs, hints, 4);
}
static GtkWidget *build_on3_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Osoby", "Kdo může mít práva",
            "Tip: právnická osoba není člověk, ale právo s ní jedná.",
            {
                "Fyzická osoba je člověk.",
                "Právnická osoba je třeba firma, spolek, obec nebo stát.",
                "Právní osobnost má člověk od narození: může mít práva a povinnosti.",
                "Právní vztah spojuje subjekty právy a povinnostmi.",
                NULL,
            },
        },
        {
            "2 / 2   •   Věk", "Svéprávnost a trestní odpovědnost",
            "Tip: patnáct let ještě není plná svéprávnost.",
            {
                "Svéprávnost je způsobilost sám právně jednat.",
                "Plná svéprávnost přichází zletilostí v 18 letech.",
                "Soud může svéprávnost z vážných důvodů omezit.",
                "Trestně odpovědný je člověk od 15 let, je-li příčetný.",
                NULL,
            },
        },
    };

    return build_on3_unit_page(&on3_lessons[3], "on3_unit4", "on3_unit4_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on3_unit4_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je fyzická osoba?",
         {
          "Firma",
          "Člověk",
          "Úřad",
          "Smlouva",
         },
         4, 1},
        {"Od kdy má člověk právní osobnost?",
         {
          "Od 18 let",
          "Od narození",
          "Od 15 let",
          "Od první smlouvy",
         },
         4, 1},
        {"Kdy nastává plná svéprávnost?",
         {
          "V 15 letech",
          "Zletilostí v 18 letech",
          "Nástupem do práce",
          "Zápisem do občanského průkazu",
         },
         4, 1},
        {"Od kolika let je člověk trestně odpovědný?",
         {
          "Od 18 let",
          "Od 15 let, je-li příčetný",
          "Od 10 let",
          "Až od první výplaty",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Fyzická osoba je člověk",
        "Práva může mít i dítě",
        "Svéprávnost je způsobilost jednat",
        "Pod 15 let trestní odpovědnost není",
    };
    return build_on3_mcq_page(3, "on3unit4", "on3_ex4_title",
                             "on3_quiz4_head", qs, hints, 4);
}
static GtkWidget *build_on3_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Věc", "Co vlastník smí",
            "Tip: vlastnictví není bez mezí.",
            {
                "Vlastník věc drží, užívá a může s ní nakládat.",
                "Zákon ho omezuje, třeba kvůli sousedům.",
                "Vyvlastnit lze jen ve veřejném zájmu, na základě zákona a za náhradu.",
                "Duševní vlastnictví chrání třeba autorské dílo nebo ochrannou známku.",
                NULL,
            },
        },
        {
            "2 / 2   •   Škoda", "Kdo ji nahradí",
            "Tip: cizí dílo se nesmí jen tak zkopírovat.",
            {
                "Kdo způsobí škodu, má ji zpravidla nahradit.",
                "Škoda může být na věci, na zdraví i v penězích, o které člověk přišel.",
                "Náhrada má uvést stav co nejblíž tomu, jaký byl před škodou.",
                "U autora se bere i to, že dílo někdo užije bez svolení.",
                NULL,
            },
        },
    };

    return build_on3_unit_page(&on3_lessons[4], "on3_unit5", "on3_unit5_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on3_unit5_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co vlastník se svou věcí smí?",
         {
          "Jen se na ni dívat",
          "Držet ji, užívat ji a nakládat s ní",
          "Jen ji půjčovat státu",
          "Nic, dokud ji nepojistí",
         },
         4, 1},
        {"Lze v České republice vyvlastnit bez náhrady?",
         {
          "Ano, kdykoli",
          "Ne, jen ve veřejném zájmu, podle zákona a za náhradu",
          "Ano, rozhodnutím souseda",
          "Ano, pokud jde o auto",
         },
         4, 1},
        {"Co chrání duševní vlastnictví?",
         {
          "Jen pozemky",
          "Třeba autorské dílo",
          "Jen hotovost",
          "Jen nemovitosti v katastru",
         },
         4, 1},
        {"Kdo má zpravidla nahradit způsobenou škodu?",
         {
          "Stát vždy",
          "Ten, kdo ji způsobil",
          "Poškozený sám",
          "Soused",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Vlastnictví zahrnuje držbu, užívání i nakládání",
        "Vyvlastnění má přísné podmínky",
        "Dílo není totéž co hmotná věc",
        "Škůdce škodu nahrazuje",
    };
    return build_on3_mcq_page(4, "on3unit5", "on3_ex5_title",
                             "on3_quiz5_head", qs, hints, 4);
}
static GtkWidget *build_on3_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Vůle", "Smlouva zavazuje",
            "Tip: ústní smlouva může platit, ale hůř se dokazuje.",
            {
                "Smlouva je souhlasný projev vůle dvou nebo více stran.",
                "Strany si sjednají, kdo, co a za jakých podmínek plní.",
                "Platná smlouva zavazuje. Nelze od ní odejít jen proto, že se to přestalo hodit.",
                "Zákon u některých smluv žádá písemnou formu.",
                NULL,
            },
        },
        {
            "2 / 2   •   Druhy", "Koupě, nájem, dar a dílo",
            "Tip: dům se nepřevede jen podáním ruky.",
            {
                "Kupní smlouvou přechází věc za kupní cenu.",
                "Nájem přenechává věc k dočasnému užívání za nájemné.",
                "Darování je bezúplatné. Smlouvou o dílo se zhotoví, opraví nebo upraví věc.",
                "Vlastnictví k nemovitosti se zapisuje do katastru.",
                NULL,
            },
        },
    };

    return build_on3_unit_page(&on3_lessons[5], "on3_unit6", "on3_unit6_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on3_unit6_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je smlouva?",
         {
          "Jednostranný rozkaz úřadu",
          "Souhlasný projev vůle, který strany zavazuje",
          "Reklama",
          "Ústní slib bez obsahu",
         },
         4, 1},
        {"Lze od každé smlouvy odejít, když se to přestane hodit?",
         {
          "Ano",
          "Ne, platná smlouva zavazuje",
          "Ano, do hodiny",
          "Ano, pokud druhá strana nesouhlasí",
         },
         4, 1},
        {"Co je nájem?",
         {
          "Darování věci",
          "Dočasné užívání věci za nájemné",
          "Trest za škodu",
          "Převod nemovitosti darem",
         },
         4, 1},
        {"Kam se zapisuje vlastnictví k nemovitosti?",
         {
          "Do občanského průkazu",
          "Do katastru nemovitostí",
          "Jen do smlouvy v šuplíku",
          "Na finanční úřad",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Smlouva stojí na shodě stran",
        "Závazek se plní",
        "Nájem není převod vlastnictví",
        "U nemovitosti rozhoduje vklad do katastru",
    };
    return build_on3_mcq_page(5, "on3unit6", "on3_ex6_title",
                             "on3_quiz6_head", qs, hints, 4);
}
static GtkWidget *build_on3_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Ochrana", "Slabší strana smlouvy",
            "Tip: spotřebitel je člověk, který nejedná jako podnikatel.",
            {
                "Spotřebitel uzavírá smlouvu s podnikatelem mimo své podnikání.",
                "Zákon ho chrání, protože podnikatel zná podmínky líp.",
                "Nekalé obchodní praktiky a klamavá reklama jsou zakázané.",
                "Smlouva nemá schovávat podstatné podmínky do nečitelného textu.",
                NULL,
            },
        },
        {
            "2 / 2   •   Reklamace", "Vada a čtrnáct dnů",
            "Tip: účtenka pomáhá, ale není jediný důkaz koupě.",
            {
                "Vadu zboží nebo služby lze reklamovat u prodávajícího.",
                "Smlouvu sjednanou po internetu nebo po telefonu lze často zrušit do 14 dnů.",
                "Výjimkou je třeba zboží vyrobené na míru nebo rychle se kazící věc.",
                "Když obchodník neplní, pomáhá Česká obchodní inspekce.",
                NULL,
            },
        },
    };

    return build_on3_unit_page(&on3_lessons[6], "on3_unit7", "on3_unit7_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on3_unit7_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo je spotřebitel?",
         {
          "Každá firma",
          "Člověk, který s podnikatelem nejedná ve svém podnikání",
          "Jen nezletilý",
          "Jen vlastník obchodu",
         },
         4, 1},
        {"Co je reklamace?",
         {
          "Darování věci",
          "Uplatnění vady zboží nebo služby",
          "Výpověď z práce",
          "Žaloba na souseda",
         },
         4, 1},
        {"Dokdy lze často odstoupit od smlouvy uzavřené po internetu?",
         {
          "Do 14 dnů",
          "Do druhého dne",
          "Do jednoho roku vždy",
          "Nelze nikdy",
         },
         4, 0},
        {"Kdo pomáhá, když obchodník spotřebiteli neplní?",
         {
          "Česká obchodní inspekce",
          "Jen školní parlament",
          "Katastr nemovitostí",
          "Česká národní banka u každého nákupu",
         },
         4, 0},
    };
    static const char *hints[] = {
        "Spotřebitel je slabší strana",
        "Reklamace se obrací na prodávajícího",
        "U smluv na dálku běží čtrnáctidenní lhůta",
        "ČOI dohlíží na ochranu spotřebitele",
    };
    return build_on3_mcq_page(6, "on3unit7", "on3_ex7_title",
                             "on3_quiz7_head", qs, hints, 4);
}
static GtkWidget *build_on3_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Nabytí", "Zákon, nebo závěť",
            "Tip: dědictví lze odmítnout.",
            {
                "Dědí se podle závěti, a kde závěť není, podle zákona.",
                "Ze zákona dědí především děti, manžel nebo partner a další blízcí.",
                "Pozůstalost projednává notář jako soudní komisař.",
                "Dědic může dědictví odmítnout. Odmítnutí se nedá jen tak vzít zpět.",
                NULL,
            },
        },
        {
            "2 / 2   •   Dluhy", "Soupis pozůstalosti",
            "Tip: dědictví nejsou jen věci, ale i dluhy.",
            {
                "S majetkem mohou přejít i dluhy zůstavitele.",
                "Bez soupisu může dědic ručit i nad hodnotu toho, co nabyl.",
                "Požádá-li o soupis pozůstalosti, hradí dluhy jen do výše nabytého majetku.",
                "Proto se před přijetím vyplatí vědět, co v pozůstalosti je.",
                NULL,
            },
        },
    };

    return build_on3_unit_page(&on3_lessons[7], "on3_unit8", "on3_unit8_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on3_unit8_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Podle čeho se dědí, když není závěť?",
         {
          "Podle losu",
          "Podle zákona",
          "Podle přání sousedů",
          "Majetek propadá vždy obci",
         },
         4, 1},
        {"Kdo projednává pozůstalost?",
         {
          "Notář",
          "Starosta",
          "Zaměstnavatel",
          "Škola",
         },
         4, 0},
        {"Lze dědictví odmítnout?",
         {
          "Ne",
          "Ano",
          "Jen nemovitost",
          "Jen se souhlasem věřitele",
         },
         4, 1},
        {"K čemu je soupis pozůstalosti?",
         {
          "Aby se dluhy hradily jen do výše nabytého majetku",
          "Aby dědic přišel o všechno",
          "Aby se závěť zrušila",
          "Aby se dědictví nedalo odmítnout",
         },
         4, 0},
    };
    static const char *hints[] = {
        "Zákon určuje okruh dědiců",
        "Notář jedná jako soudní komisař",
        "Odmítnutí je právo dědice",
        "Soupis omezuje rozsah dluhů",
    };
    return build_on3_mcq_page(7, "on3unit8", "on3_ex8_title",
                             "on3_quiz8_head", qs, hints, 4);
}
static GtkWidget *build_on3_unit9_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Svazek", "Manželství a partnerství",
            "Tip: partnerství z roku 2025 není totéž co staré registrované partnerství.",
            {
                "Manželství je trvalý svazek muže a ženy.",
                "Uzavírá se svobodně, zpravidla od 18 let. Výjimečně od 16 let se souhlasem soudu.",
                "Dva lidé stejného pohlaví mohou od roku 2025 uzavřít partnerství.",
                "Partnerství se v řadě práv podobá manželství. Dřívější registrovaná partnerství zůstávají v platnosti.",
                NULL,
            },
        },
        {
            "2 / 2   •   Dítě", "Péče, výživa a náhradní výchova",
            "Tip: pěstoun není totéž co osvojitel.",
            {
                "Rodiče mají o dítě pečovat, zastupovat je a živit je.",
                "Dítě má právo být slyšeno ve věcech, které se ho týkají.",
                "Osvojením se dítě přijímá za vlastní. Pěstoun o něj pečuje, když nemůže být u rodičů.",
                "Poručník dítě zastupuje, když to rodiče nemohou.",
                NULL,
            },
        },
    };

    return build_on3_unit_page(&on3_lessons[8], "on3_unit9", "on3_unit9_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on3_unit9_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je manželství podle občanského zákoníku?",
         {
          "Jakýkoli společný byt",
          "Trvalý svazek muže a ženy",
          "Pracovní smlouva",
          "Smlouva o nájmu",
         },
         4, 1},
        {"Co mohou od roku 2025 uzavřít dva lidé stejného pohlaví?",
         {
          "Nic",
          "Partnerství",
          "Jen pracovní poměr",
          "Už jen registrované partnerství jako dřív",
         },
         4, 1},
        {"Od kolika let lze zpravidla uzavřít manželství?",
         {
          "Od 15 let",
          "Od 18 let",
          "Od 21 let",
          "Od 16 let bez další podmínky",
         },
         4, 1},
        {"Čím se liší pěstoun od osvojitele?",
         {
          "Pěstoun o dítě pečuje, osvojení je přijímá za vlastní",
          "Ničím",
          "Pěstoun je vždy poručník soudu",
          "Osvojitel jen platí nájem",
         },
         4, 0},
    };
    static const char *hints[] = {
        "Manželství je svazek muže a ženy",
        "Nové registrované partnerství se už neuzavírá",
        "Šestnáct let jen výjimečně se souhlasem soudu",
        "Osvojení mění právní vztah rodiče a dítěte",
    };
    return build_on3_mcq_page(8, "on3unit9", "on3_ex9_title",
                             "on3_quiz9_head", qs, hints, 4);
}
static GtkWidget *build_on3_unit10_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Smlouva", "Vznik pracovního poměru",
            "Tip: zkušební doba musí být písemná.",
            {
                "Pracovní smlouva určí druh práce, místo výkonu a den nástupu.",
                "Uzavírá se písemně.",
                "Zkušební doba smí trvat nejvýše 4 měsíce, u vedoucího zaměstnance 8 měsíců.",
                "Ve zkušební době lze poměr zrušit i bez důvodu, ne však v prvních 14 dnech neschopnosti.",
                NULL,
            },
        },
        {
            "2 / 2   •   Konec", "Výpověď a bezpečná práce",
            "Tip: zaměstnavatel si důvod výpovědi nesmí vymyslet.",
            {
                "Zaměstnanec může dát výpověď z jakéhokoli důvodu.",
                "Zaměstnavatel jen ze zákonných důvodů, třeba organizačních nebo pro porušení povinností.",
                "Výpovědní doba trvá nejméně dva měsíce a začíná dnem doručení.",
                "Zaměstnanec má právo na mzdu, odpočinek, dovolenou a bezpečnou práci.",
                NULL,
            },
        },
    };

    return build_on3_unit_page(&on3_lessons[9], "on3_unit10", "on3_unit10_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on3_unit10_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co musí pracovní smlouva určit?",
         {
          "Jen výši prémií",
          "Druh práce, místo a den nástupu",
          "Jméno manžela",
          "Číslo občanského průkazu rodičů",
         },
         4, 1},
        {"Jak dlouhá smí být zkušební doba řadového zaměstnance?",
         {
          "Nejvýše 4 měsíce",
          "Nejvýše jeden rok",
          "Bez omezení",
          "Jen jeden týden",
         },
         4, 0},
        {"Kdy smí zaměstnavatel dát výpověď?",
         {
          "Z jakéhokoli důvodu",
          "Jen ze zákonných důvodů",
          "Jen ústně na chodbě",
          "Nikdy",
         },
         4, 1},
        {"Jak dlouhá je základní výpovědní doba?",
         {
          "Jeden týden",
          "Nejméně dva měsíce",
          "Půl roku vždy",
          "Do konce směny",
         },
         4, 1},
    };
    static const char *hints[] = {
        "To jsou podstatné náležitosti",
        "U vedoucího je mez osm měsíců",
        "Důvody výpovědi stanoví zákoník práce",
        "Běží ode dne doručení výpovědi",
    };
    return build_on3_mcq_page(9, "on3unit10", "on3_ex10_title",
                             "on3_quiz10_head", qs, hints, 4);
}
static GtkWidget *build_on3_unit11_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Stupně", "Od okresního soudu výš",
            "Tip: Ústavní soud není další odvolací stupeň.",
            {
                "Nejblíž lidem jsou okresní soudy. V Praze se jim říká obvodní.",
                "Nad nimi jsou krajské soudy. V Praze je to Městský soud.",
                "Vrchní soudy sídlí v Praze a v Olomouci.",
                "Nejvyšší soud sjednocuje rozhodování obecných soudů.",
                NULL,
            },
        },
        {
            "2 / 2   •   Mimo řadu", "Správní a ústavní soudnictví",
            "Tip: každý soudce je nezávislý.",
            {
                "Žaloby proti rozhodnutím úřadů řeší správní soudy.",
                "Na vrcholu správního soudnictví je Nejvyšší správní soud.",
                "Ústavní soud stojí mimo tuto soustavu a hlídá ústavní pořádek.",
                "Soudce není podřízený ministrovi ani politikovi.",
                NULL,
            },
        },
    };

    return build_on3_unit_page(&on3_lessons[10], "on3_unit11", "on3_unit11_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on3_unit11_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak se jmenuje první stupeň soudů v Praze?",
         {
          "Krajský soud",
          "Obvodní soudy",
          "Vrchní soud",
          "Ústavní soud",
         },
         4, 1},
        {"Kde sídlí vrchní soudy?",
         {
          "V Brně a Ostravě",
          "V Praze a Olomouci",
          "Jen v Praze",
          "V každém kraji",
         },
         4, 1},
        {"Kdo završuje správní soudnictví?",
         {
          "Nejvyšší správní soud",
          "Obecní úřad",
          "Poslanecká sněmovna",
          "Notářská komora",
         },
         4, 0},
        {"Je Ústavní soud běžný odvolací soud?",
         {
          "Ano, je nad každým rozsudkem automaticky",
          "Ne, stojí mimo soustavu obecných soudů",
          "Ano, nahrazuje okresní soud",
          "Ano, rozhoduje každou pokutu",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Mimo Prahu jsou to okresní soudy",
        "Vrchní soudy jsou dva",
        "Správní soudy přezkoumávají úřady",
        "Ústavní soud hlídá ústavní pořádek",
    };
    return build_on3_mcq_page(10, "on3unit11", "on3_ex11_title",
                             "on3_quiz11_head", qs, hints, 4);
}
static GtkWidget *build_on3_unit12_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Žaloba", "Občanské soudní řízení",
            "Tip: kdo žaluje, je žalobce. Koho žaluje, je žalovaný.",
            {
                "Občanské soudní řízení začíná zpravidla žalobou.",
                "Žalobce se domáhá svého práva proti žalovanému.",
                "Soud vyslechne obě strany a rozhodne rozsudkem, nebo řízení jinak skončí.",
                "Proti rozsudku je odvolání. Dovolání k Nejvyššímu soudu je výjimečné.",
                NULL,
            },
        },
        {
            "2 / 2   •   Úřad", "Správní řízení",
            "Tip: odvolání je řádný opravný prostředek.",
            {
                "Správní řízení vede úřad, třeba když vydává povolení nebo ukládá povinnost.",
                "Řídí se správním řádem.",
                "Proti rozhodnutí se lze odvolat k nadřízenému úřadu.",
                "Když to nepomůže, zbývá žaloba ke správnímu soudu.",
                NULL,
            },
        },
    };

    return build_on3_unit_page(&on3_lessons[11], "on3_unit12", "on3_unit12_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on3_unit12_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo je žalobce?",
         {
          "Soudce",
          "Ten, kdo se žalobou domáhá svého práva",
          "Svědek",
          "Každý, kdo sedí v síni",
         },
         4, 1},
        {"Čím občanské soudní řízení zpravidla začíná?",
         {
          "Výpovědí",
          "Žalobou",
          "Závětí",
          "Reklamací v obchodě",
         },
         4, 1},
        {"Co je odvolání?",
         {
          "Nová pracovní smlouva",
          "Řádný opravný prostředek proti rozhodnutí",
          "Odmítnutí dědictví",
          "Stížnost na počasí",
         },
         4, 1},
        {"Kam se lze obrátit, když neuspěje odvolání proti úřadu?",
         {
          "Ke správnímu soudu",
          "Do katastru",
          "K zaměstnavateli",
          "Na obecnou nástěnku",
         },
         4, 0},
    };
    static const char *hints[] = {
        "Žalovaný je ten, proti komu žaloba míří",
        "Žaloba otevírá spor",
        "Odvolání přezkoumá vyšší stupeň",
        "Správní soud přezkoumává rozhodnutí úřadu",
    };
    return build_on3_mcq_page(11, "on3unit12", "on3_ex12_title",
                             "on3_quiz12_head", qs, hints, 4);
}
static GtkWidget *build_on3_unit13_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Soud a žaloba", "Kdo rozhoduje a kdo žaluje",
            "Tip: advokát není soudce.",
            {
                "Soudce je nezávislý, nestranný a spor rozhoduje.",
                "Státní zástupce podává obžalobu a ve veřejném zájmu hlídá zákonnost trestního řízení.",
                "Advokát zastupuje klienta a váže ho povinnost mlčenlivosti.",
                "Kdo si advokáta nemůže dovolit, může v zákonem daných případech dostat právní pomoc.",
                NULL,
            },
        },
        {
            "2 / 2   •   Listiny", "Notář a exekutor",
            "Tip: exekutor nevymýšlí dluh, vymáhá už uloženou povinnost.",
            {
                "Notář sepisuje veřejné listiny a projednává dědictví.",
                "Veřejná listina má silnější důkazní váhu než obyčejné potvrzení.",
                "Exekutor vymáhá splnění rozhodnutí, když je dlužník sám neplní.",
                "Veřejný ochránce práv není soudce a rozsudky neruší.",
                NULL,
            },
        },
    };

    return build_on3_unit_page(&on3_lessons[12], "on3_unit13", "on3_unit13_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on3_unit13_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo v trestním řízení podává obžalobu?",
         {
          "Advokát obžalovaného",
          "Státní zástupce",
          "Notář",
          "Starosta",
         },
         4, 1},
        {"Čím je vázán advokát?",
         {
          "Pokyny protistrany",
          "Povinností mlčenlivosti vůči klientovi",
          "Příkazem ministra v každém sporu",
          "Ničím",
         },
         4, 1},
        {"Kdo projednává dědictví?",
         {
          "Exekutor",
          "Notář",
          "Policie vždy",
          "Zaměstnavatel",
         },
         4, 1},
        {"Co dělá exekutor?",
         {
          "Ruší zákony",
          "Vymáhá splnění uložené povinnosti",
          "Uzavírá manželství",
          "Rozhoduje o vině v trestním řízení",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Státní zástupce žaluje ve veřejném zájmu",
        "Advokát hájí klienta, ne úřad",
        "Notář je soudní komisař",
        "Exekuce není nový soud o vině",
    };
    return build_on3_mcq_page(12, "on3unit13", "on3_ex13_title",
                             "on3_quiz13_head", qs, hints, 4);
}
static GtkWidget *build_on3_unit14_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Rozdíl", "Závažnost rozhoduje",
            "Tip: přestupek není malý trestný čin.",
            {
                "Trestný čin je závažný protiprávní čin, který tak označuje trestní zákoník.",
                "Přestupek je mírnější protiprávní čin a trestným činem není.",
                "Rušení nočního klidu bývá přestupek. Úmyslné těžké ublížení na zdraví je trestný čin.",
                "Za každý z nich odpovídá jiný zákon a jiné řízení.",
                NULL,
            },
        },
        {
            "2 / 2   •   Vina", "Věk, příčetnost a presumpce",
            "Tip: obviněný není odsouzený.",
            {
                "Trestní odpovědnost vyžaduje věk aspoň 15 let, příčetnost a zavinění.",
                "Bez zavinění, tedy úmyslu nebo nedbalosti, o trestný čin nejde.",
                "Dokud soud pravomocně nerozhodne, hledí se na člověka jako na nevinného.",
                "Tomu se říká presumpce neviny.",
                NULL,
            },
        },
    };

    return build_on3_unit_page(&on3_lessons[13], "on3_unit14", "on3_unit14_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on3_unit14_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Čím se trestný čin liší od přestupku?",
         {
          "Ničím",
          "Trestný čin je závažnější a popisuje ho trestní zákoník",
          "Přestupek řeší jen Ústavní soud",
          "Trestný čin nemá oběť",
         },
         4, 1},
        {"Kam spíš patří rušení nočního klidu?",
         {
          "Mezi přestupky",
          "Vždy mezi zločiny",
          "Mimo právo",
          "Jen do pracovní smlouvy",
         },
         4, 0},
        {"Co presumpce neviny znamená?",
         {
          "Policie nesmí nikoho vyslechnout",
          "Dokud soud nerozhodne, hledí se na člověka jako na nevinného",
          "Trest se ukládá před soudem",
          "Obviněný se nesmí hájit",
         },
         4, 1},
        {"Stačí ke trestnému činu samotný následek bez zavinění?",
         {
          "Ano",
          "Ne, trestní právo vyžaduje zavinění",
          "Ano u každého přestupku školy",
          "Ano, pokud o tom napíší noviny",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Závažnost a zákon, který čin upravuje",
        "Jde o mírnější protiprávní čin",
        "Vinu vyslovuje až soud",
        "Úmysl nebo nedbalost jsou podmínkou",
    };
    return build_on3_mcq_page(13, "on3unit14", "on3_ex14_title",
                             "on3_quiz14_head", qs, hints, 4);
}
static GtkWidget *build_on3_unit15_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Trest", "Ne jen vězení",
            "Tip: trest smrti se v České republice neukládá.",
            {
                "Trest má chránit společnost a vést pachatele k nápravě, ne být pomstou.",
                "Vedle odnětí svobody je třeba podmíněné odsouzení, obecně prospěšné práce nebo peněžitý trest.",
                "Patří sem i domácí vězení nebo zákaz činnosti.",
                "Tomuto posunu od pouhého zavírání se říká humanizace trestání.",
                NULL,
            },
        },
        {
            "2 / 2   •   Řízení", "Policie, žalobce, soud",
            "Tip: obhájce má obviněný právo mít.",
            {
                "Policie věc prověřuje a vyšetřuje.",
                "Státní zástupce rozhoduje, zda podá obžalobu.",
                "O vině a trestu rozhoduje soud.",
                "Obviněný se může hájit a v zákonem daných případech musí mít obhájce.",
                NULL,
            },
        },
    };

    return build_on3_unit_page(&on3_lessons[14], "on3_unit15", "on3_unit15_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on3_unit15_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jaký má být smysl trestu?",
         {
          "Pomsta",
          "Ochrana společnosti a náprava pachatele",
          "Zisk pro soudce",
          "Veřejné ponížení",
         },
         4, 1},
        {"Ukládá Česká republika trest smrti?",
         {
          "Ano",
          "Ne",
          "Ano u mladistvých",
          "Ano, pokud o to oběť požádá",
         },
         4, 1},
        {"Kdo rozhoduje o vině a trestu?",
         {
          "Policie",
          "Soud",
          "Noviny",
          "Poškozený sám",
         },
         4, 1},
        {"Co je podmíněné odsouzení?",
         {
          "Trest smrti s odkladem",
          "Odklad výkonu trestu, pokud se odsouzený osvědčí",
          "Zproštění obžaloby",
          "Přestupek",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Humanizace odmítá pomstu jako účel",
        "Trest smrti byl zrušen",
        "Policie vyšetřuje, soud rozhoduje",
        "Je to alternativa k nástupu do vězení",
    };
    return build_on3_mcq_page(14, "on3unit15", "on3_ex15_title",
                             "on3_quiz15_head", qs, hints, 4);
}
static GtkWidget *build_on3_unit16_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 2   •   Věk", "Děti a mladiství",
            "Tip: patnáct let je hranice trestní odpovědnosti, ne dospělosti.",
            {
                "Dítě mladší 15 let není trestně odpovědné.",
                "Mladistvý je člověk od 15 do 18 let.",
                "U mladistvého zákon hledí i na výchovu, ne jen na potrestání.",
                "I čin dítěte pod 15 let se řeší, třeba opatřeními na ochranu dítěte a okolí.",
                NULL,
            },
        },
        {
            "2 / 2   •   Pomoc", "Když se stanete obětí nebo svědkem",
            "Tip: linky 158 a 112 jsou pro přivolání pomoci.",
            {
                "Šikanu, násilí, vydírání, lichvu nebo korupci si nenechávejte pro sebe.",
                "Řekněte to důvěryhodnému dospělému, škole nebo policii.",
                "Při ohrožení odejděte do bezpečí a zavolejte 158 nebo 112. Do střetu se nepouštějte.",
                "Svědek má mluvit pravdu. Lichva je půjčka, která zneužívá cizí tísně, a patří na policii, ne do další půjčky.",
                NULL,
            },
        },
    };

    return build_on3_unit_page(&on3_lessons[15], "on3_unit16", "on3_unit16_sub",
                               slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_on3_unit16_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Je dítě mladší 15 let trestně odpovědné?",
         {
          "Ano, jako dospělý",
          "Ne",
          "Ano, od 10 let",
          "Jen když chodí do školy",
         },
         4, 1},
        {"Kdo je mladistvý?",
         {
          "Každý student",
          "Člověk od 15 do 18 let",
          "Člověk do 15 let",
          "Jen pachatel přestupku",
         },
         4, 1},
        {"Co udělat při šikaně nebo vydírání?",
         {
          "Nechat si to pro sebe",
          "Říct dospělému, škole nebo policii",
          "Vrátit to stejnou silou",
          "Půjčit si peníze na vyplacení",
         },
         4, 1},
        {"Co má udělat svědek u policie?",
         {
          "Mlčet",
          "Mluvit pravdu",
          "Domyslet si, co policie chce slyšet",
          "Oznámit jen to, co se hodí kamarádovi",
         },
         4, 1},
    };
    static const char *hints[] = {
        "Trestní odpovědnost začíná v 15 letech",
        "Mladistvý už je trestně odpovědný, ale jinak než dospělý",
        "Pomoc je mimo tu situaci",
        "Křivá výpověď věc zhoršuje",
    };
    return build_on3_mcq_page(15, "on3unit16", "on3_ex16_title",
                             "on3_quiz16_head", qs, hints, 4);
}

void add_on3_pages(GtkStack *stack) {
    typedef GtkWidget *(*OnBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        OnBuilder build_unit;
        OnBuilder build_ex;
    } pages[] = {
        {"on3unit1", "on3ex1", build_on3_unit1_page, build_on3_unit1_exercise_page},
        {"on3unit2", "on3ex2", build_on3_unit2_page, build_on3_unit2_exercise_page},
        {"on3unit3", "on3ex3", build_on3_unit3_page, build_on3_unit3_exercise_page},
        {"on3unit4", "on3ex4", build_on3_unit4_page, build_on3_unit4_exercise_page},
        {"on3unit5", "on3ex5", build_on3_unit5_page, build_on3_unit5_exercise_page},
        {"on3unit6", "on3ex6", build_on3_unit6_page, build_on3_unit6_exercise_page},
        {"on3unit7", "on3ex7", build_on3_unit7_page, build_on3_unit7_exercise_page},
        {"on3unit8", "on3ex8", build_on3_unit8_page, build_on3_unit8_exercise_page},
        {"on3unit9", "on3ex9", build_on3_unit9_page, build_on3_unit9_exercise_page},
        {"on3unit10", "on3ex10", build_on3_unit10_page, build_on3_unit10_exercise_page},
        {"on3unit11", "on3ex11", build_on3_unit11_page, build_on3_unit11_exercise_page},
        {"on3unit12", "on3ex12", build_on3_unit12_page, build_on3_unit12_exercise_page},
        {"on3unit13", "on3ex13", build_on3_unit13_page, build_on3_unit13_exercise_page},
        {"on3unit14", "on3ex14", build_on3_unit14_page, build_on3_unit14_exercise_page},
        {"on3unit15", "on3ex15", build_on3_unit15_page, build_on3_unit15_exercise_page},
        {"on3unit16", "on3ex16", build_on3_unit16_page, build_on3_unit16_exercise_page}
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
