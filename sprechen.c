#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib/gstdio.h>
#include <pango/pangocairo.h>
#include <math.h>

#define NUM_UNITS 10
#define FIRST_LOCKED 1
#define NUM_EXERCISES 13

#define PROGRESS_DIR  "progress"
#define PROGRESS_FILE "progress/unit1.conf"

#define NODE_SIZE    88.0
#define PATH_SPAC    240.0

/* Serpentine roadmap layout */
#define ROAD_MX    150.0   /* horizontal canvas margin                 */
#define ROAD_MY    150.0   /* vertical canvas margin                   */
#define ROAD_GAP   240.0   /* vertical space between folded rows       */
#define ROAD_WAVE   44.0   /* wavy vertical offset of the nodes        */

static GtkStack *main_stack;

static gboolean ex_done[NUM_EXERCISES + 1]; /* indexed 1..13 */

static GtkWidget *ex_bubbles[NUM_EXERCISES + 1];
static GtkWidget *ex_done_icons[NUM_EXERCISES + 1];
static GtkWidget *unit1_node;
static GtkWidget *unit1_done_icon;

/* Adaptive serpentine roadmap geometry */
static GtkWidget *road_scroll;
static GtkWidget *road_fixed;
static GtkWidget *road_rail;
static GtkWidget *road_nodes[NUM_UNITS];
static GtkWidget *road_labels[NUM_UNITS];
static double road_cx[NUM_UNITS];
static double road_cy[NUM_UNITS];
static int road_cols = NUM_UNITS;
static int road_rows = 1;
static int road_cw = (int)(2.0 * ROAD_MX + (NUM_UNITS - 1) * PATH_SPAC);
static int road_ch = (int)(2.0 * ROAD_MY);

static const char *unit_names[] = {
    "Neue Freunde",
    "Aus aller Welt",
    "Bei uns zu Hause",
    "Schule und Freizeit",
    "Guten Appetit!",
    "Mein Tagesablauf",
    "Meine Freunde",
    "Wir treffen uns in Salzburg",
    "Mein Haus ist meine Burg",
    "Urlaub in Österreich",
};

static const char *ex_names[NUM_EXERCISES + 1] = {
    NULL,
    "Dialog",
    "Sätze bilden",
    "Was ist richtig?",
    "Freie Antwort",
    "Zahlen",
    "Wie viel?",
    "Zahlenreihe",
    "Verb einsetzen",
    "Wer? Wie? Wo?",
    "Wörter trennen",
    "Grußformen",
    "Was macht er/sie?",
    "Länder",
};

/* Scattered bubble positions on the unit-1 map, as fractions of the
 * design box (SCATTER_NAT_W x SCATTER_NAT_H); the whole box is scaled
 * uniformly and centered within the available area. */
static const double scatter_pos[NUM_EXERCISES + 1][2] = {
    {0.0, 0.0},
    {0.100, 0.233},
    {0.271, 0.105},
    {0.457, 0.209},
    {0.643, 0.093},
    {0.843, 0.221},
    {0.186, 0.488},
    {0.400, 0.430},
    {0.586, 0.500},
    {0.771, 0.442},
    {0.100, 0.744},
    {0.307, 0.767},
    {0.514, 0.756},
    {0.714, 0.779},
};

typedef struct { double r, g, b; } Rgb;

static Rgb color_from_hex(unsigned int hex) {
    Rgb c;
    c.r = (double)((hex >> 16) & 0xFF) / 255.0;
    c.g = (double)((hex >> 8) & 0xFF) / 255.0;
    c.b = (double)(hex & 0xFF) / 255.0;
    return c;
}

/* Vector icons drawn with cairo so they never depend on the system
 * icon theme (which is missing on some installs). */

static void draw_check_icon(GtkDrawingArea *area, cairo_t *cr,
                            int width, int height, gpointer data) {
    Rgb *c = data;

    (void)area;
    cairo_set_source_rgb(cr, c->r, c->g, c->b);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
    cairo_set_line_width(cr, width * 0.16);
    cairo_move_to(cr, width * 0.24, height * 0.56);
    cairo_line_to(cr, width * 0.43, height * 0.74);
    cairo_line_to(cr, width * 0.78, height * 0.30);
    cairo_stroke(cr);
}

/* Paint a padlock centred in the size x size box with top-left (x0,y0). */
static void lock_paint(cairo_t *cr, double x0, double y0, double size,
                       const Rgb *c) {
    double sl = MAX(1.6, size * 0.13);   /* shackle stroke width          */
    double bw = size * 0.60;             /* body width                    */
    double bh = size * 0.44;             /* body height                   */
    double cx = x0 + size / 2.0;
    double body_top = y0 + size - bh - size * 0.04;
    double bx = cx - bw / 2.0;
    double r = bw / 2.0 - sl / 2.0;      /* shackle arc radius            */

    cairo_set_source_rgb(cr, c->r, c->g, c->b);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_width(cr, sl);

    cairo_new_path(cr);
    cairo_arc(cr, cx, body_top, r, G_PI, 2.0 * G_PI);
    cairo_stroke(cr);

    cairo_new_path(cr);
    cairo_rectangle(cr, bx, body_top, bw, bh);
    cairo_fill(cr);

    cairo_new_path(cr);
    cairo_arc(cr, cx, body_top + bh * 0.42, size * 0.07, 0.0, 2.0 * G_PI);
    cairo_set_source_rgb(cr, 0.1216, 0.1255, 0.1804); /* #1f202e keyhole */
    cairo_fill(cr);
}

static void draw_lock_icon(GtkDrawingArea *area, cairo_t *cr,
                           int width, int height, gpointer data) {
    (void)area;
    lock_paint(cr, 0.0, 0.0, MIN(width, height), data);
}

static void draw_back_icon(GtkDrawingArea *area, cairo_t *cr,
                           int width, int height, gpointer data) {
    const double size = 15.0;      /* fixed logical icon size            */
    const double stroke = 1.8;
    double ox = (width - size) / 2.0;
    double oy = (height - size) / 2.0;
    GtkWidget *parent = gtk_widget_get_parent(GTK_WIDGET(area));
    GdkRGBA color;

    (void)data;
    gtk_style_context_get_color(gtk_widget_get_style_context(parent), &color);
    cairo_set_source_rgb(cr, color.red, color.green, color.blue);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
    cairo_set_line_width(cr, stroke);
    cairo_move_to(cr, ox + size * 0.72, oy + size * 0.24);
    cairo_line_to(cr, ox + size * 0.30, oy + size * 0.50);
    cairo_line_to(cr, ox + size * 0.72, oy + size * 0.76);
    cairo_stroke(cr);
}

static GtkWidget *icon_area_new(GtkDrawingAreaDrawFunc fn,
                                double r, double g, double b, int px) {
    Rgb *col = g_new(Rgb, 1);
    GtkWidget *d = gtk_drawing_area_new();

    col->r = r;
    col->g = g;
    col->b = b;
    gtk_widget_set_size_request(d, px, px);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(d), fn,
                                   col, (GDestroyNotify)g_free);
    return d;
}

/* ------------------------------------------------------------------ */
/* Utilities                                                          */
/* ------------------------------------------------------------------ */

static char *normalize_answer(const char *input) {
    GString *out = g_string_new(NULL);
    const gchar *down = g_utf8_strdown(input, -1);
    const gchar *p = down;
    gboolean prev_space = FALSE;

    while (*p) {
        gunichar ch = g_utf8_get_char(p);
        p = g_utf8_next_char(p);
        if (g_unichar_isspace(ch)) {
            if (out->len > 0 && !prev_space)
                g_string_append_c(out, ' ');
            prev_space = TRUE;
            continue;
        }
        prev_space = FALSE;
        switch (ch) {
            case 0x00e4: g_string_append(out, "ae"); break; /* ä */
            case 0x00f6: g_string_append(out, "oe"); break; /* ö */
            case 0x00fc: g_string_append(out, "ue"); break; /* ü */
            case 0x00df: g_string_append(out, "ss"); break; /* ß */
            default:     g_string_append_unichar(out, ch); break;
        }
    }

    if (out->len > 0 && out->str[out->len - 1] == ' ')
        g_string_truncate(out, out->len - 1);

    g_free((gpointer)down);
    return g_string_free(out, FALSE);
}

static void shuffle_indices(int *arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = g_random_int_range(0, i + 1);
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

static void flow_clear(GtkFlowBox *fb) {
    GtkWidget *c = gtk_widget_get_first_child(GTK_WIDGET(fb));
    while (c) {
        GtkWidget *next = gtk_widget_get_next_sibling(c);
        gtk_flow_box_remove(fb, c);
        c = next;
    }
}

static void answer_mark(GtkWidget *widget, gboolean ok) {
    if (ok) {
        gtk_widget_remove_css_class(widget, "answer-wrong");
        gtk_widget_add_css_class(widget, "answer-ok");
    } else {
        gtk_widget_remove_css_class(widget, "answer-ok");
        gtk_widget_add_css_class(widget, "answer-wrong");
    }
}

static void set_feedback(GtkWidget *label, gboolean ok, const char *text) {
    gtk_widget_remove_css_class(label, "feedback-ok");
    gtk_widget_remove_css_class(label, "feedback-err");
    gtk_widget_add_css_class(label, ok ? "feedback-ok" : "feedback-err");
    gtk_label_set_text(GTK_LABEL(label), text);
}

/* ------------------------------------------------------------------ */
/* Progress                                                           */
/* ------------------------------------------------------------------ */

static void save_progress(void) {
    GKeyFile *kf = g_key_file_new();
    for (int i = 1; i <= NUM_EXERCISES; i++) {
        gchar key[8];
        g_snprintf(key, sizeof(key), "%d", i);
        g_key_file_set_boolean(kf, "done", key, ex_done[i]);
    }

    if (g_mkdir_with_parents(PROGRESS_DIR, 0755) != 0) {
        g_key_file_free(kf);
        return;
    }

    gchar *data = g_key_file_to_data(kf, NULL, NULL);
    g_key_file_free(kf);

    if (data) {
        GError *err = NULL;
        g_file_set_contents(PROGRESS_FILE, data, -1, &err);
        if (err)
            g_error_free(err);
        g_free(data);
    }
}

static void load_progress(void) {
    GKeyFile *kf = g_key_file_new();
    GError *err = NULL;

    if (!g_key_file_load_from_file(kf, PROGRESS_FILE, G_KEY_FILE_NONE, &err)) {
        if (err)
            g_error_free(err);
        g_key_file_free(kf);
        return;
    }

    for (int i = 1; i <= NUM_EXERCISES; i++) {
        gchar key[8];
        g_snprintf(key, sizeof(key), "%d", i);
        ex_done[i] = g_key_file_get_boolean(kf, "done", key, NULL);
    }

    g_key_file_free(kf);
}

static void refresh_completion_ui(void) {
    gboolean all = TRUE;

    for (int i = 1; i <= NUM_EXERCISES; i++) {
        if (!ex_done[i])
            all = FALSE;
        if (ex_bubbles[i]) {
            if (ex_done[i]) {
                gtk_widget_add_css_class(ex_bubbles[i], "done");
                gtk_widget_set_visible(ex_done_icons[i], TRUE);
            } else {
                gtk_widget_remove_css_class(ex_bubbles[i], "done");
                gtk_widget_set_visible(ex_done_icons[i], FALSE);
            }
        }
    }

    if (unit1_node) {
        if (all) {
            gtk_widget_remove_css_class(unit1_node, "current");
            gtk_widget_add_css_class(unit1_node, "done");
            gtk_widget_set_visible(unit1_done_icon, TRUE);
        } else {
            gtk_widget_remove_css_class(unit1_node, "done");
            gtk_widget_add_css_class(unit1_node, "current");
            gtk_widget_set_visible(unit1_done_icon, FALSE);
        }
    }
}

static void mark_done(int n) {
    if (n < 1 || n > NUM_EXERCISES)
        return;
    ex_done[n] = TRUE;
    refresh_completion_ui();
    save_progress();
}

/* ------------------------------------------------------------------ */
/* Navigation                                                         */
/* ------------------------------------------------------------------ */

static void on_nav_clicked(GtkButton *button, gpointer user_data) {
    const char *target = g_object_get_data(G_OBJECT(button), "target");
    (void)user_data;
    if (target)
        gtk_stack_set_visible_child_name(main_stack, target);
}

static void on_continue_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    gtk_stack_set_visible_child_name(main_stack, "roadmap");
}

