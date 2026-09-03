#include <gtk/gtk.h>

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWindow *window;
    GtkWidget *headerbar;
    GtkWidget *title_label;
    GtkWidget *box;
    GtkWidget *heading;
    GtkWidget *card;
    GtkWidget *button;
    GtkCssProvider *provider;

    // Create a new application window
    window = GTK_WINDOW(gtk_application_window_new(app));
    gtk_window_set_title(window, "Sprechen.c");
    gtk_window_set_default_size(window, 720, 520);

    // Create a styled headerbar (titlebar)
    headerbar = gtk_header_bar_new();
    gtk_widget_add_css_class(headerbar, "titlebar");
    title_label = gtk_label_new("Sprechen.c");
    gtk_widget_add_css_class(title_label, "app-title");
    gtk_header_bar_set_title_widget(GTK_HEADER_BAR(headerbar), title_label);
    gtk_window_set_titlebar(window, headerbar);

    // Create a vertical box to hold contents
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(GTK_WIDGET(box), 40);
    gtk_widget_set_margin_bottom(GTK_WIDGET(box), 40);
    gtk_window_set_child(window, box);

    // Heading
    heading = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(heading),
        "Vítejte ve <span color=\"#cba6f7\">Sprechen.C</span>!");
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

    // Load Catppuccin Mocha CSS
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider,
        "window {"
        "   background-color: #1e1e2e;"
        "   background-image: linear-gradient(160deg, #11111b 0%, #1e1e2e 40%, #313244 100%);"
        "}"
        ".titlebar {"
        "   background-color: #1e1e2e;"
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
