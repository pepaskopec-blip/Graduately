#include "maturita.h"

/* ------------------------------------------------------------------ */
/* Settings                                                           */
/* ------------------------------------------------------------------ */

void draw_theme_swatch(GtkDrawingArea *area, cairo_t *cr,
                              int width, int height, gpointer data) {
    ThemeId id = (ThemeId)GPOINTER_TO_INT(data);
    ThemePalette p = theme_palette(id, app_color_mode);
    Rgb base = color_from_hex(p.base);
    Rgb a1 = color_from_hex(p.accent);
    Rgb a2 = color_from_hex(p.accent3);
    Rgb ok = color_from_hex(p.success);
    Rgb accent = color_from_hex(app_theme.accent);
    gboolean selected = (app_theme_id == id);
    double size = MIN(width, height);
    double cx = width / 2.0;
    double cy = height / 2.0;
    double r = size / 2.0 - (selected ? 3.0 : 1.0);
    double dot = MAX(3.5, r * 0.26);
    double gap = dot * 2.05;

    (void)area;

    if (selected) {
        cairo_new_path(cr);
        cairo_arc(cr, cx, cy, size / 2.0 - 0.5, 0.0, 2.0 * G_PI);
        cairo_set_source_rgb(cr, accent.r, accent.g, accent.b);
        cairo_set_line_width(cr, 2.2);
        cairo_stroke(cr);
    }

    cairo_new_path(cr);
    cairo_arc(cr, cx, cy, r, 0.0, 2.0 * G_PI);
    cairo_set_source_rgb(cr, base.r, base.g, base.b);
    cairo_fill(cr);

    cairo_new_path(cr);
    cairo_arc(cr, cx - gap, cy, dot, 0.0, 2.0 * G_PI);
    cairo_set_source_rgb(cr, a1.r, a1.g, a1.b);
    cairo_fill(cr);

    cairo_new_path(cr);
    cairo_arc(cr, cx, cy, dot, 0.0, 2.0 * G_PI);
    cairo_set_source_rgb(cr, a2.r, a2.g, a2.b);
    cairo_fill(cr);

    cairo_new_path(cr);
    cairo_arc(cr, cx + gap, cy, dot, 0.0, 2.0 * G_PI);
    cairo_set_source_rgb(cr, ok.r, ok.g, ok.b);
    cairo_fill(cr);
}

void on_mode_toggled(GtkToggleButton *btn, gpointer user_data) {
    ColorMode mode = (ColorMode)GPOINTER_TO_INT(user_data);

    if (!gtk_toggle_button_get_active(btn))
        return;
    if (app_color_mode == mode)
        return;
    app_color_mode = mode;
    apply_theme();
    save_settings();
}

void on_theme_toggled(GtkToggleButton *btn, gpointer user_data) {
    ThemeId id = (ThemeId)GPOINTER_TO_INT(user_data);

    if (!gtk_toggle_button_get_active(btn))
        return;
    if (app_theme_id == id)
        return;
    app_theme_id = id;
    apply_theme();
    save_settings();
}

void on_lang_toggled(GtkToggleButton *btn, gpointer user_data) {
    UiLang lang = (UiLang)GPOINTER_TO_INT(user_data);

    if (!gtk_toggle_button_get_active(btn))
        return;
    if (app_lang == lang)
        return;
    app_lang = lang;
    apply_language();
    save_settings();
}

GtkWidget *make_lang_chip(const char *label, UiLang lang,
                                 GtkToggleButton *group) {
    GtkWidget *btn = gtk_toggle_button_new_with_label(label);

    gtk_widget_add_css_class(btn, "mode-chip");
    gtk_widget_set_hexpand(btn, TRUE);
    if (group)
        gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(btn), group);
    g_signal_connect(btn, "toggled", G_CALLBACK(on_lang_toggled),
                     GINT_TO_POINTER(lang));
    return btn;
}

GtkWidget *make_mode_chip(const char *label, ColorMode mode,
                                 GtkToggleButton *group) {
    GtkWidget *btn = gtk_toggle_button_new();

    i18n_bind(btn, label, 1);
    gtk_widget_add_css_class(btn, "mode-chip");
    gtk_widget_set_hexpand(btn, TRUE);
    if (group)
        gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(btn), group);
    g_signal_connect(btn, "toggled", G_CALLBACK(on_mode_toggled),
                     GINT_TO_POINTER(mode));
    return btn;
}