static GtkWidget *make_back_button(const char *target) {
    GtkWidget *back = gtk_button_new();
    GtkWidget *icon = gtk_drawing_area_new();

    gtk_widget_set_size_request(icon, 14, 14);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(icon), draw_back_icon,
                                   NULL, NULL);
    gtk_button_set_child(GTK_BUTTON(back), icon);
    gtk_widget_add_css_class(back, "back-btn");
    gtk_widget_set_valign(back, GTK_ALIGN_CENTER);
    gtk_widget_set_tooltip_text(back, "Zpět");
    g_signal_connect_swapped(back, "state-flags-changed",
                             G_CALLBACK(gtk_widget_queue_draw), icon);
    g_object_set_data_full(G_OBJECT(back), "target", g_strdup(target), g_free);
    g_signal_connect(back, "clicked", G_CALLBACK(on_nav_clicked), NULL);
    return back;
}

static GtkWidget *top_bar(const char *back_target, const char *title,
                          const char *subtitle) {
    GtkWidget *top = gtk_center_box_new();
    gtk_widget_set_margin_top(top, 2);
    gtk_widget_set_margin_bottom(top, 6);

    GtkWidget *back = make_back_button(back_target);
    gtk_center_box_set_start_widget(GTK_CENTER_BOX(top), back);

    GtkWidget *center = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_widget_set_valign(center, GTK_ALIGN_CENTER);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(top), center);

    GtkWidget *heading = gtk_label_new(title);
    gtk_widget_set_halign(heading, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(heading, "roadmap-title");
    gtk_box_append(GTK_BOX(center), heading);

    if (subtitle) {
        GtkWidget *sub = gtk_label_new(subtitle);
        gtk_widget_set_halign(sub, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(sub, "roadmap-sub");
        gtk_label_set_wrap(GTK_LABEL(sub), TRUE);
        gtk_label_set_max_width_chars(GTK_LABEL(sub), 60);
        gtk_box_append(GTK_BOX(center), sub);
    }

    return top;
}

static gboolean on_window_key_pressed(GtkEventControllerKey *controller,
                                      guint keyval, guint keycode,
                                      GdkModifierType state,
                                      gpointer user_data) {
    GtkWindow *window = user_data;

    (void)controller;
    (void)keycode;

    if ((keyval == GDK_KEY_q &&
         (state & (GDK_SUPER_MASK | GDK_META_MASK)) != 0) ||
        (keyval == GDK_KEY_F4 &&
         (state & GDK_ALT_MASK) != 0)) {
        GtkApplication *app = gtk_window_get_application(window);
        if (app != NULL)
            g_application_quit(G_APPLICATION(app));
        return GDK_EVENT_STOP;
    }

    return GDK_EVENT_PROPAGATE;
}

/* ------------------------------------------------------------------ */
/* Welcome page                                                       */
/* ------------------------------------------------------------------ */

static GtkWidget *build_welcome_page(void) {
    GtkWidget *box;
    GtkWidget *heading;
    GtkWidget *card;
    GtkWidget *button;

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(box, 40);
    gtk_widget_set_margin_bottom(box, 40);

    heading = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(heading),
        "Vítejte ve <span color=\"#cba6f7\">Sprechen.C</span>!");
    gtk_widget_set_halign(heading, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(heading, "heading");
    gtk_box_append(GTK_BOX(box), heading);

    card = gtk_label_new(
        "Sprechen.C je vzdělávací program na procvičování Němčiny, určený "
        "pro studenty středních škol a gymnázií.\n\n"
        "Program je napsaný v Céčku studentama ze SSŠVT!");
    gtk_label_set_justify(GTK_LABEL(card), GTK_JUSTIFY_CENTER);
    gtk_label_set_wrap(GTK_LABEL(card), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(card), 48);
    gtk_widget_add_css_class(card, "card");
    gtk_box_append(GTK_BOX(box), card);

    button = gtk_button_new_with_label("Pokračuj");
    gtk_widget_set_halign(button, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(button, 6);
    gtk_widget_add_css_class(button, "btn-primary");
    gtk_box_append(GTK_BOX(box), button);

    g_signal_connect(button, "clicked", G_CALLBACK(on_continue_clicked), NULL);

    return box;
}

/* ------------------------------------------------------------------ */
/* Roadmap page                                                       */
/* ------------------------------------------------------------------ */

static void draw_node_halo(cairo_t *cr, double cx, double cy,
                           double radius, double r, double g, double b,
                           double alpha) {
    cairo_pattern_t *grad;
    grad = cairo_pattern_create_radial(cx, cy, 6.0, cx, cy, radius);
    cairo_pattern_add_color_stop_rgba(grad, 0.0, r, g, b, alpha);
    cairo_pattern_add_color_stop_rgba(grad, 0.55, r, g, b, alpha * 0.30);
    cairo_pattern_add_color_stop_rgba(grad, 1.0, r, g, b, 0.0);
    cairo_set_source(cr, grad);
    cairo_paint(cr);
    cairo_pattern_destroy(grad);
}

/* Draw the checkered "finish line" node: checkerboard behind the unit
 * number and the same lock glyph the locked units use. */
static void draw_finish_cell(GtkDrawingArea *area, cairo_t *cr,
                             int width, int height, gpointer data) {
    const double s = 11.0;
    const Rgb gray = {0.4235, 0.4392, 0.5255}; /* #6c7086 */
    int n = GPOINTER_TO_INT(data);
    double cx = width / 2.0;
    double cy = height / 2.0;
    double r = MIN(width, height) / 2.0 - 2.0;
    double start_x = cx - r;
    double start_y = cy - r;
    int cols = (int)ceil(2.0 * r / s);
    int rows = (int)ceil(2.0 * r / s);
    char buf[8];
    PangoLayout *layout;
    PangoFontDescription *fd;
    PangoRectangle ink;
    double num_cy;
    int ix, iy;

    (void)area;

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

    cairo_save(cr);
    cairo_new_path(cr);
    cairo_arc(cr, cx, cy, r, 0.0, 2.0 * G_PI);
    cairo_clip(cr);

    cairo_rectangle(cr, start_x, start_y, 2.0 * r, 2.0 * r);
    cairo_set_source_rgb(cr, 0.1216, 0.1255, 0.1804); /* #1f202e */
    cairo_fill(cr);

    for (iy = 0; iy < rows; iy++) {
        for (ix = 0; ix < cols; ix++) {
            if (((ix + iy) & 1) == 0)
                continue;
            cairo_rectangle(cr, start_x + ix * s, start_y + iy * s, s, s);
            cairo_set_source_rgb(cr, 0.2314, 0.2431, 0.3451); /* #3b3e58 */
            cairo_fill(cr);
        }
    }

    /* unit number, styled like the other locked nodes */
    g_snprintf(buf, sizeof(buf), "%d", n);
    layout = pango_cairo_create_layout(cr);
    fd = pango_font_description_new();
    pango_font_description_set_family(fd, "sans");
    pango_font_description_set_weight(fd, PANGO_WEIGHT_ULTRABOLD);
    pango_font_description_set_size(fd, 22 * PANGO_SCALE);
    pango_layout_set_font_description(layout, fd);
    pango_layout_set_text(layout, buf, -1);
    pango_layout_get_pixel_extents(layout, &ink, NULL);

    cairo_set_source_rgb(cr, gray.r, gray.g, gray.b);
    num_cy = cy - 3.0;
    cairo_move_to(cr, cx - ink.width / 2.0 - ink.x,
                  num_cy - ink.height / 2.0 - ink.y);
    pango_cairo_show_layout(cr, layout);

    /* lock glyph below the number, as on the other locked units */
    lock_paint(cr, cx - 8.0, num_cy + ink.height / 2.0 + 6.0, 16.0, &gray);

    g_object_unref(layout);
    pango_font_description_free(fd);
    cairo_restore(cr);

    cairo_arc(cr, cx, cy, r, 0.0, 2.0 * G_PI);
    cairo_set_source_rgb(cr, 0.3216, 0.3333, 0.4196);
    cairo_set_line_width(cr, 2.0);
    cairo_stroke(cr);
}

static void road_point(double t, double *ox, double *oy) {
    int k = (int)t;
    double u = t - (double)k;
    double u2, u3;
    double p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y;

    if (k < 0) { k = 0; u = 0.0; }
    if (k >= NUM_UNITS - 1) { k = NUM_UNITS - 2; u = 1.0; }

    p1x = road_cx[k];     p1y = road_cy[k];
    p2x = road_cx[k + 1]; p2y = road_cy[k + 1];
    if (k - 1 >= 0) { p0x = road_cx[k - 1]; p0y = road_cy[k - 1]; }
    else            { p0x = p1x - (p2x - p1x); p0y = p1y - (p2y - p1y); }
    if (k + 2 < NUM_UNITS) { p3x = road_cx[k + 2]; p3y = road_cy[k + 2]; }
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

static void road_path(cairo_t *cr, double t0, double t1) {
    const double steps_per_unit = 48.0;
    int n = (int)((t1 - t0) * steps_per_unit);
    double x, y;
    int s;

    if (n < 1)
        n = 1;
    road_point(t0, &x, &y);
    cairo_move_to(cr, x, y);
    for (s = 1; s <= n; s++) {
        road_point(t0 + (t1 - t0) * (double)s / (double)n, &x, &y);
        cairo_line_to(cr, x, y);
    }
}

/* Compute the serpentine node layout for the given available width. */
static void roadmap_geometry(double avail) {
    const double phase = 2.0 * G_PI * 1.7 / (double)(NUM_UNITS - 1);
    int rows;
    int r, c, i;

    for (rows = 1; rows <= NUM_UNITS; rows++) {
        int cols = (NUM_UNITS + rows - 1) / rows;
        if (2.0 * ROAD_MX + (cols - 1) * PATH_SPAC <= avail + 1.0)
            break;
    }
    if (rows > NUM_UNITS)
        rows = NUM_UNITS;
    road_rows = rows;
    road_cols = (NUM_UNITS + rows - 1) / rows;
    if (road_cols < 1)
        road_cols = 1;
    road_cw = (int)(2.0 * ROAD_MX + (road_cols - 1) * PATH_SPAC);
    road_ch = (int)(2.0 * ROAD_MY + (rows - 1) * ROAD_GAP);

    for (r = 0; r < rows; r++) {
        int base = r * road_cols;
        int len = MIN(road_cols, NUM_UNITS - base);
        int fwd = (r % 2) == 0;
        for (c = 0; c < len; c++) {
            i = base + c;
            int cc = fwd ? c : (road_cols - 1 - c);
            road_cx[i] = ROAD_MX + cc * PATH_SPAC;
            road_cy[i] = ROAD_MY + r * ROAD_GAP
                         + ROAD_WAVE * sin((double)i * phase);
        }
    }
}

static void road_apply_layout(void) {
    int i;

    if (!road_fixed)
        return;

    for (i = 0; i < NUM_UNITS; i++) {
        if (!road_nodes[i] || !road_labels[i])
            continue;
        gtk_fixed_move(GTK_FIXED(road_fixed), road_nodes[i],
                       (int)(road_cx[i] - NODE_SIZE / 2.0),
                       (int)(road_cy[i] - NODE_SIZE / 2.0));
        gtk_fixed_move(GTK_FIXED(road_fixed), road_labels[i],
                       (int)(road_cx[i] - (PATH_SPAC - 20.0) / 2.0),
                       (int)(road_cy[i] + NODE_SIZE / 2.0 + 10.0));
    }

    gtk_widget_set_size_request(road_fixed, road_cw, road_ch);
    gtk_widget_set_size_request(road_rail, road_cw, road_ch);
    gtk_fixed_move(GTK_FIXED(road_fixed), road_rail, 0, 0);
    gtk_widget_queue_draw(road_rail);
}

static void road_relayout(void) {
    GtkAdjustment *hadj;
    double avail;

    if (!road_scroll)
        return;
    hadj = gtk_scrolled_window_get_hadjustment(
        GTK_SCROLLED_WINDOW(road_scroll));
    avail = gtk_adjustment_get_page_size(hadj);
    if (avail < 1.0)
        return;
    roadmap_geometry(avail);
    road_apply_layout();
}

static guint road_idle;

static gboolean road_relayout_idle(gpointer data) {
    (void)data;
    road_idle = 0;
    road_relayout();
    return G_SOURCE_REMOVE;
}

static void road_relayout_later(void) {
    if (road_idle == 0)
        road_idle = g_idle_add(road_relayout_idle, NULL);
}

static void road_adjust_notify(GtkAdjustment *adj, GParamSpec *ps,
                               gpointer data) {
    (void)adj;
    (void)ps;
    (void)data;
    road_relayout_later();
}

static void draw_rail(GtkDrawingArea *area, cairo_t *cr,
                      int width, int height, gpointer user_data) {
    const double t_lit = (double)(FIRST_LOCKED - 1);
    const double t_end = (double)(NUM_UNITS - 1);
    const double x0 = ROAD_MX;
    const double x1 = (double)road_cw - ROAD_MX;
    const Rgb mauve = color_from_hex(0xcba6f7);
    const Rgb blue = color_from_hex(0x89b4fa);
    double hx, hy;

    (void)area;
    (void)width;
    (void)height;
    (void)user_data;

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

    road_point((double)(FIRST_LOCKED - 1), &hx, &hy);
    draw_node_halo(cr, hx, hy, NODE_SIZE * 1.05,
                   mauve.r, mauve.g, mauve.b, 0.18);

    cairo_new_path(cr);
    road_path(cr, 0.0, t_end);
    cairo_set_line_width(cr, 16);
    cairo_set_source_rgb(cr, 0.153, 0.157, 0.235);
    cairo_stroke(cr);

    if (t_lit > 0.0) {
        cairo_pattern_t *grad;

        cairo_new_path(cr);
        road_path(cr, 0.0, t_lit);

        grad = cairo_pattern_create_linear(x0, 0.0, x1, 0.0);
        cairo_pattern_add_color_stop_rgba(grad, 0.0,
                                          mauve.r, mauve.g, mauve.b, 0.16);
        cairo_pattern_add_color_stop_rgba(grad, 1.0,
                                          blue.r, blue.g, blue.b, 0.16);
        cairo_set_source(cr, grad);
        cairo_set_line_width(cr, 34);
        cairo_stroke_preserve(cr);
        cairo_pattern_destroy(grad);

        grad = cairo_pattern_create_linear(x0, 0.0, x1, 0.0);
        cairo_pattern_add_color_stop_rgba(grad, 0.0,
                                          mauve.r, mauve.g, mauve.b, 1.0);
        cairo_pattern_add_color_stop_rgba(grad, 1.0,
                                          blue.r, blue.g, blue.b, 1.0);
        cairo_set_source(cr, grad);
        cairo_set_line_width(cr, 16);
        cairo_stroke_preserve(cr);
        cairo_pattern_destroy(grad);
    }

    {
        double start_t = t_lit > 0.0 ? t_lit : 0.0;

        cairo_set_source_rgba(cr, 0.651, 0.678, 0.784, 0.17);
        cairo_set_dash(cr, (double[]){1.0, 26.0}, 2, 0.0);
        cairo_new_path(cr);
        road_path(cr, start_t, t_end);
        cairo_stroke(cr);
        cairo_set_dash(cr, NULL, 0, 0.0);
    }
}

static void add_path_node(GtkFixed *fixed, int index) {
    int num = index + 1;
    gboolean done = index < FIRST_LOCKED - 1;
    gboolean current = index == FIRST_LOCKED - 1;
    gboolean locked = index >= FIRST_LOCKED;
    GtkWidget *card;
    GtkWidget *name;
    char *text;

    card = gtk_button_new();
    gtk_widget_add_css_class(card, "unit-node");
    gtk_widget_set_can_focus(card, FALSE);
    gtk_widget_set_sensitive(card, !locked);
    gtk_widget_set_size_request(card, (int)NODE_SIZE, (int)NODE_SIZE);

    if (index == NUM_UNITS - 1) {
        GtkWidget *fin = gtk_drawing_area_new();

        gtk_widget_add_css_class(card, "finish");
        gtk_widget_add_css_class(card, "locked");
        gtk_widget_set_size_request(fin, (int)NODE_SIZE, (int)NODE_SIZE);
        gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(fin),
                                       draw_finish_cell,
                                       GINT_TO_POINTER(num), NULL);
        gtk_button_set_child(GTK_BUTTON(card), fin);
    } else {
        GtkWidget *vbox;
        GtkWidget *number;

        if (done)
            gtk_widget_add_css_class(card, "done");
        else if (current)
            gtk_widget_add_css_class(card, "current");
        else
            gtk_widget_add_css_class(card, "locked");

        vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
        gtk_button_set_child(GTK_BUTTON(card), vbox);

        text = g_strdup_printf("%d", num);
        number = gtk_label_new(text);
        g_free(text);
        gtk_widget_add_css_class(number, "unit-number");
        gtk_box_append(GTK_BOX(vbox), number);

        if (done) {
            GtkWidget *icon;
            icon = icon_area_new(draw_check_icon,
                                 0.1176, 0.1176, 0.1804, 18);
            gtk_box_append(GTK_BOX(vbox), icon);
        } else if (locked) {
            GtkWidget *lock;
            lock = icon_area_new(draw_lock_icon,
                                 0.3451, 0.3569, 0.4392, 16);
            gtk_box_append(GTK_BOX(vbox), lock);
        } else if (index == 0) {
            GtkWidget *icon;
            icon = icon_area_new(draw_check_icon,
                                 0.1176, 0.1176, 0.1804, 18);
            gtk_widget_set_visible(icon, FALSE);
            gtk_box_append(GTK_BOX(vbox), icon);
            unit1_done_icon = icon;
            unit1_node = card;
            g_object_set_data_full(G_OBJECT(card), "target",
                                   g_strdup("unit1"), g_free);
            g_signal_connect(card, "clicked", G_CALLBACK(on_nav_clicked), NULL);
        }
    }

    road_nodes[index] = card;
    gtk_fixed_put(fixed, card, 0, 0);

    name = gtk_label_new(unit_names[index]);
    gtk_widget_set_size_request(name, (int)(PATH_SPAC - 20.0), -1);
    gtk_widget_set_halign(name, GTK_ALIGN_CENTER);
    gtk_label_set_justify(GTK_LABEL(name), GTK_JUSTIFY_CENTER);
    gtk_label_set_wrap(GTK_LABEL(name), TRUE);
    gtk_widget_add_css_class(name, "unit-name");
    if (locked)
        gtk_widget_add_css_class(name, "unit-name-locked");
    road_labels[index] = name;
    gtk_fixed_put(fixed, name, 0, 0);
}

static GtkWidget *build_roadmap_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *fixed;
    GtkWidget *rail;
    GtkAdjustment *ha;
    GtkAdjustment *va;
    int i;

    roadmap_geometry(1000.0);

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("welcome", "Učební plán",
                           "Vyberte jednotku na cestě a začněte procvičovat."));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);
    road_scroll = scroll;

    wrap = gtk_center_box_new();
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), wrap);

    fixed = gtk_fixed_new();
    gtk_widget_set_halign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(fixed, road_cw, road_ch);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), fixed);
    road_fixed = fixed;

    rail = gtk_drawing_area_new();
    gtk_widget_set_size_request(rail, road_cw, road_ch);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(rail), draw_rail, NULL, NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rail, 0, 0);
    road_rail = rail;

    for (i = 0; i < NUM_UNITS; i++)
        add_path_node(GTK_FIXED(fixed), i);

    ha = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scroll));
    va = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));
    g_signal_connect(ha, "notify::page-size",
                     G_CALLBACK(road_adjust_notify), NULL);
    g_signal_connect(va, "notify::page-size",
                     G_CALLBACK(road_adjust_notify), NULL);

    road_relayout_later();

    return page;
}


