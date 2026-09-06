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
#define SETTINGS_FILE "progress/settings.conf"

#define NODE_SIZE    88.0
#define PATH_SPAC    240.0

/* Serpentine roadmap layout */
#define ROAD_MX    150.0   /* horizontal canvas margin                 */
#define ROAD_MY    150.0   /* vertical canvas margin                   */
#define ROAD_GAP   240.0   /* vertical space between folded rows       */
#define ROAD_WAVE   44.0   /* wavy vertical offset of the nodes        */

static GtkStack *main_stack;
static GtkWindow *main_window;
static GtkWidget *welcome_heading;
static GtkCssProvider *theme_provider;

typedef enum {
    THEME_CATPPUCCIN = 0,
    THEME_NORD,
    THEME_DRACULA,
    THEME_ROSE_PINE,
    THEME_OCEAN,
    THEME_GRUVBOX,
    THEME_SOLARIZED,
    THEME_EVERFOREST,
    THEME_MONOKAI,
    THEME_ONE_DARK,
    THEME_COUNT
} ThemeId;

typedef enum {
    MODE_DARK = 0,
    MODE_LIGHT
} ColorMode;

typedef enum {
    LANG_CS = 0,
    LANG_EN
} UiLang;

typedef struct {
    unsigned crust;
    unsigned base;
    unsigned mantle;
    unsigned surface0;
    unsigned surface1;
    unsigned surface2;
    unsigned overlay;
    unsigned text;
    unsigned subtext;
    unsigned accent;
    unsigned accent2;
    unsigned accent3;
    unsigned success;
    unsigned success2;
    unsigned warning;
    unsigned error;
    unsigned on_accent;
    unsigned node;
    unsigned locked_bg;
    unsigned locked_border;
    unsigned rail;
    unsigned finish_dark;
    unsigned finish_light;
    unsigned finish_stroke;
} ThemePalette;

static ThemeId   app_theme_id = THEME_CATPPUCCIN;
static ColorMode app_color_mode = MODE_DARK;
static UiLang    app_lang = LANG_CS;
static ThemePalette app_theme;

static const char *theme_names[THEME_COUNT] = {
    "Catppuccin",
    "Nord",
    "Dracula",
    "Rose Pine",
    "Ocean",
    "Gruvbox",
    "Solarized",
    "Everforest",
    "Monokai",
    "One Dark",
};

static GtkWidget *theme_swatch_areas[THEME_COUNT];

static gboolean ex_done[NUM_EXERCISES + 1]; /* indexed 1..13 */

static GtkWidget *ex_bubbles[NUM_EXERCISES + 1];
static GtkWidget *ex_done_icons[NUM_EXERCISES + 1];
static GtkWidget *unit1_node;
static GtkWidget *unit1_done_icon;

/* Adaptive serpentine roadmap geometry */
static GtkWidget *road_scroll;
static GtkWidget *road_fixed;
static GtkWidget *road_rail;
static GtkWidget *ex_rail;
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


typedef struct { double r, g, b; } Rgb;

static Rgb color_from_hex(unsigned int hex) {
    Rgb c;
    c.r = (double)((hex >> 16) & 0xFF) / 255.0;
    c.g = (double)((hex >> 8) & 0xFF) / 255.0;
    c.b = (double)(hex & 0xFF) / 255.0;
    return c;
}

