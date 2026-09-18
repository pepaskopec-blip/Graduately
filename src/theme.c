#include "maturita.h"

Rgb color_from_hex(unsigned int hex) {
    Rgb c;
    c.r = (double)((hex >> 16) & 0xFF) / 255.0;
    c.g = (double)((hex >> 8) & 0xFF) / 255.0;
    c.b = (double)(hex & 0xFF) / 255.0;
    return c;
}

/* Linear blend of two colours: t = 0 keeps a, t = 1 returns b. */
Rgb mix_rgb(Rgb a, Rgb b, double t) {
    Rgb c;
    c.r = a.r + (b.r - a.r) * t;
    c.g = a.g + (b.g - a.g) * t;
    c.b = a.b + (b.b - a.b) * t;
    return c;
}

ThemePalette theme_palette(ThemeId id, ColorMode mode) {
    /* Paper: cream page, white cards, yellow CTA, near-black type. */
    static const ThemePalette cat_dark = {
        0x000000, 0x000000, 0x111111, 0x1d1d1f, 0x2c2c2e, 0x3a3a3c,
        0x86868b, 0xf5f5f7, 0xa1a1a6, 0xffd60a, 0x2997ff, 0x64d2ff,
        0x30d158, 0x64d2ff, 0xffd60a, 0xff453a, 0x1d1d1f,
        0x1d1d1f, 0x111111, 0x3a3a3c, 0x2c2c2e, 0x000000, 0x3a3a3c, 0x86868b
    };
    static const ThemePalette cat_light = {
        0xf3eee6, 0xfaf7f2, 0xffffff, 0xffffff, 0xf0ebe3, 0xe5dfd4,
        0x8a8378, 0x1a1a1a, 0x5c574f, 0xffcc00, 0x0071e3, 0x2997ff,
        0x1f8a4c, 0x1a7f4a, 0xe6a800, 0xd32f2f, 0x1a1a1a,
        0xffffff, 0xf0ebe3, 0xd5cfc4, 0xe5dfd4, 0xf3eee6, 0xd5cfc4, 0x8a8378
    };
    /* Nord */
    static const ThemePalette nord_dark = {
        0x2e3440, 0x3b4252, 0x2e3440, 0x434c5e, 0x4c566a, 0x616e88,
        0x7b88a1, 0xeceff4, 0xd8dee9, 0x88c0d0, 0x81a1c1, 0x5e81ac,
        0xa3be8c, 0x8fbcbb, 0xebcb8b, 0xbf616a, 0x2e3440,
        0x434c5e, 0x3b4252, 0x4c566a, 0x2e3440, 0x2e3440, 0x434c5e, 0x4c566a
    };
    static const ThemePalette nord_light = {
        0xd8dee9, 0xeceff4, 0xe5e9f0, 0xd8dee9, 0xc7ceda, 0xb0b8c8,
        0x7b88a1, 0x2e3440, 0x4c566a, 0x5e81ac, 0x81a1c1, 0x5e81ac,
        0xa3be8c, 0x8fbcbb, 0xebcb8b, 0xbf616a, 0xeceff4,
        0xe5e9f0, 0xd8dee9, 0xc7ceda, 0xd8dee9, 0xd8dee9, 0xc7ceda, 0x7b88a1
    };
    /* Dracula / light soft */
    static const ThemePalette dra_dark = {
        0x191a21, 0x282a36, 0x21222c, 0x44475a, 0x6272a4, 0x707eb0,
        0x6272a4, 0xf8f8f2, 0xbfbfb2, 0xbd93f9, 0xff79c6, 0x8be9fd,
        0x50fa7b, 0x8be9fd, 0xf1fa8c, 0xff5555, 0x282a36,
        0x383a4a, 0x2f3240, 0x44475a, 0x21222c, 0x191a21, 0x383a4a, 0x6272a4
    };
    static const ThemePalette dra_light = {
        0xd5d7e0, 0xf8f8f2, 0xefefea, 0xe2e2d8, 0xd0d0c4, 0xbfbfb2,
        0x9a9a8c, 0x282a36, 0x44475a, 0x7c3aed, 0xd946a6, 0x0891b2,
        0x16a34a, 0x0e7490, 0xca8a04, 0xdc2626, 0xf8f8f2,
        0xefefea, 0xe2e2d8, 0xd0d0c4, 0xe2e2d8, 0xd5d7e0, 0xd0d0c4, 0x9a9a8c
    };
    /* Rose Pine / Dawn */
    static const ThemePalette rose_dark = {
        0x191724, 0x1f1d2e, 0x1f1d2e, 0x26233a, 0x403d52, 0x524f67,
        0x6e6a86, 0xe0def4, 0x908caa, 0xc4a7e7, 0xebbcba, 0x9ccfd8,
        0x31748f, 0x9ccfd8, 0xf6c177, 0xeb6f92, 0x191724,
        0x26233a, 0x21202e, 0x403d52, 0x191724, 0x191724, 0x26233a, 0x524f67
    };
    static const ThemePalette rose_light = {
        0xf2e9e1, 0xfaf4ed, 0xfffaf3, 0xf2e9e1, 0xdfdad9, 0xcecacd,
        0x9893a5, 0x575279, 0x797593, 0x907aa9, 0xd7827e, 0x56949f,
        0x286983, 0x56949f, 0xea9d34, 0xb4637a, 0xfaf4ed,
        0xfffaf3, 0xf2e9e1, 0xdfdad9, 0xf2e9e1, 0xf2e9e1, 0xdfdad9, 0x9893a5
    };
    /* Ocean / Tokyo Night inspired */
    static const ThemePalette ocean_dark = {
        0x16161e, 0x1a1b26, 0x16161e, 0x24283b, 0x414868, 0x565f89,
        0x565f89, 0xc0caf5, 0xa9b1d6, 0x7aa2f7, 0xbb9af7, 0x7dcfff,
        0x9ece6a, 0x73daca, 0xe0af68, 0xf7768e, 0x1a1b26,
        0x24283b, 0x1f2335, 0x3b4261, 0x16161e, 0x16161e, 0x24283b, 0x414868
    };
    static const ThemePalette ocean_light = {
        0xd6dae8, 0xe1e2ec, 0xd5d6e2, 0xc8c9d6, 0xb4b6c5, 0x9aa0b5,
        0x7a8199, 0x2e3446, 0x4a5168, 0x2e7de9, 0x7847bd, 0x007197,
        0x387068, 0x007197, 0x8c6c3e, 0xc64343, 0xe1e2ec,
        0xd5d6e2, 0xc8c9d6, 0xb4b6c5, 0xc8c9d6, 0xd6dae8, 0xb4b6c5, 0x7a8199
    };

    /* Gruvbox */
    static const ThemePalette gruv_dark = {
        0x1d2021, 0x282828, 0x1d2021, 0x3c3836, 0x504945, 0x665c54,
        0x928374, 0xebdbb2, 0xa89984, 0xfe8019, 0xd3869b, 0x83a598,
        0xb8bb26, 0x8ec07c, 0xfabd2f, 0xfb4934, 0x1d2021,
        0x3c3836, 0x32302f, 0x504945, 0x1d2021, 0x1d2021, 0x3c3836, 0x504945
    };
    static const ThemePalette gruv_light = {
        0xebdbb2, 0xfbf1c7, 0xf2e5bc, 0xebdbb2, 0xd5c4a1, 0xbdae93,
        0x928374, 0x3c3836, 0x504945, 0xd65d0e, 0xb16286, 0x076678,
        0x79740e, 0x427b58, 0xb57614, 0x9d0006, 0xfbf1c7,
        0xf2e5bc, 0xebdbb2, 0xd5c4a1, 0xebdbb2, 0xebdbb2, 0xd5c4a1, 0x928374
    };
    /* Solarized */
    static const ThemePalette sol_dark = {
        0x002b36, 0x073642, 0x002b36, 0x586e75, 0x657b83, 0x839496,
        0x93a1a1, 0xfdf6e3, 0x93a1a1, 0x268bd2, 0x6c71c4, 0x2aa198,
        0x859900, 0x2aa198, 0xb58900, 0xdc322f, 0x002b36,
        0x073642, 0x002b36, 0x586e75, 0x002b36, 0x002b36, 0x073642, 0x586e75
    };
    static const ThemePalette sol_light = {
        0xeee8d5, 0xfdf6e3, 0xeee8d5, 0xeee8d5, 0xe2dbc6, 0xc9c0a6,
        0x657b83, 0x586e75, 0x657b83, 0x268bd2, 0x6c71c4, 0x2aa198,
        0x859900, 0x2aa198, 0xb58900, 0xdc322f, 0xfdf6e3,
        0xeee8d5, 0xe6dfc8, 0x93a1a1, 0xeee8d5, 0xeee8d5, 0x93a1a1, 0x586e75
    };
    /* Everforest */
    static const ThemePalette ever_dark = {
        0x1e2326, 0x272e33, 0x2d353b, 0x374145, 0x4f5b58, 0x7a8478,
        0x859289, 0xd3c6aa, 0xa7c080, 0xa7c080, 0xd699b6, 0x7fbbb3,
        0xa7c080, 0x83c092, 0xdbbc7f, 0xe67e80, 0x272e33,
        0x374145, 0x2d353b, 0x4f5b58, 0x1e2326, 0x1e2326, 0x374145, 0x4f5b58
    };
    static const ThemePalette ever_light = {
        0xe6e2cc, 0xfdf6e3, 0xf4f0d9, 0xefebd4, 0xe0dcc7, 0xa6b0a0,
        0x939f91, 0x5c6a72, 0x829181, 0x8da101, 0xdf69ba, 0x35a77c,
        0x8da101, 0x35a77c, 0xdfa000, 0xf85552, 0xfdf6e3,
        0xf4f0d9, 0xefebd4, 0xe0dcc7, 0xe6e2cc, 0xe6e2cc, 0xe0dcc7, 0x939f91
    };
    /* Monokai */
    static const ThemePalette mono_dark = {
        0x1d1e19, 0x272822, 0x1e1f1a, 0x3e3d32, 0x49483e, 0x75715e,
        0x75715e, 0xf8f8f2, 0xa59f85, 0xf92672, 0xae81ff, 0x66d9ef,
        0xa6e22e, 0xa1efe4, 0xe6db74, 0xf92672, 0x272822,
        0x3e3d32, 0x2e2f29, 0x49483e, 0x1d1e19, 0x1d1e19, 0x3e3d32, 0x49483e
    };
    static const ThemePalette mono_light = {
        0xe6e3d4, 0xfaf8f0, 0xf3f0e4, 0xe8e4d4, 0xd6d2c0, 0xb8b49e,
        0x8a8674, 0x2d2a2e, 0x5b5854, 0xe14775, 0x7c5cbf, 0x1c7d9b,
        0x5b8c2a, 0x1c7d9b, 0xb08900, 0xe14775, 0xfaf8f0,
        0xf3f0e4, 0xe8e4d4, 0xd6d2c0, 0xe6e3d4, 0xe6e3d4, 0xd6d2c0, 0x8a8674
    };
    /* One Dark / One Light */
    static const ThemePalette one_dark = {
        0x21252b, 0x282c34, 0x21252b, 0x3b4048, 0x4b5263, 0x5c6370,
        0x5c6370, 0xabb2bf, 0x7f848e, 0xc678dd, 0x61afef, 0x56b6c2,
        0x98c379, 0x56b6c2, 0xe5c07b, 0xe06c75, 0x282c34,
        0x3b4048, 0x2c313a, 0x4b5263, 0x21252b, 0x21252b, 0x3b4048, 0x4b5263
    };
    static const ThemePalette one_light = {
        0xe5e5e6, 0xfafafa, 0xf0f0f0, 0xe5e5e6, 0xd0d0d2, 0xa0a1a7,
        0xa0a1a7, 0x383a42, 0x696c77, 0xa626a4, 0x4078f2, 0x0184bc,
        0x50a14f, 0x0184bc, 0xc18401, 0xe45649, 0xfafafa,
        0xf0f0f0, 0xe5e5e6, 0xd0d0d2, 0xe5e5e6, 0xe5e5e6, 0xd0d0d2, 0xa0a1a7
    };

    switch (id) {
        case THEME_NORD:
            return mode == MODE_LIGHT ? nord_light : nord_dark;
        case THEME_DRACULA:
            return mode == MODE_LIGHT ? dra_light : dra_dark;
        case THEME_ROSE_PINE:
            return mode == MODE_LIGHT ? rose_light : rose_dark;
        case THEME_OCEAN:
            return mode == MODE_LIGHT ? ocean_light : ocean_dark;
        case THEME_GRUVBOX:
            return mode == MODE_LIGHT ? gruv_light : gruv_dark;
        case THEME_SOLARIZED:
            return mode == MODE_LIGHT ? sol_light : sol_dark;
        case THEME_EVERFOREST:
            return mode == MODE_LIGHT ? ever_light : ever_dark;
        case THEME_MONOKAI:
            return mode == MODE_LIGHT ? mono_light : mono_dark;
        case THEME_ONE_DARK:
            return mode == MODE_LIGHT ? one_light : one_dark;
        case THEME_CATPPUCCIN:
        default:
            return mode == MODE_LIGHT ? cat_light : cat_dark;
    }
}
char *read_css_file(void) {
    gchar *contents = NULL;
    GError *err = NULL;

    if (!g_file_get_contents(CSS_FILE, &contents, NULL, &err)) {
        g_warning("Could not load stylesheet '%s': %s", CSS_FILE,
                  err ? err->message : "unknown error");
        if (err)
            g_error_free(err);
        return g_strdup("");
    }
    return contents;
}