/* ------------------------------------------------------------------ */
/* Scatter layout (scales a centered design box to the window)         */
/* ------------------------------------------------------------------ */

#define SCATTER_MIN_W 684
#define SCATTER_MIN_H 396
#define SCATTER_NAT_W 760
#define SCATTER_NAT_H 440

typedef struct {
    GtkWidget parent_instance;
    GtkWidget *children[NUM_EXERCISES + 1];
} ScatterBox;

typedef struct {
    GtkWidgetClass parent_class;
} ScatterBoxClass;

G_DEFINE_TYPE(ScatterBox, scatter_box, GTK_TYPE_WIDGET)

#define SCATTER_BOX(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), scatter_box_get_type(), ScatterBox))

static void scatter_box_measure(GtkWidget *widget, GtkOrientation orientation,
                                int for_size, int *minimum, int *natural,
                                int *minimum_baseline, int *natural_baseline) {
    (void)widget;
    (void)for_size;
    if (orientation == GTK_ORIENTATION_HORIZONTAL) {
        *minimum = SCATTER_MIN_W;
        *natural = SCATTER_MIN_W;
    } else {
        *minimum = SCATTER_MIN_H;
        *natural = SCATTER_MIN_H;
    }
    *minimum_baseline = -1;
    *natural_baseline = -1;
}

static void scatter_box_size_allocate(GtkWidget *widget, int width, int height,
                                      int baseline) {
    ScatterBox *self = SCATTER_BOX(widget);
    double scale = MIN((double)width / SCATTER_NAT_W,
                       (double)height / SCATTER_NAT_H);
    double mw, mh, ox, oy;

    mw = SCATTER_NAT_W * scale;
    mh = SCATTER_NAT_H * scale;
    ox = (width - mw) / 2.0;
    oy = (height - mh) / 2.0;

    for (int i = 1; i <= NUM_EXERCISES; i++) {
        GtkWidget *child = self->children[i];
        GtkRequisition req;
        GtkAllocation alloc;

        if (!child)
            continue;

        gtk_widget_get_preferred_size(child, &req, NULL);
        alloc.x = (int)(ox + scatter_pos[i][0] * mw) - req.width / 2;
        alloc.y = (int)(oy + scatter_pos[i][1] * mh) - 32;
        alloc.width = req.width;
        alloc.height = req.height;
        gtk_widget_size_allocate(child, &alloc, baseline);
    }
}

static void scatter_box_init(ScatterBox *self) {
    for (int i = 0; i <= NUM_EXERCISES; i++)
        self->children[i] = NULL;
}

static void scatter_box_class_init(ScatterBoxClass *klass) {
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    widget_class->measure = scatter_box_measure;
    widget_class->size_allocate = scatter_box_size_allocate;
}

/* ------------------------------------------------------------------ */
/* Unit 1 bubble map (scattered)                                      */
/* ------------------------------------------------------------------ */

static GtkWidget *make_bubble(int n) {
    GtkWidget *cell = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *btn;
    GtkWidget *vbox;
    GtkWidget *num_label;
    GtkWidget *icon;
    GtkWidget *name_label;
    char *num;
    char *target;

    gtk_widget_set_halign(cell, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(cell, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(cell, 120, -1);

    btn = gtk_button_new();
    gtk_widget_add_css_class(btn, "ex-bubble");
    gtk_widget_set_size_request(btn, 64, 64);
    if (ex_done[n])
        gtk_widget_add_css_class(btn, "done");

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
    gtk_button_set_child(GTK_BUTTON(btn), vbox);

    num = g_strdup_printf("%d", n);
    num_label = gtk_label_new(num);
    g_free(num);
    gtk_widget_add_css_class(num_label, "bubble-number");
    gtk_box_append(GTK_BOX(vbox), num_label);

    icon = icon_area_new(draw_check_icon, 0.1176, 0.1176, 0.1804, 16);
    gtk_widget_set_visible(icon, ex_done[n]);
    gtk_box_append(GTK_BOX(vbox), icon);

    gtk_box_append(GTK_BOX(cell), btn);

    name_label = gtk_label_new(ex_names[n]);
    gtk_widget_set_halign(name_label, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(name_label, "ex-label");
    gtk_label_set_wrap(GTK_LABEL(name_label), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(name_label), 12);
    gtk_box_append(GTK_BOX(cell), name_label);

    ex_bubbles[n] = btn;
    ex_done_icons[n] = icon;

    target = g_strdup_printf("ex%d", n);
    g_object_set_data_full(G_OBJECT(btn), "target", target, g_free);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_nav_clicked), NULL);

    return cell;
}

static GtkWidget *build_unit1_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *scatter;
    int i;

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("roadmap", "Neue Freunde",
                           "Vyberte cvičení a dokončete je."));

    scatter = g_object_new(scatter_box_get_type(), NULL);
    gtk_widget_set_hexpand(scatter, TRUE);
    gtk_widget_set_vexpand(scatter, TRUE);

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_widget_set_margin_bottom(scroll, 14);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), scatter);
    gtk_box_append(GTK_BOX(page), scroll);

    for (i = 1; i <= NUM_EXERCISES; i++) {
        GtkWidget *cell = make_bubble(i);
        SCATTER_BOX(scatter)->children[i] = cell;
        gtk_widget_set_parent(cell, scatter);
    }

    return page;
}

