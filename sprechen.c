#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <math.h>

#define NUM_UNITS 12
#define FIRST_LOCKED 1

#define NODE_SIZE    88.0
#define PATH_SPAC    150.0
#define PATH_CYCLES  2.0
#define PATH_MARGIN  72.0
#define PATH_HEIGHT  380.0
#define PATH_AMP     100.0

static GtkStack *main_stack;

static double path_x_at(double t) {
    return PATH_MARGIN + (NODE_SIZE / 2.0) + t * PATH_SPAC;
}

static double path_y_at(double t) {
    double mid = PATH_HEIGHT / 2.0;
    double phase = 2.0 * G_PI * PATH_CYCLES / (NUM_UNITS - 1);
    return mid + PATH_AMP * sin(t * phase);
}

static void on_continue_clicked(GtkButton *button, gpointer user_data) {
    gtk_stack_set_visible_child_name(main_stack, "roadmap");
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

static GtkWidget *build_welcome_page(void) {
    GtkWidget *box;
    GtkWidget *heading;
    GtkWidget *card;
    GtkWidget *button;

    // Create a vertical box to hold contents
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(box, 40);
    gtk_widget_set_margin_bottom(box, 40);

    // Heading
    heading = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(heading),
        "Vítejte ve <span color=\"#cba6f7\">Sprechen.C</span>!");
    gtk_widget_set_halign(heading, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(heading, "heading");
    gtk_box_append(GTK_BOX(box), heading);

    // Card with the welcome message
    card = gtk_label_new(
        "Sprechen.C je vzdělávací program na procvičování Němčiny, určený "
        "pro studenty středních škol a gymnázií.\n\n"
        "Program je napsaný v Céčku studentama ze SSŠVT!");
    gtk_label_set_justify(GTK_LABEL(card), GTK_JUSTIFY_CENTER);
    gtk_label_set_wrap(GTK_LABEL(card), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(card), 48);
    gtk_widget_add_css_class(card, "card");
    gtk_box_append(GTK_BOX(box), card);

    // Continue button
    button = gtk_button_new_with_label("Pokračuj");
    gtk_widget_set_halign(button, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(button, 6);
    gtk_widget_add_css_class(button, "btn-primary");
    gtk_box_append(GTK_BOX(box), button);

    g_signal_connect(button, "clicked", G_CALLBACK(on_continue_clicked), NULL);

    return box;
}

static void draw_rail(GtkDrawingArea *area, cairo_t *cr,
                      int width, int height, gpointer user_data) {
    const int segments = (NUM_UNITS - 1) * 60;
    const double t_end = (double)(NUM_UNITS - 1);
    const double t_lit = (double)(FIRST_LOCKED - 1);
    int k;

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
    cairo_move_to(cr, path_x_at(0), path_y_at(0));
    for (k = 1; k <= segments; k++) {
        double t = t_end * (double)k / (double)segments;
        cairo_line_to(cr, path_x_at(t), path_y_at(t));
    }
    cairo_set_line_width(cr, 16);
    cairo_set_source_rgb(cr, 0.27059, 0.27843, 0.35294);
    cairo_stroke(cr);

    if (t_lit > 0.0) {
        int lit_segments = (int)(t_lit / t_end * (double)segments + 0.5);
        cairo_new_path(cr);
        cairo_move_to(cr, path_x_at(0), path_y_at(0));
        for (k = 1; k <= lit_segments; k++) {
            double t = t_end * (double)k / (double)segments;
            cairo_line_to(cr, path_x_at(t), path_y_at(t));
        }
        cairo_set_source_rgba(cr, 0.79608, 0.65098, 0.96863, 0.20);
        cairo_set_line_width(cr, 36);
        cairo_stroke_preserve(cr);
        cairo_set_source_rgb(cr, 0.79608, 0.65098, 0.96863);
        cairo_set_line_width(cr, 16);
        cairo_stroke(cr);
    }
}

static void add_path_node(GtkFixed *fixed, int index) {
    int num = index + 1;
    gboolean locked = index >= FIRST_LOCKED;
    double cx = path_x_at(index);
    double cy = path_y_at(index);
    GtkWidget *card;
    GtkWidget *vbox;
    GtkWidget *number;
    char *text;

    card = gtk_button_new();
    gtk_widget_add_css_class(card, "unit-node");
    gtk_widget_set_sensitive(card, !locked);
    gtk_widget_set_size_request(card, (int)NODE_SIZE, (int)NODE_SIZE);
    if (locked)
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

    if (locked) {
        GtkWidget *lock;
        lock = gtk_image_new_from_icon_name("system-lock-screen-symbolic");
        gtk_image_set_pixel_size(GTK_IMAGE(lock), 16);
        gtk_widget_add_css_class(lock, "lock-icon");
        gtk_box_append(GTK_BOX(vbox), lock);
    }

    text = g_strdup_printf("Jednotka %d%s", num, locked ? " (zamčeno)" : "");
    gtk_widget_set_tooltip_text(card, text);
    g_free(text);

    gtk_fixed_put(fixed, card,
                  (int)(cx - NODE_SIZE / 2.0),
                  (int)(cy - NODE_SIZE / 2.0));
}

static GtkWidget *build_roadmap_page(void) {
    GtkWidget *page;
    GtkWidget *heading;
    GtkWidget *sub;
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *fixed;
    GtkWidget *rail;
    int canvas_w;
    int i;

    canvas_w = (int)(path_x_at(NUM_UNITS - 1) + PATH_MARGIN);

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 28);
    gtk_widget_set_margin_bottom(page, 24);

    heading = gtk_label_new("Učební plán");
    gtk_widget_set_halign(heading, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(heading, "roadmap-title");
    gtk_box_append(GTK_BOX(page), heading);

    sub = gtk_label_new("Vyberte jednotku na cestě a začněte procvičovat.");
    gtk_widget_set_halign(sub, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(sub, "roadmap-sub");
    gtk_box_append(GTK_BOX(page), sub);

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 18);
    gtk_box_append(GTK_BOX(page), scroll);

    wrap = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), wrap);

    fixed = gtk_fixed_new();
    gtk_widget_set_halign(fixed, GTK_ALIGN_START);
    gtk_widget_set_valign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(fixed, canvas_w, (int)PATH_HEIGHT);
    gtk_box_append(GTK_BOX(wrap), fixed);

    rail = gtk_drawing_area_new();
    gtk_widget_set_size_request(rail, canvas_w, (int)PATH_HEIGHT);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(rail), draw_rail, NULL, NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rail, 0, 0);

    for (i = 0; i < NUM_UNITS; i++)
        add_path_node(GTK_FIXED(fixed), i);

    return page;
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWindow *window;
    GtkWidget *headerbar;
    GtkWidget *title_label;
    GtkWidget *welcome_page;
    GtkWidget *roadmap_page;
    GtkEventController *keys;
    GtkCssProvider *provider;

    // Create a new application window
    window = GTK_WINDOW(gtk_application_window_new(app));
    gtk_window_set_title(window, "Sprechen.c");
    gtk_window_set_default_size(window, 760, 560);

    // Create a styled headerbar (titlebar)
    headerbar = gtk_header_bar_new();
    gtk_widget_add_css_class(headerbar, "titlebar");
    title_label = gtk_label_new("Sprechen.c");
    gtk_widget_add_css_class(title_label, "app-title");
    gtk_header_bar_set_title_widget(GTK_HEADER_BAR(headerbar), title_label);

    gtk_window_set_titlebar(window, headerbar);

    // Stack navigation between welcome and roadmap pages
    main_stack = GTK_STACK(gtk_stack_new());
    gtk_stack_set_transition_type(GTK_STACK(main_stack),
                                  GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_window_set_child(window, GTK_WIDGET(main_stack));

    welcome_page = build_welcome_page();
    roadmap_page = build_roadmap_page();

    gtk_stack_add_named(main_stack, welcome_page, "welcome");
    gtk_stack_add_named(main_stack, roadmap_page, "roadmap");
    gtk_stack_set_visible_child_name(main_stack, "welcome");

    keys = gtk_event_controller_key_new();
    gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(keys),
                                               GTK_PHASE_CAPTURE);
    g_signal_connect(keys, "key-pressed",
                     G_CALLBACK(on_window_key_pressed), window);
    gtk_widget_add_controller(GTK_WIDGET(window), GTK_EVENT_CONTROLLER(keys));

    // Load Catppuccin Mocha CSS
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider,
        "window {"
        "   background-color: #1e1e2e;"
        "   background-image: linear-gradient(160deg, #11111b 0%, #1e1e2e 40%, #313244 100%);"
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
        ".unit-node {"
        "   background-image: linear-gradient(135deg, #cba6f7 0%, #b4befe 100%);"
        "   color: #1e1e2e;"
        "   border: none;"
        "   border-radius: 999px;"
        "   padding: 0;"
        "   box-shadow: 0 4px 16px rgba(203, 166, 247, 0.35), 0 0 0 1px rgba(203, 166, 247, 0.25);"
        "   transition: transform 150ms ease, box-shadow 150ms ease;"
        "}"
        ".unit-node:hover {"
        "   transform: translateY(-3px);"
        "   box-shadow: 0 10px 26px rgba(203, 166, 247, 0.55), 0 0 0 2px rgba(203, 166, 247, 0.35);"
        "}"
        ".unit-node:active {"
        "   transform: translateY(0px);"
        "   box-shadow: 0 4px 12px rgba(203, 166, 247, 0.45);"
        "}"
        ".unit-node.locked {"
        "   background-image: none;"
        "   background-color: #313244;"
        "   color: #a6adc8;"
        "   border: 1px solid #45475a;"
        "   box-shadow: none;"
        "}"
        ".unit-node.locked:hover {"
        "   transform: none;"
        "   box-shadow: none;"
        "}"
        ".unit-number {"
        "   font-size: 24px;"
        "   font-weight: 800;"
        "}"
        ".unit-node.locked .unit-number {"
        "   color: #a6adc8;"
        "}"
        ".lock-icon {"
        "   color: #6c7086;"
        "}"
    );

    // Apply CSS to the window
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