static ThemePalette theme_palette(ThemeId id, ColorMode mode) {
    /* Catppuccin Mocha / Latte */
    static const ThemePalette cat_dark = {
        0x11111b, 0x1e1e2e, 0x181825, 0x313244, 0x45475a, 0x585b70,
        0x6c7086, 0xcdd6f4, 0xa6adc8, 0xcba6f7, 0xb4befe, 0x89b4fa,
        0xa6e3a1, 0x94e2d5, 0xf9e2af, 0xf38ba8, 0x1e1e2e,
        0x2b2d40, 0x24263a, 0x383b52, 0x27293c, 0x1f202e, 0x3b3e58, 0x52556b
    };
    static const ThemePalette cat_light = {
        0xdce0e8, 0xeff1f5, 0xe6e9ef, 0xccd0da, 0xbcc0cc, 0xacb0be,
        0x9ca0b0, 0x4c4f69, 0x6c6f85, 0x8839ef, 0x7287fd, 0x1e66f5,
        0x40a02b, 0x179299, 0xdf8e1d, 0xd20f39, 0xeff1f5,
        0xe6e9ef, 0xdce0e8, 0xbcc0cc, 0xccd0da, 0xdce0e8, 0xbcc0cc, 0x9ca0b0
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
        0xeee8d5, 0xfdf6e3, 0xeee8d5, 0x93a1a1, 0x839496, 0x657b83,
        0x586e75, 0x657b83, 0x93a1a1, 0x268bd2, 0x6c71c4, 0x2aa198,
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

/* ------------------------------------------------------------------ */
/* i18n                                                               */
/* ------------------------------------------------------------------ */

typedef struct {
    GtkWidget *widget;
    const char *key;
    int kind; /* 0 label, 1 button, 2 tooltip, 3 placeholder, 4 ex4 sample */
} I18nBind;

static void ex4_fill_sample(GtkWidget *label);

static GArray *i18n_binds;

typedef struct {
    const char *key;
    const char *cs;
    const char *en;
} TrEntry;

static const TrEntry tr_ui[] = {
    {"back", "Zpět", "Back"},
    {"settings", "Nastavení", "Settings"},
    {"settings_title", "Nastavení", "Settings"},
    {"mode", "REŽIM", "MODE"},
    {"mode_dark", "Tmavý", "Dark"},
    {"mode_light", "Světlý", "Light"},
    {"theme", "TÉMA", "THEME"},
    {"language", "JAZYK", "LANGUAGE"},
    {"welcome_body",
     "Sprechen.C je vzdělávací program na procvičování Němčiny, určený "
     "pro studenty středních škol a gymnázií.\n\n"
     "Program je napsaný v Céčku studentama ze SSŠVT!",
     "Sprechen.C is an educational app for practicing German, made for "
     "high-school and gymnasium students.\n\n"
     "The program is written in C by students from SSŠVT!"},
    {"continue", "Pokračuj", "Continue"},
    {"roadmap_title", "Učební plán", "Learning path"},
    {"roadmap_sub", "Vyberte jednotku na cestě a začněte procvičovat.",
     "Pick a unit on the path and start practicing."},
    {"unit1_sub", "Vyberte cvičení a dokončete je.",
     "Choose an exercise and complete it."},
    {"check", "Zkontrolovat", "Check"},
    {"finish", "Dokončit", "Finish"},
    {"show_sample", "Ukázat vzor", "Show sample"},
    {"your_answer", "Vaše odpověď…", "Your answer…"},
    {"feedback_ok", "Výborně!", "Great!"},
    {"feedback_retry", "Ještě to není správně. Zkuste to znovu.",
     "Not quite right yet. Try again."},
    {"feedback_retry_short", "Ještě to není správně.", "Not quite right yet."},
    {"feedback_sentences", "Některé věty nejsou správně. Zkuste to znovu.",
     "Some sentences are not correct. Try again."},
    {"tip_ss", "Písmeno „ß“ se dá na klávesnici zaměnit za „ss“!",
     "You can type “ss” instead of “ß” on the keyboard!"},
    {"sample_fmt", "Otázka: %s\nVzor: %s  (česky: %s)",
     "Question: %s\nSample: %s  (English: %s)"},
    {"sub_dialog", "Doplňte chybějící slova v dialogu.",
     "Fill in the missing words in the dialogue."},
    {"sub_assembly", "Přetáhněte slova do správného pořadí.",
     "Drag the words into the correct order."},
    {"sub_choice_num", "Vyberte správnou číslovku.",
     "Choose the correct numeral."},
    {"sub_free", "Odpovězte vlastními slovy, poté klikněte na Dokončit.",
     "Answer in your own words, then click Finish."},
    {"sub_zahlen", "Vyberte správné slovo pro dané číslo.",
     "Choose the correct word for the given number."},
    {"sub_wieviel", "Spočítejte předměty a vyberte počet.",
     "Count the items and choose the amount."},
    {"sub_reihe", "Doplňte chybějící číslovku.",
     "Fill in the missing numeral."},
    {"sub_verb", "Vyberte správný tvar slovesa.",
     "Choose the correct verb form."},
    {"sub_wer", "Vyberte správné tázací slovo.",
     "Choose the correct question word."},
    {"sub_bild", "Vyberte správný tvar slovesa podle obrázku.",
     "Choose the correct verb form for the picture."},
    {"sub_gruss", "Roztřiďte pozdravy na pozdravy a rozloučení.",
     "Sort the phrases into greetings and farewells."},
    {"sub_land", "Přiřaďte symboly ke správné zemi.",
     "Match the symbols to the correct country."},
    {"assign_hint",
     "Vyberte skupinu a klikněte na kartu. Kliknutím na kartu ve skupině "
     "ji vrátíte zpět.",
     "Pick a group and click a card. Click a card inside a group to send "
     "it back."},
    {NULL, NULL, NULL}
};

/* Exercise content: the Czech text doubles as the lookup key, so `cs`
 * stays NULL and tr() falls back to the key itself. */
static const TrEntry tr_content[] = {
    {"Ahoj! Jmenuji se Anna.", NULL, "Hi! My name is Anna."},
    {"Ahoj! Jmenuji se Petr.", NULL, "Hi! My name is Petr."},
    {"Jmenuji se Anna.", NULL, "My name is Anna."},
    {"Jmenuji se Petr.", NULL, "My name is Petr."},
    {"Odkud jsi?", NULL, "Where are you from?"},
    {"Odkud pocházíš?", NULL, "Where do you come from?"},
    {"Pocházím z Česka.", NULL, "I come from Czechia."},
    {"Dobré ráno!", NULL, "Good morning!"},
    {"Jak se máš?", NULL, "How are you?"},
    {"Mám se dobře, děkuji.", NULL, "I am fine, thank you."},
    {"Musím jít. Na shledanou!", NULL, "I have to go. Goodbye!"},
    {"Brzy na viděnou!", NULL, "See you soon!"},
    {"Čau!", NULL, "Bye!"},
    {"Jak se jmenuješ?", NULL, "What is your name?"},
    {"Kde bydlíš?", NULL, "Where do you live?"},
    {"Kdo to je?", NULL, "Who is that?"},
    {"Kolik je ti let?", NULL, "How old are you?"},
    {"Je mi šestnáct let.", NULL, "I am sixteen years old."},
    {"Bydlím v Praze.", NULL, "I live in Prague."},
    {"Hraji fotbal.", NULL, "I play football."},
    {"Co rád/a děláš?", NULL, "What do you like to do?"},
    {"Kolik je hodin?", NULL, "What time is it?"},
    {"Na shledanou.", NULL, "Goodbye."},
    {"Moc děkuji.", NULL, "Thank you very much."},
    {"„vierzehn“ = čtrnáct (14)", NULL, "“vierzehn” = fourteen (14)"},
    {"„siebzehn“ = sedmnáct (17)", NULL, "“siebzehn” = seventeen (17)"},
    {"„zwanzig“ = dvacet (20)", NULL, "“zwanzig” = twenty (20)"},
    {"„sechs“ = šest (6)", NULL, "“sechs” = six (6)"},
    {"čtrnáct", NULL, "fourteen"},
    {"sedmnáct", NULL, "seventeen"},
    {"dvacet", NULL, "twenty"},
    {"šest", NULL, "six"},
    {"třináct", NULL, "thirteen"},
    {"dvě jablka", NULL, "two apples"},
    {"pět hvězd", NULL, "five stars"},
    {"sedm koček", NULL, "seven cats"},
    {"devět květin", NULL, "nine flowers"},
    {"tři auta", NULL, "three cars"},
    {"jedenáct knih", NULL, "eleven books"},
    {"čtyři svíčky", NULL, "four candles"},
    {"osm míčů", NULL, "eight balls"},
    {"šest ptáků", NULL, "six birds"},
    {"šest, sedm, osm, devět", NULL, "six, seven, eight, nine"},
    {"dvanáct, třináct, čtrnáct, patnáct", NULL,
     "twelve, thirteen, fourteen, fifteen"},
    {"osmnáct, devatenáct, dvacet, dvacet jedna", NULL,
     "eighteen, nineteen, twenty, twenty-one"},
    {"čtyři, pět, šest, sedm", NULL, "four, five, six, seven"},
    {"deset, jedenáct, dvanáct, třináct", NULL,
     "ten, eleven, twelve, thirteen"},
    {"Plave v jezeře.", NULL, "He/She is swimming in the lake."},
    {"Zpívá píseň.", NULL, "He/She is singing a song."},
    {"Vaří polévku.", NULL, "He/She is cooking soup."},
    {"Čte knihu.", NULL, "He/She is reading a book."},
    {"Řídí auto.", NULL, "He/She is driving a car."},
    {"Maluje obraz.", NULL, "He/She is painting a picture."},
    {"Hraje fotbal.", NULL, "He/She is playing football."},
    {"Spí.", NULL, "He/She is sleeping."},
    {"Ahoj", NULL, "Hi"},
    {"Dobré ráno", NULL, "Good morning"},
    {"Dobrý den", NULL, "Good day"},
    {"Dobrý večer", NULL, "Good evening"},
    {"Dobrý den (Bavorsko, Rakousko)", NULL, "Good day (Bavaria, Austria)"},
    {"Ahoj / Čau", NULL, "Hi / Hey"},
    {"Čau", NULL, "Bye"},
    {"Na shledanou", NULL, "Goodbye"},
    {"Brzy na viděnou", NULL, "See you soon"},
    {"Do zítřka", NULL, "See you tomorrow"},
    {"Dobrou noc", NULL, "Good night"},
    {"Zatím / Na viděnou", NULL, "See you later"},
    {"Preclík", NULL, "Pretzel"},
    {"Klobása", NULL, "Sausage"},
    {"Braniborská brána", NULL, "Brandenburg Gate"},
    {"Značka aut", NULL, "Car brand"},
    {"Rakouský skladatel", NULL, "Austrian composer"},
    {"Čokoládový dort (Vídeň)", NULL, "Chocolate cake (Vienna)"},
    {"Vídeňský řízek", NULL, "Viennese schnitzel"},
    {"Sýr", NULL, "Cheese"},
    {"Švýcarská hora", NULL, "Swiss mountain"},
    {"Čokoláda", NULL, "Chocolate"},
    {NULL, NULL, NULL}
};

static const char *tr(const char *key) {
    static GHashTable *idx;
    const TrEntry *e;
    int i;

    if (!key)
        return NULL;

    if (!idx) {
        idx = g_hash_table_new(g_str_hash, g_str_equal);
        for (i = 0; tr_ui[i].key; i++)
            g_hash_table_insert(idx, (gpointer)tr_ui[i].key,
                                (gpointer)&tr_ui[i]);
        for (i = 0; tr_content[i].key; i++)
            g_hash_table_insert(idx, (gpointer)tr_content[i].key,
                                (gpointer)&tr_content[i]);
    }

    e = g_hash_table_lookup(idx, key);
    if (!e)
        return key;
    if (app_lang == LANG_EN)
        return e->en;
    return e->cs ? e->cs : e->key;
}

static void i18n_ensure(void) {
    if (!i18n_binds)
        i18n_binds = g_array_new(FALSE, FALSE, sizeof(I18nBind));
}

static void i18n_apply_one(const I18nBind *b) {
    if (!b->widget || !b->key)
        return;
    switch (b->kind) {
        case 0:
            gtk_label_set_text(GTK_LABEL(b->widget), tr(b->key));
            break;
        case 1:
            gtk_button_set_label(GTK_BUTTON(b->widget), tr(b->key));
            break;
        case 2:
            gtk_widget_set_tooltip_text(b->widget, tr(b->key));
            break;
        case 3:
            gtk_entry_set_placeholder_text(GTK_ENTRY(b->widget), tr(b->key));
            break;
        case 4:
            ex4_fill_sample(b->widget);
            break;
        default:
            break;
    }
}

static void i18n_bind(GtkWidget *widget, const char *key, int kind) {
    I18nBind b;

    i18n_ensure();
    b.widget = widget;
    b.key = key;
    b.kind = kind;
    g_array_append_val(i18n_binds, b);

    i18n_apply_one(&b);
}

static void refresh_welcome_heading(void) {
    char *markup;

    if (!welcome_heading)
        return;
    if (app_lang == LANG_EN)
        markup = g_strdup_printf(
            "Welcome to <span color=\"#%06x\">Sprechen.C</span>!",
            app_theme.accent);
    else
        markup = g_strdup_printf(
            "Vítejte ve <span color=\"#%06x\">Sprechen.C</span>!",
            app_theme.accent);
    gtk_label_set_markup(GTK_LABEL(welcome_heading), markup);
    g_free(markup);
}

static void apply_language(void) {
    guint i;

    i18n_ensure();
    for (i = 0; i < i18n_binds->len; i++)
        i18n_apply_one(&g_array_index(i18n_binds, I18nBind, i));
    refresh_welcome_heading();
}

static char *build_theme_css(const ThemePalette *p) {
    return g_strdup_printf(
        "@define-color bg_crust #%06x;"
        "@define-color bg_base #%06x;"
        "@define-color bg_mantle #%06x;"
        "@define-color bg_surface0 #%06x;"
        "@define-color bg_surface1 #%06x;"
        "@define-color bg_surface2 #%06x;"
        "@define-color fg_overlay #%06x;"
        "@define-color fg_text #%06x;"
        "@define-color fg_subtext #%06x;"
        "@define-color accent #%06x;"
        "@define-color accent2 #%06x;"
        "@define-color accent3 #%06x;"
        "@define-color success #%06x;"
        "@define-color success2 #%06x;"
        "@define-color warning #%06x;"
        "@define-color error #%06x;"
        "@define-color on_accent #%06x;"
        "@define-color node_bg #%06x;"
        "@define-color locked_bg #%06x;"
        "@define-color locked_border #%06x;"
        "window {"
        "   background-color: @bg_base;"
        "   background-image: linear-gradient(160deg, @bg_crust 0%%, @bg_base 40%%, @bg_surface0 100%%);"
        "}"
        "button {"
        "   background-image: none;"
        "}"
        ".app-title {"
        "   color: @fg_text;"
        "   font-weight: 700;"
        "   font-size: 14px;"
        "}"
        ".settings-btn,"
        ".settings-btn:hover,"
        ".settings-btn:active,"
        ".settings-btn:checked,"
        ".settings-btn:focus,"
        ".settings-btn:focus-visible {"
        "   min-width: 34px;"
        "   min-height: 34px;"
        "   padding: 0;"
        "   border-radius: 999px;"
        "   background: transparent;"
        "   background-color: transparent;"
        "   background-image: none;"
        "   border: none;"
        "   outline: none;"
        "   box-shadow: none;"
        "   color: @fg_text;"
        "}"
        ".settings-btn:hover {"
        "   color: @accent;"
        "   background-color: alpha(@bg_surface0, 0.55);"
        "}"
        ".settings-btn:active {"
        "   color: @accent;"
        "   background-color: alpha(@bg_surface1, 0.4);"
        "}"
        "popover,"
        "popover.background,"
        ".settings-popover {"
        "   background: @bg_mantle;"
        "   background-color: @bg_mantle;"
        "   color: @fg_text;"
        "   border: 1px solid @bg_surface1;"
        "   border-radius: 18px;"
        "   box-shadow: 0 18px 40px alpha(@bg_crust, 0.45);"
        "}"
        "popover > contents,"
        ".settings-popover > contents {"
        "   background: transparent;"
        "   background-color: transparent;"
        "   border: none;"
        "   border-radius: 18px;"
        "   box-shadow: none;"
        "   padding: 0;"
        "}"
        ".settings-box {"
        "   background-color: transparent;"
        "   color: @fg_text;"
        "   min-width: 292px;"
        "   padding: 16px 16px 14px 16px;"
        "}"
        ".settings-title {"
        "   color: @fg_text;"
        "   font-size: 17px;"
        "   font-weight: 800;"
        "   letter-spacing: -0.2px;"
        "}"
        ".settings-label {"
        "   color: @fg_overlay;"
        "   font-size: 11px;"
        "   font-weight: 700;"
        "   letter-spacing: 0.8px;"
        "}"
        ".settings-seg {"
        "   background-color: alpha(@bg_surface0, 0.55);"
        "   border: none;"
        "   border-radius: 12px;"
        "   padding: 3px;"
        "}"
        ".mode-chip {"
        "   background-image: none;"
        "   background-color: transparent;"
        "   color: @fg_subtext;"
        "   border: none;"
        "   border-radius: 10px;"
        "   padding: 9px 12px;"
        "   font-size: 13px;"
        "   font-weight: 600;"
        "   min-width: 0;"
        "   box-shadow: none;"
        "   transition: background-color 140ms ease, color 140ms ease,"
        "                box-shadow 140ms ease;"
        "}"
        ".mode-chip:hover {"
        "   background-color: alpha(@bg_surface1, 0.28);"
        "   color: @fg_text;"
        "}"
        ".mode-chip:checked {"
        "   background-image: linear-gradient(135deg, @accent 0%%, @accent2 100%%);"
        "   color: @on_accent;"
        "   font-weight: 800;"
        "   box-shadow: 0 4px 12px alpha(@accent, 0.25);"
        "}"
        ".mode-chip:checked:hover {"
        "   background-image: linear-gradient(135deg, @accent 0%%, @accent2 100%%);"
        "   color: @on_accent;"
        "}"
        ".theme-grid {"
        "   background-color: transparent;"
        "   border: none;"
        "   padding: 0;"
        "}"
        "flowboxchild {"
        "   padding: 0;"
        "   margin: 0;"
        "   background: transparent;"
        "   border: none;"
        "   box-shadow: none;"
        "}"
        ".theme-card {"
        "   background-image: none;"
        "   background-color: transparent;"
        "   color: @fg_text;"
        "   border: none;"
        "   border-radius: 12px;"
        "   padding: 6px 4px;"
        "   min-width: 0;"
        "   box-shadow: none;"
        "   transition: background-color 140ms ease;"
        "}"
        ".theme-card:hover {"
        "   background-color: alpha(@bg_surface0, 0.45);"
        "}"
        ".theme-card:checked {"
        "   background-color: transparent;"
        "   box-shadow: none;"
        "}"
        ".theme-card:checked:hover {"
        "   background-color: alpha(@bg_surface0, 0.35);"
        "}"
        ".theme-card-name {"
        "   color: @fg_subtext;"
        "   font-size: 11px;"
        "   font-weight: 600;"
        "}"
        ".theme-card:checked .theme-card-name {"
        "   color: @fg_text;"
        "   font-weight: 700;"
        "}"
        "headerbar,"
        "headerbar.titlebar,"
        ".titlebar {"
        "   background-color: @bg_base;"
        "   background-image: none;"
        "   color: @fg_text;"
        "   box-shadow: none;"
        "   border: none;"
        "}"
        ".heading {"
        "   color: @fg_text;"
        "   font-size: 32px;"
        "   font-weight: 800;"
        "}"
        ".card {"
        "   background-color: alpha(@bg_surface0, 0.92);"
        "   border: 1px solid @bg_surface1;"
        "   border-radius: 18px;"
        "   padding: 30px 34px;"
        "   color: @fg_text;"
        "   font-size: 16px;"
        "}"
        ".btn-primary {"
        "   background-image: linear-gradient(135deg, @accent 0%%, @accent2 100%%);"
        "   color: @on_accent;"
        "   font-size: 16px;"
        "   font-weight: 700;"
        "   padding: 12px 40px;"
        "   border: none;"
        "   border-radius: 999px;"
        "   box-shadow: 0 6px 18px alpha(@accent, 0.25);"
        "   transition: box-shadow 150ms ease, background-image 150ms ease;"
        "}"
        ".btn-primary:hover {"
        "   box-shadow: 0 8px 24px alpha(@accent, 0.45);"
        "   background-image: linear-gradient(135deg, @accent2 0%%, @accent3 100%%);"
        "}"
        ".btn-primary:active {"
        "   box-shadow: 0 3px 10px alpha(@accent, 0.35);"
        "}"
        ".roadmap-title {"
        "   color: @fg_text;"
        "   font-size: 28px;"
        "   font-weight: 800;"
        "}"
        ".roadmap-sub {"
        "   color: @fg_subtext;"
        "   font-size: 14px;"
        "}"
        ".back-btn {"
        "   min-width: 42px;"
        "   min-height: 42px;"
        "   padding: 0;"
        "   border-radius: 999px;"
        "   background-color: alpha(@bg_surface0, 0.9);"
        "   border: 1px solid @bg_surface1;"
        "   color: @fg_text;"
        "   box-shadow: 0 4px 14px alpha(@bg_crust, 0.35);"
        "   transition: background-color 150ms ease, color 150ms ease,"
        "                border-color 150ms ease, box-shadow 150ms ease;"
        "}"
        ".back-btn:hover {"
        "   background-color: @bg_surface1;"
        "   border-color: @bg_surface2;"
        "   color: @accent;"
        "   box-shadow: 0 6px 20px alpha(@accent, 0.35);"
        "}"
        ".back-btn:active {"
        "   background-color: @bg_surface0;"
        "   box-shadow: 0 2px 8px alpha(@bg_crust, 0.35);"
        "}"
        ".unit-node {"
        "   background-image: none;"
        "   background-color: @node_bg;"
        "   color: @fg_text;"
        "   border: 2px solid @locked_border;"
        "   border-radius: 999px;"
        "   padding: 0;"
        "   box-shadow: 0 4px 14px alpha(@bg_crust, 0.35);"
        "   transition: transform 150ms ease, box-shadow 150ms ease,"
        "                border-color 150ms ease;"
        "}"
        ".unit-node.done {"
        "   background-image: linear-gradient(135deg, @success 0%%, @success2 100%%);"
        "   border-color: @success;"
        "   color: @on_accent;"
        "   box-shadow: 0 4px 16px alpha(@success, 0.35);"
        "}"
        ".unit-node.current {"
        "   background-image: linear-gradient(135deg, @accent 0%%, @accent2 55%%, @accent3 100%%);"
        "   border-color: @accent;"
        "   color: @on_accent;"
        "   box-shadow: 0 4px 18px alpha(@accent, 0.5),"
        "               0 0 0 4px alpha(@bg_base, 0.9),"
        "               0 0 0 7px alpha(@accent, 0.45);"
        "}"
        ".unit-node.done:hover,"
        ".unit-node.current:hover {"
        "   transition: box-shadow 150ms ease, border-color 150ms ease;"
        "}"
        ".unit-node.done:hover {"
        "   box-shadow: 0 8px 22px alpha(@success, 0.5);"
        "}"
        ".unit-node.current:hover {"
        "   box-shadow: 0 8px 26px alpha(@accent, 0.65),"
        "               0 0 0 4px alpha(@bg_base, 0.9),"
        "               0 0 0 7px alpha(@accent, 0.55);"
        "}"
        ".unit-node.locked {"
        "   background-image: none;"
        "   background-color: @locked_bg;"
        "   border: 2px solid @locked_border;"
        "   color: @fg_overlay;"
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
        "   color: @fg_overlay;"
        "   box-shadow: 0 4px 14px alpha(@bg_crust, 0.35);"
        "}"
        ".unit-node.finish:hover {"
        "   transform: none;"
        "   box-shadow: 0 4px 14px alpha(@bg_crust, 0.35);"
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
        "   color: @on_accent;"
        "}"
        ".unit-node.locked .unit-number {"
        "   color: @fg_overlay;"
        "}"
        ".state-icon {"
        "   color: @on_accent;"
        "}"
        ".unit-node.locked .lock-icon {"
        "   color: @bg_surface2;"
        "}"
        ".unit-name {"
        "   color: @fg_text;"
        "   font-size: 13px;"
        "   font-weight: 600;"
        "}"
        ".unit-name-locked {"
        "   color: @fg_overlay;"
        "}"
        ".ex-bubble {"
        "   min-width: 64px;"
        "   min-height: 64px;"
        "   padding: 0;"
        "   border-radius: 999px;"
        "   background-color: @node_bg;"
        "   color: @fg_text;"
        "   border: 2px solid @locked_border;"
        "   box-shadow: 0 4px 14px alpha(@bg_crust, 0.35);"
        "   transition: border-color 150ms ease, box-shadow 150ms ease;"
        "}"
        ".ex-bubble:hover {"
        "   border-color: @accent;"
        "   box-shadow: 0 6px 20px alpha(@accent, 0.35);"
        "}"
        ".ex-bubble.done {"
        "   background-image: linear-gradient(135deg, @success 0%%, @success2 100%%);"
        "   border-color: @success;"
        "   color: @on_accent;"
        "   box-shadow: 0 4px 16px alpha(@success, 0.35);"
        "}"
        ".ex-bubble.done:hover {"
        "   box-shadow: 0 8px 22px alpha(@success, 0.5);"
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
        "   color: @on_accent;"
        "}"
        ".bubble-icon {"
        "   color: @on_accent;"
        "}"
        ".ex-label {"
        "   color: @fg_text;"
        "   font-size: 12px;"
        "   font-weight: 600;"
        "}"
        ".ex-prompt {"
        "   color: @fg_text;"
        "   font-size: 16px;"
        "   font-weight: 500;"
        "}"
        ".ex-sub {"
        "   color: @fg_subtext;"
        "   font-size: 13px;"
        "   font-weight: 700;"
        "}"
        ".hint {"
        "   color: @fg_subtext;"
        "   font-size: 13px;"
        "   font-style: italic;"
        "}"
        ".meaning {"
        "   color: @fg_subtext;"
        "   font-size: 13px;"
        "   font-style: italic;"
        "}"
        ".dlg-speaker {"
        "   color: @accent;"
        "   font-weight: 700;"
        "   font-size: 15px;"
        "}"
        ".number-big {"
        "   color: @warning;"
        "   font-size: 26px;"
        "   font-weight: 800;"
        "}"
        ".chain {"
        "   color: @warning;"
        "   font-size: 20px;"
        "   font-weight: 700;"
        "   letter-spacing: 1px;"
        "}"
        "entry {"
        "   background-color: @bg_mantle;"
        "   color: @fg_text;"
        "   border: 1px solid @bg_surface1;"
        "   border-radius: 10px;"
        "   padding: 8px 12px;"
        "   font-size: 15px;"
        "   caret-color: @accent;"
        "}"
        "entry:focus {"
        "   border-color: @accent;"
        "}"
        "combobox {"
        "   background-color: @bg_mantle;"
        "   color: @fg_text;"
        "}"
        "combobox button {"
        "   background-image: none;"
        "   background-color: @bg_mantle;"
        "   color: @fg_text;"
        "   border: 1px solid @bg_surface1;"
        "   border-radius: 10px;"
        "   padding: 4px 10px;"
        "}"
        "combobox button:hover {"
        "   border-color: @accent;"
        "}"
        "combobox.answer-ok button {"
        "   border-color: @success;"
        "   background-color: alpha(@success, 0.12);"
        "}"
        "combobox.answer-wrong button {"
        "   border-color: @error;"
        "   background-color: alpha(@error, 0.12);"
        "}"
        ".pill {"
        "   background-color: @node_bg;"
        "   color: @fg_text;"
        "   border: 2px solid @locked_border;"
        "   border-radius: 999px;"
        "   padding: 6px 18px;"
        "   font-size: 15px;"
        "   transition: border-color 150ms ease, box-shadow 150ms ease;"
        "}"
        ".pill:hover {"
        "   border-color: @accent;"
        "}"
        ".pill:checked {"
        "   background-image: linear-gradient(135deg, @accent 0%%, @accent2 100%%);"
        "   color: @on_accent;"
        "   border-color: @accent;"
        "   font-weight: 700;"
        "}"
        ".pill.ok {"
        "   border-color: @success;"
        "   box-shadow: 0 0 0 2px alpha(@success, 0.4);"
        "}"
        ".pill.wrong {"
        "   border-color: @error;"
        "   box-shadow: 0 0 0 2px alpha(@error, 0.4);"
        "}"
        ".chip {"
        "   background-color: @bg_surface0;"
        "   color: @fg_text;"
        "   border: 1px solid @bg_surface1;"
        "   border-radius: 999px;"
        "   padding: 6px 14px;"
        "   font-size: 14px;"
        "   font-weight: 600;"
        "   transition: background-color 150ms ease, border-color 150ms ease;"
        "}"
        ".chip:hover {"
        "   background-color: @bg_surface1;"
        "   border-color: @accent;"
        "}"
        ".chip.ghost-host {"
        "   opacity: 0;"
        "}"
        ".group-btn {"
        "   background-color: @node_bg;"
        "   color: @fg_text;"
        "   border: 2px solid @locked_border;"
        "   border-radius: 12px;"
        "   padding: 8px 18px;"
        "   font-size: 14px;"
        "   font-weight: 700;"
        "   transition: border-color 150ms ease, box-shadow 150ms ease;"
        "}"
        ".group-btn:hover {"
        "   border-color: @accent;"
        "}"
        ".group-btn:checked {"
        "   background-image: linear-gradient(135deg, @accent3 0%%, @success2 100%%);"
        "   color: @on_accent;"
        "   border-color: @accent3;"
        "}"
        ".group-panel {"
        "   background-color: alpha(@bg_surface0, 0.5);"
        "   border: 2px dashed @bg_surface1;"
        "   border-radius: 14px;"
        "   padding: 10px;"
        "}"
        ".group-panel-label {"
        "   color: @fg_subtext;"
        "   font-size: 13px;"
        "   font-weight: 700;"
        "}"
        ".sent-group {"
        "   background-color: alpha(@bg_surface0, 0.5);"
        "   border: 1px solid @bg_surface1;"
        "   border-radius: 14px;"
        "   padding: 12px;"
        "}"
        ".sent-group.ok {"
        "   border-color: @success;"
        "}"
        ".sent-group.wrong {"
        "   border-color: @error;"
        "}"
        ".feedback-ok {"
        "   color: @success;"
        "   font-size: 15px;"
        "   font-weight: 700;"
        "}"
        ".feedback-err {"
        "   color: @error;"
        "   font-size: 15px;"
        "   font-weight: 700;"
        "}"
        "tooltip {"
        "   background-color: @bg_mantle;"
        "   border: 1px solid @bg_surface1;"
        "   border-radius: 10px;"
        "   color: @fg_text;"
        "}",
        p->crust, p->base, p->mantle, p->surface0, p->surface1, p->surface2,
        p->overlay, p->text, p->subtext, p->accent, p->accent2, p->accent3,
        p->success, p->success2, p->warning, p->error, p->on_accent,
        p->node, p->locked_bg, p->locked_border);
}

static void save_settings(void) {
    GKeyFile *kf = g_key_file_new();
    gchar *data;

    g_key_file_set_string(kf, "ui", "theme", theme_names[app_theme_id]);
    g_key_file_set_string(kf, "ui", "mode",
                          app_color_mode == MODE_LIGHT ? "light" : "dark");
    g_key_file_set_string(kf, "ui", "lang",
                          app_lang == LANG_EN ? "en" : "cs");

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

static void load_settings(void) {
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

    g_key_file_free(kf);
}

static void apply_theme(void) {
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

    gtk_css_provider_load_from_string(theme_provider, css);
    g_free(css);

    refresh_welcome_heading();

    if (main_window)
        gtk_widget_queue_draw(GTK_WIDGET(main_window));
    if (road_rail)
        gtk_widget_queue_draw(road_rail);
    if (ex_rail)
        gtk_widget_queue_draw(ex_rail);
    for (int i = 0; i < THEME_COUNT; i++) {
        if (theme_swatch_areas[i])
            gtk_widget_queue_draw(theme_swatch_areas[i]);
    }
}

/* Vector icons drawn with cairo so they never depend on the system
 * icon theme (which is missing on some installs). */

static void draw_check_icon(GtkDrawingArea *area, cairo_t *cr,
                            int width, int height, gpointer data) {
    Rgb c = color_from_hex(app_theme.on_accent);

    (void)area;
    (void)data;
    cairo_set_source_rgb(cr, c.r, c.g, c.b);
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
    {
        Rgb hole = color_from_hex(app_theme.crust);
        cairo_set_source_rgb(cr, hole.r, hole.g, hole.b);
    }
    cairo_fill(cr);
}

static void draw_lock_icon(GtkDrawingArea *area, cairo_t *cr,
                           int width, int height, gpointer data) {
    Rgb c = color_from_hex(app_theme.overlay);
    (void)area;
    (void)data;
    lock_paint(cr, 0.0, 0.0, MIN(width, height), &c);
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

static void draw_settings_icon(GtkDrawingArea *area, cairo_t *cr,
                               int width, int height, gpointer data) {
    const int teeth = 8;
    const double size = 16.0;
    double cx = width / 2.0;
    double cy = height / 2.0;
    double r_outer = size * 0.48;
    double r_inner = size * 0.34;
    double r_hole = size * 0.16;
    double tooth_half = G_PI / (double)teeth;
    GtkWidget *parent = gtk_widget_get_parent(GTK_WIDGET(area));
    GdkRGBA color;
    int i;

    (void)data;
    gtk_style_context_get_color(gtk_widget_get_style_context(parent), &color);
    cairo_set_source_rgb(cr, color.red, color.green, color.blue);
    cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);

    cairo_new_path(cr);
    for (i = 0; i < teeth; i++) {
        double a = -G_PI / 2.0 + (2.0 * G_PI * (double)i) / (double)teeth;
        double a0 = a - tooth_half * 0.55;
        double a1 = a - tooth_half * 0.28;
        double a2 = a + tooth_half * 0.28;
        double a3 = a + tooth_half * 0.55;

        if (i == 0)
            cairo_move_to(cr, cx + cos(a0) * r_inner, cy + sin(a0) * r_inner);
        else
            cairo_line_to(cr, cx + cos(a0) * r_inner, cy + sin(a0) * r_inner);

        cairo_line_to(cr, cx + cos(a1) * r_outer, cy + sin(a1) * r_outer);
        cairo_line_to(cr, cx + cos(a2) * r_outer, cy + sin(a2) * r_outer);
        cairo_line_to(cr, cx + cos(a3) * r_inner, cy + sin(a3) * r_inner);
    }
    cairo_close_path(cr);
    cairo_new_sub_path(cr);
    cairo_arc(cr, cx, cy, r_hole, 0.0, 2.0 * G_PI);
    cairo_fill(cr);
}

static GtkWidget *icon_area_new(GtkDrawingAreaDrawFunc fn,
                                double r, double g, double b, int px) {
    Rgb *col = g_new(Rgb, 1);
    GtkWidget *d = gtk_drawing_area_new();

    col->r = r;
    col->g = g;
    col->b = b;
    gtk_widget_set_size_request(d, px, px);
    gtk_widget_set_halign(d, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(d, GTK_ALIGN_CENTER);
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

static void ex_rail_refresh(void);

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

    ex_rail_refresh();
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
    i18n_bind(back, "back", 2);
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
    welcome_heading = heading;
    gtk_widget_set_halign(heading, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(heading, "heading");
    gtk_box_append(GTK_BOX(box), heading);

    card = gtk_label_new(NULL);
    i18n_bind(card, "welcome_body", 0);
    gtk_label_set_justify(GTK_LABEL(card), GTK_JUSTIFY_CENTER);
    gtk_label_set_wrap(GTK_LABEL(card), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(card), 48);
    gtk_widget_add_css_class(card, "card");
    gtk_box_append(GTK_BOX(box), card);

    button = gtk_button_new();
    i18n_bind(button, "continue", 1);
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
    const Rgb gray = color_from_hex(app_theme.overlay);
    const Rgb fin_dark = color_from_hex(app_theme.finish_dark);
    const Rgb fin_light = color_from_hex(app_theme.finish_light);
    const Rgb fin_stroke = color_from_hex(app_theme.finish_stroke);
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
    cairo_set_source_rgb(cr, fin_dark.r, fin_dark.g, fin_dark.b);
    cairo_fill(cr);

    for (iy = 0; iy < rows; iy++) {
        for (ix = 0; ix < cols; ix++) {
            if (((ix + iy) & 1) == 0)
                continue;
            cairo_rectangle(cr, start_x + ix * s, start_y + iy * s, s, s);
            cairo_set_source_rgb(cr, fin_light.r, fin_light.g, fin_light.b);
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
    cairo_set_source_rgb(cr, fin_stroke.r, fin_stroke.g, fin_stroke.b);
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
    const Rgb mauve = color_from_hex(app_theme.accent);
    const Rgb blue = color_from_hex(app_theme.accent3);
    const Rgb rail = color_from_hex(app_theme.rail);
    const Rgb muted = color_from_hex(app_theme.subtext);
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
    cairo_set_source_rgb(cr, rail.r, rail.g, rail.b);
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

        cairo_set_source_rgba(cr, muted.r, muted.g, muted.b, 0.17);
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
                   top_bar("welcome", "roadmap_title", "roadmap_sub"));

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
/* Unit 1 exercise path (roadmap-style serpentine)                    */
/* ------------------------------------------------------------------ */

#define EX_BUBBLE  64.0    /* exercise bubble size (matches .ex-bubble) */
#define EX_MX      80.0    /* horizontal canvas margin                  */
#define EX_MY      120.0   /* vertical canvas margin                    */
#define EX_SPAC    150.0   /* horizontal distance between bubbles       */
#define EX_GAP     200.0   /* vertical space between folded rows        */
#define EX_WAVE    26.0    /* wavy vertical offset of the bubbles       */

static GtkWidget *ex_scroll;
static GtkWidget *ex_fixed;
static GtkWidget *ex_cells[NUM_EXERCISES];
static GtkWidget *ex_labels[NUM_EXERCISES];
static double ex_cx[NUM_EXERCISES];
static double ex_cy[NUM_EXERCISES];
static int ex_rows = 1;
static int ex_cols = NUM_EXERCISES;
static int ex_cw = (int)(2.0 * EX_MX + (NUM_EXERCISES - 1) * EX_SPAC);
static int ex_ch = (int)(2.0 * EX_MY);

static void ex_rail_refresh(void) {
    if (ex_rail)
        gtk_widget_queue_draw(ex_rail);
}

/* Compute the serpentine exercise layout for the given available width. */
static void ex_layout_geometry(double avail) {
    const double phase = 2.0 * G_PI * 1.7 / (double)(NUM_EXERCISES - 1);
    int rows;
    int r, c, i;

    for (rows = 1; rows <= NUM_EXERCISES; rows++) {
        int cols = (NUM_EXERCISES + rows - 1) / rows;
        if (2.0 * EX_MX + (cols - 1) * EX_SPAC <= avail + 1.0)
            break;
    }
    if (rows > NUM_EXERCISES)
        rows = NUM_EXERCISES;
    ex_rows = rows;
    ex_cols = (NUM_EXERCISES + rows - 1) / rows;
    if (ex_cols < 1)
        ex_cols = 1;
    ex_cw = (int)(2.0 * EX_MX + (ex_cols - 1) * EX_SPAC);
    ex_ch = (int)(2.0 * EX_MY + (rows - 1) * EX_GAP);

    for (r = 0; r < rows; r++) {
        int base = r * ex_cols;
        int len = MIN(ex_cols, NUM_EXERCISES - base);
        int fwd = (r % 2) == 0;
        for (c = 0; c < len; c++) {
            i = base + c;
            int cc = fwd ? c : (ex_cols - 1 - c);
            ex_cx[i] = EX_MX + cc * EX_SPAC;
            ex_cy[i] = EX_MY + r * EX_GAP + EX_WAVE * sin((double)i * phase);
        }
    }
}

static void ex_point(double t, double *ox, double *oy) {
    int k = (int)t;
    double u = t - (double)k;
    double u2, u3;
    double p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y;

    if (k < 0) { k = 0; u = 0.0; }
    if (k >= NUM_EXERCISES - 1) { k = NUM_EXERCISES - 2; u = 1.0; }

    p1x = ex_cx[k];     p1y = ex_cy[k];
    p2x = ex_cx[k + 1]; p2y = ex_cy[k + 1];
    if (k - 1 >= 0) { p0x = ex_cx[k - 1]; p0y = ex_cy[k - 1]; }
    else            { p0x = p1x - (p2x - p1x); p0y = p1y - (p2y - p1y); }
    if (k + 2 < NUM_EXERCISES) { p3x = ex_cx[k + 2]; p3y = ex_cy[k + 2]; }
    else                       { p3x = p2x + (p2x - p1x); p3y = p2y + (p2y - p1y); }

    u2 = u * u;
    u3 = u2 * u;
    *ox = 0.5 * (2.0 * p1x + (-p0x + p2x) * u
                 + (2.0 * p0x - 5.0 * p1x + 4.0 * p2x - p3x) * u2
                 + (-p0x + 3.0 * p1x - 3.0 * p2x + p3x) * u3);
    *oy = 0.5 * (2.0 * p1y + (-p0y + p2y) * u
                 + (2.0 * p0y - 5.0 * p1y + 4.0 * p2y - p3y) * u2
                 + (-p0y + 3.0 * p1y - 3.0 * p2y + p3y) * u3);
}

static void ex_path(cairo_t *cr, double t0, double t1) {
    const double steps_per_unit = 48.0;
    int n = (int)((t1 - t0) * steps_per_unit);
    double x, y;
    int s;

    if (n < 1)
        n = 1;
    ex_point(t0, &x, &y);
    cairo_move_to(cr, x, y);
    for (s = 1; s <= n; s++) {
        ex_point(t0 + (t1 - t0) * (double)s / (double)n, &x, &y);
        cairo_line_to(cr, x, y);
    }
}

static void draw_ex_rail(GtkDrawingArea *area, cairo_t *cr,
                         int width, int height, gpointer user_data) {
    const double t_end = (double)(NUM_EXERCISES - 1);
    const Rgb slate = color_from_hex(app_theme.surface1);
    const Rgb green = color_from_hex(app_theme.success);
    const Rgb rail = color_from_hex(app_theme.rail);

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
    ex_path(cr, 0.0, t_end);
    cairo_set_line_width(cr, 14);
    cairo_set_source_rgb(cr, rail.r, rail.g, rail.b);
    cairo_stroke(cr);

    cairo_new_path(cr);
    ex_path(cr, 0.0, t_end);
    cairo_set_line_width(cr, 9);
    cairo_set_source_rgb(cr, slate.r, slate.g, slate.b);
    cairo_stroke(cr);

    for (int k = 0; k < NUM_EXERCISES - 1; k++) {
        if (!(ex_done[k + 1] || ex_done[k + 2]))
            continue;
        cairo_new_path(cr);
        ex_path(cr, (double)k, (double)(k + 1));
        cairo_set_source_rgba(cr, green.r, green.g, green.b, 0.85);
        cairo_set_line_width(cr, 9);
        cairo_stroke(cr);
    }
}

static void ex_apply_layout(void) {
    int i;

    if (!ex_fixed)
        return;

    for (i = 0; i < NUM_EXERCISES; i++) {
        if (ex_cells[i])
            gtk_fixed_move(GTK_FIXED(ex_fixed), ex_cells[i],
                           (int)(ex_cx[i] - EX_BUBBLE / 2.0),
                           (int)(ex_cy[i] - EX_BUBBLE / 2.0));
        if (ex_labels[i])
            gtk_fixed_move(GTK_FIXED(ex_fixed), ex_labels[i],
                           (int)(ex_cx[i] - (EX_SPAC - 20.0) / 2.0),
                           (int)(ex_cy[i] + EX_BUBBLE / 2.0 + 8.0));
    }

    gtk_widget_set_size_request(ex_fixed, ex_cw, ex_ch);
    gtk_widget_set_size_request(ex_rail, ex_cw, ex_ch);
    gtk_fixed_move(GTK_FIXED(ex_fixed), ex_rail, 0, 0);
    gtk_widget_queue_draw(ex_rail);
}

static void ex_relayout(void) {
    GtkAdjustment *hadj;
    double avail;

    if (!ex_scroll)
        return;
    hadj = gtk_scrolled_window_get_hadjustment(
        GTK_SCROLLED_WINDOW(ex_scroll));
    avail = gtk_adjustment_get_page_size(hadj);
    if (avail < 1.0)
        return;
    ex_layout_geometry(avail);
    ex_apply_layout();
}

static guint ex_idle;

static gboolean ex_relayout_idle(gpointer data) {
    (void)data;
    ex_idle = 0;
    ex_relayout();
    return G_SOURCE_REMOVE;
}

static void ex_relayout_later(void) {
    if (ex_idle == 0)
        ex_idle = g_idle_add(ex_relayout_idle, NULL);
}

static void ex_adjust_notify(GtkAdjustment *adj, GParamSpec *ps,
                             gpointer data) {
    (void)adj;
    (void)ps;
    (void)data;
    ex_relayout_later();
}

static GtkWidget *make_bubble(int n) {
    GtkWidget *btn;
    GtkWidget *vbox;
    GtkWidget *num_label;
    GtkWidget *icon;
    char *num;
    char *target;

    btn = gtk_button_new();
    gtk_widget_add_css_class(btn, "ex-bubble");
    gtk_widget_set_size_request(btn, (int)EX_BUBBLE, (int)EX_BUBBLE);
    gtk_widget_set_can_focus(btn, FALSE);
    if (ex_done[n])
        gtk_widget_add_css_class(btn, "done");

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
    gtk_button_set_child(GTK_BUTTON(btn), vbox);

    num = g_strdup_printf("%d", n);
    num_label = gtk_label_new(num);
    g_free(num);
    gtk_widget_set_halign(num_label, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(num_label, "bubble-number");
    gtk_box_append(GTK_BOX(vbox), num_label);

    icon = icon_area_new(draw_check_icon, 0.1176, 0.1176, 0.1804, 16);
    gtk_widget_set_visible(icon, ex_done[n]);
    gtk_box_append(GTK_BOX(vbox), icon);

    ex_bubbles[n] = btn;
    ex_done_icons[n] = icon;

    target = g_strdup_printf("ex%d", n);
    g_object_set_data_full(G_OBJECT(btn), "target", target, g_free);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_nav_clicked), NULL);

    return btn;
}

static GtkWidget *build_unit1_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *fixed;
    GtkWidget *rail;
    GtkAdjustment *ha;
    GtkAdjustment *va;
    int i;

    ex_layout_geometry(1000.0);

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("roadmap", "Neue Freunde", "unit1_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);
    ex_scroll = scroll;

    wrap = gtk_center_box_new();
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), wrap);

    fixed = gtk_fixed_new();
    gtk_widget_set_halign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(fixed, ex_cw, ex_ch);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), fixed);
    ex_fixed = fixed;

    rail = gtk_drawing_area_new();
    gtk_widget_set_size_request(rail, ex_cw, ex_ch);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(rail), draw_ex_rail,
                                   NULL, NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rail, 0, 0);
    ex_rail = rail;

    for (i = 0; i < NUM_EXERCISES; i++) {
        GtkWidget *btn = make_bubble(i + 1);
        GtkWidget *lbl = gtk_label_new(ex_names[i + 1]);

        ex_cells[i] = btn;
        ex_labels[i] = lbl;

        gtk_widget_set_size_request(lbl, (int)(EX_SPAC - 20.0), -1);
        gtk_widget_set_halign(lbl, GTK_ALIGN_CENTER);
        gtk_label_set_justify(GTK_LABEL(lbl), GTK_JUSTIFY_CENTER);
        gtk_label_set_wrap(GTK_LABEL(lbl), TRUE);
        gtk_widget_add_css_class(lbl, "ex-label");

        gtk_fixed_put(GTK_FIXED(fixed), btn, 0, 0);
        gtk_fixed_put(GTK_FIXED(fixed), lbl, 0, 0);
    }

    ha = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scroll));
    va = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));
    g_signal_connect(ha, "notify::page-size",
                     G_CALLBACK(ex_adjust_notify), NULL);
    g_signal_connect(va, "notify::page-size",
                     G_CALLBACK(ex_adjust_notify), NULL);

    ex_apply_layout();
    ex_relayout_later();

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

    btn = gtk_button_new();
    i18n_bind(btn, btn_label, 1);
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
    GArray *trans;    /* GtkWidget* hidden meaning labels              */
    int ex_num;
    GtkWidget *feedback;
} ComboListCtx;