/* ------------------------------------------------------------------ */
/* Exercise page shell                                                */
/* ------------------------------------------------------------------ */

static GtkWidget *ex_page_shell(const char *title, const char *subtitle,
                                const char *btn_label,
                                GtkWidget **body_out, GtkWidget **feedback_out,
                                GtkWidget **btn_out) {
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *scroll;
    GtkWidget *body;
    GtkWidget *bottom;
    GtkWidget *feedback;
    GtkWidget *btn;

    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page), top_bar("unit1", title, subtitle));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_widget_set_margin_bottom(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);

    body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_widget_set_vexpand(body, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), body);
    *body_out = body;

    bottom = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_margin_top(bottom, 4);
    gtk_box_append(GTK_BOX(page), bottom);

    feedback = gtk_label_new("");
    gtk_widget_set_halign(feedback, GTK_ALIGN_START);
    gtk_widget_set_valign(feedback, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(feedback, TRUE);
    gtk_box_append(GTK_BOX(bottom), feedback);
    *feedback_out = feedback;

    btn = gtk_button_new_with_label(btn_label);
    gtk_widget_add_css_class(btn, "btn-primary");
    gtk_widget_set_valign(btn, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(bottom), btn);
    *btn_out = btn;

    return page;
}

/* ------------------------------------------------------------------ */
/* Word-selection (combo box) exercise                                */
/* ------------------------------------------------------------------ */

typedef struct {
    GArray *combos;   /* GtkComboBox* */
    GArray *answers;  /* const char* */
    int ex_num;
    GtkWidget *feedback;
} ComboListCtx;

static ComboListCtx *combo_list_ctx_new(int ex_num, GtkWidget *feedback) {
    ComboListCtx *ctx = g_new0(ComboListCtx, 1);
    ctx->combos = g_array_new(FALSE, FALSE, sizeof(GtkComboBox *));
    ctx->answers = g_array_new(FALSE, FALSE, sizeof(const char *));
    ctx->ex_num = ex_num;
    ctx->feedback = feedback;
    return ctx;
}

static void combo_list_add(ComboListCtx *ctx, GtkWidget *combo, const char *ans) {
    g_array_append_val(ctx->combos, combo);
    g_array_append_val(ctx->answers, ans);
}

static GtkWidget *make_word_combo(const char **words, int n) {
    GtkWidget *combo = gtk_combo_box_text_new();
    int *order = g_new0(int, n);

    for (int i = 0; i < n; i++)
        order[i] = i;
    shuffle_indices(order, n);
    for (int i = 0; i < n; i++)
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), NULL,
                                  words[order[i]]);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), -1);
    g_free(order);
    return combo;
}

static gboolean answer_accepts(const char *sel_norm, const char *ans) {
    gchar **parts = g_strsplit(ans, "|", -1);
    gboolean ok = FALSE;

    for (int i = 0; parts[i] != NULL; i++) {
        gchar *a = normalize_answer(parts[i]);
        if (g_strcmp0(sel_norm, a) == 0) {
            ok = TRUE;
            g_free(a);
            break;
        }
        g_free(a);
    }

    g_strfreev(parts);
    return ok;
}

static void combo_list_check(GtkButton *button, gpointer data) {
    ComboListCtx *ctx = data;
    guint total = ctx->combos->len;
    guint ok = 0;

    (void)button;

    for (guint i = 0; i < total; i++) {
        GtkComboBox *combo = g_array_index(ctx->combos, GtkComboBox *, i);
        const char *ans = g_array_index(ctx->answers, const char *, i);
        gchar *sel = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));
        gboolean good = FALSE;

        if (sel) {
            gchar *t = normalize_answer(sel);
            good = answer_accepts(t, ans);
            g_free(t);
            g_free(sel);
        }
        answer_mark(GTK_WIDGET(combo), good);
        if (good)
            ok++;
    }

    if (ok == total) {
        set_feedback(ctx->feedback, TRUE, "Výborně!");
        mark_done(ctx->ex_num);
    } else {
        set_feedback(ctx->feedback, FALSE, "Ještě to není správně. Zkuste to znovu.");
    }
}

/* ------------------------------------------------------------------ */
/* Word-arrangement (chips) exercise                                  */
/* ------------------------------------------------------------------ */

typedef struct {
    const char *prompt;   /* optional label shown above the sentence */
    const char *words[6];
    int n;
} AssemblyItem;

typedef struct {
    GtkWidget *pool;
    GtkWidget *target;
    GtkWidget *group;
    GtkWidget *chips[6];
    int placed[6];
    int placed_count;
    int n_words;
} SentBuilder;

static void sent_rebuild(SentBuilder *sb) {
    for (int i = 0; i < sb->n_words; i++) {
        GtkWidget *parent = gtk_widget_get_parent(sb->chips[i]);
        if (parent != NULL && GTK_IS_FLOW_BOX_CHILD(parent))
            gtk_flow_box_child_set_child(GTK_FLOW_BOX_CHILD(parent), NULL);
    }

    flow_clear(GTK_FLOW_BOX(sb->pool));
    flow_clear(GTK_FLOW_BOX(sb->target));

    for (int i = 0; i < sb->placed_count; i++)
        gtk_flow_box_insert(GTK_FLOW_BOX(sb->target),
                            sb->chips[sb->placed[i]], -1);

    for (int i = 0; i < sb->n_words; i++) {
        gboolean in_target = FALSE;
        for (int j = 0; j < sb->placed_count; j++) {
            if (sb->placed[j] == i) {
                in_target = TRUE;
                break;
            }
        }
        if (!in_target)
            gtk_flow_box_insert(GTK_FLOW_BOX(sb->pool), sb->chips[i], -1);
    }
}

typedef struct {
    SentBuilder *sb;
    int idx;
} ChipRef;

static GdkContentProvider *asm_drag_prepare(GtkDragSource *source,
                                            double x, double y,
                                            gpointer data) {
    ChipRef *ref = data;
    (void)source;
    (void)x;
    (void)y;
    return gdk_content_provider_new_typed(G_TYPE_INT, ref->idx);
}

static gboolean asm_rebuild_idle(gpointer data) {
    sent_rebuild(data);
    return G_SOURCE_REMOVE;
}

static void asm_drag_end(GtkDragSource *source, GdkDrag *drag,
                         gboolean delete_data, gpointer data) {
    ChipRef *ref = data;
    (void)source;
    (void)drag;
    (void)delete_data;
    g_idle_add(asm_rebuild_idle, ref->sb);
}

static gboolean asm_drop_to_sentence(GtkDropTarget *target, const GValue *value,
                                     double x, double y, gpointer data) {
    SentBuilder *sb = data;
    int idx;
    int pos = -1;

    (void)target;
    (void)x;
    (void)y;

    if (!G_VALUE_HOLDS_INT(value))
        return FALSE;

    idx = g_value_get_int(value);
    for (int i = 0; i < sb->placed_count; i++) {
        if (sb->placed[i] == idx) {
            pos = i;
            break;
        }
    }
    if (pos >= 0) {
        for (int i = pos; i < sb->placed_count - 1; i++)
            sb->placed[i] = sb->placed[i + 1];
        sb->placed_count--;
    }
    sb->placed[sb->placed_count++] = idx;
    return TRUE;
}

static gboolean asm_drop_to_pool(GtkDropTarget *target, const GValue *value,
                                 double x, double y, gpointer data) {
    SentBuilder *sb = data;
    int idx;
    int pos = -1;

    (void)target;
    (void)x;
    (void)y;

    if (!G_VALUE_HOLDS_INT(value))
        return FALSE;

    idx = g_value_get_int(value);
    for (int i = 0; i < sb->placed_count; i++) {
        if (sb->placed[i] == idx) {
            pos = i;
            break;
        }
    }
    if (pos < 0)
        return FALSE;

    for (int i = pos; i < sb->placed_count - 1; i++)
        sb->placed[i] = sb->placed[i + 1];
    sb->placed_count--;
    return TRUE;
}

typedef struct {
    SentBuilder **sbs;
    int n;
    int ex_num;
    GtkWidget *feedback;
} AsmCtx;

static void assembly_check(GtkButton *button, gpointer data) {
    AsmCtx *ctx = data;
    int ok = 0;

    (void)button;

    for (int i = 0; i < ctx->n; i++) {
        SentBuilder *sb = ctx->sbs[i];
        gboolean good = (sb->placed_count == sb->n_words);
        if (good) {
            for (int j = 0; j < sb->placed_count; j++) {
                if (sb->placed[j] != j) {
                    good = FALSE;
                    break;
                }
            }
        }
        if (good) {
            ok++;
            gtk_widget_remove_css_class(sb->group, "wrong");
            gtk_widget_add_css_class(sb->group, "ok");
        } else {
            gtk_widget_remove_css_class(sb->group, "ok");
            gtk_widget_add_css_class(sb->group, "wrong");
        }
    }

    if (ok == ctx->n) {
        set_feedback(ctx->feedback, TRUE, "Výborně!");
        mark_done(ctx->ex_num);
    } else {
        set_feedback(ctx->feedback, FALSE, "Některé věty nejsou správně. Zkuste to znovu.");
    }
}

static GtkWidget *build_assembly(const char *title, const char *subtitle,
                                 int ex_num, const AssemblyItem *items, int n) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(title, subtitle, "Zkontrolovat",
                                    &body, &feedback, &check);
    AsmCtx *ctx = g_new0(AsmCtx, 1);

    ctx->sbs = g_new0(SentBuilder *, n);
    ctx->n = n;
    ctx->ex_num = ex_num;
    ctx->feedback = feedback;

    for (int s = 0; s < n; s++) {
        const AssemblyItem *item = &items[s];
        SentBuilder *sb = g_new0(SentBuilder, 1);
        GtkWidget *group;
        GtkWidget *target;
        GtkWidget *pool;

        sb->n_words = item->n;
        ctx->sbs[s] = sb;

        if (item->prompt) {
            GtkWidget *prompt = gtk_label_new(item->prompt);
            gtk_widget_set_halign(prompt, GTK_ALIGN_START);
            gtk_widget_add_css_class(prompt, "chain");
            gtk_box_append(GTK_BOX(body), prompt);
        }

        group = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
        gtk_widget_add_css_class(group, "sent-group");
        sb->group = group;

        target = gtk_flow_box_new();
        gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(target), GTK_SELECTION_NONE);
        gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(target), 2);
        gtk_widget_set_size_request(target, -1, 44);
        sb->target = target;
        gtk_box_append(GTK_BOX(group), target);

        pool = gtk_flow_box_new();
        gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(pool), GTK_SELECTION_NONE);
        gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(pool), 2);
        gtk_widget_set_size_request(pool, -1, 44);
        sb->pool = pool;
        gtk_box_append(GTK_BOX(group), pool);

        {
            GtkDropTarget *dt = gtk_drop_target_new(G_TYPE_INT, GDK_ACTION_COPY);
            GtkDropTarget *dp = gtk_drop_target_new(G_TYPE_INT, GDK_ACTION_COPY);
            g_signal_connect(dt, "drop", G_CALLBACK(asm_drop_to_sentence), sb);
            gtk_widget_add_controller(target, GTK_EVENT_CONTROLLER(dt));
            g_signal_connect(dp, "drop", G_CALLBACK(asm_drop_to_pool), sb);
            gtk_widget_add_controller(pool, GTK_EVENT_CONTROLLER(dp));
        }

        for (int w = 0; w < item->n; w++) {
            GtkWidget *chip = gtk_button_new_with_label(item->words[w]);
            GtkDragSource *src = gtk_drag_source_new();
            ChipRef *ref = g_new(ChipRef, 1);
            gtk_widget_add_css_class(chip, "chip");
            g_object_ref(chip);
            sb->chips[w] = chip;
            ref->sb = sb;
            ref->idx = w;
            gtk_drag_source_set_actions(src, GDK_ACTION_COPY);
            g_signal_connect(src, "prepare", G_CALLBACK(asm_drag_prepare), ref);
            g_signal_connect(src, "drag-end", G_CALLBACK(asm_drag_end), ref);
            gtk_widget_add_controller(chip, GTK_EVENT_CONTROLLER(src));
        }

        {
            int order[6];
            for (int w = 0; w < item->n; w++)
                order[w] = w;
            shuffle_indices(order, item->n);
            for (int w = 0; w < item->n; w++)
                gtk_flow_box_insert(GTK_FLOW_BOX(pool), sb->chips[order[w]], -1);
        }

        gtk_box_append(GTK_BOX(body), group);
    }

    g_signal_connect(check, "clicked", G_CALLBACK(assembly_check), ctx);
    return page;
}

