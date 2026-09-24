#include "maturita.h"

/* ---- Technical equipment (Technické vybavení) --------------------- */

NetLesson hw_lessons[HW_LESSONS] = {
    { .n_slides = HW_SLIDES,  .unit_page = "hwunit1", .ex_page = "hwex1" },
    { .n_slides = HW2_SLIDES, .unit_page = "hwunit2", .ex_page = "hwex2" },
    { .n_slides = HW3_SLIDES, .unit_page = "hwunit3", .ex_page = "hwex3" },
    { .n_slides = HW4_SLIDES, .unit_page = "hwunit4", .ex_page = "hwex4" },
};

void hw_rail_theme_reset(void);
void refresh_hw_completion_ui(void);

GtkWidget *hw_scroll;
GtkWidget *hw_fixed;
GtkWidget *hw_rail;
GtkWidget *hw_nodes[HW_UNITS];
GtkWidget *hw_labels[HW_UNITS];
double hw_cx[HW_UNITS];
double hw_cy[HW_UNITS];
int hw_cols = 1;
int hw_rows = 1;
int hw_cw = (int)(2.0 * ROAD_MX + (HW_UNITS - 1) * PATH_SPAC);
int hw_ch = (int)(2.0 * ROAD_MY);
guint hw_idle;
double hw_last_avail = -1.0;
int hw_last_cols = -1;
int hw_last_rows = -1;
int hw_last_cw = -1;
int hw_last_ch = -1;
cairo_surface_t *hw_rail_cache;
int hw_cache_w;
int hw_cache_h;