typedef struct {
    double scale;
} FontScale;

static gboolean scale_font_size_eval(const GMatchInfo *info, GString *result,
                                     gpointer user_data) {
    const FontScale *fs = user_data;
    char *space = g_match_info_fetch(info, 1);
    char *num = g_match_info_fetch(info, 2);
    int px = (int)g_ascii_strtoll(num, NULL, 10);
    int scaled = (int)(px * fs->scale + 0.5);

    g_string_append_printf(result, "font-size:%s%dpx", space, scaled);
    g_free(space);
    g_free(num);
    return FALSE;
}

/* Scale every explicit `font-size: Npx` declaration so the interface grows on
 * large (e.g. maximised / high-DPI) windows. Layout metrics stay untouched.
 * Rules after the no-scale marker keep their fixed size, so symbol / icon
 * glyphs do not scale. */
static char *scale_font_sizes(const char *css, double scale) {
    static const char marker[] = "/* no-scale */";
    const char *pos;
    char *head;
    char *scaled_head;
    char *out;
    GRegex *re;
    FontScale fs;

    if (scale <= 1.0001)
        return g_strdup(css);

    pos = g_strstr_len(css, -1, marker);
    head = pos ? g_strndup(css, (gsize)(pos - css)) : g_strdup(css);

    fs.scale = scale;
    re = g_regex_new("font-size:([ \t]*)([0-9]+)px", 0, 0, NULL);
    scaled_head = g_regex_replace_eval(re, head, -1, 0, 0,
                                       scale_font_size_eval, &fs, NULL);
    g_regex_unref(re);
    g_free(head);

    if (pos)
        out = g_strconcat(scaled_head, pos, NULL);
    else
        out = scaled_head;
    g_free(scaled_head);
    return out;
}