/* ------------------------------------------------------------------ */
/* Exercise 1: Dialog (word selection)                                */
/* ------------------------------------------------------------------ */

typedef struct {
    const char *speaker;
    const char *before;
    const char *answer;
    const char *after;
} DialogRow;

typedef struct {
    const char *name;
    const DialogRow *rows;
    int n;
} Dialogue;

static const DialogRow ex1_d1[] = {
    {"A:", "Hallo! Ich ", "bin|heiße", " Anna."},
    {"B:", "Hallo! Ich ", "bin|heiße", " Peter."},
    {"A:", "Woher ", "kommst", " du?"},
    {"B:", "Ich komme ", "aus", " Tschechien."},
};

static const DialogRow ex1_d2[] = {
    {"A:", "Guten ", "Morgen", "!"},
    {"B:", "Wie ", "geht", " es dir?"},
    {"A:", "Mir geht es gut, ", "danke", "."},
};

static const DialogRow ex1_d3[] = {
    {"A:", "Ich muss gehen. Auf ", "Wiedersehen", "!"},
    {"B:", "", "Bis", " bald!"},
    {"A:", "", "Tschüss", "!"},
};

static const Dialogue ex1_dialogues[] = {
    {"Sich vorstellen", ex1_d1, G_N_ELEMENTS(ex1_d1)},
    {"Begrüßung", ex1_d2, G_N_ELEMENTS(ex1_d2)},
    {"Verabschiedung", ex1_d3, G_N_ELEMENTS(ex1_d3)},
};

static const char *ex1_pool[] = {
    "bin", "heiße", "kommst", "aus", "Morgen",
    "geht", "danke", "Wiedersehen", "Bis", "Tschüss",
};

static GtkWidget *build_ex1(void) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell("Dialog",
                                    "Doplňte chybějící slova v dialogu.",
                                    "Zkontrolovat", &body, &feedback, &check);
    ComboListCtx *ctx = combo_list_ctx_new(1, feedback);

    for (guint d = 0; d < G_N_ELEMENTS(ex1_dialogues); d++) {
        const Dialogue *diag = &ex1_dialogues[d];
        GtkWidget *name = gtk_label_new(diag->name);
        gtk_widget_set_halign(name, GTK_ALIGN_START);
        gtk_widget_add_css_class(name, "ex-sub");
        gtk_widget_set_margin_top(name, d > 0 ? 10 : 0);
        gtk_box_append(GTK_BOX(body), name);

        for (int r = 0; r < diag->n; r++) {
            const DialogRow *row = &diag->rows[r];
            GtkWidget *line = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
            GtkWidget *combo;
            gtk_widget_set_halign(line, GTK_ALIGN_START);
            gtk_box_append(GTK_BOX(body), line);

            if (row->speaker && row->speaker[0]) {
                GtkWidget *spk = gtk_label_new(row->speaker);
                gtk_widget_add_css_class(spk, "dlg-speaker");
                gtk_widget_set_valign(spk, GTK_ALIGN_CENTER);
                gtk_box_append(GTK_BOX(line), spk);
            }
            if (row->before && row->before[0]) {
                GtkWidget *lb = gtk_label_new(row->before);
                gtk_widget_set_valign(lb, GTK_ALIGN_CENTER);
                gtk_widget_add_css_class(lb, "ex-prompt");
                gtk_box_append(GTK_BOX(line), lb);
            }

            combo = make_word_combo(ex1_pool, G_N_ELEMENTS(ex1_pool));
            gtk_widget_set_size_request(combo, 130, -1);
            gtk_widget_set_valign(combo, GTK_ALIGN_CENTER);
            gtk_box_append(GTK_BOX(line), combo);
            combo_list_add(ctx, combo, row->answer);

            if (row->after && row->after[0]) {
                GtkWidget *la = gtk_label_new(row->after);
                gtk_widget_set_valign(la, GTK_ALIGN_CENTER);
                gtk_widget_add_css_class(la, "ex-prompt");
                gtk_box_append(GTK_BOX(line), la);
            }
        }
    }

    g_signal_connect(check, "clicked", G_CALLBACK(combo_list_check), ctx);
    return page;
}

/* ------------------------------------------------------------------ */
/* Exercise 2: Sätze bilden (word arrangement)                        */
/* ------------------------------------------------------------------ */

static const AssemblyItem ex2_items[] = {
    {NULL, {"Ich", "heiße", "Anna", "."}, 4},
    {NULL, {"Woher", "kommst", "du", "?"}, 4},
    {NULL, {"Ich", "komme", "aus", "Tschechien", "."}, 5},
    {NULL, {"Wie", "geht", "es", "dir", "?"}, 4},
};

/* ------------------------------------------------------------------ */
/* Choice exercises (3 + 9)                                           */
/* ------------------------------------------------------------------ */

typedef struct {
    const char *prompt;
    const char *options[3];
    int correct;
} ChoiceQ;

static const ChoiceQ ex3_questions[] = {
    {"Welche Zahl ist „vierzehn“?", {"4", "14", "40"}, 1},
    {"Welche Zahl ist „siebzehn“?", {"7", "17", "70"}, 1},
    {"Welche Zahl ist „zwanzig“?", {"2", "12", "20"}, 2},
    {"Welche Zahl ist „sechs“?", {"6", "16", "60"}, 0},
};

static const ChoiceQ ex9_questions[] = {
    {"___ heißt du?", {"Wer", "Wie", "Wo"}, 1},
    {"___ kommst du?", {"Woher", "Wie", "Wer"}, 0},
    {"___ wohnst du?", {"Wo", "Wer", "Was"}, 0},
    {"___ ist das?", {"Wer", "Wo", "Wie"}, 0},
    {"___ alt bist du?", {"Wie", "Woher", "Was"}, 0},
};

typedef struct {
    const ChoiceQ *qs;
    int n;
    int ex_num;
    GtkWidget *feedback;
    GtkToggleButton **toggles;
} ChoiceCtx;

static void choice_check(GtkButton *button, gpointer data) {
    ChoiceCtx *ctx = data;
    int ok = 0;

    (void)button;

    for (int i = 0; i < ctx->n; i++) {
        int active = -1;
        for (int o = 0; o < 3; o++) {
            GtkToggleButton *tb = ctx->toggles[i * 3 + o];
            gtk_widget_remove_css_class(GTK_WIDGET(tb), "ok");
            gtk_widget_remove_css_class(GTK_WIDGET(tb), "wrong");
            if (gtk_toggle_button_get_active(tb))
                active = o;
        }
        if (active == ctx->qs[i].correct) {
            ok++;
            gtk_widget_add_css_class(GTK_WIDGET(ctx->toggles[i * 3 + active]), "ok");
        } else if (active >= 0) {
            gtk_widget_add_css_class(GTK_WIDGET(ctx->toggles[i * 3 + active]), "wrong");
        }
    }

    if (ok == ctx->n) {
        set_feedback(ctx->feedback, TRUE, "Výborně!");
        mark_done(ctx->ex_num);
    } else {
        set_feedback(ctx->feedback, FALSE, "Ještě to není správně.");
    }
}

static GtkWidget *build_choice(const char *title, const char *subtitle, int ex_num,
                               const ChoiceQ *qs, int n) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(title, subtitle, "Zkontrolovat",
                                    &body, &feedback, &check);
    ChoiceCtx *ctx = g_new0(ChoiceCtx, 1);

    ctx->qs = qs;
    ctx->n = n;
    ctx->ex_num = ex_num;
    ctx->feedback = feedback;
    ctx->toggles = g_new0(GtkToggleButton *, n * 3);

    for (int i = 0; i < n; i++) {
        GtkWidget *prompt = gtk_label_new(qs[i].prompt);
        GtkWidget *row;
        GtkToggleButton *first = NULL;

        gtk_widget_set_halign(prompt, GTK_ALIGN_START);
        gtk_widget_add_css_class(prompt, "ex-prompt");
        gtk_box_append(GTK_BOX(body), prompt);

        row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        gtk_widget_set_halign(row, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(body), row);

        for (int o = 0; o < 3; o++) {
            GtkWidget *tb = gtk_toggle_button_new_with_label(qs[i].options[o]);
            gtk_widget_add_css_class(tb, "pill");
            gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(tb), first);
            if (!first)
                first = GTK_TOGGLE_BUTTON(tb);
            gtk_box_append(GTK_BOX(row), tb);
            ctx->toggles[i * 3 + o] = GTK_TOGGLE_BUTTON(tb);
        }
    }

    g_signal_connect(check, "clicked", G_CALLBACK(choice_check), ctx);
    return page;
}

/* ------------------------------------------------------------------ */
/* Exercise 4: Freie Antwort                                          */
/* ------------------------------------------------------------------ */

typedef struct {
    const char *question;
    const char *q_cs;   /* Czech translation of the question     */
    const char *sample;
    const char *a_cs;   /* Czech translation of the sample answer */
} FreeQ;

static const FreeQ ex4_questions[] = {
    {"Wie heißt du?", "Jak se jmenuješ?",
     "Ich heiße Anna.", "Jmenuji se Anna."},
    {"Woher kommst du?", "Odkud pocházíš?",
     "Ich komme aus Tschechien.", "Pocházím z Česka."},
    {"Wie alt bist du?", "Kolik je ti let?",
     "Ich bin sechzehn Jahre alt.", "Je mi šestnáct let."},
    {"Wie geht es dir?", "Jak se máš?",
     "Mir geht es gut, danke.", "Mám se dobře, děkuji."},
};

static void ex4_reveal(GtkButton *button, gpointer data) {
    GtkWidget *sample = data;
    (void)button;
    gtk_widget_set_visible(sample, !gtk_widget_get_visible(sample));
}

static void ex4_finish(GtkButton *button, gpointer data) {
    GtkWidget *feedback = data;
    (void)button;
    set_feedback(feedback, TRUE, "Výborně!");
    mark_done(4);
}

static GtkWidget *build_ex4(void) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *tip;
    GtkWidget *page = ex_page_shell("Freie Antwort",
                                    "Odpovězte vlastními slovy, poté klikněte na Dokončit.",
                                    "Dokončit", &body, &feedback, &check);

    tip = gtk_label_new("Písmeno „ß“ se dá na klávesnici zaměnit za „ss“!");
    gtk_widget_set_halign(tip, GTK_ALIGN_START);
    gtk_widget_add_css_class(tip, "hint");
    gtk_box_append(GTK_BOX(body), tip);

    for (guint i = 0; i < G_N_ELEMENTS(ex4_questions); i++) {
        GtkWidget *q = gtk_label_new(ex4_questions[i].question);
        GtkWidget *entry;
        GtkWidget *reveal;
        GtkWidget *sample;
        char *txt;

        gtk_widget_set_halign(q, GTK_ALIGN_START);
        gtk_widget_add_css_class(q, "ex-prompt");
        gtk_box_append(GTK_BOX(body), q);

        entry = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Vaše odpověď…");
        gtk_box_append(GTK_BOX(body), entry);

        reveal = gtk_button_new_with_label("Ukázat vzor");
        gtk_widget_add_css_class(reveal, "pill");
        gtk_widget_set_halign(reveal, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(body), reveal);

        txt = g_strdup_printf("Otázka: %s\nVzor: %s  (česky: %s)",
                              ex4_questions[i].q_cs,
                              ex4_questions[i].sample,
                              ex4_questions[i].a_cs);
        sample = gtk_label_new(txt);
        g_free(txt);
        gtk_widget_set_halign(sample, GTK_ALIGN_START);
        gtk_widget_add_css_class(sample, "hint");
        gtk_label_set_wrap(GTK_LABEL(sample), TRUE);
        gtk_widget_set_visible(sample, FALSE);
        gtk_box_append(GTK_BOX(body), sample);

        g_signal_connect(reveal, "clicked", G_CALLBACK(ex4_reveal), sample);
    }

    g_signal_connect(check, "clicked", G_CALLBACK(ex4_finish), feedback);
    return page;
}