void hw_point(double t, double *ox, double *oy) {
    int k = (int)t;
    double u = t - (double)k;
    double u2, u3;
    double p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y;

    if (k < 0) { k = 0; u = 0.0; }
    if (k >= HW_UNITS - 1) { k = HW_UNITS - 2; u = 1.0; }

    p1x = hw_cx[k];     p1y = hw_cy[k];
    p2x = hw_cx[k + 1]; p2y = hw_cy[k + 1];
    if (k - 1 >= 0) { p0x = hw_cx[k - 1]; p0y = hw_cy[k - 1]; }
    else            { p0x = p1x - (p2x - p1x); p0y = p1y - (p2y - p1y); }
    if (k + 2 < HW_UNITS) { p3x = hw_cx[k + 2]; p3y = hw_cy[k + 2]; }
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

void hw_path(cairo_t *cr, double t0, double t1) {
    const double steps_per_unit = 24.0;
    int n = (int)((t1 - t0) * steps_per_unit);
    double x, y;
    int s;

    if (n < 1)
        n = 1;
    hw_point(t0, &x, &y);
    cairo_move_to(cr, x, y);
    for (s = 1; s <= n; s++) {
        hw_point(t0 + (t1 - t0) * (double)s / (double)n, &x, &y);
        cairo_line_to(cr, x, y);
    }
}

void hw_geometry(double avail) {
    const double phase = 2.0 * G_PI * 1.7 / (double)(HW_UNITS - 1);
    int rows;
    int r, c, i;

    for (rows = 1; rows <= HW_UNITS; rows++) {
        int cols = (HW_UNITS + rows - 1) / rows;
        if (2.0 * ROAD_MX + (cols - 1) * PATH_SPAC <= avail + 1.0)
            break;
    }
    if (rows > HW_UNITS)
        rows = HW_UNITS;
    hw_rows = rows;
    hw_cols = (HW_UNITS + rows - 1) / rows;
    if (hw_cols < 1)
        hw_cols = 1;
    hw_cw = (int)(2.0 * ROAD_MX + (hw_cols - 1) * PATH_SPAC);
    hw_ch = (int)(2.0 * ROAD_MY + (rows - 1) * ROAD_GAP);

    for (r = 0; r < rows; r++) {
        int base = r * hw_cols;
        int len = MIN(hw_cols, HW_UNITS - base);
        int fwd = (r % 2) == 0;
        for (c = 0; c < len; c++) {
            i = base + c;
            int cc = fwd ? c : (hw_cols - 1 - c);
            hw_cx[i] = ROAD_MX + cc * PATH_SPAC;
            hw_cy[i] = ROAD_MY + r * ROAD_GAP
                        + ROAD_WAVE * sin((double)i * phase);
        }
    }
}

void hw_apply_layout(void) {
    if (!hw_fixed)
        return;

    if (hw_cols == hw_last_cols && hw_rows == hw_last_rows &&
        hw_cw == hw_last_cw && hw_ch == hw_last_ch)
        return;

    hw_last_cols = hw_cols;
    hw_last_rows = hw_rows;
    hw_last_cw = hw_cw;
    hw_last_ch = hw_ch;
    if (hw_rail_cache) {
        cairo_surface_destroy(hw_rail_cache);
        hw_rail_cache = NULL;
    }

    for (int i = 0; i < HW_UNITS; i++) {
        if (!hw_nodes[i] || !hw_labels[i])
            continue;
        gtk_fixed_move(GTK_FIXED(hw_fixed), hw_nodes[i],
                       (int)(hw_cx[i] - NODE_SIZE / 2.0),
                       (int)(hw_cy[i] - NODE_SIZE / 2.0));
        gtk_fixed_move(GTK_FIXED(hw_fixed), hw_labels[i],
                       (int)(hw_cx[i] - (PATH_SPAC - 20.0) / 2.0),
                       (int)(hw_cy[i] + NODE_SIZE / 2.0 + 10.0));
    }

    gtk_widget_set_size_request(hw_fixed, hw_cw, hw_ch);
    gtk_widget_set_size_request(hw_rail, hw_cw, hw_ch);
    gtk_fixed_move(GTK_FIXED(hw_fixed), hw_rail, 0, 0);
    gtk_widget_queue_draw(hw_rail);
}

void hw_relayout(void) {
    GtkAdjustment *hadj;
    double avail;

    if (!hw_scroll)
        return;
    hadj = gtk_scrolled_window_get_hadjustment(
        GTK_SCROLLED_WINDOW(hw_scroll));
    avail = gtk_adjustment_get_page_size(hadj);
    if (avail < 1.0)
        return;
    if (fabs(avail - hw_last_avail) < 1.0)
        return;
    hw_last_avail = avail;
    hw_geometry(avail);
    hw_apply_layout();
}

gboolean hw_relayout_idle(gpointer data) {
    (void)data;
    hw_idle = 0;
    hw_relayout();
    return G_SOURCE_REMOVE;
}

void hw_relayout_later(void) {
    if (hw_idle == 0)
        hw_idle = g_idle_add(hw_relayout_idle, NULL);
}

void hw_adjust_notify(GtkAdjustment *adj, GParamSpec *ps,
                              gpointer data) {
    (void)adj;
    (void)ps;
    (void)data;
    hw_relayout_later();
}

void hw_render_rail_to(cairo_t *cr) {
    const double t_end = (double)(HW_UNITS - 1);
    const Rgb rail = color_from_hex(app_theme.rail);
    const Rgb muted = color_from_hex(app_theme.subtext);

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

    cairo_new_path(cr);
    hw_path(cr, 0.0, t_end);
    cairo_set_line_width(cr, 12);
    cairo_set_source_rgb(cr, rail.r, rail.g, rail.b);
    cairo_stroke(cr);

    cairo_set_source_rgba(cr, muted.r, muted.g, muted.b, 0.17);
    cairo_set_dash(cr, (double[]){2.0, 40.0}, 2, 0.0);
    cairo_new_path(cr);
    hw_path(cr, 0.0, t_end);
    cairo_stroke(cr);
    cairo_set_dash(cr, NULL, 0, 0.0);
}

void hw_draw_rail(GtkDrawingArea *area, cairo_t *cr,
                          int width, int height, gpointer data) {
    (void)area;
    (void)width;
    (void)height;
    (void)data;

    if (!hw_rail_cache || hw_cache_w != hw_cw || hw_cache_h != hw_ch) {
        cairo_t *rcr;

        if (hw_rail_cache)
            cairo_surface_destroy(hw_rail_cache);
        hw_cache_w = hw_cw;
        hw_cache_h = hw_ch;
        if (hw_cache_w < 1)
            hw_cache_w = 1;
        if (hw_cache_h < 1)
            hw_cache_h = 1;
        hw_rail_cache = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                    hw_cache_w, hw_cache_h);
        rcr = cairo_create(hw_rail_cache);
        hw_render_rail_to(rcr);
        cairo_destroy(rcr);
    }

    cairo_set_source_surface(cr, hw_rail_cache, 0, 0);
    cairo_paint(cr);
}