/* The palette is data-driven, so its @define-color bindings are generated
 * here and prepended to the rules loaded from the external stylesheet. */
char *build_theme_css(const ThemePalette *p) {
    char *palette = g_strdup_printf(
        "@define-color bg_crust #%06x;\n"
        "@define-color bg_base #%06x;\n"
        "@define-color bg_mantle #%06x;\n"
        "@define-color bg_surface0 #%06x;\n"
        "@define-color bg_surface1 #%06x;\n"
        "@define-color bg_surface2 #%06x;\n"
        "@define-color fg_overlay #%06x;\n"
        "@define-color fg_text #%06x;\n"
        "@define-color fg_subtext #%06x;\n"
        "@define-color accent #%06x;\n"
        "@define-color accent2 #%06x;\n"
        "@define-color accent3 #%06x;\n"
        "@define-color success #%06x;\n"
        "@define-color success2 #%06x;\n"
        "@define-color warning #%06x;\n"
        "@define-color error #%06x;\n"
        "@define-color on_accent #%06x;\n"
        "@define-color node_bg #%06x;\n"
        "@define-color locked_bg #%06x;\n"
        "@define-color locked_border #%06x;\n",
        p->crust, p->base, p->mantle, p->surface0, p->surface1, p->surface2,
        p->overlay, p->text, p->subtext, p->accent, p->accent2, p->accent3,
        p->success, p->success2, p->warning, p->error, p->on_accent,
        p->node, p->locked_bg, p->locked_border);
    char *rules = read_css_file();
    char *scaled = scale_font_sizes(rules, app_ui_scale);
    char *css = g_strconcat(palette, scaled, NULL);

    g_free(palette);
    g_free(rules);
    g_free(scaled);
    return css;
}