/* ------------------------------------------------------------------ */
/* Exercise 5: Zahlen (digits -> word)                                */
/* ------------------------------------------------------------------ */

typedef struct {
    const char *digits;
    const char *answer;
} NumberQ;

static const NumberQ ex5_data[] = {
    {"14", "vierzehn"},
    {"17", "siebzehn"},
    {"20", "zwanzig"},
    {"6", "sechs"},
    {"13", "dreizehn"},
};

static const char *ex5_pool[] = {
    "vierzehn", "siebzehn", "zwanzig", "sechs", "dreizehn",
};

static GtkWidget *build_ex5(void) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell("Zahlen",
                                    "Vyberte správné slovo pro dané číslo.",
                                    "Zkontrolovat", &body, &feedback, &check);
    ComboListCtx *ctx = combo_list_ctx_new(5, feedback);

    for (guint i = 0; i < G_N_ELEMENTS(ex5_data); i++) {
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        GtkWidget *digits = gtk_label_new(ex5_data[i].digits);
        GtkWidget *eq = gtk_label_new("=");
        GtkWidget *combo;

        gtk_widget_set_halign(row, GTK_ALIGN_START);
        gtk_widget_add_css_class(digits, "number-big");
        gtk_widget_set_valign(digits, GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(row), digits);

        gtk_widget_set_valign(eq, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(eq, "ex-prompt");
        gtk_box_append(GTK_BOX(row), eq);

        combo = make_word_combo(ex5_pool, G_N_ELEMENTS(ex5_pool));
        gtk_widget_set_size_request(combo, 140, -1);
        gtk_widget_set_valign(combo, GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(row), combo);
        combo_list_add(ctx, combo, ex5_data[i].answer);

        gtk_box_append(GTK_BOX(body), row);
    }

    g_signal_connect(check, "clicked", G_CALLBACK(combo_list_check), ctx);
    return page;
}

/* ------------------------------------------------------------------ */
/* Exercise 6: Wie viel?                                              */
/* ------------------------------------------------------------------ */

typedef struct {
    const char *emoji;
    int count;
    const char *noun;
    const char *answer;
} CountQ;

static const CountQ ex6_data[] = {
    {"🍎", 2, "Äpfel", "zwei"},
    {"⭐", 5, "Sterne", "fünf"},
    {"🐱", 7, "Katzen", "sieben"},
    {"🌸", 9, "Blumen", "neun"},
    {"🚗", 3, "Autos", "drei"},
    {"📚", 11, "Bücher", "elf"},
    {"🕯️", 4, "Kerzen", "vier"},
    {"⚽", 8, "Bälle", "acht"},
    {"🐦", 6, "Vögel", "sechs"},
};

static const char *ex6_pool[] = {
    "zwei", "fünf", "sieben", "neun", "drei", "elf", "vier", "acht", "sechs",
};

static GtkWidget *build_ex6(void) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell("Wie viel?",
                                    "Spočítejte předměty a vyberte počet.",
                                    "Zkontrolovat", &body, &feedback, &check);
    ComboListCtx *ctx = combo_list_ctx_new(6, feedback);

    for (guint i = 0; i < G_N_ELEMENTS(ex6_data); i++) {
        const CountQ *q = &ex6_data[i];
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        GString *em = g_string_new(NULL);
        GtkWidget *icons;
        GtkWidget *noun;
        GtkWidget *combo;

        for (int c = 0; c < q->count; c++) {
            if (c)
                g_string_append_c(em, ' ');
            g_string_append(em, q->emoji);
        }
        icons = gtk_label_new(em->str);
        g_string_free(em, TRUE);
        gtk_widget_set_valign(icons, GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(row), icons);

        noun = gtk_label_new(q->noun);
        gtk_widget_set_valign(noun, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(noun, "ex-prompt");
        gtk_box_append(GTK_BOX(row), noun);

        combo = make_word_combo(ex6_pool, G_N_ELEMENTS(ex6_pool));
        gtk_widget_set_size_request(combo, 130, -1);
        gtk_widget_set_valign(combo, GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(row), combo);
        combo_list_add(ctx, combo, q->answer);

        gtk_box_append(GTK_BOX(body), row);
    }

    g_signal_connect(check, "clicked", G_CALLBACK(combo_list_check), ctx);
    return page;
}

/* ------------------------------------------------------------------ */
/* Exercise 7: Zahlenreihe                                            */
/* ------------------------------------------------------------------ */

typedef struct {
    const char *before;
    const char *after;
    const char *answer;
} SeqQ;

static const SeqQ ex7_data[] = {
    {"sechs, sieben, ", ", neun", "acht"},
    {"zwölf, dreizehn, ", ", fünfzehn", "vierzehn"},
    {"achtzehn, neunzehn, ", ", einundzwanzig", "zwanzig"},
    {"vier, fünf, ", ", sieben", "sechs"},
    {"zehn, elf, ", ", dreizehn", "zwölf"},
};

static const char *ex7_pool[] = {
    "acht", "vierzehn", "zwanzig", "sechs", "zwölf",
};