void hw_rail_theme_reset(void) {
    if (hw_rail_cache) {
        cairo_surface_destroy(hw_rail_cache);
        hw_rail_cache = NULL;
    }
    if (hw_rail)
        gtk_widget_queue_draw(hw_rail);
}

void hw_add_node(GtkFixed *fixed, int index) {
    int num = index + 1;
    gboolean locked = index >= HW_LESSONS;
    GtkWidget *card;
    GtkWidget *vbox;
    GtkWidget *number;
    GtkWidget *icon;
    GtkWidget *name;
    char *text;
    static const char *unit_keys[HW_LESSONS] = {
        "hw_unit1", "hw_unit2", "hw_unit3", "hw_unit4"
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
        hw_lessons[index].done_icon = icon;
        g_signal_connect(card, "clicked", G_CALLBACK(net_open_unit),
                         &hw_lessons[index]);
    }

    hw_nodes[index] = card;
    gtk_fixed_put(fixed, card, 0, 0);

    name = gtk_label_new(NULL);
    gtk_widget_set_size_request(name, (int)(PATH_SPAC - 20.0), -1);
    gtk_widget_set_halign(name, GTK_ALIGN_CENTER);
    gtk_label_set_justify(GTK_LABEL(name), GTK_JUSTIFY_CENTER);
    gtk_label_set_wrap(GTK_LABEL(name), TRUE);
    gtk_widget_add_css_class(name, "unit-name");
    if (index < HW_LESSONS) {
        i18n_bind(name, unit_keys[index], 0);
    } else {
        gtk_widget_add_css_class(name, "unit-name-locked");
    }
    hw_labels[index] = name;
    gtk_fixed_put(fixed, name, 0, 0);
}

GtkWidget *build_hwmap_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *fixed;
    GtkWidget *rail;
    GtkAdjustment *ha;
    GtkAdjustment *va;

    hw_geometry(1000.0);

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("subjects", "Technické vybavení", "hw_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);
    hw_scroll = scroll;

    wrap = gtk_center_box_new();
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), wrap);

    fixed = gtk_fixed_new();
    gtk_widget_set_halign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(fixed, hw_cw, hw_ch);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), fixed);
    hw_fixed = fixed;

    rail = gtk_drawing_area_new();
    gtk_widget_set_size_request(rail, hw_cw, hw_ch);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(rail), hw_draw_rail,
                                   NULL, NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rail, 0, 0);
    hw_rail = rail;

    for (int i = 0; i < HW_UNITS; i++)
        hw_add_node(GTK_FIXED(fixed), i);

    ha = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scroll));
    va = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));
    g_signal_connect(ha, "notify::page-size",
                     G_CALLBACK(hw_adjust_notify), NULL);
    g_signal_connect(va, "notify::page-size",
                     G_CALLBACK(hw_adjust_notify), NULL);

    hw_apply_layout();
    hw_relayout_later();
    refresh_hw_completion_ui();

    return page;
}


void hw_slide_apply(NetLesson *L);
void hw_slide_prev(GtkButton *button, gpointer data);
void hw_slide_next(GtkButton *button, gpointer data);

/* ---- Unit 1: Architektura počítače -------------------------------- */

static GtkWidget *hw_note_host;
static int hw_note_last_w = -1;

static void hw_rescale_notes_at(int w) {
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
    for (int i = 0; i < HW_LESSONS; i++)
        net_rescale_lesson_notes(&hw_lessons[i], body, head, kick);
}