/* Build a hidden translation label ("meaning") appended under an item. */
static GtkWidget *meaning_add(GtkWidget *body, const char *text) {
    GtkWidget *lbl = gtk_label_new(NULL);

    i18n_bind(lbl, text, 0);
    gtk_label_set_xalign(GTK_LABEL(lbl), 0.0);
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_widget_add_css_class(lbl, "meaning");
    gtk_label_set_wrap(GTK_LABEL(lbl), TRUE);
    gtk_widget_set_visible(lbl, FALSE);
    gtk_widget_set_margin_top(lbl, 2);
    gtk_widget_set_margin_bottom(lbl, 2);
    gtk_widget_set_margin_start(lbl, 8);
    gtk_box_append(GTK_BOX(body), lbl);
    return lbl;
}

static void meaning_reveal_all(GtkWidget **labels, int n) {
    for (int i = 0; i < n; i++)
        if (labels[i])
            gtk_widget_set_visible(labels[i], TRUE);
}

static ComboListCtx *combo_list_ctx_new(int ex_num, GtkWidget *feedback) {
    ComboListCtx *ctx = g_new0(ComboListCtx, 1);
    ctx->combos = g_array_new(FALSE, FALSE, sizeof(GtkComboBox *));
    ctx->answers = g_array_new(FALSE, FALSE, sizeof(const char *));
    ctx->trans = g_array_new(FALSE, FALSE, sizeof(GtkWidget *));
    ctx->ex_num = ex_num;
    ctx->feedback = feedback;
    return ctx;
}