void save_settings(void) {
    GKeyFile *kf = g_key_file_new();
    gchar *data;

    g_key_file_set_string(kf, "ui", "theme", theme_names[app_theme_id]);
    g_key_file_set_string(kf, "ui", "mode",
                          app_color_mode == MODE_LIGHT ? "light" : "dark");
    g_key_file_set_string(kf, "ui", "lang",
                          app_lang == LANG_EN ? "en" : "cs");
    g_key_file_set_string(kf, "ui", "seen_commit", changelog_seen());

    if (g_mkdir_with_parents(PROGRESS_DIR, 0755) != 0) {
        g_key_file_free(kf);
        return;
    }

    data = g_key_file_to_data(kf, NULL, NULL);
    g_key_file_free(kf);
    if (data) {
        GError *err = NULL;
        g_file_set_contents(SETTINGS_FILE, data, -1, &err);
        if (err)
            g_error_free(err);
        g_free(data);
    }
}

void load_settings(void) {
    GKeyFile *kf = g_key_file_new();
    GError *err = NULL;
    gchar *theme = NULL;
    gchar *mode = NULL;

    if (!g_key_file_load_from_file(kf, SETTINGS_FILE, G_KEY_FILE_NONE, &err)) {
        if (err)
            g_error_free(err);
        g_key_file_free(kf);
        return;
    }

    theme = g_key_file_get_string(kf, "ui", "theme", NULL);
    mode = g_key_file_get_string(kf, "ui", "mode", NULL);
    if (theme) {
        for (int i = 0; i < THEME_COUNT; i++) {
            if (g_ascii_strcasecmp(theme, theme_names[i]) == 0) {
                app_theme_id = (ThemeId)i;
                break;
            }
        }
        g_free(theme);
    }
    if (mode) {
        if (g_ascii_strcasecmp(mode, "light") == 0 ||
            g_ascii_strcasecmp(mode, "white") == 0)
            app_color_mode = MODE_LIGHT;
        else
            app_color_mode = MODE_DARK;
        g_free(mode);
    }
    {
        gchar *lang = g_key_file_get_string(kf, "ui", "lang", NULL);
        if (lang) {
            if (g_ascii_strcasecmp(lang, "en") == 0 ||
                g_ascii_strcasecmp(lang, "english") == 0)
                app_lang = LANG_EN;
            else
                app_lang = LANG_CS;
            g_free(lang);
        }
    }
    {
        gchar *seen = g_key_file_get_string(kf, "ui", "seen_commit", NULL);

        changelog_set_seen(seen);
        g_free(seen);
    }

    g_key_file_free(kf);
}