static gboolean hw_note_tick(GtkWidget *w, GdkFrameClock *clock,
                             gpointer data) {
    int width = gtk_widget_get_allocated_width(w);

    (void)clock;
    (void)data;
    if (width != hw_note_last_w) {
        hw_note_last_w = width;
        hw_rescale_notes_at(width);
    }
    return G_SOURCE_CONTINUE;
}

static GtkWidget *build_hw_unit_page(NetLesson *L, const char *title_key,
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

    gtk_box_append(GTK_BOX(page), top_bar("hwmap", title_key, sub_key));

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
    hw_note_host = L->stack;

    for (guint s = 0; s < n_slides; s++) {
        GtkWidget *stage = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        GtkWidget *card;
        char name[16];

        gtk_widget_set_vexpand(stage, TRUE);
        g_snprintf(name, sizeof(name), "hw_slide%u", s);
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

    gtk_widget_add_tick_callback(L->stack, hw_note_tick, NULL, NULL);
    hw_note_last_w = -1;
    hw_rescale_notes_at(0);
    net_notes_target = NULL;

    nav = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(nav), TRUE);
    gtk_widget_set_margin_top(nav, 10);
    gtk_box_append(GTK_BOX(page), nav);

    L->prev_btn = gtk_button_new_with_label(tr("net_slide_prev"));
    gtk_widget_add_css_class(L->prev_btn, "btn-primary");
    g_signal_connect(L->prev_btn, "clicked", G_CALLBACK(hw_slide_prev), L);
    gtk_box_append(GTK_BOX(nav), L->prev_btn);

    L->next_btn = gtk_button_new_with_label(tr("net_slide_next"));
    gtk_widget_add_css_class(L->next_btn, "btn-primary");
    g_signal_connect(L->next_btn, "clicked", G_CALLBACK(hw_slide_next), L);
    gtk_box_append(GTK_BOX(nav), L->next_btn);

    L->idx = 0;
    hw_slide_apply(L);
    return page;
}