static void combo_list_add(ComboListCtx *ctx, GtkWidget *combo, const char *ans) {
    g_array_append_val(ctx->combos, combo);
    g_array_append_val(ctx->answers, ans);
}

static void combo_list_add_trans(ComboListCtx *ctx, GtkWidget *label) {
    g_array_append_val(ctx->trans, label);
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

    if (ctx->trans->len > 0) {
        for (guint i = 0; i < ctx->trans->len; i++) {
            GtkWidget *lbl = g_array_index(ctx->trans, GtkWidget *, i);
            if (lbl)
                gtk_widget_set_visible(lbl, TRUE);
        }
    }

    if (ok == total) {
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        mark_done(ctx->ex_num);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry"));
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
    GtkWidget *stage;    /* transparent overlay layer for the fly sprite */
    GtkWidget *trans;    /* hidden meaning label under the group */
    GtkWidget *chips[6];
    int pool_order[6]; /* fixed shuffled order of the word pool       */
    int placed[6];
    int placed_count;
    int n_words;
    gboolean flying;     /* a click-move animation is in progress       */
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
        int w = sb->pool_order[i];
        gboolean in_target = FALSE;
        for (int j = 0; j < sb->placed_count; j++) {
            if (sb->placed[j] == w) {
                in_target = TRUE;
                break;
            }
        }
        if (!in_target)
            gtk_flow_box_insert(GTK_FLOW_BOX(sb->pool), sb->chips[w], -1);
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

/* ------------------------------------------------------------------ */
/* Click-to-move with a fly ("glide") animation                        */
/* ------------------------------------------------------------------ */

typedef struct {
    GtkWidget *chip;    /* real chip, kept invisible in its final slot */
    GtkWidget *ghost;   /* sprite flying from start to finish          */
    GtkWidget *stage;
    gboolean *flying;   /* pointer to the owner's "animating" flag     */
    double sx, sy;      /* start position (stage coords)               */
    double tx, ty;      /* target position (stage coords)              */
    guint wait;         /* ticks waited for relayout                   */
    gint64 t0;          /* < 0 while still measuring the target        */
} FlyCtx;

static void asm_finish_fly(FlyCtx *fc) {
    gtk_widget_remove_css_class(fc->chip, "ghost-host");
    gtk_fixed_remove(GTK_FIXED(fc->stage), fc->ghost);
    if (fc->flying)
        *fc->flying = FALSE;
    g_free(fc);
}

static gboolean asm_fly_tick(gpointer data) {
    FlyCtx *fc = data;
    gint64 now = g_get_monotonic_time() / 1000;

    if (fc->t0 < 0) {
        double nx = 0.0, ny = 0.0;
        gboolean ok;

        fc->wait++;
        ok = gtk_widget_translate_coordinates(fc->chip, fc->stage,
                                              0, 0, &nx, &ny);
        if (fc->wait < 2)
            return G_SOURCE_CONTINUE;

        if (ok && fc->wait < 14 &&
            fabs(nx - fc->sx) + fabs(ny - fc->sy) < 0.5)
            return G_SOURCE_CONTINUE;   /* still waiting for relayout   */

        if (ok) {
            fc->tx = nx;
            fc->ty = ny;
        } else {
            fc->tx = fc->sx;
            fc->ty = fc->sy;
        }
        fc->t0 = now;
        return G_SOURCE_CONTINUE;
    }

    {
        double t = (double)(now - fc->t0) / 240.0;
        double e, x, y;

        if (t > 1.0)
            t = 1.0;
        e = 1.0 - pow(1.0 - t, 3.0);   /* ease-out cubic                */
        x = fc->sx + (fc->tx - fc->sx) * e;
        y = fc->sy + (fc->ty - fc->sy) * e;
        gtk_fixed_move(GTK_FIXED(fc->stage), fc->ghost, (int)x, (int)y);

        if (t >= 1.0) {
            asm_finish_fly(fc);
            return G_SOURCE_REMOVE;
        }
    }
    return G_SOURCE_CONTINUE;
}

/* Start a fly animation: hide `chip` in place and animate a ghost from
 * (sx, sy) to the chip's new location. */
static void asm_begin_fly(GtkWidget *chip, GtkWidget *stage,
                          gboolean *flying, double sx, double sy) {
    GtkWidget *child = gtk_button_get_child(GTK_BUTTON(chip));
    const char *txt = (child && GTK_IS_LABEL(child))
                          ? gtk_label_get_text(GTK_LABEL(child)) : NULL;
    GtkWidget *ghost = gtk_button_new_with_label(txt ? txt : "");
    FlyCtx *fc;

    gtk_widget_add_css_class(ghost, "chip");
    gtk_widget_set_sensitive(ghost, FALSE);
    gtk_widget_set_can_focus(ghost, FALSE);
    gtk_fixed_put(GTK_FIXED(stage), ghost, (int)sx, (int)sy);

    fc = g_new0(FlyCtx, 1);
    fc->chip = chip;
    fc->ghost = ghost;
    fc->stage = stage;
    fc->flying = flying;
    fc->sx = sx;
    fc->sy = sy;
    fc->t0 = -1;

    if (flying)
        *flying = TRUE;
    gtk_widget_add_css_class(chip, "ghost-host");

    g_timeout_add(16, asm_fly_tick, fc);
}

static void asm_move_by_click(GtkButton *button, gpointer data) {
    ChipRef *ref = data;
    SentBuilder *sb = ref->sb;
    int idx = ref->idx;
    int pos = -1;
    double sx = 0.0, sy = 0.0;
    GtkWidget *chip = GTK_WIDGET(button);

    (void)button;

    if (sb->flying)
        return;

    for (int i = 0; i < sb->placed_count; i++) {
        if (sb->placed[i] == idx) {
            pos = i;
            break;
        }
    }

    if (sb->stage &&
        gtk_widget_translate_coordinates(chip, sb->stage, 0, 0, &sx, &sy)) {
        if (pos >= 0) {
            for (int i = pos; i < sb->placed_count - 1; i++)
                sb->placed[i] = sb->placed[i + 1];
            sb->placed_count--;
        } else {
            sb->placed[sb->placed_count++] = idx;
        }

        sent_rebuild(sb);
        asm_begin_fly(chip, sb->stage, &sb->flying, sx, sy);
    } else {
        /* No usable animation layer yet – just move instantly. */
        if (pos >= 0) {
            for (int i = pos; i < sb->placed_count - 1; i++)
                sb->placed[i] = sb->placed[i + 1];
            sb->placed_count--;
        } else {
            sb->placed[sb->placed_count++] = idx;
        }
        sent_rebuild(sb);
    }
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

    for (int i = 0; i < ctx->n; i++) {
        if (ctx->sbs[i]->trans)
            gtk_widget_set_visible(ctx->sbs[i]->trans, TRUE);
    }

    if (ok == ctx->n) {
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        mark_done(ctx->ex_num);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_sentences"));
    }
}

static GtkWidget *build_assembly(const char *title, const char *subtitle,
                                 int ex_num, const AssemblyItem *items,
                                 const char **meanings, int n) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(title, subtitle, "check",
                                    &body, &feedback, &check);
    AsmCtx *ctx = g_new0(AsmCtx, 1);

    ctx->sbs = g_new0(SentBuilder *, n);
    ctx->n = n;
    ctx->ex_num = ex_num;
    ctx->feedback = feedback;

    for (int s = 0; s < n; s++) {
        const AssemblyItem *item = &items[s];
        SentBuilder *sb = g_new0(SentBuilder, 1);
        GtkWidget *holder;
        GtkWidget *group;
        GtkWidget *target;
        GtkWidget *pool;
        GtkWidget *stage;

        sb->n_words = item->n;
        ctx->sbs[s] = sb;

        if (item->prompt) {
            GtkWidget *prompt = gtk_label_new(item->prompt);
            gtk_widget_set_halign(prompt, GTK_ALIGN_START);
            gtk_widget_add_css_class(prompt, "chain");
            gtk_box_append(GTK_BOX(body), prompt);
        }

        holder = gtk_overlay_new();
        gtk_widget_set_hexpand(holder, TRUE);

        group = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
        gtk_widget_add_css_class(group, "sent-group");
        sb->group = group;
        gtk_overlay_set_child(GTK_OVERLAY(holder), group);

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

        stage = gtk_fixed_new();
        gtk_widget_set_halign(stage, GTK_ALIGN_FILL);
        gtk_widget_set_valign(stage, GTK_ALIGN_FILL);
        gtk_widget_set_can_target(stage, FALSE);
        gtk_overlay_add_overlay(GTK_OVERLAY(holder), stage);
        sb->stage = stage;

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
            g_signal_connect(chip, "clicked", G_CALLBACK(asm_move_by_click), ref);
            gtk_widget_add_controller(chip, GTK_EVENT_CONTROLLER(src));
        }

        {
            for (int w = 0; w < item->n; w++)
                sb->pool_order[w] = w;
            shuffle_indices(sb->pool_order, item->n);
            for (int w = 0; w < item->n; w++)
                gtk_flow_box_insert(GTK_FLOW_BOX(pool),
                                    sb->chips[sb->pool_order[w]], -1);
        }

        gtk_box_append(GTK_BOX(body), holder);

        if (meanings[s])
            sb->trans = meaning_add(body, meanings[s]);
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

/* One Czech meaning per dialog line, in the same order as the rows. */
static const char *ex1_meaning[] = {
    "Ahoj! Jmenuji se Anna.",
    "Ahoj! Jmenuji se Petr.",
    "Odkud jsi?",
    "Pocházím z Česka.",
    "Dobré ráno!",
    "Jak se máš?",
    "Mám se dobře, děkuji.",
    "Musím jít. Na shledanou!",
    "Brzy na viděnou!",
    "Čau!",
};

static GtkWidget *build_ex1(void) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell("Dialog",
                                    "sub_dialog",
                                    "check", &body, &feedback, &check);
    ComboListCtx *ctx = combo_list_ctx_new(1, feedback);
    int ri = 0;

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

            if (ri < (int)G_N_ELEMENTS(ex1_meaning))
                combo_list_add_trans(ctx, meaning_add(body, ex1_meaning[ri]));
            ri++;
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
    {NULL, {"Wie", "geht", "es", "dir", "?"}, 5},
};

static const char *ex2_meaning[] = {
    "Jmenuji se Anna.",
    "Odkud jsi?",
    "Pocházím z Česka.",
    "Jak se máš?",
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

static const char *ex3_meaning[] = {
    "„vierzehn“ = čtrnáct (14)",
    "„siebzehn“ = sedmnáct (17)",
    "„zwanzig“ = dvacet (20)",
    "„sechs“ = šest (6)",
};

static const ChoiceQ ex9_questions[] = {
    {"___ heißt du?", {"Wer", "Wie", "Wo"}, 1},
    {"___ kommst du?", {"Woher", "Wie", "Wer"}, 0},
    {"___ wohnst du?", {"Wo", "Wer", "Was"}, 0},
    {"___ ist das?", {"Wer", "Wo", "Wie"}, 0},
    {"___ alt bist du?", {"Wie", "Woher", "Was"}, 0},
};

static const char *ex9_meaning[] = {
    "Jak se jmenuješ?",
    "Odkud jsi?",
    "Kde bydlíš?",
    "Kdo to je?",
    "Kolik je ti let?",
};

typedef struct {
    const ChoiceQ *qs;
    int n;
    int ex_num;
    GtkWidget *feedback;
    GtkToggleButton **toggles;
    GtkWidget **trans;   /* hidden meaning labels, one per question */
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
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        mark_done(ctx->ex_num);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry_short"));
    }

    meaning_reveal_all(ctx->trans, ctx->n);
}

static GtkWidget *build_choice(const char *title, const char *subtitle, int ex_num,
                               const ChoiceQ *qs, const char **meanings, int n) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(title, subtitle, "check",
                                    &body, &feedback, &check);
    ChoiceCtx *ctx = g_new0(ChoiceCtx, 1);

    ctx->qs = qs;
    ctx->n = n;
    ctx->ex_num = ex_num;
    ctx->feedback = feedback;
    ctx->toggles = g_new0(GtkToggleButton *, n * 3);
    ctx->trans = g_new0(GtkWidget *, n);

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

        if (meanings[i])
            ctx->trans[i] = meaning_add(body, meanings[i]);
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

static void ex4_fill_sample(GtkWidget *label) {
    int i = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(label), "qidx"));
    char *txt = g_strdup_printf(tr("sample_fmt"),
                                tr(ex4_questions[i].q_cs),
                                ex4_questions[i].sample,
                                tr(ex4_questions[i].a_cs));

    gtk_label_set_text(GTK_LABEL(label), txt);
    g_free(txt);
}

