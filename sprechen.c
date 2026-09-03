#include <gtk/gtk.h>

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWindow *window;
    GtkWidget *box;
    GtkLabel *label;
    GtkCssProvider *provider;

    // Create a new application window
    window = GTK_WINDOW(gtk_application_window_new(app));
    gtk_window_set_title(window, "Sprechen.c");
    gtk_window_set_default_size(window, 600, 400);

    // Create a vertical box to hold contents
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_window_set_child(window, box);

    // Create a label with welcome message
    label = GTK_LABEL(gtk_label_new("Vítejte ve Sprechen.C!\n\nSprechen.C je vzdělávací program na procvičování Němčiny, určený pro studenty středních škol a gymnázií. Program je napsaný v Céčku studentama ze SSŠVT!"));
    gtk_label_set_justify(label, GTK_JUSTIFY_CENTER);
    gtk_label_set_lines(label, -1);  // Allow multiple lines
    gtk_label_set_xalign(label, 0.5);  // Center horizontally
    gtk_label_set_yalign(label, 0.5);  // Center vertically
    gtk_label_set_wrap(label, TRUE);  // Enable line wrapping
    gtk_label_set_max_width_chars(label, 40);
    gtk_widget_add_css_class(GTK_WIDGET(label), "welcome");
    gtk_box_append(GTK_BOX(box), GTK_WIDGET(label));

    // Load CSS for beautiful background
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider,
        "window {"
        "   background-color: #2E3440;"  /* Dark bluish background */
        "   background-image: linear-gradient(135deg, #2E3440 0%, #3B4252 100%);"
        "}"
        "label.welcome {"
        "   color: #ECEFF4;"
        "   font-family: \"Helvetica Neue\", Helvetica, Arial, sans-serif;"
        "   font-size: 18px;"
        "   margin: 30px;"
        "   padding: 30px;"
        "   background-color: rgba(59, 66, 82, 0.8);"
        "   border-radius: 16px;"
        "   box-shadow: 0 8px 16px rgba(0,0,0,0.4);"
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