void hw_slide_apply(NetLesson *L) {
    char name[16];

    if (!L || !L->stack)
        return;
    g_snprintf(name, sizeof(name), "hw_slide%u", L->idx);
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

void hw_slide_prev(GtkButton *button, gpointer data) {
    NetLesson *L = data;
    (void)button;
    if (!L) return;
    if (L->idx > 0) {
        L->idx--;
        hw_slide_apply(L);
    }
}

void hw_slide_next(GtkButton *button, gpointer data) {
    NetLesson *L = data;
    (void)button;
    if (!L) return;
    if (L->idx + 1 < L->n_slides) {
        L->idx++;
        hw_slide_apply(L);
    } else {
        gtk_stack_set_visible_child_name(main_stack, L->ex_page);
    }
}

void hw_lessons_apply_lang(void) {
    for (int i = 0; i < HW_LESSONS; i++)
        hw_slide_apply(&hw_lessons[i]);
}

GtkWidget *build_hw_unit1_page(void) {
    static const NetSlide slides[HW_SLIDES] = {
        {
            "1 / 3   •   Úvod", "Architektura počítače",
            "Architektura popisuje, z jakých částí se počítač skládá "
            "a jak spolu spolupracují.",
            {
                "Architektura počítače je základní model, podle kterého "
                "jsou počítače navrhovány.",
                "Nejznámější model je Von Neumannovo schéma.",
                "Používá se dodnes u většiny běžných počítačů.",
                NULL,
            },
        },
        {
            "2 / 3   •   Osobnost", "John von Neumann",
            "Tip: zapamatuj si jméno – podle něj se jmenuje celé schéma.",
            {
                "John von Neumann byl americký matematik.",
                "Pracoval na poli digitálních počítačů a operační teorie "
                "kvantové mechaniky.",
                "Je autorem Von Neumannovy algebry.",
                "Byl tvůrcem teorie her.",
                "Podílel se na vývoji atomové a vodíkové bomby.",
                NULL,
            },
        },
        {
            "3 / 3   •   Schéma", "Von Neumannovo schéma",
            "ALU = aritmeticko-logická jednotka",
            {
                "Von Neumannovo schéma má pět hlavních částí:",
                "ALU – aritmeticko-logická jednotka (výpočty a logika)",
                "Řadič – řídí činnost ostatních částí",
                "Operační paměť – uchovává program i data",
                "Vstupní jednotka – přivádí data do počítače",
                "Výstupní jednotka – předává výsledky ven",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[0], "hw_unit1", "hw_unit1_sub",
                               slides, HW_SLIDES);
}

/* ---- Quiz --------------------------------------------------------- */

static void hw_mcq_check(GtkButton *button, gpointer data) {
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
        mark_hw_done(ctx->lesson_id);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry_short"));
    }
}

static GtkWidget *build_hw_mcq_page(int lesson_id, const char *back_page,
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
    g_signal_connect(check, "clicked", G_CALLBACK(hw_mcq_check), ctx);

    ctx->feedback = gtk_label_new("");
    gtk_widget_set_halign(ctx->feedback, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(body), ctx->feedback);

    return page;
}

GtkWidget *build_hw_unit1_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo je autorem Von Neumannova schématu?",
         {"Alan Turing", "John von Neumann", "Bill Gates", "Steve Jobs"},
         4, 1},
        {"Čím byl John von Neumann?",
         {"Americký matematik", "Německý fyzik", "Anglický chemik",
          "Francouzský lékař"},
         4, 0},
        {"Co znamená zkratka ALU?",
         {"Automatická logická síť", "Aritmeticko-logická jednotka",
          "Analogová linková sestava", "Adresová listová sekce"},
         4, 1},
        {"Která část Von Neumannova schématu řídí ostatní části?",
         {"Operační paměť", "Vstupní jednotka", "Řadič", "Výstupní jednotka"},
         4, 2},
        {"Kde se podle Von Neumannova schématu uchovává program i data?",
         {"Jen na pevném disku", "V operační paměti", "Jen ve vstupní jednotce",
          "Jen v ALU"},
         4, 1},
        {"Čím se John von Neumann také zabýval?",
         {"Jen malířstvím", "Teorií her", "Jen botanikou", "Jen hudbou"},
         4, 1},
    };
    static const char *hints[] = {
        "John von Neumann – americký matematik",
        "Americký matematik s objevy v digitálních počítačích",
        "ALU = aritmeticko-logická jednotka",
        "Řadič řídí činnost ostatních částí",
        "Operační paměť uchovává program i data",
        "Byl tvůrcem teorie her",
    };
    return build_hw_mcq_page(0, "hwunit1", "hw_ex1_title", "hw_quiz1_head",
                             qs, hints, 6);
}