GtkWidget *make_theme_card(ThemeId id, GtkToggleButton *group) {
    GtkWidget *btn;
    GtkWidget *vbox;
    GtkWidget *swatch;
    GtkWidget *name;

    btn = gtk_toggle_button_new();
    gtk_widget_add_css_class(btn, "theme-card");
    gtk_widget_set_hexpand(btn, TRUE);
    if (group)
        gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(btn), group);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_button_set_child(GTK_BUTTON(btn), vbox);

    swatch = gtk_drawing_area_new();
    gtk_widget_set_size_request(swatch, 44, 44);
    gtk_widget_set_halign(swatch, GTK_ALIGN_CENTER);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(swatch),
                                   draw_theme_swatch,
                                   GINT_TO_POINTER(id), NULL);
    theme_swatch_areas[id] = swatch;
    gtk_box_append(GTK_BOX(vbox), swatch);

    name = gtk_label_new(theme_names[id]);
    gtk_widget_add_css_class(name, "theme-card-name");
    gtk_widget_set_halign(name, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vbox), name);

    g_signal_connect(btn, "toggled", G_CALLBACK(on_theme_toggled),
                     GINT_TO_POINTER(id));
    return btn;
}

void on_settings_clicked(GtkButton *button, gpointer user_data) {
    GtkPopover *popover = GTK_POPOVER(user_data);

    (void)button;
    gtk_popover_popup(popover);
}

GtkWidget *build_stats_button(void) {
    GtkWidget *btn;
    GtkWidget *icon;

    btn = gtk_button_new();
    gtk_widget_add_css_class(btn, "settings-btn");
    gtk_widget_add_css_class(btn, "flat");
    i18n_bind(btn, "stats", 2);
    gtk_widget_set_valign(btn, GTK_ALIGN_CENTER);
    gtk_widget_set_focus_on_click(btn, FALSE);

    icon = gtk_drawing_area_new();
    gtk_widget_set_size_request(icon, 18, 18);
    gtk_widget_set_can_target(icon, FALSE);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(icon),
                                   draw_stats_icon, NULL, NULL);
    gtk_button_set_child(GTK_BUTTON(btn), icon);
    g_signal_connect_swapped(btn, "state-flags-changed",
                             G_CALLBACK(gtk_widget_queue_draw), icon);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_stats_clicked), NULL);

    return btn;
}

