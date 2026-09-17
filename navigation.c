#include "maturita.h"

/* ------------------------------------------------------------------ */
/* Navigation                                                         */
/* ------------------------------------------------------------------ */

void on_nav_clicked(GtkButton *button, gpointer user_data) {
    const char *target = g_object_get_data(G_OBJECT(button), "target");
    (void)user_data;
    if (target)
        gtk_stack_set_visible_child_name(main_stack, target);
}

void on_continue_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    gtk_stack_set_visible_child_name(main_stack, "subjects");
}

GtkWidget *make_back_button(const char *target) {
    GtkWidget *back = gtk_button_new();
    GtkWidget *icon = gtk_drawing_area_new();

    gtk_widget_set_size_request(icon, 14, 14);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(icon), draw_back_icon,
                                   NULL, NULL);
    gtk_button_set_child(GTK_BUTTON(back), icon);
    gtk_widget_add_css_class(back, "back-btn");
    gtk_widget_set_valign(back, GTK_ALIGN_CENTER);
    i18n_bind(back, "back", 2);
    g_signal_connect_swapped(back, "state-flags-changed",
                             G_CALLBACK(gtk_widget_queue_draw), icon);
    g_object_set_data_full(G_OBJECT(back), "target", g_strdup(target), g_free);
    g_signal_connect(back, "clicked", G_CALLBACK(on_nav_clicked), NULL);
    return back;
}

GtkWidget *top_bar(const char *back_target, const char *title,
                          const char *subtitle) {
    GtkWidget *top = gtk_center_box_new();
    gtk_widget_set_margin_top(top, 2);
    gtk_widget_set_margin_bottom(top, 6);

    GtkWidget *back = make_back_button(back_target);
    gtk_center_box_set_start_widget(GTK_CENTER_BOX(top), back);

    GtkWidget *center = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_widget_set_valign(center, GTK_ALIGN_CENTER);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(top), center);

    GtkWidget *heading = gtk_label_new(NULL);
    i18n_bind(heading, title, 0);
    gtk_widget_set_halign(heading, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(heading, "roadmap-title");
    gtk_box_append(GTK_BOX(center), heading);

    if (subtitle) {
        GtkWidget *sub = gtk_label_new(NULL);
        i18n_bind(sub, subtitle, 0);
        gtk_widget_set_halign(sub, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(sub, "roadmap-sub");
        gtk_label_set_wrap(GTK_LABEL(sub), TRUE);
        gtk_label_set_max_width_chars(GTK_LABEL(sub), 60);
        gtk_box_append(GTK_BOX(center), sub);
    }

    return top;
}

gboolean on_window_key_pressed(GtkEventControllerKey *controller,
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

GtkWidget *build_welcome_page(void) {
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *heading;
    GtkWidget *body;
    GtkWidget *button;

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(box, 40);
    gtk_widget_set_margin_bottom(box, 40);

    icon = gtk_image_new_from_icon_name(ICON_NAME);
    gtk_image_set_pixel_size(GTK_IMAGE(icon), 88);
    gtk_widget_set_halign(icon, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom(icon, 22);
    gtk_box_append(GTK_BOX(box), icon);

    heading = gtk_label_new(NULL);
    welcome_heading = heading;
    gtk_widget_set_halign(heading, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(heading, "heading");
    gtk_box_append(GTK_BOX(box), heading);

    body = gtk_label_new(NULL);
    i18n_bind(body, "welcome_body", 0);
    gtk_label_set_justify(GTK_LABEL(body), GTK_JUSTIFY_CENTER);
    gtk_label_set_wrap(GTK_LABEL(body), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(body), 46);
    gtk_widget_set_margin_top(body, 10);
    gtk_widget_add_css_class(body, "welcome-body");
    gtk_box_append(GTK_BOX(box), body);

    button = gtk_button_new();
    i18n_bind(button, "continue", 1);
    gtk_widget_set_halign(button, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(button, 28);
    gtk_widget_add_css_class(button, "btn-primary");
    gtk_box_append(GTK_BOX(box), button);

    g_signal_connect(button, "clicked", G_CALLBACK(on_continue_clicked), NULL);

    return box;
}