GtkWidget *build_hw_unit2_page(void) {
    static const NetSlide slides[HW2_SLIDES] = {
        {
            "1 / 6   •   Přehled", "Historie počítačů",
            "Generace se dělí podle klíčové technologie své doby.",
            {
                "1. Předchůdci – do 30. let 19. století",
                "2. Nultá generace – 1938–1944",
                "3. První generace – 1944–1955",
                "4. Druhá generace – 1955–1964",
                "5. Třetí generace – 1964–1971",
                "Další snímky shrnují znaky jednotlivých generací.",
                NULL,
            },
        },
        {
            "2 / 6   •   0. generace", "Počítače 0. generace (1938–1944)",
            "Tip: počítačů bylo málo a často sloužily armádě.",
            {
                "Existovalo jen několik počítačů na světě.",
                "Většinou měly vojenský účel.",
                NULL,
            },
        },
        {
            "3 / 6   •   1. generace", "Počítače 1. generace (1944–1955)",
            "Klíčový objev: elektronka",
            {
                "Objev elektronky umožnil tuto generaci.",
                "Ještě neexistoval software v dnešním smyslu.",
                "Data a programy se zadávaly pomocí děrných štítků.",
                NULL,
            },
        },
        {
            "4 / 6   •   2. generace", "Počítače 2. generace (1955–1964)",
            "Klíčový objev: tranzistor",
            {
                "Objev tranzistoru nahradil elektronky.",
                "Používala se hlavní externí paměť.",
                "Počítače měly menší rozměry než v 1. generaci.",
                NULL,
            },
        },
        {
            "5 / 6   •   3. generace", "Počítače 3. generace (1964–1971)",
            "Klíčový objev: integrovaný obvod",
            {
                "Objev integrovaného obvodu.",
                "Stále se používala hlavní externí paměť.",
                "Rozměry počítačů se dál zmenšovaly.",
                NULL,
            },
        },
        {
            "6 / 6   •   4. generace", "Počítače 4. generace",
            "Klíčové: programovatelné mikroprocesory",
            {
                "Nastupují programovatelné mikroprocesory.",
                "1972 – první počítačová hra",
                "1973 – první disketa",
                "1979 – první CD",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[1], "hw_unit2", "hw_unit2_sub",
                               slides, HW2_SLIDES);
}

GtkWidget *build_hw_unit2_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdy probíhala nultá generace počítačů?",
         {"1938–1944", "1944–1955", "1955–1964", "1964–1971"}, 4, 0},
        {"Co bylo typické pro počítače 0. generace?",
         {"Byly v každé domácnosti", "Jen několik kusů, často vojenský účel",
          "Měly mikroprocesory", "Používaly CD"},
         4, 1},
        {"Která technologie patří k 1. generaci?",
         {"Tranzistor", "Elektronka", "Integrovaný obvod", "Mikroprocesor"},
         4, 1},
        {"Čím se zadávaly data v 1. generaci?",
         {"USB fleškou", "Děrnými štítky", "CD", "Disketou"}, 4, 1},
        {"Která technologie patří k 2. generaci?",
         {"Elektronka", "Tranzistor", "Mikroprocesor", "CD"}, 4, 1},
        {"Která technologie patří k 3. generaci?",
         {"Elektronka", "Tranzistor", "Integrovaný obvod", "Disketa"},
         4, 2},
        {"Co je typické pro 4. generaci?",
         {"Jen děrné štítky", "Programovatelné mikroprocesory",
          "Jen vojenské použití", "Žádný software"},
         4, 1},
        {"Kdy vznikla první počítačová hra (podle lekce)?",
         {"1964", "1972", "1973", "1979"}, 4, 1},
    };
    static const char *hints[] = {
        "Nultá generace: 1938–1944",
        "Jen několik počítačů, většinou vojenský účel",
        "1. generace – objev elektronky",
        "1. generace – děrné štítky",
        "2. generace – objev tranzistoru",
        "3. generace – integrovaný obvod",
        "4. generace – programovatelné mikroprocesory",
        "1972 – první počítačová hra",
    };
    return build_hw_mcq_page(1, "hwunit2", "hw_ex2_title", "hw_quiz2_head",
                             qs, hints, 8);
}

GtkWidget *build_hw_unit3_page(void) {
    static const NetSlide slides[HW3_SLIDES] = {
        {
            "1 / 2   •   Bit", "Bit – nejmenší jednotka informace",
            "Bit = binary digit (dvojková číslice)",
            {
                "Bit je dvojková číslice – nabývá hodnoty 0 nebo 1.",
                "Je to nejmenší zobrazitelná jednotka informace.",
                "Z bitů se skládají větší jednotky (např. byte).",
                NULL,
            },
        },
        {
            "2 / 2   •   Byte", "Byte – adresovatelná jednotka paměti",
            "Tip: 1 B = 8 b",
            {
                "Byte je nejmenší adresovatelná jednotka paměti.",
                "Počítač tedy paměť adresuje po bytech, ne po jednotlivých "
                "bitech.",
                "Jeden byte obvykle obsahuje 8 bitů.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[2], "hw_unit3", "hw_unit3_sub",
                               slides, HW3_SLIDES);
}

GtkWidget *build_hw_unit3_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je bit?",
         {"Největší jednotka paměti", "Dvojková číslice – nejmenší jednotka "
          "informace", "Jen klávesa na klávesnici", "Typ procesoru"},
         4, 1},
        {"Jaké hodnoty může nabývat bit?",
         {"Jen 0", "Jen 1", "0 nebo 1", "0 až 255"}, 4, 2},
        {"Co je byte?",
         {"Nejmenší adresovatelná jednotka paměti",
          "Nejmenší zobrazitelná jednotka informace",
          "Jen síťový kabel", "Jen grafická karta"},
         4, 0},
        {"Kolik bitů má obvykle jeden byte?",
         {"2", "4", "8", "16"}, 4, 2},
    };
    static const char *hints[] = {
        "Bit = dvojková číslice, nejmenší jednotka informace",
        "Bit nabývá hodnoty 0 nebo 1",
        "Byte = nejmenší adresovatelná jednotka paměti",
        "1 B = 8 b",
    };
    return build_hw_mcq_page(2, "hwunit3", "hw_ex3_title", "hw_quiz3_head",
                             qs, hints, 4);
}