static void ex4_reveal(GtkButton *button, gpointer data) {
    GtkWidget *sample = data;
    (void)button;
    ex4_fill_sample(sample);
    gtk_widget_set_visible(sample, !gtk_widget_get_visible(sample));
}

typedef struct {
    GtkWidget *feedback;
    GtkWidget **samples;
    int n;
} Ex4Ctx;

static void ex4_finish(GtkButton *button, gpointer data) {
    Ex4Ctx *ctx = data;
    (void)button;
    set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
    for (int i = 0; i < ctx->n; i++) {
        ex4_fill_sample(ctx->samples[i]);
        gtk_widget_set_visible(ctx->samples[i], TRUE);
    }
    mark_done(4);
}

static GtkWidget *build_ex4(void) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *tip;
    Ex4Ctx *ctx;
    GtkWidget *page = ex_page_shell("Freie Antwort",
                                    "sub_free",
                                    "finish", &body, &feedback, &check);

    ctx = g_new0(Ex4Ctx, 1);
    ctx->feedback = feedback;
    ctx->n = (int)G_N_ELEMENTS(ex4_questions);
    ctx->samples = g_new0(GtkWidget *, ctx->n);

    tip = gtk_label_new(NULL);
    i18n_bind(tip, "tip_ss", 0);
    gtk_widget_set_halign(tip, GTK_ALIGN_START);
    gtk_widget_add_css_class(tip, "hint");
    gtk_box_append(GTK_BOX(body), tip);

    for (guint i = 0; i < G_N_ELEMENTS(ex4_questions); i++) {
        GtkWidget *q = gtk_label_new(ex4_questions[i].question);
        GtkWidget *entry;
        GtkWidget *reveal;
        GtkWidget *sample;

        gtk_widget_set_halign(q, GTK_ALIGN_START);
        gtk_widget_add_css_class(q, "ex-prompt");
        gtk_box_append(GTK_BOX(body), q);

        entry = gtk_entry_new();
        i18n_bind(entry, "your_answer", 3);
        gtk_box_append(GTK_BOX(body), entry);

        reveal = gtk_button_new();
        i18n_bind(reveal, "show_sample", 1);
        gtk_widget_add_css_class(reveal, "pill");
        gtk_widget_set_halign(reveal, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(body), reveal);

        sample = gtk_label_new(NULL);
        g_object_set_data(G_OBJECT(sample), "qidx", GINT_TO_POINTER(i));
        i18n_bind(sample, "sample_fmt", 4);
        gtk_widget_set_halign(sample, GTK_ALIGN_START);
        gtk_widget_add_css_class(sample, "hint");
        gtk_label_set_wrap(GTK_LABEL(sample), TRUE);
        gtk_widget_set_visible(sample, FALSE);
        gtk_box_append(GTK_BOX(body), sample);
        ctx->samples[i] = sample;

        g_signal_connect(reveal, "clicked", G_CALLBACK(ex4_reveal), sample);
    }

    g_signal_connect(check, "clicked", G_CALLBACK(ex4_finish), ctx);
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

