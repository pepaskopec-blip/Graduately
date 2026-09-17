#include "maturita.h"

GtkStack *main_stack;

GtkWindow *main_window;

GtkWidget *welcome_heading;

GtkCssProvider *theme_provider;

ThemeId   app_theme_id = THEME_CATPPUCCIN;

ColorMode app_color_mode = MODE_LIGHT;

UiLang    app_lang = LANG_CS;

ThemePalette app_theme;

double app_ui_scale = 1.0;

const char *theme_names[THEME_COUNT] = {
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

GtkWidget *theme_swatch_areas[THEME_COUNT];

UnitCtx units[NUM_UNITS] = {0};

const char *const u1_ex_names[MAX_UNIT_EX + 1] = {
    NULL,
    "Dialog", "Sätze bilden", "Was ist richtig?", "Freie Antwort",
    "Zahlen", "Wie viel?", "Zahlenreihe", "Verb einsetzen",
    "Wer? Wie? Wo?", "Wörter trennen", "Grußformen",
    "Was macht er/sie?", "Länder",
};

const char *const u2_ex_names[MAX_UNIT_EX + 1] = {
    NULL,
    "Verben konjugieren", "aus oder in", "Fragewörter", "sprechen",
    "Nationalitäten", "Woher?", "Länder schreiben", "Verben einsetzen",
    "Freie Antwort", "Euro", "Wörter suchen", "Was ist richtig?",
    "Ordne zu", "Lückentext", "Verbinde", "Zahlen",
    "Steckbrief", "Berufe", "Nationalität",
};

const char *const u3_ex_names[MAX_UNIT_EX + 1] = {
    NULL,
    "Familienpaare", "mein oder dein", "ein / kein", "Lückentext",
    "Marcos Familie", "Sortieren", "Possessivtabelle", "Akkusativ",
    "kein / nicht", "Sätze bauen", "Was siehst du?", "Wem gehört das?",
    "Es gibt …", "Beschreiben", "Wochenende",
};

GtkWidget *road_scroll;

GtkWidget *road_fixed;

GtkWidget *road_rail;

GtkWidget *road_nodes[NUM_UNITS];

GtkWidget *road_labels[NUM_UNITS];

double road_cx[NUM_UNITS];

double road_cy[NUM_UNITS];

int road_cols = NUM_UNITS;

int road_rows = 1;

int road_cw = (int)(2.0 * ROAD_MX + (NUM_UNITS - 1) * PATH_SPAC);

int road_ch = (int)(2.0 * ROAD_MY);