GtkWidget *build_hw_unit4_page(void) {
    static const NetSlide slides[HW4_SLIDES] = {
        {
            "1 / 2   •   Integer", "Proměnná typu integer (celá čísla)",
            "Tip: n bitů → 2ⁿ různých hodnot (např. 0 až 2ⁿ − 1)",
            {
                "Celá čísla se ukládají binárně v pevné řádové čárce.",
                "Mají pevný počet bitů – standardně 16 nebo 32 "
                "(tedy 2 nebo 4 byty).",
                "Při 16bitovém formátu je 2¹⁶ = 65 536 různých hodnot; "
                "u nezáporných čísel interval 0 až 2¹⁶ − 1, tedy 0–65 535.",
                "Obecně u n bitů je M = 2ⁿ různých hodnot "
                "(u 16 bitů je nejvyšší nezáporné číslo 65 535).",
                "Ostatní čísla se do tohoto intervalu převádějí transformací "
                "(mapováním na přirozená čísla z daného rozsahu).",
                NULL,
            },
        },
        {
            "2 / 2   •   Real", "Proměnná typu real (desetinná čísla)",
            "Tip: tvar = mantisa × základ^exponent",
            {
                "Desetinná čísla se ukládají v pohyblivé řádové čárce "
                "ve tvaru mantisa a exponent.",
                "Mantisa je normalizovaná tak, že první platná číslice "
                "je hned za desetinnou čárkou.",
                "Posun řádové čárky vyrovná odpovídající změna exponentu.",
                "Standardně zabírá 4 nebo 8 bytů v paměti.",
                "Více bytů se používá jen tehdy, když potřebujeme vyšší "
                "přesnost výpočtů.",
                NULL,
            },
        },
    };

    return build_hw_unit_page(&hw_lessons[3], "hw_unit4", "hw_unit4_sub",
                               slides, HW4_SLIDES);
}

GtkWidget *build_hw_unit4_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak se ukládají celá čísla (integer)?",
         {"Jen jako text",
          "Binárně v pevné řádové čárce s pevným počtem bitů",
          "Jen jako obrázek",
          "Jen v pohyblivé řádové čárce bez exponentu"},
         4, 1},
        {"Kolik různých hodnot má 16bitový nezáporný integer?",
         {"256", "1024", "65 536 (0 až 65 535)", "4"},
         4, 2},
        {"Jak se ukládají desetinná čísla (real)?",
         {"Jen jako celá čísla bez tečky",
          "V pohyblivé řádové čárce jako mantisa a exponent",
          "Jen jako 1 bit",
          "Jen jako název souboru"},
         4, 1},
        {"Co znamená normalizace mantisy?",
         {"Že se číslo smaže",
          "Že první platná číslice je hned za desetinnou čárkou "
          "a exponent se upraví",
          "Že se použije jen 1 bit",
          "Že se číslo uloží jako text"},
         4, 1},
    };
    static const char *hints[] = {
        "Integer = pevná řádová čárka, pevný počet bitů (16/32…)",
        "16 bitů → 2¹⁶ = 65 536 hodnot (0–65 535 u nezáporných)",
        "Real = pohyblivá řádová čárka = mantisa + exponent",
        "Normalizace: 1. platná číslice hned za desetinnou čárkou",
    };
    return build_hw_mcq_page(3, "hwunit4", "hw_ex4_title", "hw_quiz4_head",
                             qs, hints, 4);
}