static const char *ex5_meaning[] = {
    "čtrnáct",
    "sedmnáct",
    "dvacet",
    "šest",
    "třináct",
};

static GtkWidget *build_ex5(void) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell("Zahlen",
                                    "sub_zahlen",
                                    "check", &body, &feedback, &check);
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
        combo_list_add_trans(ctx, meaning_add(body, ex5_meaning[i]));
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

static const char *ex6_meaning[] = {
    "dvě jablka",
    "pět hvězd",
    "sedm koček",
    "devět květin",
    "tři auta",
    "jedenáct knih",
    "čtyři svíčky",
    "osm míčů",
    "šest ptáků",
};

static GtkWidget *build_ex6(void) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell("Wie viel?",
                                    "sub_wieviel",
                                    "check", &body, &feedback, &check);
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
        combo_list_add_trans(ctx, meaning_add(body, ex6_meaning[i]));
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

static const char *ex7_meaning[] = {
    "šest, sedm, osm, devět",
    "dvanáct, třináct, čtrnáct, patnáct",
    "osmnáct, devatenáct, dvacet, dvacet jedna",
    "čtyři, pět, šest, sedm",
    "deset, jedenáct, dvanáct, třináct",
};

static GtkWidget *build_ex7(void) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell("Zahlenreihe",
                                    "sub_reihe",
                                    "check", &body, &feedback, &check);
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

        combo_list_add_trans(ctx, meaning_add(body, ex7_meaning[i]));
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