GtkWidget *build_settings_button(void) {
    GtkWidget *btn;
    GtkWidget *icon;
    GtkWidget *popover;
    GtkWidget *box;
    GtkWidget *title;
    GtkWidget *label;
    GtkWidget *mode_row;
    GtkWidget *lang_row;
    GtkWidget *theme_grid;
    GtkWidget *dark_btn;
    GtkWidget *light_btn;
    GtkWidget *cs_btn;
    GtkWidget *en_btn;
    GtkWidget *first_theme = NULL;
    int i;

    btn = gtk_button_new();
    gtk_widget_add_css_class(btn, "settings-btn");
    gtk_widget_add_css_class(btn, "flat");
    i18n_bind(btn, "settings", 2);
    gtk_widget_set_valign(btn, GTK_ALIGN_CENTER);
    gtk_widget_set_focus_on_click(btn, FALSE);

    icon = gtk_drawing_area_new();
    gtk_widget_set_size_request(icon, 18, 18);
    gtk_widget_set_can_target(icon, FALSE);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(icon),
                                   draw_settings_icon, NULL, NULL);
    gtk_button_set_child(GTK_BUTTON(btn), icon);
    g_signal_connect_swapped(btn, "state-flags-changed",
                             G_CALLBACK(gtk_widget_queue_draw), icon);

    popover = gtk_popover_new();
    gtk_widget_add_css_class(popover, "settings-popover");
    gtk_popover_set_has_arrow(GTK_POPOVER(popover), TRUE);
    gtk_popover_set_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    gtk_widget_set_parent(popover, btn);
    g_signal_connect_swapped(btn, "destroy",
                             G_CALLBACK(gtk_widget_unparent), popover);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_settings_clicked), popover);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_widget_add_css_class(box, "settings-box");

    title = gtk_label_new(NULL);
    i18n_bind(title, "settings_title", 0);
    gtk_widget_set_halign(title, GTK_ALIGN_START);
    gtk_widget_add_css_class(title, "settings-title");
    gtk_box_append(GTK_BOX(box), title);

    label = gtk_label_new(NULL);
    i18n_bind(label, "mode", 0);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_add_css_class(label, "settings-label");
    gtk_widget_set_margin_top(label, 4);
    gtk_box_append(GTK_BOX(box), label);

    mode_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_add_css_class(mode_row, "settings-seg");
    gtk_box_append(GTK_BOX(box), mode_row);

    dark_btn = make_mode_chip("mode_dark", MODE_DARK, NULL);
    light_btn = make_mode_chip("mode_light", MODE_LIGHT,
                               GTK_TOGGLE_BUTTON(dark_btn));
    g_signal_handlers_block_matched(dark_btn, G_SIGNAL_MATCH_FUNC,
                                    0, 0, NULL, on_mode_toggled, NULL);
    g_signal_handlers_block_matched(light_btn, G_SIGNAL_MATCH_FUNC,
                                    0, 0, NULL, on_mode_toggled, NULL);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dark_btn),
                                 app_color_mode == MODE_DARK);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(light_btn),
                                 app_color_mode == MODE_LIGHT);
    g_signal_handlers_unblock_matched(dark_btn, G_SIGNAL_MATCH_FUNC,
                                      0, 0, NULL, on_mode_toggled, NULL);
    g_signal_handlers_unblock_matched(light_btn, G_SIGNAL_MATCH_FUNC,
                                      0, 0, NULL, on_mode_toggled, NULL);
    gtk_box_append(GTK_BOX(mode_row), dark_btn);
    gtk_box_append(GTK_BOX(mode_row), light_btn);

    label = gtk_label_new(NULL);
    i18n_bind(label, "theme", 0);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_add_css_class(label, "settings-label");
    gtk_widget_set_margin_top(label, 2);
    gtk_box_append(GTK_BOX(box), label);

    theme_grid = gtk_flow_box_new();
    gtk_widget_add_css_class(theme_grid, "theme-grid");
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(theme_grid),
                                    GTK_SELECTION_NONE);
    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(theme_grid), 3);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(theme_grid), 3);
    gtk_flow_box_set_column_spacing(GTK_FLOW_BOX(theme_grid), 8);
    gtk_flow_box_set_row_spacing(GTK_FLOW_BOX(theme_grid), 8);
    gtk_flow_box_set_homogeneous(GTK_FLOW_BOX(theme_grid), TRUE);
    gtk_widget_set_hexpand(theme_grid, TRUE);
    gtk_box_append(GTK_BOX(box), theme_grid);

    for (i = 0; i < THEME_COUNT; i++) {
        GtkWidget *card = make_theme_card(
            (ThemeId)i,
            first_theme ? GTK_TOGGLE_BUTTON(first_theme) : NULL);
        if (!first_theme)
            first_theme = card;
        if (app_theme_id == (ThemeId)i) {
            g_signal_handlers_block_matched(card, G_SIGNAL_MATCH_FUNC,
                                            0, 0, NULL, on_theme_toggled, NULL);
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(card), TRUE);
            g_signal_handlers_unblock_matched(card, G_SIGNAL_MATCH_FUNC,
                                              0, 0, NULL, on_theme_toggled, NULL);
        }
        gtk_flow_box_insert(GTK_FLOW_BOX(theme_grid), card, -1);
    }

    label = gtk_label_new(NULL);
    i18n_bind(label, "language", 0);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_add_css_class(label, "settings-label");
    gtk_widget_set_margin_top(label, 4);
    gtk_box_append(GTK_BOX(box), label);

    lang_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_add_css_class(lang_row, "settings-seg");
    gtk_box_append(GTK_BOX(box), lang_row);

    cs_btn = make_lang_chip("Čeština", LANG_CS, NULL);
    en_btn = make_lang_chip("English", LANG_EN, GTK_TOGGLE_BUTTON(cs_btn));
    g_signal_handlers_block_matched(cs_btn, G_SIGNAL_MATCH_FUNC,
                                    0, 0, NULL, on_lang_toggled, NULL);
    g_signal_handlers_block_matched(en_btn, G_SIGNAL_MATCH_FUNC,
                                    0, 0, NULL, on_lang_toggled, NULL);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cs_btn),
                                 app_lang == LANG_CS);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(en_btn),
                                 app_lang == LANG_EN);
    g_signal_handlers_unblock_matched(cs_btn, G_SIGNAL_MATCH_FUNC,
                                      0, 0, NULL, on_lang_toggled, NULL);
    g_signal_handlers_unblock_matched(en_btn, G_SIGNAL_MATCH_FUNC,
                                      0, 0, NULL, on_lang_toggled, NULL);
    gtk_box_append(GTK_BOX(lang_row), cs_btn);
    gtk_box_append(GTK_BOX(lang_row), en_btn);

    {
        GtkWidget *sw = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw),
                                       GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        gtk_scrolled_window_set_max_content_height(GTK_SCROLLED_WINDOW(sw), 470);
        gtk_scrolled_window_set_has_frame(GTK_SCROLLED_WINDOW(sw), FALSE);
        gtk_scrolled_window_set_propagate_natural_width(GTK_SCROLLED_WINDOW(sw), TRUE);
        gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(sw), TRUE);
        gtk_widget_set_hexpand(sw, FALSE);
        gtk_widget_set_vexpand(sw, FALSE);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(sw), box);
        gtk_popover_set_child(GTK_POPOVER(popover), sw);
    }

    return btn;
}
