#include <gtk/gtk.h>

#define NUM_UNITS 12
#define FIRST_LOCKED 1

static GtkStack *main_stack;

static void on_continue_clicked(GtkButton *button, gpointer user_data) {
    gtk_stack_set_visible_child_name(main_stack, "roadmap");
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

static GtkWidget *build_roadmap_page(void) {
    GtkWidget *page;
    GtkWidget *heading;
    GtkWidget *sub;
    GtkWidget *scroll;
    GtkWidget *flow;
    int i;

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 28);
    gtk_widget_set_margin_bottom(page, 24);

    // Page heading
    heading = gtk_label_new("Učební plán");
    gtk_widget_set_halign(heading, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(heading, "roadmap-title");
    gtk_box_append(GTK_BOX(page), heading);

    sub = gtk_label_new("Vyberte jednotku a začněte procvičovat.");
    gtk_widget_set_halign(sub, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(sub, "roadmap-sub");
    gtk_box_append(GTK_BOX(page), sub);

    // Scrollable grid of unit cards
    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 18);
    gtk_box_append(GTK_BOX(page), scroll);

    flow = gtk_flow_box_new();
    gtk_flow_box_set_homogeneous(GTK_FLOW_BOX(flow), TRUE);
    gtk_flow_box_set_column_spacing(GTK_FLOW_BOX(flow), 16);
    gtk_flow_box_set_row_spacing(GTK_FLOW_BOX(flow), 16);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(flow), 3);
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(flow), GTK_SELECTION_NONE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), flow);

    for (i = 0; i < NUM_UNITS; i++) {
        int num = i + 1;
        gboolean locked = i >= FIRST_LOCKED;
        GtkWidget *card;
        GtkWidget *vbox;
        GtkWidget *badge;
        GtkWidget *row;
        GtkWidget *title;
        char *text;

        card = gtk_button_new();
        gtk_widget_add_css_class(card, "unit-card");
        gtk_widget_set_sensitive(card, !locked);
        gtk_widget_set_size_request(card, 190, 170);
        if (locked)
            gtk_widget_add_css_class(card, "locked");

        vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
        gtk_button_set_child(GTK_BUTTON(card), vbox);

        // Number badge
        text = g_strdup_printf("%d", num);
        badge = gtk_label_new(text);
        g_free(text);
        gtk_widget_add_css_class(badge, "unit-badge");
        gtk_box_append(GTK_BOX(vbox), badge);

        // Title row (with lock icon when locked)
        row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        gtk_widget_set_halign(row, GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(vbox), row);

        if (locked) {
            GtkWidget *lock = gtk_image_new_from_icon_name("system-lock-screen-symbolic");
            gtk_image_set_pixel_size(GTK_IMAGE(lock), 14);
            gtk_widget_add_css_class(lock, "lock-icon");
            gtk_box_append(GTK_BOX(row), lock);
        }

        text = g_strdup_printf("Jednotka %d", num);
        title = gtk_label_new(text);
        g_free(text);
        gtk_widget_add_css_class(title, "unit-title");
        gtk_box_append(GTK_BOX(row), title);

        gtk_flow_box_insert(GTK_FLOW_BOX(flow), card, -1);
    }

    return page;
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWindow *window;
    GtkWidget *headerbar;
    GtkWidget *title_label;
    GtkWidget *welcome_page;
    GtkWidget *roadmap_page;
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
        ".unit-card {"
        "   background-color: rgba(30, 30, 46, 0.65);"
        "   border: 1px solid #45475a;"
        "   border-radius: 20px;"
        "   padding: 24px 18px;"
        "   box-shadow: 0 4px 14px rgba(0, 0, 0, 0.25);"
        "   transition: transform 150ms ease, box-shadow 150ms ease, border-color 150ms ease;"
        "}"
        ".unit-card:hover {"
        "   transform: translateY(-3px);"
        "   border-color: #cba6f7;"
        "   box-shadow: 0 10px 26px rgba(0, 0, 0, 0.4), 0 0 0 1px rgba(203, 166, 247, 0.25);"
        "}"
        ".unit-card:active {"
        "   transform: translateY(0px);"
        "}"
        ".unit-card.locked {"
        "   opacity: 0.55;"
        "   background-color: rgba(30, 30, 46, 0.5);"
        "   box-shadow: none;"
        "}"
        ".unit-badge {"
        "   min-width: 48px;"
        "   min-height: 48px;"
        "   border-radius: 999px;"
        "   padding: 6px 14px;"
        "   color: #11111b;"
        "   background-image: linear-gradient(135deg, #cba6f7 0%, #b4befe 100%);"
        "   font-size: 22px;"
        "   font-weight: 800;"
        "   box-shadow: 0 4px 12px rgba(203, 166, 247, 0.35);"
        "}"
        ".unit-card.locked .unit-badge {"
        "   background-image: none;"
        "   background-color: #45475a;"
        "   color: #a6adc8;"
        "   box-shadow: none;"
        "}"
        ".unit-title {"
        "   color: #cdd6f4;"
        "   font-size: 15px;"
        "   font-weight: 600;"
        "}"
        ".unit-card.locked .unit-title {"
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