static GtkWidget *build_ex7(void) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell("Zahlenreihe",
                                    "Doplňte chybějící číslovku.",
                                    "Zkontrolovat", &body, &feedback, &check);
    ComboListCtx *ctx = combo_list_ctx_new(7, feedback);

    for (guint i = 0; i < G_N_ELEMENTS(ex7_data); i++) {
        const SeqQ *q = &ex7_data[i];
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        GtkWidget *before = gtk_label_new(q->before);
        GtkWidget *after = gtk_label_new(q->after);
        GtkWidget *combo;

        gtk_widget_set_halign(row, GTK_ALIGN_START);

        gtk_widget_set_valign(before, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(before, "ex-prompt");
        gtk_box_append(GTK_BOX(row), before);

        combo = make_word_combo(ex7_pool, G_N_ELEMENTS(ex7_pool));
        gtk_widget_set_size_request(combo, 140, -1);
        gtk_widget_set_valign(combo, GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(row), combo);
        combo_list_add(ctx, combo, q->answer);

        gtk_widget_set_valign(after, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(after, "ex-prompt");
        gtk_box_append(GTK_BOX(row), after);

        gtk_box_append(GTK_BOX(body), row);
    }

    g_signal_connect(check, "clicked", G_CALLBACK(combo_list_check), ctx);
    return page;
}

/* ------------------------------------------------------------------ */
/* Exercise 8: Verb einsetzen (word selection)                        */
/* ------------------------------------------------------------------ */

typedef struct {
    const char *before;
    const char *after;
    const char *answer;
} VerbQ;

static const VerbQ ex8_data[] = {
    {"Ich ", " Peter.", "heiße"},
    {"Woher ", " du?", "kommst"},
    {"Ich ", " in Prag.", "wohne"},
    {"Wie alt ", " du?", "bist"},
    {"Ich ", " Fußball.", "spiele"},
    {"Was ", " du gern?", "machst"},
};

static const char *ex8_pool[] = {
    "heißen", "kommen", "wohnen", "sein", "spielen", "machen",
    "heiße", "kommst", "wohne", "bist", "spiele", "machst",
};

static GtkWidget *build_ex8(void) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell("Verb einsetzen",
                                    "Vyberte správný tvar slovesa.",
                                    "Zkontrolovat", &body, &feedback, &check);
    ComboListCtx *ctx = combo_list_ctx_new(8, feedback);

    for (guint i = 0; i < G_N_ELEMENTS(ex8_data); i++) {
        const VerbQ *q = &ex8_data[i];
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        GtkWidget *before = gtk_label_new(q->before);
        GtkWidget *after = gtk_label_new(q->after);
        GtkWidget *combo;

        gtk_widget_set_halign(row, GTK_ALIGN_START);

        gtk_widget_set_valign(before, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(before, "ex-prompt");
        gtk_box_append(GTK_BOX(row), before);

        combo = make_word_combo(ex8_pool, G_N_ELEMENTS(ex8_pool));
        gtk_widget_set_size_request(combo, 120, -1);
        gtk_widget_set_valign(combo, GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(row), combo);
        combo_list_add(ctx, combo, q->answer);

        gtk_widget_set_valign(after, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(after, "ex-prompt");
        gtk_box_append(GTK_BOX(row), after);

        gtk_box_append(GTK_BOX(body), row);
    }

    g_signal_connect(check, "clicked", G_CALLBACK(combo_list_check), ctx);
    return page;
}

/* ------------------------------------------------------------------ */
/* Exercise 10: Wörter trennen (word arrangement)                     */
/* ------------------------------------------------------------------ */

static const AssemblyItem ex10_items[] = {
    {"wiespätistes", {"wie", "spät", "ist", "es"}, 4},
    {"ichheißeanna", {"ich", "heiße", "anna"}, 3},
    {"woherkommstdu", {"woher", "kommst", "du"}, 3},
    {"aufwiedersehen", {"auf", "wiedersehen"}, 2},
    {"dankeschön", {"danke", "schön"}, 2},
};

/* ------------------------------------------------------------------ */
/* Exercise 12: Was macht er/sie?                                     */
/* ------------------------------------------------------------------ */

typedef struct {
    const char *emoji;
    const char *before;
    const char *after;
    const char *answer;
} VerbClueQ;

static const VerbClueQ ex12_data[] = {
    {"🏊", "Er ", " im See.", "schwimmt"},
    {"🎤", "Sie ", " ein Lied.", "singt"},
    {"🍳", "Er ", " Suppe.", "kocht"},
    {"📖", "Sie ", " ein Buch.", "liest"},
    {"🚗", "Er ", " Auto.", "fährt"},
    {"🎨", "Sie ", " ein Bild.", "malt"},
    {"⚽", "Er ", " Fußball.", "spielt"},
    {"🛏️", "Sie ", " .", "schläft"},
};

static const char *ex12_pool[] = {
    "schwimmt", "singt", "kocht", "liest", "fährt", "malt", "spielt", "schläft",
};

static GtkWidget *build_ex12(void) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell("Was macht er/sie?",
                                    "Vyberte správný tvar slovesa podle obrázku.",
                                    "Zkontrolovat", &body, &feedback, &check);
    ComboListCtx *ctx = combo_list_ctx_new(12, feedback);

    for (guint i = 0; i < G_N_ELEMENTS(ex12_data); i++) {
        const VerbClueQ *q = &ex12_data[i];
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        GtkWidget *emoji = gtk_label_new(q->emoji);
        GtkWidget *before = gtk_label_new(q->before);
        GtkWidget *after = gtk_label_new(q->after);
        GtkWidget *combo;

        gtk_widget_set_halign(row, GTK_ALIGN_START);

        gtk_widget_set_valign(emoji, GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(row), emoji);

        gtk_widget_set_valign(before, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(before, "ex-prompt");
        gtk_box_append(GTK_BOX(row), before);

        combo = make_word_combo(ex12_pool, G_N_ELEMENTS(ex12_pool));
        gtk_widget_set_size_request(combo, 120, -1);
        gtk_widget_set_valign(combo, GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(row), combo);
        combo_list_add(ctx, combo, q->answer);

        gtk_widget_set_valign(after, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(after, "ex-prompt");
        gtk_box_append(GTK_BOX(row), after);

        gtk_box_append(GTK_BOX(body), row);
    }

    g_signal_connect(check, "clicked", G_CALLBACK(combo_list_check), ctx);
    return page;
}

/* ------------------------------------------------------------------ */
/* Assign exercises (11 + 13)                                         */
/* ------------------------------------------------------------------ */

typedef struct {
    const char *emoji;
    const char *label;
    int correct_group;
} AssignItem;

static const AssignItem ex11_items[] = {
    {NULL, "Hallo", 0},
    {NULL, "Guten Morgen", 0},
    {NULL, "Guten Tag", 0},
    {NULL, "Guten Abend", 0},
    {NULL, "Grüß Gott", 0},
    {NULL, "Servus", 0},
    {NULL, "Tschüss", 1},
    {NULL, "Auf Wiedersehen", 1},
    {NULL, "Bis bald", 1},
    {NULL, "Bis morgen", 1},
    {NULL, "Gute Nacht", 1},
    {NULL, "Bis später", 1},
};

static const AssignItem ex13_items[] = {
    {"🥨", "Brezel", 0},
    {"🌭", "Bratwurst", 0},
    {"🏰", "Brandenburger Tor", 0},
    {"🚗", "Volkswagen", 0},
    {"🎻", "Mozart", 1},
    {"🍰", "Sachertorte", 1},
    {"🥩", "Wiener Schnitzel", 1},
    {"🧀", "Käse", 2},
    {"⛰️", "Matterhorn", 2},
    {"🍫", "Schokolade", 2},
};

static const char *ex11_groups[] = {"Begrüßung", "Verabschiedung"};
static const char *ex13_groups[] = {"Deutschland (D)", "Österreich (A)", "Schweiz (CH)"};

typedef struct {
    GtkWidget *pool;
    GtkWidget *group_flow[3];
    GtkWidget *group_btn[3];
    int n_groups;
    int active_group;
    GtkWidget **chips;
    int n_items;
    int *current_group;
    const AssignItem *items;
    int ex_num;
    GtkWidget *feedback;
} AssignCtx;

typedef struct {
    AssignCtx *ac;
    int idx;
} AssignChipRef;

static void assign_rebuild(AssignCtx *ac) {
    for (int i = 0; i < ac->n_items; i++) {
        GtkWidget *parent = gtk_widget_get_parent(ac->chips[i]);
        if (parent != NULL && GTK_IS_FLOW_BOX_CHILD(parent))
            gtk_flow_box_child_set_child(GTK_FLOW_BOX_CHILD(parent), NULL);
    }

    flow_clear(GTK_FLOW_BOX(ac->pool));
    for (int g = 0; g < ac->n_groups; g++)
        flow_clear(GTK_FLOW_BOX(ac->group_flow[g]));

    for (int i = 0; i < ac->n_items; i++) {
        if (ac->current_group[i] < 0)
            gtk_flow_box_insert(GTK_FLOW_BOX(ac->pool), ac->chips[i], -1);
        else
            gtk_flow_box_insert(GTK_FLOW_BOX(ac->group_flow[ac->current_group[i]]),
                                ac->chips[i], -1);
    }
}

static void assign_chip_clicked(GtkButton *button, gpointer data) {
    AssignChipRef *ref = data;
    AssignCtx *ac = ref->ac;
    int idx = ref->idx;

    (void)button;

    if (ac->current_group[idx] < 0)
        ac->current_group[idx] = ac->active_group;
    else
        ac->current_group[idx] = -1;

    assign_rebuild(ac);
}

static void assign_group_toggled(GtkToggleButton *button, gpointer data) {
    AssignCtx *ac = data;

    if (!gtk_toggle_button_get_active(button))
        return;

    for (int g = 0; g < ac->n_groups; g++) {
        if (GTK_TOGGLE_BUTTON(ac->group_btn[g]) == button) {
            ac->active_group = g;
            break;
        }
    }
}

static void assign_check(GtkButton *button, gpointer data) {
    AssignCtx *ac = data;
    int ok = 0;

    (void)button;

    for (int i = 0; i < ac->n_items; i++)
        if (ac->current_group[i] == ac->items[i].correct_group)
            ok++;

    if (ok == ac->n_items) {
        set_feedback(ac->feedback, TRUE, "Výborně!");
        mark_done(ac->ex_num);
    } else {
        set_feedback(ac->feedback, FALSE, "Ještě to není správně. Zkuste to znovu.");
    }
}

static GtkWidget *build_assign(const char *title, const char *subtitle, int ex_num,
                               const AssignItem *items, int n_items,
                               const char **group_labels, int n_groups) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(title, subtitle, "Zkontrolovat",
                                    &body, &feedback, &check);
    AssignCtx *ac = g_new0(AssignCtx, 1);
    GtkWidget *hint;
    GtkWidget *row;
    GtkToggleButton *first = NULL;
    int *order;

    ac->n_items = n_items;
    ac->n_groups = n_groups;
    ac->active_group = 0;
    ac->ex_num = ex_num;
    ac->feedback = feedback;
    ac->items = items;
    ac->chips = g_new0(GtkWidget *, n_items);
    ac->current_group = g_new0(int, n_items);
    for (int i = 0; i < n_items; i++)
        ac->current_group[i] = -1;

    hint = gtk_label_new(
        "Vyberte skupinu a klikněte na kartu. Kliknutím na kartu ve skupině ji vrátíte zpět.");
    gtk_widget_set_halign(hint, GTK_ALIGN_START);
    gtk_widget_add_css_class(hint, "ex-sub");
    gtk_box_append(GTK_BOX(body), hint);

    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_halign(row, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(body), row);
    for (int g = 0; g < n_groups; g++) {
        GtkWidget *gb = gtk_toggle_button_new_with_label(group_labels[g]);
        gtk_widget_add_css_class(gb, "group-btn");
        gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(gb), first);
        if (!first)
            first = GTK_TOGGLE_BUTTON(gb);
        gtk_box_append(GTK_BOX(row), gb);
        ac->group_btn[g] = gb;
        g_signal_connect(gb, "toggled", G_CALLBACK(assign_group_toggled), ac);
    }
    gtk_toggle_button_set_active(first, TRUE);

    ac->pool = gtk_flow_box_new();
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(ac->pool), GTK_SELECTION_NONE);
    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(ac->pool), 2);
    gtk_widget_set_size_request(ac->pool, -1, 48);
    gtk_box_append(GTK_BOX(body), ac->pool);

    for (int g = 0; g < n_groups; g++) {
        GtkWidget *panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        GtkWidget *gl = gtk_label_new(group_labels[g]);
        GtkWidget *gf = gtk_flow_box_new();

        gtk_widget_add_css_class(panel, "group-panel");

        gtk_widget_set_halign(gl, GTK_ALIGN_START);
        gtk_widget_add_css_class(gl, "group-panel-label");
        gtk_box_append(GTK_BOX(panel), gl);

        gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(gf), GTK_SELECTION_NONE);
        gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(gf), 2);
        gtk_widget_set_size_request(gf, -1, 48);
        ac->group_flow[g] = gf;
        gtk_box_append(GTK_BOX(panel), gf);

        gtk_box_append(GTK_BOX(body), panel);
    }

    order = g_new0(int, n_items);
    for (int i = 0; i < n_items; i++)
        order[i] = i;
    shuffle_indices(order, n_items);

    for (int k = 0; k < n_items; k++) {
        int i = order[k];
        GString *lab = g_string_new(NULL);
        GtkWidget *chip;
        AssignChipRef *ref = g_new(AssignChipRef, 1);

        if (items[i].emoji) {
            g_string_append(lab, items[i].emoji);
            g_string_append(lab, " ");
        }
        g_string_append(lab, items[i].label);

        chip = gtk_button_new_with_label(lab->str);
        g_string_free(lab, TRUE);
        gtk_widget_add_css_class(chip, "chip");
        g_object_ref(chip);
        ac->chips[i] = chip;

        ref->ac = ac;
        ref->idx = i;
        g_signal_connect(chip, "clicked", G_CALLBACK(assign_chip_clicked), ref);

        gtk_flow_box_insert(GTK_FLOW_BOX(ac->pool), chip, -1);
    }
    g_free(order);

    g_signal_connect(check, "clicked", G_CALLBACK(assign_check), ac);
    return page;
}

