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
    gtk_widget_add_css_class(top, "page-top");
    gtk_widget_set_margin_top(top, 8);
    gtk_widget_set_margin_bottom(top, 18);

    GtkWidget *back = make_back_button(back_target);
    gtk_center_box_set_start_widget(GTK_CENTER_BOX(top), back);

    GtkWidget *center = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
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
        gtk_label_set_max_width_chars(GTK_LABEL(sub), 56);
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

static GtkWidget *welcome_feature(const char *title_key, const char *body_key,
                                  const char *link_key) {
    GtkWidget *card;
    GtkWidget *title;
    GtkWidget *body;
    GtkWidget *link;

    card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_add_css_class(card, "welcome-feature");
    gtk_widget_set_hexpand(card, TRUE);

    title = gtk_label_new(NULL);
    i18n_bind(title, title_key, 0);
    gtk_widget_set_halign(title, GTK_ALIGN_START);
    gtk_widget_add_css_class(title, "welcome-feature-title");
    gtk_box_append(GTK_BOX(card), title);

    body = gtk_label_new(NULL);
    i18n_bind(body, body_key, 0);
    gtk_widget_set_halign(body, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(body), TRUE);
    gtk_label_set_xalign(GTK_LABEL(body), 0.0);
    gtk_label_set_max_width_chars(GTK_LABEL(body), 52);
    gtk_widget_add_css_class(body, "welcome-feature-body");
    gtk_box_append(GTK_BOX(card), body);

    link = gtk_button_new();
    i18n_bind(link, link_key, 1);
    gtk_widget_set_halign(link, GTK_ALIGN_START);
    gtk_widget_add_css_class(link, "welcome-link");
    gtk_box_append(GTK_BOX(card), link);
    g_signal_connect(link, "clicked", G_CALLBACK(on_continue_clicked), NULL);

    return card;
}

static GtkWidget *welcome_stat(void) {
    GtkWidget *badge;
    GtkWidget *value;
    GtkWidget *label;

    badge = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_add_css_class(badge, "welcome-badge");
    gtk_widget_set_valign(badge, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(badge, GTK_ALIGN_CENTER);

    value = gtk_label_new(NULL);
    i18n_bind(value, "welcome_stat_value", 0);
    gtk_widget_add_css_class(value, "welcome-badge-value");
    gtk_box_append(GTK_BOX(badge), value);

    label = gtk_label_new(NULL);
    i18n_bind(label, "welcome_stat_label", 0);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
    gtk_widget_add_css_class(label, "welcome-badge-label");
    gtk_box_append(GTK_BOX(badge), label);

    return badge;
}

GtkWidget *build_welcome_page(void) {
    GtkWidget *scroll;
    GtkWidget *page;
    GtkWidget *hero;
    GtkWidget *copy;
    GtkWidget *kicker;
    GtkWidget *heading;
    GtkWidget *body;
    GtkWidget *actions;
    GtkWidget *button;
    GtkWidget *features;

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_hexpand(scroll, TRUE);
    gtk_widget_set_vexpand(scroll, TRUE);

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(page, "welcome-page");
    gtk_widget_set_halign(page, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_size_request(page, 760, -1);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), page);

    hero = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 40);
    gtk_widget_add_css_class(hero, "welcome-hero");
    gtk_widget_set_hexpand(hero, TRUE);
    gtk_box_append(GTK_BOX(page), hero);

    copy = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(copy, TRUE);
    gtk_box_append(GTK_BOX(hero), copy);

    kicker = gtk_label_new(NULL);
    i18n_bind(kicker, "welcome_kicker", 0);
    gtk_widget_set_halign(kicker, GTK_ALIGN_START);
    gtk_widget_add_css_class(kicker, "welcome-kicker");
    gtk_box_append(GTK_BOX(copy), kicker);

    heading = gtk_label_new(NULL);
    welcome_heading = heading;
    i18n_bind(heading, "welcome_title", 0);
    gtk_widget_set_halign(heading, GTK_ALIGN_START);
    gtk_label_set_justify(GTK_LABEL(heading), GTK_JUSTIFY_LEFT);
    gtk_label_set_xalign(GTK_LABEL(heading), 0.0);
    gtk_label_set_wrap(GTK_LABEL(heading), TRUE);
    gtk_widget_add_css_class(heading, "heading");
    gtk_widget_set_margin_top(heading, 10);
    gtk_box_append(GTK_BOX(copy), heading);

    body = gtk_label_new(NULL);
    i18n_bind(body, "welcome_body", 0);
    gtk_label_set_wrap(GTK_LABEL(body), TRUE);
    gtk_label_set_xalign(GTK_LABEL(body), 0.0);
    gtk_label_set_max_width_chars(GTK_LABEL(body), 42);
    gtk_widget_set_halign(body, GTK_ALIGN_START);
    gtk_widget_set_margin_top(body, 14);
    gtk_widget_add_css_class(body, "welcome-body");
    gtk_box_append(GTK_BOX(copy), body);

    actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_top(actions, 28);
    gtk_box_append(GTK_BOX(copy), actions);

    button = gtk_button_new();
    i18n_bind(button, "continue", 1);
    gtk_widget_set_halign(button, GTK_ALIGN_START);
    gtk_widget_add_css_class(button, "btn-primary");
    gtk_box_append(GTK_BOX(actions), button);
    g_signal_connect(button, "clicked", G_CALLBACK(on_continue_clicked), NULL);

    gtk_box_append(GTK_BOX(hero), welcome_stat());

    features = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_widget_set_margin_top(features, 36);
    gtk_box_append(GTK_BOX(page), features);
    gtk_box_append(GTK_BOX(features),
                   welcome_feature("welcome_feat1_title",
                                   "welcome_feat1_body",
                                   "welcome_feat1_link"));
    gtk_box_append(GTK_BOX(features),
                   welcome_feature("welcome_feat2_title",
                                   "welcome_feat2_body",
                                   "welcome_feat2_link"));

    return scroll;
}