static const char *ex8_meaning[] = {
    "Jmenuji se Petr.",
    "Odkud jsi?",
    "Bydlím v Praze.",
    "Kolik je ti let?",
    "Hraji fotbal.",
    "Co rád/a děláš?",
};

static GtkWidget *build_ex8(void) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell("Verb einsetzen",
                                    "sub_verb",
                                    "check", &body, &feedback, &check);
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

        combo_list_add_trans(ctx, meaning_add(body, ex8_meaning[i]));
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

static const char *ex10_meaning[] = {
    "Kolik je hodin?",
    "Jmenuji se Anna.",
    "Odkud jsi?",
    "Na shledanou.",
    "Moc děkuji.",
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

static const char *ex12_meaning[] = {
    "Plave v jezeře.",
    "Zpívá píseň.",
    "Vaří polévku.",
    "Čte knihu.",
    "Řídí auto.",
    "Maluje obraz.",
    "Hraje fotbal.",
    "Spí.",
};

static GtkWidget *build_ex12(void) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell("Was macht er/sie?",
                                    "sub_bild",
                                    "check", &body, &feedback, &check);
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

        combo_list_add_trans(ctx, meaning_add(body, ex12_meaning[i]));
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

static const char *ex11_meaning[] = {
    "Ahoj",
    "Dobré ráno",
    "Dobrý den",
    "Dobrý večer",
    "Dobrý den (Bavorsko, Rakousko)",
    "Ahoj / Čau",
    "Čau",
    "Na shledanou",
    "Brzy na viděnou",
    "Do zítřka",
    "Dobrou noc",
    "Zatím / Na viděnou",
};

static const char *ex13_meaning[] = {
    "Preclík",
    "Klobása",
    "Braniborská brána",
    "Značka aut",
    "Rakouský skladatel",
    "Čokoládový dort (Vídeň)",
    "Vídeňský řízek",
    "Sýr",
    "Švýcarská hora",
    "Čokoláda",
};

typedef struct {
    GtkWidget *pool;
    GtkWidget *group_flow[3];
    GtkWidget *group_btn[3];
    int n_groups;
    int active_group;
    GtkWidget **items;    /* per-item boxes (chip + hidden meaning)   */
    GtkWidget **trans;    /* hidden meaning labels                    */
    GtkWidget *stage;     /* transparent overlay layer for the fly    */
    gboolean flying;      /* a click-move animation is in progress    */
    int n_items;
    int *current_group;
    const AssignItem *items_data;
    int ex_num;
    GtkWidget *feedback;
} AssignCtx;

typedef struct {
    AssignCtx *ac;
    int idx;
} AssignChipRef;

static void assign_rebuild(AssignCtx *ac) {
    for (int i = 0; i < ac->n_items; i++) {
        GtkWidget *parent = gtk_widget_get_parent(ac->items[i]);
        if (parent != NULL && GTK_IS_FLOW_BOX_CHILD(parent))
            gtk_flow_box_child_set_child(GTK_FLOW_BOX_CHILD(parent), NULL);
    }

    flow_clear(GTK_FLOW_BOX(ac->pool));
    for (int g = 0; g < ac->n_groups; g++)
        flow_clear(GTK_FLOW_BOX(ac->group_flow[g]));

    for (int i = 0; i < ac->n_items; i++) {
        if (ac->current_group[i] < 0)
            gtk_flow_box_insert(GTK_FLOW_BOX(ac->pool), ac->items[i], -1);
        else
            gtk_flow_box_insert(GTK_FLOW_BOX(ac->group_flow[ac->current_group[i]]),
                                ac->items[i], -1);
    }
}

static void assign_chip_clicked(GtkButton *button, gpointer data) {
    AssignChipRef *ref = data;
    AssignCtx *ac = ref->ac;
    int idx = ref->idx;
    double sx = 0.0, sy = 0.0;
    GtkWidget *chip = GTK_WIDGET(button);

    (void)button;

    if (ac->flying)
        return;

    if (ac->stage &&
        gtk_widget_translate_coordinates(chip, ac->stage, 0, 0, &sx, &sy)) {
        if (ac->current_group[idx] < 0)
            ac->current_group[idx] = ac->active_group;
        else
            ac->current_group[idx] = -1;

        assign_rebuild(ac);
        asm_begin_fly(chip, ac->stage, &ac->flying, sx, sy);
    } else {
        /* No usable animation layer yet – just move instantly. */
        if (ac->current_group[idx] < 0)
            ac->current_group[idx] = ac->active_group;
        else
            ac->current_group[idx] = -1;

        assign_rebuild(ac);
    }
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
        if (ac->current_group[i] == ac->items_data[i].correct_group)
            ok++;

    if (ok == ac->n_items) {
        set_feedback(ac->feedback, TRUE, tr("feedback_ok"));
        mark_done(ac->ex_num);
    } else {
        set_feedback(ac->feedback, FALSE, tr("feedback_retry"));
    }

    for (int i = 0; i < ac->n_items; i++)
        if (ac->trans[i])
            gtk_widget_set_visible(ac->trans[i], TRUE);
}

static GtkWidget *build_assign(const char *title, const char *subtitle, int ex_num,
                               const AssignItem *items, int n_items,
                               const char **group_labels, int n_groups,
                               const char **meanings) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(title, subtitle, "check",
                                    &body, &feedback, &check);
    AssignCtx *ac = g_new0(AssignCtx, 1);
    GtkWidget *hint;
    GtkWidget *row;
    GtkWidget *holder;
    GtkWidget *content;
    GtkWidget *stage;
    GtkToggleButton *first = NULL;
    int *order;

    ac->n_items = n_items;
    ac->n_groups = n_groups;
    ac->active_group = 0;
    ac->ex_num = ex_num;
    ac->feedback = feedback;
    ac->items_data = items;
    ac->items = g_new0(GtkWidget *, n_items);
    ac->trans = g_new0(GtkWidget *, n_items);
    ac->current_group = g_new0(int, n_items);
    for (int i = 0; i < n_items; i++)
        ac->current_group[i] = -1;

    holder = gtk_overlay_new();
    gtk_widget_set_hexpand(holder, TRUE);
    gtk_box_append(GTK_BOX(body), holder);

    content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_widget_set_halign(content, GTK_ALIGN_FILL);
    gtk_overlay_set_child(GTK_OVERLAY(holder), content);

    stage = gtk_fixed_new();
    gtk_widget_set_halign(stage, GTK_ALIGN_FILL);
    gtk_widget_set_valign(stage, GTK_ALIGN_FILL);
    gtk_widget_set_can_target(stage, FALSE);
    gtk_overlay_add_overlay(GTK_OVERLAY(holder), stage);
    ac->stage = stage;

    hint = gtk_label_new(NULL);
    i18n_bind(hint, "assign_hint", 0);
    gtk_label_set_wrap(GTK_LABEL(hint), TRUE);
    gtk_widget_set_halign(hint, GTK_ALIGN_START);
    gtk_widget_add_css_class(hint, "ex-sub");
    gtk_box_append(GTK_BOX(content), hint);

    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_halign(row, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(content), row);
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
    gtk_box_append(GTK_BOX(content), ac->pool);

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

        gtk_box_append(GTK_BOX(content), panel);
    }

    order = g_new0(int, n_items);
    for (int i = 0; i < n_items; i++)
        order[i] = i;
    shuffle_indices(order, n_items);

    for (int k = 0; k < n_items; k++) {
        int i = order[k];
        GString *lab = g_string_new(NULL);
        GtkWidget *box;
        GtkWidget *chip;
        GtkWidget *m = NULL;
        AssignChipRef *ref = g_new(AssignChipRef, 1);

        if (items[i].emoji) {
            g_string_append(lab, items[i].emoji);
            g_string_append(lab, " ");
        }
        g_string_append(lab, items[i].label);

        box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        gtk_widget_set_halign(box, GTK_ALIGN_CENTER);

        chip = gtk_button_new_with_label(lab->str);
        g_string_free(lab, TRUE);
        gtk_widget_add_css_class(chip, "chip");
        gtk_widget_set_halign(chip, GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(box), chip);

        if (meanings[i]) {
            m = gtk_label_new(NULL);
            i18n_bind(m, meanings[i], 0);
            gtk_label_set_xalign(GTK_LABEL(m), 0.5);
            gtk_widget_set_halign(m, GTK_ALIGN_CENTER);
            gtk_widget_add_css_class(m, "meaning");
            gtk_label_set_wrap(GTK_LABEL(m), TRUE);
            gtk_label_set_max_width_chars(GTK_LABEL(m), 16);
            gtk_widget_set_visible(m, FALSE);
            gtk_box_append(GTK_BOX(box), m);
            ac->trans[i] = m;
        }

        g_object_ref(box);
        ac->items[i] = box;

        ref->ac = ac;
        ref->idx = i;
        g_signal_connect(chip, "clicked", G_CALLBACK(assign_chip_clicked), ref);

        gtk_flow_box_insert(GTK_FLOW_BOX(ac->pool), box, -1);
    }
    g_free(order);

    g_signal_connect(check, "clicked", G_CALLBACK(assign_check), ac);
    return page;
}