/* ------------------------------------------------------------------ */
/* Activate                                                           */
/* ------------------------------------------------------------------ */

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWindow *window;
    GtkWidget *headerbar;
    GtkWidget *title_label;
    GtkWidget *welcome_page;
    GtkWidget *roadmap_page;
    GtkWidget *unit1_page;
    GtkWidget *ex_pages[NUM_EXERCISES + 1];
    GtkEventController *keys;
    GtkCssProvider *provider;
    char name[16];

    (void)user_data;

    load_progress();

    window = GTK_WINDOW(gtk_application_window_new(app));
    gtk_window_set_title(window, "Sprechen.c");
    gtk_window_set_default_size(window, 760, 560);

    headerbar = gtk_header_bar_new();
    gtk_widget_add_css_class(headerbar, "titlebar");
    title_label = gtk_label_new("Sprechen.c");
    gtk_widget_add_css_class(title_label, "app-title");
    gtk_header_bar_set_title_widget(GTK_HEADER_BAR(headerbar), title_label);
    gtk_window_set_titlebar(window, headerbar);

    main_stack = GTK_STACK(gtk_stack_new());
    gtk_stack_set_transition_type(GTK_STACK(main_stack),
                                  GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_window_set_child(window, GTK_WIDGET(main_stack));

    welcome_page = build_welcome_page();
    roadmap_page = build_roadmap_page();
    unit1_page = build_unit1_page();

    ex_pages[1] = build_ex1();
    ex_pages[2] = build_assembly("Sätze bilden",
                                 "Přetáhněte slova do správného pořadí.",
                                 2, ex2_items, G_N_ELEMENTS(ex2_items));
    ex_pages[3] = build_choice("Was ist richtig?", "Vyberte správnou číslovku.",
                               3, ex3_questions, G_N_ELEMENTS(ex3_questions));
    ex_pages[4] = build_ex4();
    ex_pages[5] = build_ex5();
    ex_pages[6] = build_ex6();
    ex_pages[7] = build_ex7();
    ex_pages[8] = build_ex8();
    ex_pages[9] = build_choice("Wer? Wie? Wo?", "Vyberte správné tázací slovo.",
                               9, ex9_questions, G_N_ELEMENTS(ex9_questions));
    ex_pages[10] = build_assembly("Wörter trennen",
                                  "Přetáhněte slova do správného pořadí.",
                                  10, ex10_items, G_N_ELEMENTS(ex10_items));
    ex_pages[11] = build_assign("Grußformen", "Roztřiďte pozdravy na pozdravy a rozloučení.",
                                11, ex11_items, G_N_ELEMENTS(ex11_items),
                                ex11_groups, G_N_ELEMENTS(ex11_groups));
    ex_pages[12] = build_ex12();
    ex_pages[13] = build_assign("Länder", "Přiřaďte symboly ke správné zemi.",
                                13, ex13_items, G_N_ELEMENTS(ex13_items),
                                ex13_groups, G_N_ELEMENTS(ex13_groups));

    gtk_stack_add_named(main_stack, welcome_page, "welcome");
    gtk_stack_add_named(main_stack, roadmap_page, "roadmap");
    gtk_stack_add_named(main_stack, unit1_page, "unit1");
    for (int i = 1; i <= NUM_EXERCISES; i++) {
        g_snprintf(name, sizeof(name), "ex%d", i);
        gtk_stack_add_named(main_stack, ex_pages[i], name);
    }
    gtk_stack_set_visible_child_name(main_stack, "welcome");

    refresh_completion_ui();

    keys = gtk_event_controller_key_new();
    gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(keys),
                                               GTK_PHASE_CAPTURE);
    g_signal_connect(keys, "key-pressed",
                     G_CALLBACK(on_window_key_pressed), window);
    gtk_widget_add_controller(GTK_WIDGET(window), GTK_EVENT_CONTROLLER(keys));

    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider,
        "window {"
        "   background-color: #1e1e2e;"
        "   background-image: linear-gradient(160deg, #11111b 0%, #1e1e2e 40%, #313244 100%);"
        "}"
        "button {"
        "   background-image: none;"
        "}"
        ".titlebar {"
        "   background-color: #1e1e2e;"
        "   background-image: none;"
        "   box-shadow: none;"
        "   border: none;"
        "}"
        ".app-title {"
        "   color: #cdd6f4;"
        "   font-weight: 700;"
        "   font-size: 14px;"
        "}"
        ".heading {"
        "   color: #cdd6f4;"
        "   font-size: 32px;"
        "   font-weight: 800;"
        "}"
        ".card {"
        "   background-color: rgba(49, 50, 68, 0.92);"
        "   border: 1px solid #45475a;"
        "   border-radius: 18px;"
        "   padding: 30px 34px;"
        "   color: #cdd6f4;"
        "   font-size: 16px;"
        "}"
        ".btn-primary {"
        "   background-image: linear-gradient(135deg, #cba6f7 0%, #b4befe 100%);"
        "   color: #1e1e2e;"
        "   font-size: 16px;"
        "   font-weight: 700;"
        "   padding: 12px 40px;"
        "   border: none;"
        "   border-radius: 999px;"
        "   box-shadow: 0 6px 18px rgba(203, 166, 247, 0.25);"
        "   transition: box-shadow 150ms ease, background-image 150ms ease;"
        "}"
        ".btn-primary:hover {"
        "   box-shadow: 0 8px 24px rgba(203, 166, 247, 0.45);"
        "   background-image: linear-gradient(135deg, #b4befe 0%, #89b4fa 100%);"
        "}"
        ".btn-primary:active {"
        "   box-shadow: 0 3px 10px rgba(203, 166, 247, 0.35);"
        "}"
        ".roadmap-title {"
        "   color: #cdd6f4;"
        "   font-size: 28px;"
        "   font-weight: 800;"
        "}"
        ".roadmap-sub {"
        "   color: #a6adc8;"
        "   font-size: 14px;"
        "}"
        ".back-btn {"
        "   min-width: 42px;"
        "   min-height: 42px;"
        "   padding: 0;"
        "   border-radius: 999px;"
        "   background-color: rgba(49, 50, 68, 0.9);"
        "   border: 1px solid #45475a;"
        "   color: #cdd6f4;"
        "   box-shadow: 0 4px 14px rgba(0, 0, 0, 0.35);"
        "   transition: background-color 150ms ease, color 150ms ease,"
        "                border-color 150ms ease, box-shadow 150ms ease;"
        "}"
        ".back-btn:hover {"
        "   background-color: #45475a;"
        "   border-color: #585b70;"
        "   color: #cba6f7;"
        "   box-shadow: 0 6px 20px rgba(203, 166, 247, 0.35);"
        "}"
        ".back-btn:active {"
        "   background-color: #313244;"
        "   box-shadow: 0 2px 8px rgba(0, 0, 0, 0.35);"
        "}"
        ".unit-node {"
        "   background-image: none;"
        "   background-color: #2b2d40;"
        "   color: #cdd6f4;"
        "   border: 2px solid #3b3e56;"
        "   border-radius: 999px;"
        "   padding: 0;"
        "   box-shadow: 0 4px 14px rgba(0, 0, 0, 0.35);"
        "   transition: transform 150ms ease, box-shadow 150ms ease,"
        "                border-color 150ms ease;"
        "}"
        ".unit-node.done {"
        "   background-image: linear-gradient(135deg, #a6e3a1 0%, #94e2d5 100%);"
        "   border-color: #a6e3a1;"
        "   color: #1e1e2e;"
        "   box-shadow: 0 4px 16px rgba(166, 227, 161, 0.35);"
        "}"
        ".unit-node.current {"
        "   background-image: linear-gradient(135deg, #cba6f7 0%, #b4befe 55%, #89b4fa 100%);"
        "   border-color: #cba6f7;"
        "   color: #1e1e2e;"
        "   box-shadow: 0 4px 18px rgba(203, 166, 247, 0.5),"
        "               0 0 0 4px rgba(30, 30, 46, 0.9),"
        "               0 0 0 7px rgba(203, 166, 247, 0.45);"
        "}"
        ".unit-node.done:hover,"
        ".unit-node.current:hover {"
        "   transition: box-shadow 150ms ease, border-color 150ms ease;"
        "}"
        ".unit-node.done:hover {"
        "   box-shadow: 0 8px 22px rgba(166, 227, 161, 0.5);"
        "}"
        ".unit-node.current:hover {"
        "   box-shadow: 0 8px 26px rgba(203, 166, 247, 0.65),"
        "               0 0 0 4px rgba(30, 30, 46, 0.9),"
        "               0 0 0 7px rgba(203, 166, 247, 0.55);"
        "}"
        ".unit-node.locked {"
        "   background-image: none;"
        "   background-color: #24263a;"
        "   border: 2px solid #383b52;"
        "   color: #6c7086;"
        "   box-shadow: none;"
        "}"
        ".unit-node.locked:hover {"
        "   transform: none;"
        "   box-shadow: none;"
        "}"
        ".unit-node.finish {"
        "   background-color: transparent;"
        "   background-image: none;"
        "   border: none;"
        "   color: #6c7086;"
        "   box-shadow: 0 4px 14px rgba(0, 0, 0, 0.35);"
        "}"
        ".unit-node.finish:hover {"
        "   transform: none;"
        "   box-shadow: 0 4px 14px rgba(0, 0, 0, 0.35);"
        "}"
        ".unit-node:focus,"
        ".unit-node:focus-visible,"
        ".unit-node:hover {"
        "   outline: none;"
        "}"
        ".unit-number {"
        "   font-size: 24px;"
        "   font-weight: 800;"
        "}"
        ".unit-node.done .unit-number,"
        ".unit-node.current .unit-number {"
        "   color: #1e1e2e;"
        "}"
        ".unit-node.locked .unit-number {"
        "   color: #6c7086;"
        "}"
        ".state-icon {"
        "   color: #1e1e2e;"
        "}"
        ".unit-node.locked .lock-icon {"
        "   color: #585b70;"
        "}"
        ".unit-name {"
        "   color: #cdd6f4;"
        "   font-size: 13px;"
        "   font-weight: 600;"
        "}"
        ".unit-name-locked {"
        "   color: #6c7086;"
        "}"
        ".ex-bubble {"
        "   min-width: 64px;"
        "   min-height: 64px;"
        "   padding: 0;"
        "   border-radius: 999px;"
        "   background-color: #2b2d40;"
        "   color: #cdd6f4;"
        "   border: 2px solid #3b3e56;"
        "   box-shadow: 0 4px 14px rgba(0, 0, 0, 0.35);"
        "   transition: border-color 150ms ease, box-shadow 150ms ease;"
        "}"
        ".ex-bubble:hover {"
        "   border-color: #cba6f7;"
        "   box-shadow: 0 6px 20px rgba(203, 166, 247, 0.35);"
        "}"
        ".ex-bubble.done {"
        "   background-image: linear-gradient(135deg, #a6e3a1 0%, #94e2d5 100%);"
        "   border-color: #a6e3a1;"
        "   color: #1e1e2e;"
        "   box-shadow: 0 4px 16px rgba(166, 227, 161, 0.35);"
        "}"
        ".ex-bubble.done:hover {"
        "   box-shadow: 0 8px 22px rgba(166, 227, 161, 0.5);"
        "}"
        ".ex-bubble:focus,"
        ".ex-bubble:focus-visible {"
        "   outline: none;"
        "}"
        ".bubble-number {"
        "   font-size: 22px;"
        "   font-weight: 800;"
        "}"
        ".ex-bubble.done .bubble-number {"
        "   color: #1e1e2e;"
        "}"
        ".bubble-icon {"
        "   color: #1e1e2e;"
        "}"
        ".ex-label {"
        "   color: #cdd6f4;"
        "   font-size: 12px;"
        "   font-weight: 600;"
        "}"
        ".ex-prompt {"
        "   color: #cdd6f4;"
        "   font-size: 16px;"
        "   font-weight: 500;"
        "}"
        ".ex-sub {"
        "   color: #a6adc8;"
        "   font-size: 13px;"
        "   font-weight: 700;"
        "}"
        ".hint {"
        "   color: #a6adc8;"
        "   font-size: 13px;"
        "   font-style: italic;"
        "}"
        ".dlg-speaker {"
        "   color: #cba6f7;"
        "   font-weight: 700;"
        "   font-size: 15px;"
        "}"
        ".number-big {"
        "   color: #f9e2af;"
        "   font-size: 26px;"
        "   font-weight: 800;"
        "}"
        ".chain {"
        "   color: #f9e2af;"
        "   font-size: 20px;"
        "   font-weight: 700;"
        "   letter-spacing: 1px;"
        "}"
        "entry {"
        "   background-color: #181825;"
        "   color: #cdd6f4;"
        "   border: 1px solid #45475a;"
        "   border-radius: 10px;"
        "   padding: 8px 12px;"
        "   font-size: 15px;"
        "   caret-color: #cba6f7;"
        "}"
        "entry:focus {"
        "   border-color: #cba6f7;"
        "}"
        "combobox {"
        "   background-color: #181825;"
        "   color: #cdd6f4;"
        "}"
        "combobox button {"
        "   background-image: none;"
        "   background-color: #181825;"
        "   color: #cdd6f4;"
        "   border: 1px solid #45475a;"
        "   border-radius: 10px;"
        "   padding: 4px 10px;"
        "}"
        "combobox button:hover {"
        "   border-color: #cba6f7;"
        "}"
        "combobox.answer-ok button {"
        "   border-color: #a6e3a1;"
        "   background-color: rgba(166, 227, 161, 0.12);"
        "}"
        "combobox.answer-wrong button {"
        "   border-color: #f38ba8;"
        "   background-color: rgba(243, 139, 168, 0.12);"
        "}"
        ".pill {"
        "   background-color: #2b2d40;"
        "   color: #cdd6f4;"
        "   border: 2px solid #3b3e56;"
        "   border-radius: 999px;"
        "   padding: 6px 18px;"
        "   font-size: 15px;"
        "   transition: border-color 150ms ease, box-shadow 150ms ease;"
        "}"
        ".pill:hover {"
        "   border-color: #cba6f7;"
        "}"
        ".pill:checked {"
        "   background-image: linear-gradient(135deg, #cba6f7 0%, #b4befe 100%);"
        "   color: #1e1e2e;"
        "   border-color: #cba6f7;"
        "   font-weight: 700;"
        "}"
        ".pill.ok {"
        "   border-color: #a6e3a1;"
        "   box-shadow: 0 0 0 2px rgba(166, 227, 161, 0.4);"
        "}"
        ".pill.wrong {"
        "   border-color: #f38ba8;"
        "   box-shadow: 0 0 0 2px rgba(243, 139, 168, 0.4);"
        "}"
        ".chip {"
        "   background-color: #313244;"
        "   color: #cdd6f4;"
        "   border: 1px solid #45475a;"
        "   border-radius: 999px;"
        "   padding: 6px 14px;"
        "   font-size: 14px;"
        "   font-weight: 600;"
        "   transition: background-color 150ms ease, border-color 150ms ease;"
        "}"
        ".chip:hover {"
        "   background-color: #45475a;"
        "   border-color: #cba6f7;"
        "}"
        ".group-btn {"
        "   background-color: #2b2d40;"
        "   color: #cdd6f4;"
        "   border: 2px solid #3b3e56;"
        "   border-radius: 12px;"
        "   padding: 8px 18px;"
        "   font-size: 14px;"
        "   font-weight: 700;"
        "   transition: border-color 150ms ease, box-shadow 150ms ease;"
        "}"
        ".group-btn:hover {"
        "   border-color: #cba6f7;"
        "}"
        ".group-btn:checked {"
        "   background-image: linear-gradient(135deg, #89b4fa 0%, #89dceb 100%);"
        "   color: #1e1e2e;"
        "   border-color: #89b4fa;"
        "}"
        ".group-panel {"
        "   background-color: rgba(49, 50, 68, 0.5);"
        "   border: 2px dashed #45475a;"
        "   border-radius: 14px;"
        "   padding: 10px;"
        "}"
        ".group-panel-label {"
        "   color: #a6adc8;"
        "   font-size: 13px;"
        "   font-weight: 700;"
        "}"
        ".sent-group {"
        "   background-color: rgba(49, 50, 68, 0.5);"
        "   border: 1px solid #45475a;"
        "   border-radius: 14px;"
        "   padding: 12px;"
        "}"
        ".sent-group.ok {"
        "   border-color: #a6e3a1;"
        "}"
        ".sent-group.wrong {"
        "   border-color: #f38ba8;"
        "}"
        ".feedback-ok {"
        "   color: #a6e3a1;"
        "   font-size: 15px;"
        "   font-weight: 700;"
        "}"
        ".feedback-err {"
        "   color: #f38ba8;"
        "   font-size: 15px;"
        "   font-weight: 700;"
        "}"
        "tooltip {"
        "   background-color: #181825;"
        "   border: 1px solid #45475a;"
        "   border-radius: 10px;"
        "   color: #cdd6f4;"
        "}"
    );

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_object_unref(provider);

    gtk_window_present(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.example.Sprechen", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