/* GTK only re-resolves the style of widgets that are actually drawn after a
 * CSS provider reload. Pages that are not the current stack child keep their
 * previously resolved colors, so after switching the theme any page you then
 * open still shows the old palette (e.g. near-white text in light mode).
 * Adding and immediately removing a dummy class forces GTK to invalidate the
 * style node of every widget, including hidden pages. */
void force_restyle_tree(GtkWidget *w) {
    GtkWidget *child;

    if (!w)
        return;

    gtk_widget_add_css_class(w, "m4-restyle");
    gtk_widget_remove_css_class(w, "m4-restyle");

    for (child = gtk_widget_get_first_child(w); child;
         child = gtk_widget_get_next_sibling(child))
        force_restyle_tree(child);
}

void apply_theme(void) {
    char *css;
    GtkSettings *settings = gtk_settings_get_default();

    app_theme = theme_palette(app_theme_id, app_color_mode);

    if (settings) {
        g_object_set(settings,
                     "gtk-application-prefer-dark-theme",
                     app_color_mode == MODE_DARK,
                     NULL);
    }

    css = build_theme_css(&app_theme);

    if (!theme_provider) {
        theme_provider = gtk_css_provider_new();
        gtk_style_context_add_provider_for_display(
            gdk_display_get_default(),
            GTK_STYLE_PROVIDER(theme_provider),
            GTK_STYLE_PROVIDER_PRIORITY_USER);
    }

    #if GTK_CHECK_VERSION(4, 12, 0)
    gtk_css_provider_load_from_string(theme_provider, css);
#else
    /* GTK 4.6 (Ubuntu 22.04) — load_from_string arrived in 4.12. */
    gtk_css_provider_load_from_data(theme_provider, css, -1);
#endif
    g_free(css);

    refresh_welcome_heading();

    if (main_window) {
        gtk_widget_queue_draw(GTK_WIDGET(main_window));
        force_restyle_tree(GTK_WIDGET(main_window));
    }
    if (road_rail)
        gtk_widget_queue_draw(road_rail);
    for (int i = 0; i < NUM_UNLOCKED; i++)
        if (units[i].ex_rail)
            gtk_widget_queue_draw(units[i].ex_rail);
    for (int i = 0; i < THEME_COUNT; i++) {
        if (theme_swatch_areas[i])
            gtk_widget_queue_draw(theme_swatch_areas[i]);
    }
    net_rail_theme_reset();
    hw_rail_theme_reset();
    czech_rail_theme_reset();
    book_rail_theme_reset();
}