/* ------------------------------------------------------------------ */
/* Settings                                                           */
/* ------------------------------------------------------------------ */

static void draw_theme_swatch(GtkDrawingArea *area, cairo_t *cr,
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

static void on_mode_toggled(GtkToggleButton *btn, gpointer user_data) {
    ColorMode mode = (ColorMode)GPOINTER_TO_INT(user_data);

    if (!gtk_toggle_button_get_active(btn))
        return;
    if (app_color_mode == mode)
        return;
    app_color_mode = mode;
    apply_theme();
    save_settings();
}

static void on_theme_toggled(GtkToggleButton *btn, gpointer user_data) {
    ThemeId id = (ThemeId)GPOINTER_TO_INT(user_data);

    if (!gtk_toggle_button_get_active(btn))
        return;
    if (app_theme_id == id)
        return;
    app_theme_id = id;
    apply_theme();
    save_settings();
}

static void on_lang_toggled(GtkToggleButton *btn, gpointer user_data) {
    UiLang lang = (UiLang)GPOINTER_TO_INT(user_data);

    if (!gtk_toggle_button_get_active(btn))
        return;
    if (app_lang == lang)
        return;
    app_lang = lang;
    apply_language();
    save_settings();
}

static GtkWidget *make_lang_chip(const char *label, UiLang lang,
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

static GtkWidget *make_mode_chip(const char *label, ColorMode mode,
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

static GtkWidget *make_theme_card(ThemeId id, GtkToggleButton *group) {
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

static void on_settings_clicked(GtkButton *button, gpointer user_data) {
    GtkPopover *popover = GTK_POPOVER(user_data);

    (void)button;
    gtk_popover_popup(popover);
}

static GtkWidget *build_settings_button(void) {
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
    gtk_widget_set_parent(popover, btn);
    g_signal_connect_swapped(btn, "destroy",
                             G_CALLBACK(gtk_widget_unparent), popover);
    gtk_popover_set_has_arrow(GTK_POPOVER(popover), TRUE);
    gtk_popover_set_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_settings_clicked), popover);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_widget_add_css_class(box, "settings-box");
    gtk_popover_set_child(GTK_POPOVER(popover), box);

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

    return btn;
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
    char name[16];

    (void)user_data;

    load_settings();
    load_progress();
    app_theme = theme_palette(app_theme_id, app_color_mode);

    window = GTK_WINDOW(gtk_application_window_new(app));
    main_window = window;
    gtk_window_set_title(window, "Sprechen.c");
    gtk_window_set_default_size(window, 760, 560);

    headerbar = gtk_header_bar_new();
    gtk_widget_add_css_class(headerbar, "titlebar");
    title_label = gtk_label_new("Sprechen.c");
    gtk_widget_add_css_class(title_label, "app-title");
    gtk_header_bar_set_title_widget(GTK_HEADER_BAR(headerbar), title_label);
    gtk_window_set_titlebar(window, headerbar);
    apply_theme();
    gtk_header_bar_pack_end(GTK_HEADER_BAR(headerbar), build_settings_button());

    main_stack = GTK_STACK(gtk_stack_new());
    gtk_stack_set_transition_type(GTK_STACK(main_stack),
                                  GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_window_set_child(window, GTK_WIDGET(main_stack));

    welcome_page = build_welcome_page();
    roadmap_page = build_roadmap_page();
    unit1_page = build_unit1_page();

    ex_pages[1] = build_ex1();
    ex_pages[2] = build_assembly("Sätze bilden",
                                 "sub_assembly",
                                 2, ex2_items, ex2_meaning, G_N_ELEMENTS(ex2_items));
    ex_pages[3] = build_choice("Was ist richtig?", "sub_choice_num",
                               3, ex3_questions, ex3_meaning, G_N_ELEMENTS(ex3_questions));
    ex_pages[4] = build_ex4();
    ex_pages[5] = build_ex5();
    ex_pages[6] = build_ex6();
    ex_pages[7] = build_ex7();
    ex_pages[8] = build_ex8();
    ex_pages[9] = build_choice("Wer? Wie? Wo?", "sub_wer",
                               9, ex9_questions, ex9_meaning, G_N_ELEMENTS(ex9_questions));
    ex_pages[10] = build_assembly("Wörter trennen",
                                  "sub_assembly",
                                  10, ex10_items, ex10_meaning, G_N_ELEMENTS(ex10_items));
    ex_pages[11] = build_assign("Grußformen", "sub_gruss",
                                11, ex11_items, G_N_ELEMENTS(ex11_items),
                                ex11_groups, G_N_ELEMENTS(ex11_groups), ex11_meaning);
    ex_pages[12] = build_ex12();
    ex_pages[13] = build_assign("Länder", "sub_land",
                                13, ex13_items, G_N_ELEMENTS(ex13_items),
                                ex13_groups, G_N_ELEMENTS(ex13_groups), ex13_meaning);

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

    apply_theme();
    apply_language();

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
