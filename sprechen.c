#include <gtk/gtk.h>
#include <glib.h>
#include <string.h>

typedef struct {
    GtkWidget *label;
    GtkCssProvider *provider;
} ScaleData;

static void
on_window_notify (GObject *object,
                  GParamSpec *pspec,
                  gpointer user_data)
{
    // We get the ScaleData from the window
    ScaleData *scale_data = g_object_get_data(G_OBJECT(object), "scale-data");
    if (!scale_data || !scale_data->label || !scale_data->provider)
        return;

    GtkWidget *window = GTK_WIDGET(object);
    int width;
    g_object_get(window, "width", &width, NULL);

    // Calculate font size in points based on width
    // Base: 600px width -> 18 points
    double points_per_pixel = 18.0 / 600.0;
    double font_size_points = width * points_per_pixel;
    // Clamp between 10 and 24 points
    if (font_size_points < 10.0) font_size_points = 10.0;
    if (font_size_points > 24.0) font_size_points = 24.0;

    // Update the CSS provider with the new font size
    char *css_data = g_strdup_printf(
        "window {"
        "   background-color: #2E3440;"
        "   background-image: linear-gradient(135deg, #2E3440 0%%, #3B4252 100%%);"
        "   color: #ECEFF4;"
        "   font-family: \"Helvetica Neue\", Helvetica, Arial, sans-serif;"
        "}"
        "label {"
        "   margin: 30px;"
        "   padding: 30px;"
        "   background-color: rgba(59, 66, 82, 0.8);"
        "   border-radius: 16px;"
        "   box-shadow: 0 8px 16px rgba(0,0,0,0.4);"
        "   font-size: %dpt;"
        "}",
        (int)font_size_points);

    gtk_css_provider_load_from_string(scale_data->provider, css_data);
    g_free(css_data);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *label;
    GtkCssProvider *provider;
    ScaleData *scale_data;

    // Create a new application window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Sprechen.c");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    // Create a vertical box to hold contents
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_window_set_child(GTK_WINDOW(window), box);

    // Create a label with welcome message
    label = gtk_label_new("Vítejte ve Sprechen.C!\n\nSprechen.C je vzdělávací program na procvičování Němčiny, určený pro studenty středních škol a gymnázií. Program je napsaný v Céčku studentama ze SSŠVT!");
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
    gtk_label_set_lines(label, -1);  // Allow multiple lines
    gtk_label_set_xalign(label, 0.5);  // Center horizontally
    gtk_label_set_yalign(label, 0.5);  // Center vertically
    gtk_label_set_wrap(label, TRUE);  // Enable line wrapping
    gtk_label_set_max_width_chars(label, 40);
    gtk_box_append(GTK_BOX(box), label);

    // Create and load CSS for beautiful background with initial font size
    provider = gtk_css_provider_new();
    int initial_width = 600; // default width
    double points_per_pixel = 18.0 / 600.0;
    double initial_font_size_points = initial_width * points_per_pixel;
    if (initial_font_size_points < 10.0) initial_font_size_points = 10.0;
    if (initial_font_size_points > 24.0) initial_font_size_points = 24.0;
    char *initial_css = g_strdup_printf(
        "window {"
        "   background-color: #2E3440;"
        "   background-image: linear-gradient(135deg, #2E3440 0%%, #3B4252 100%%);"
        "   color: #ECEFF4;"
        "   font-family: \"Helvetica Neue\", Helvetica, Arial, sans-serif;"
        "}"
        "label {"
        "   margin: 30px;"
        "   padding: 30px;"
        "   background-color: rgba(59, 66, 82, 0.8);"
        "   border-radius: 16px;"
        "   box-shadow: 0 8px 16px rgba(0,0,0,0.4);"
        "   font-size: %dpt;"
        "}",
        (int)initial_font_size_points);
    gtk_css_provider_load_from_string(provider, initial_css);
    g_free(initial_css);

    // Apply CSS to the window
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);

    // We keep the provider to update it later, so we don't unreference it yet
    // g_object_unref(provider); // We will unreference it in the cleanup? We'll store it in ScaleData and unreference when the window is destroyed.

    // Set up scaling data
    scale_data = g_new(ScaleData, 1);
    scale_data->label = label;
    scale_data->provider = provider;
    g_object_set_data_full(G_OBJECT(window), "scale-data", scale_data, g_free);

    // Connect to the window's notify::width and notify::height signals
    g_signal_connect(window, "notify::width", G_CALLBACK(on_window_notify), NULL);
    g_signal_connect(window, "notify::height", G_CALLBACK(on_window_notify), NULL);

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