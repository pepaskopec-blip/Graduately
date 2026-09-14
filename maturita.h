#ifndef MATURITA_H
#define MATURITA_H

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib/gstdio.h>
#include <pango/pangocairo.h>
#include <math.h>

#define NUM_UNITS     10
#define NUM_UNLOCKED   3   /* first NUM_UNLOCKED units (index 0..2) are open */
#define MAX_UNIT_EX   20   /* largest exercise count any unit may have       */
#define NUM_SUBJECTS  13
#define NET_SUBJ       1    /* index of "Správa počítačových sítí"     */
#define HW_SUBJ        2    /* index of "Technické vybavení"          */
#define CZ_SUBJ        3    /* index of "Český jazyk a literatura"    */
#define CSS_FILE      "style.css"
#define PROGRESS_DIR  "progress"
#define PROGRESS_U1   "progress/unit1.conf"
#define PROGRESS_U2   "progress/unit2.conf"
#define PROGRESS_U3   "progress/unit3.conf"
#define SETTINGS_FILE "progress/settings.conf"
#define PROGRESS_NET  "progress/net.conf"
#define PROGRESS_HW   "progress/hw.conf"
#define NODE_SIZE    88.0
#define PATH_SPAC    240.0
#define ROAD_MX    150.0   /* horizontal canvas margin                 */
#define ROAD_MY    150.0   /* vertical canvas margin                   */
#define ROAD_GAP   240.0   /* vertical space between folded rows       */
#define ROAD_WAVE   44.0   /* wavy vertical offset of the nodes        */
#define SUB_BUBBLE    NODE_SIZE    /* round node, matches .unit-node */
#define SUB_MX        110.0
#define SUB_MY        150.0
#define SUB_SPAC      220.0
#define SUB_GAP       210.0
#define SUB_WAVE       34.0
#define EX_BUBBLE  64.0    /* exercise bubble size (matches .ex-bubble) */
#define EX_MX      80.0    /* horizontal canvas margin                  */
#define EX_MY      120.0   /* vertical canvas margin                    */
#define EX_SPAC    150.0   /* horizontal distance between bubbles       */
#define EX_GAP     200.0   /* vertical space between folded rows        */
#define EX_WAVE    26.0    /* wavy vertical offset of the bubbles       */
#define EX_BRANCH_LIFT 150.0  /* extra top space for an off-path branch  */
#define EX_BRANCH_DX   64.0   /* sideways offset so the branch link slants */
#define HM_WORDS 5
#define HM_MAX_MISSES 6
#define HM_N_LETTERS 29   /* QWERTY rows + Ä/Ö/Ü (see hm_letters[]) */
#define HM_ROW2 10   /* start of the ASDF row */
#define HM_ROW3 19   /* start of the ZXCV row   */
#define HM_ROW4 26   /* start of the umlaut row */
#define NET_UNITS      30
#define NET_SLIDES      4
#define NET2_SLIDES     5
#define NET3_SLIDES     4
#define NET4_SLIDES     4
#define NET5_SLIDES     4
#define NET6_SLIDES     4
#define NET7_SLIDES     4
#define NET8_SLIDES     4
#define NET9_SLIDES     4
#define NET10_SLIDES    3
#define NET11_SLIDES    8
#define NET12_SLIDES    6
#define NET13_SLIDES    4
#define NET14_SLIDES    5
#define NET15_SLIDES    3
#define NET16_SLIDES    4
#define NET17_SLIDES    7
#define NET18_SLIDES    5
#define NET19_SLIDES    7
#define NET20_SLIDES    7
#define NET21_SLIDES    8
#define NET22_SLIDES    8
#define NET_LESSONS     22
#define HW_UNITS        10
#define HW_LESSONS       3
#define HW_SLIDES        3
#define HW2_SLIDES       6
#define HW3_SLIDES       2

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
typedef struct TransSection TransSection; /* vocabulary word list for a branch */
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
typedef struct {
    const char *title;         /* German unit title                       */
    const char *page;          /* roadmap-stack child name, e.g. "unit1"  */
    const char *ex_tag;        /* exercise page prefix, e.g. "u1"         */
    const char *sub_key;       /* i18n key for the exercise-map subtitle   */
    const char *progress_file;
    gboolean unlocked;
    gboolean done[MAX_UNIT_EX + 1];         /* exercise state, 1..n_ex    */
    const char *ex_names[MAX_UNIT_EX + 1];  /* bubble labels, 1..n_ex     */
    int n_ex;
    GtkWidget *node;             /* roadmap node button                    */
    GtkWidget *node_done_icon;
    /* exercise-map page widgets + serpentine geometry */
    GtkWidget *ex_scroll;
    GtkWidget *ex_fixed;
    GtkWidget *ex_rail;
    GtkWidget *ex_cells[MAX_UNIT_EX];       /* 0..n_ex-1                   */
    GtkWidget *ex_labels[MAX_UNIT_EX];      /* 0..n_ex-1                   */
    GtkWidget *ex_icons[MAX_UNIT_EX + 1];   /* per-bubble check icon 1..n  */
    double ex_cx[MAX_UNIT_EX];
    double ex_cy[MAX_UNIT_EX];
    int ex_rows, ex_cols, ex_cw, ex_ch;
    guint ex_idle;
    /* optional off-path branch exercise (e.g. vocabulary training) */
    gboolean has_branch;
    const char *branch_name;     /* label under the branch bubble          */
    const char *branch_target;   /* stack page name                        */
    int branch_ex;               /* done[] slot used by the branch          */
    GtkWidget *branch_cell;
    GtkWidget *branch_label;
    GtkWidget *branch_icon;
    double branch_cx;
    double branch_cy;
    const struct TransSection *trans_sections; /* branch vocabulary content */
    int n_trans_sections;
} UnitCtx;
typedef struct { double r, g, b; } Rgb;
typedef struct {
    GtkWidget *widget;
    const char *key;
    int kind; /* 0 label, 1 button, 2 tooltip, 3 placeholder, 4 ex4 sample */
} I18nBind;
typedef struct {
    const char *key;
    const char *cs;
    const char *en;
} TrEntry;
typedef struct {
    GtkWidget *ex_value;
    GtkWidget *pct_value;
    GtkWidget *units_value;
    GtkWidget *overall_bar;

    /* one progress block per subject (index matches sub_keys[]) */
    struct {
        GtkWidget *name;       /* subject heading label                  */
        GtkWidget *count;      /* "13 / 47" or the locked text           */
        GtkWidget *bar;        /* per-subject overall progress bar       */
        GtkWidget *units_box;  /* holds the unit rows, NULL if no units  */
    } subj[NUM_SUBJECTS];

    /* per-unit rows, indexed by the global units[] index */
    GtkWidget *unit_count[NUM_UNITS];
    GtkWidget *unit_badge[NUM_UNITS];
    GtkWidget *unit_bar[NUM_UNITS];
    GtkWidget *unit_row[NUM_UNITS];
} StatsUi;
typedef struct {
    int done_ex;
    int total_ex;
    int done_units;
    int open_units;
} ProgressSum;
typedef struct {
    GArray *combos;   /* GtkComboBox* */
    GArray *answers;  /* const char* */
    GArray *trans;    /* GtkWidget* hidden meaning labels              */
    GArray *models;   /* GtkWidget* hidden German answer labels        */
    gboolean trans_upfront;  /* meanings shown before Check            */
    gboolean reveal_german;  /* German answer shown on Check           */
    UnitCtx *unit;
    int ex_num;
    GtkWidget *feedback;
} ComboListCtx;
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
typedef struct {
    SentBuilder *sb;
    int idx;
} ChipRef;
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
typedef struct {
    SentBuilder **sbs;
    int n;
    UnitCtx *unit;
    int ex_num;
    GtkWidget *feedback;
} AsmCtx;
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
typedef struct {
    const char *prompt;
    const char *options[5];
    int n_options;
    int correct;
} ChoiceQ;
typedef struct {
    const ChoiceQ *qs;
    int n;
    int n_opts;
    UnitCtx *unit;
    int ex_num;
    GtkWidget *feedback;
    GtkToggleButton **toggles;
    GtkWidget **trans;   /* hidden meaning labels, one per question */
} ChoiceCtx;
typedef struct {
    const char *question;
    const char *q_cs;   /* Czech translation of the question     */
    const char *sample;
    const char *a_cs;   /* Czech translation of the sample answer */
} FreeQ;
typedef struct {
    UnitCtx *unit;
    int ex_num;
    GtkWidget *feedback;
    GtkWidget **samples;
    int n;
} Ex4Ctx;
typedef struct {
    const char *digits;
    const char *answer;
} NumberQ;
typedef struct {
    const char *emoji;
    int count;
    const char *noun;
    const char *answer;
} CountQ;
typedef struct {
    const char *before;
    const char *after;
    const char *answer;
} SeqQ;
typedef struct {
    const char *before;
    const char *after;
    const char *answer;
} VerbQ;
typedef struct {
    const char *emoji;
    const char *before;
    const char *after;
    const char *answer;
} VerbClueQ;
typedef struct {
    const char *emoji;
    const char *label;
    int correct_group;
} AssignItem;
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
    UnitCtx *unit;
    int ex_num;
    GtkWidget *feedback;
} AssignCtx;
typedef struct {
    AssignCtx *ac;
    int idx;
} AssignChipRef;
typedef struct {
    ComboListCtx *ctx;
    GtkWidget *note;
    const VerbQ *qs;
    int n;
    const char **gloss;   /* per-row word-level meaning keys (current lang) */
} VerbNoteCtx;
typedef struct {
    const char *prompt;    /* German prompt text                       */
    const char *answers;   /* accepted answers separated by '|'         */
    const char *meaning;   /* i18n key of the revealed meaning or NULL  */
} TypedQ;
struct TransSection {
    const char *header;   /* page label shown above the word, e.g. "Strana 22" */
    const TypedQ *rows;
    int n;
};
typedef struct {
    const TypedQ *qs;
    int n;
    UnitCtx *unit;
    int ex_num;
    GtkWidget *feedback;
    GtkWidget **entries;
    GtkWidget **trans;
} TypedCtx;
typedef struct {
    TypedCtx *ctx;
    GtkWidget *note;
    const TypedQ *qs;
    int n;
} TypedNoteCtx;
typedef struct {
    const char *mid;   /* middle part of the question, without the FW */
    const char *fw;    /* correct Fragewort */
    const char *ans;   /* correct answer */
} OrdneRow;
typedef struct {
    const char *stem;    /* German sentence beginning with an ellipsis */
    const char *sample;  /* finished German sample line               */
} ProfileQ;
typedef struct {
    UnitCtx *unit;
    int ex_num;
    GtkWidget *feedback;
    GtkWidget **samples;
    int n;
} ProfileCtx;
typedef struct {
    UnitCtx *unit;
    int ex_num;
    GtkWidget *feedback;
    GtkWidget *draw;
    GtkWidget *slots;
    GtkWidget *hint_lbl;
    GtkWidget *counter_lbl;
    GtkWidget **letter_btns;
    gboolean guessed[HM_N_LETTERS];
    gboolean locked;      /* true during the "word solved" pause */
    gboolean finished;
    int misses;
    int word_index;
} HangmanCtx;
typedef struct {
    const char *num;
    const char *s0, *a0, *s1, *a1, *s2, *a2, *s3;   /* up to 3 blanks  */
    const char *mean;                                /* Czech meaning   */
} U3Fill;
typedef struct {
    const char *num;      /* item number on the first line of the item  */
    const char *seg[4];   /* literal text segments, gaps+1 of them      */
    const char *ans[3];   /* correct word per blank                     */
    int gaps;
} Ex2Row;
typedef struct {
    const char *czech;    /* whole-sentence translation, always shown   */
    const Ex2Row *rows;
    int nrows;
} Ex2Item;
typedef struct {
    GArray *combos;        /* every blank combo (checking order)         */
    GArray *answers;
    GtkWidget **notes;     /* one hidden word-translation note per item  */
    GArray **item_combos;  /* blank combos of each item                  */
    int n_items;
    UnitCtx *unit;
    int ex_num;
    GtkWidget *feedback;
} Ex2Ctx;
typedef struct {
    const char *num;
    const char *s0, *a0, *s1, *a1, *s2, *a2, *s3, *a3, *s4;
    const char *mean;
} U3Let;
typedef struct {
    GArray *entries;
    GArray *answers;
    GArray *trans;
    GArray *models;
    UnitCtx *unit;
    int ex_num;
    GtkWidget *feedback;
} U3LetCtx;
typedef struct {
    const char *seg[5];   /* literal text segments (gaps + 1)           */
    const char *ans[4];   /* missing letters per gap                    */
    const char *czech;    /* whole-sentence translation, always shown   */
    const char *word[4];  /* completed German word per gap              */
    const char *gmean[4]; /* Czech meaning of that word                 */
    int gaps;
} Ex4Row;
typedef struct {
    GArray *entries;   /* every small letter entry (checking order)     */
    GArray *answers;
    const Ex4Row *rows;
    int n_rows;
    GtkWidget **notes; /* hidden word-translation note per row          */
    UnitCtx *unit;
    int ex_num;
    GtkWidget *feedback;
} Ex4LetterCtx;
typedef struct {
    GtkWidget **entries;
    const char **answers;
    int n;
    UnitCtx *unit;
    int ex_num;
    GtkWidget *feedback;
} U3TableCtx;
typedef struct {
    const char *prompt;
    const char *answers;   /* accepted substrings separated by '|'     */
    const char *mean;      /* Czech meaning shown under the entry       */
    const char *german;    /* German sentence(s) revealed after Check   */
} U3Kw;
typedef struct {
    const U3Kw *qs;
    int n;
    UnitCtx *unit;
    int ex_num;
    GtkWidget *feedback;
    GtkWidget **entries;
    GtkWidget **trans;
    GtkWidget **models;
} U3KwCtx;
typedef struct {
    const char *prompt;
    const char *solution;
} NetTask;
typedef struct {
    int pfx;
    const char *net;
    const char *bcast;
    const char *lo;
    const char *hi;
} NetAns;
typedef struct {
    GtkWidget *combo[4];
    GtkWidget *net[4];
    GtkWidget *bcast[4];
    GtkWidget *lo[4];
    GtkWidget *hi[4];
    GtkWidget *feed;
    GtkWidget *note;
    int id;
    gboolean infeasible;
} NetQz;
typedef struct {
    GtkWidget *stack;
    GtkWidget *prev_btn;
    GtkWidget *next_btn;
    guint idx;
    guint n_slides;
    const char *unit_page;
    const char *ex_page;
    GArray *note_kicks;
    GArray *note_titles;
    GArray *note_bodies;
    GtkWidget *done_icon;
    gboolean done;
} NetLesson;
typedef struct {
    const char *kicker;
    const char *title;
    const char *tip;
    const char *line[8];
} NetSlide;
typedef struct {
    const ChoiceQ *qs;
    int n;
    int n_opts;
    int lesson_id;
    GtkWidget *feedback;
    GtkToggleButton **toggles;
    GtkWidget **hints;
} NetMcqCtx;

extern GtkStack *main_stack;
extern GtkWindow *main_window;
extern GtkWidget *welcome_heading;
extern GtkCssProvider *theme_provider;
extern ThemeId   app_theme_id;
extern ColorMode app_color_mode;
extern UiLang    app_lang;
extern ThemePalette app_theme;
extern double app_ui_scale;
extern const char *theme_names[THEME_COUNT];
extern GtkWidget *theme_swatch_areas[THEME_COUNT];
extern UnitCtx units[NUM_UNITS];
extern const char *sub_keys[NUM_SUBJECTS];
extern const int sub_unit_start[NUM_SUBJECTS];
extern const int sub_unit_count[NUM_SUBJECTS];
extern const char *const u1_ex_names[MAX_UNIT_EX + 1];
extern const char *const u2_ex_names[MAX_UNIT_EX + 1];
extern const char *const u3_ex_names[MAX_UNIT_EX + 1];
extern GtkWidget *road_scroll;
extern GtkWidget *road_fixed;
extern GtkWidget *road_rail;
extern GtkWidget *road_nodes[NUM_UNITS];
extern GtkWidget *road_labels[NUM_UNITS];
extern double road_cx[NUM_UNITS];
extern double road_cy[NUM_UNITS];
extern int road_cols;
extern int road_rows;
extern int road_cw;
extern int road_ch;
extern GArray *i18n_binds;
extern const TrEntry tr_ui[];
extern const TrEntry tr_content[];
extern StatsUi stats_ui;
extern char *stats_return_page;
extern guint road_idle;
extern GtkWidget *sub_scroll;
extern GtkWidget *sub_fixed;
extern GtkWidget *sub_rail;
extern GtkWidget *sub_cells[NUM_SUBJECTS];
extern GtkWidget *sub_labels[NUM_SUBJECTS];
extern double sub_cx[NUM_SUBJECTS];
extern double sub_cy[NUM_SUBJECTS];
extern int sub_rows;
extern int sub_cols;
extern int sub_cw;
extern int sub_ch;
extern guint sub_idle;
extern const DialogRow ex1_d1[];
extern const DialogRow ex1_d2[];
extern const DialogRow ex1_d3[];
extern const Dialogue ex1_dialogues[];
extern const char *ex1_pool[];
extern const char *ex1_meaning[];
extern const AssemblyItem ex2_items[];
extern const char *ex2_meaning[];
extern const ChoiceQ ex3_questions[];
extern const char *ex3_meaning[];
extern const ChoiceQ ex9_questions[];
extern const char *ex9_meaning[];
extern const FreeQ ex4_questions[];
extern const NumberQ ex5_data[];
extern const char *ex5_pool[];
extern const char *ex5_meaning[];
extern const CountQ ex6_data[];
extern const char *ex6_pool[];
extern const char *ex6_meaning[];
extern const SeqQ ex7_data[];
extern const char *ex7_pool[];
extern const char *ex7_meaning[];
extern const VerbQ ex8_data[];
extern const char *ex8_pool[];
extern const char *ex8_meaning[];
extern const AssemblyItem ex10_items[];
extern const char *ex10_meaning[];
extern const VerbClueQ ex12_data[];
extern const char *ex12_pool[];
extern const char *ex12_meaning[];
extern const AssignItem ex11_items[];
extern const AssignItem ex13_items[];
extern const char *ex11_groups[];
extern const char *ex13_groups[];
extern const char *ex11_meaning[];
extern const char *ex13_meaning[];
extern const char *g05_bank;
extern const TypedQ g05_rows[];
extern const TypedQ g06_rows[];
extern const TypedQ g07_rows[];
extern const TypedQ g11_rows[];
extern const TypedQ s03_rows[];
extern const TransSection u1_trans_sections[];
extern const int u1_trans_sections_n;
extern const TransSection u2_trans_sections[];
extern const int u2_trans_sections_n;
extern const TransSection u3_trans_sections[];
extern const int u3_trans_sections_n;
extern const VerbQ g01_peter[];
extern const VerbQ g01_jana[];
extern const char *g01_pool[];
extern const char *g01_peter_mean[];
extern const char *g01_jana_mean[];
extern const VerbQ g04_rows[];
extern const char *g04_pool[];
extern const char *g04_mean[];
extern const VerbQ g08_rows[];
extern const char *g08_pool[];
extern const char *g08_mean[];
extern const VerbQ g10_rows[];
extern const char *g10_pool[];
extern const char *g10_mean[];
extern const VerbQ g14_rows[];
extern const char *g14_pool[];
extern const char *g14_mean[];
extern const char *g14_gloss[];
extern const VerbQ g15_rows[];
extern const char *g15_pool[];
extern const char *g15_mean[];
extern const char *g15_gloss[];
extern const VerbQ g16_rows[];
extern const char *g16_pool[];
extern const char *g16_mean[];
extern const ChoiceQ g02_rows[];
extern const char *g02_mean[];
extern const ChoiceQ g03_rows[];
extern const char *g03_mean[];
extern const ChoiceQ g12_rows[];
extern const char *g12_mean[];
extern const OrdneRow g13_rows[];
extern const char *g13_fw_pool[];
extern const char *g13_ans_pool[];
extern const char *g13_mean[];
extern const FreeQ ex9_free_qs[];
extern const ProfileQ s01_rows[];
extern const char *hm_words[HM_WORDS];
extern const char *hm_tips[HM_WORDS];
extern const char *hm_letters[];
extern const gunichar hm_letters_u[];
extern const char *ex1_u3_pool[];
extern const U3Fill ex1_u3_rows[];
extern const char *ex2_u3_pool[];
extern const Ex2Row ex2_u3_rows2[];
extern const Ex2Row ex2_u3_rows3[];
extern const Ex2Row ex2_u3_rows4[];
extern const Ex2Item ex2_u3_items[];
extern const char *ex3_u3_pool[];
extern const U3Fill ex3_u3_rows[];
extern const Ex4Row ex4_u3_rows[];
extern const char *ex5_u3_bank;
extern const TypedQ ex5_u3_rows[];
extern const AssignItem ex6_u3_items[];
extern const char *ex6_u3_groups[];
extern const char *ex6_u3_meaning[];
extern const char *ex8_u3_pool[];
extern const U3Fill ex8_u3_rows[];
extern const char *ex8_u3_gloss[];
extern const char *ex9_u3_pool[];
extern const U3Fill ex9_u3_rows[];
extern const AssemblyItem ex10_u3_items[];
extern const char *ex10_u3_meaning[];
extern const U3Kw ex11_u3_qs[];
extern const char *ex12_u3_pool[];
extern const U3Fill ex12_u3_rows[];
extern const char *ex13_u3_pool[];
extern const U3Fill ex13_u3_rows[];
extern const U3Kw ex14_u3_qs[];
extern const U3Let ex15_u3_rows[];
extern NetLesson net_lessons[NET_LESSONS];
extern NetLesson *net_notes_target;
extern NetLesson hw_lessons[HW_LESSONS];
extern GtkWidget *hw_nodes[HW_UNITS];
extern GtkWidget *net_note_host;
extern int net_note_last_w;
extern GtkWidget *net_scroll;
extern GtkWidget *net_fixed;
extern GtkWidget *net_rail;
extern GtkWidget *net_nodes[NET_UNITS];
extern GtkWidget *net_labels[NET_UNITS];
extern double net_cx[NET_UNITS];
extern double net_cy[NET_UNITS];
extern int net_cols;
extern int net_rows;
extern int net_cw;
extern int net_ch;
extern guint net_idle;
extern double net_last_avail;
extern int net_last_cols;
extern int net_last_rows;
extern int net_last_cw;
extern int net_last_ch;
extern cairo_surface_t *net_rail_cache;
extern int net_cache_w;
extern int net_cache_h;
extern const NetTask net_tasks[];
extern const NetAns net_ans[4][4];

Rgb color_from_hex(unsigned int hex);
Rgb mix_rgb(Rgb a, Rgb b, double t);
ThemePalette theme_palette(ThemeId id, ColorMode mode);
void ex4_fill_sample(GtkWidget *label);
const char *tr(const char *key);
void i18n_ensure(void);
void i18n_apply_one(const I18nBind *b);
void i18n_bind(GtkWidget *widget, const char *key, int kind);
void refresh_welcome_heading(void);
void refresh_stats_ui(void);
void net_rail_theme_reset(void);
void apply_language(void);
char *read_css_file(void);
char *build_theme_css(const ThemePalette *p);
void save_settings(void);
void load_settings(void);
void force_restyle_tree(GtkWidget *w);
void apply_theme(void);
void draw_check_icon(GtkDrawingArea *area, cairo_t *cr,
                            int width, int height, gpointer data);
void lock_paint(cairo_t *cr, double x0, double y0, double size,
                       const Rgb *c);
void draw_lock_icon(GtkDrawingArea *area, cairo_t *cr,
                           int width, int height, gpointer data);
void draw_wifi_icon(GtkDrawingArea *area, cairo_t *cr,
                           int width, int height, gpointer data);
void draw_back_icon(GtkDrawingArea *area, cairo_t *cr,
                           int width, int height, gpointer data);
void draw_settings_icon(GtkDrawingArea *area, cairo_t *cr,
                               int width, int height, gpointer data);
void draw_stats_icon(GtkDrawingArea *area, cairo_t *cr,
                            int width, int height, gpointer data);
GtkWidget *icon_area_new(GtkDrawingAreaDrawFunc fn,
                                double r, double g, double b, int px);
char *normalize_answer(const char *input);
void shuffle_indices(int *arr, int n);
void flow_clear(GtkFlowBox *fb);
void answer_mark(GtkWidget *widget, gboolean ok);
void set_feedback(GtkWidget *label, gboolean ok, const char *text);
void set_feedback_warn(GtkWidget *label, const char *text);
gboolean text_has_german_umlaut(const char *s);
void add_umlaut_note(GtkWidget *body);
void unit_save_progress(UnitCtx *u);
void unit_load_progress(UnitCtx *u);
void load_progress(void);
void all_rails_redraw(void);
void refresh_completion_ui(void);
void mark_done(UnitCtx *u, int n);
void net_load_progress(void);
void net_save_progress(void);
void mark_net_done(int lesson_id);
void refresh_net_completion_ui(void);
void progress_for_net(ProgressSum *out);
void hw_load_progress(void);
void hw_save_progress(void);
void mark_hw_done(int lesson_id);
void refresh_hw_completion_ui(void);
void progress_for_hw(ProgressSum *out);
void hw_rail_theme_reset(void);
void czech_rail_theme_reset(void);
void hw_lessons_apply_lang(void);
GtkWidget *build_hwmap_page(void);
GtkWidget *build_czechmap_page(void);
GtkWidget *build_hw_unit1_page(void);
GtkWidget *build_hw_unit1_exercise_page(void);
GtkWidget *build_hw_unit2_page(void);
GtkWidget *build_hw_unit2_exercise_page(void);
GtkWidget *build_hw_unit3_page(void);
GtkWidget *build_hw_unit3_exercise_page(void);
void draw_chip_icon(GtkDrawingArea *area, cairo_t *cr,
                           int width, int height, gpointer data);
void draw_czech_flag(GtkDrawingArea *area, cairo_t *cr,
                            int width, int height, gpointer data);
void net_lesson_notes_ensure(NetLesson *L);
void net_rescale_lesson_notes(NetLesson *L, int body, int head, int kick);
GtkWidget *make_back_button(const char *target);
void on_nav_clicked(GtkButton *button, gpointer user_data);
int unit_done_count(const UnitCtx *u);
void progress_for_units(int first, int n, ProgressSum *out);
void refresh_stats_ui(void);
void on_stats_back_clicked(GtkButton *button, gpointer user_data);
void on_stats_clicked(GtkButton *button, gpointer user_data);
GtkWidget *stats_metric_card(const char *label_key, GtkWidget **value_out);
GtkWidget *make_unit_stats_row(UnitCtx *u, int num, int global);
GtkWidget *build_stats_page(void);
void on_nav_clicked(GtkButton *button, gpointer user_data);
void on_continue_clicked(GtkButton *button, gpointer user_data);
GtkWidget *make_back_button(const char *target);
GtkWidget *top_bar(const char *back_target, const char *title,
                          const char *subtitle);
gboolean on_window_key_pressed(GtkEventControllerKey *controller,
                                      guint keyval, guint keycode,
                                      GdkModifierType state,
                                      gpointer user_data);
GtkWidget *build_welcome_page(void);
void draw_node_halo(cairo_t *cr, double cx, double cy,
                           double radius, double r, double g, double b,
                           double alpha);
void draw_finish_cell(GtkDrawingArea *area, cairo_t *cr,
                             int width, int height, gpointer data);
void road_point(double t, double *ox, double *oy);
void road_path(cairo_t *cr, double t0, double t1);
void roadmap_geometry(double avail);
void road_apply_layout(void);
void road_relayout(void);
gboolean road_relayout_idle(gpointer data);
void road_relayout_later(void);
void road_adjust_notify(GtkAdjustment *adj, GParamSpec *ps,
                               gpointer data);
void draw_rail(GtkDrawingArea *area, cairo_t *cr,
                      int width, int height, gpointer user_data);
void add_path_node(GtkFixed *fixed, int index);
GtkWidget *build_roadmap_page(void);
void sub_layout_geometry(double avail);
void sub_point(double t, double *ox, double *oy);
void sub_path(cairo_t *cr, double t0, double t1);
void draw_sub_rail(GtkDrawingArea *area, cairo_t *cr,
                          int width, int height, gpointer user_data);
void sub_apply_layout(void);
void sub_relayout(void);
gboolean sub_relayout_idle(gpointer data);
void sub_relayout_later(void);
void sub_adjust_notify(GtkAdjustment *adj, GParamSpec *ps,
                              gpointer data);
void draw_german_flag(GtkDrawingArea *area, cairo_t *cr,
                             int width, int height, gpointer data);
GtkWidget *build_subjects_page(void);
void ex_layout_geometry(UnitCtx *u, double avail);
void ex_point(UnitCtx *u, double t, double *ox, double *oy);
void ex_path(UnitCtx *u, cairo_t *cr, double t0, double t1);
void draw_ex_rail(GtkDrawingArea *area, cairo_t *cr,
                         int width, int height, gpointer user_data);
void ex_apply_layout(UnitCtx *u);
void ex_relayout(UnitCtx *u);
gboolean ex_relayout_idle(gpointer data);
void ex_relayout_later(UnitCtx *u);
void ex_adjust_notify(GtkAdjustment *adj, GParamSpec *ps,
                             gpointer data);
GtkWidget *make_bubble(UnitCtx *u, int n);
GtkWidget *build_unit_page(UnitCtx *u);
GtkWidget *ex_page_shell(const char *back_target, const char *title,
                                const char *subtitle, const char *btn_label,
                                GtkWidget **body_out, GtkWidget **feedback_out,
                                GtkWidget **btn_out);
GtkWidget *meaning_add(GtkWidget *body, const char *text);
GtkWidget *model_answer_add(GtkWidget *body, const char *german);
void meaning_reveal_all(GtkWidget **labels, int n);
ComboListCtx *combo_list_ctx_new(UnitCtx *unit, int ex_num,
                                        GtkWidget *feedback);
void combo_list_add(ComboListCtx *ctx, GtkWidget *combo, const char *ans);
void combo_list_add_trans(ComboListCtx *ctx, GtkWidget *label);
GtkWidget *make_word_combo(const char **words, int n);
gboolean answer_accepts(const char *sel_norm, const char *ans);
gboolean answer_is_incomplete(const char *sel_norm, const char *ans);
void combo_list_check(GtkButton *button, gpointer data);
void sent_rebuild(SentBuilder *sb);
GdkContentProvider *asm_drag_prepare(GtkDragSource *source,
                                            double x, double y,
                                            gpointer data);
gboolean asm_rebuild_idle(gpointer data);
void asm_drag_end(GtkDragSource *source, GdkDrag *drag,
                         gboolean delete_data, gpointer data);
gboolean asm_drop_to_sentence(GtkDropTarget *target, const GValue *value,
                                     double x, double y, gpointer data);
gboolean asm_drop_to_pool(GtkDropTarget *target, const GValue *value,
                                 double x, double y, gpointer data);
void asm_finish_fly(FlyCtx *fc);
gboolean asm_fly_tick(gpointer data);
void asm_begin_fly(GtkWidget *chip, GtkWidget *stage,
                          gboolean *flying, double sx, double sy);
void asm_move_by_click(GtkButton *button, gpointer data);
void assembly_check(GtkButton *button, gpointer data);
GtkWidget *build_assembly(UnitCtx *unit, const char *title,
                                 const char *subtitle, int ex_num,
                                 const AssemblyItem *items,
                                 const char **meanings, int n);
GtkWidget *build_ex1(UnitCtx *unit);
void choice_check(GtkButton *button, gpointer data);
GtkWidget *build_choice(UnitCtx *unit, const char *title,
                               const char *subtitle, int ex_num,
                               const ChoiceQ *qs, const char **meanings, int n);
void ex4_fill_sample(GtkWidget *label);
void ex4_reveal(GtkButton *button, gpointer data);
void ex4_finish(GtkButton *button, gpointer data);
GtkWidget *build_free_answer(UnitCtx *unit, int ex_num, const char *title,
                                    const char *subtitle, const char *tip_key,
                                    const FreeQ *qs, int n);
GtkWidget *build_ex4(UnitCtx *unit);
GtkWidget *build_ex5(UnitCtx *unit);
GtkWidget *build_ex6(UnitCtx *unit);
GtkWidget *build_ex7(UnitCtx *unit);
char *verbq_german(const char *before, const char *answer,
                          const char *after);
GtkWidget *build_ex8(UnitCtx *unit);
GtkWidget *build_ex12(UnitCtx *unit);
GtkWidget *build_translate(UnitCtx *unit);
void assign_rebuild(AssignCtx *ac);
void assign_chip_clicked(GtkButton *button, gpointer data);
void assign_group_toggled(GtkToggleButton *button, gpointer data);
void assign_check(GtkButton *button, gpointer data);
GtkWidget *build_assign(UnitCtx *unit, const char *title,
                               const char *subtitle, int ex_num,
                               const AssignItem *items, int n_items,
                               const char **group_labels, int n_groups,
                               const char **meanings);
void verb_rows_add(ComboListCtx *ctx, GtkWidget *body, int *counter,
                          const VerbQ *qs, int n,
                          const char **pool, int pool_n,
                          const char **meanings, gboolean pre_meaning);
void verb_note_reveal(VerbNoteCtx *nc);
void verb_ex_check(GtkButton *button, gpointer data);
GtkWidget *build_verb_ex(UnitCtx *unit, int ex_num,
                                const char *title, const char *sub,
                                const VerbQ *qs, int n,
                                const char **pool, int pool_n,
                                const char **meanings, gboolean pre_meaning,
                                const char **gloss);
void typed_check(GtkButton *button, gpointer data);
char *typed_answer_display(const char *answers);
void typed_note_reveal(TypedNoteCtx *nc);
void typed_ex_check(GtkButton *button, gpointer data);
GtkWidget *build_typed(UnitCtx *unit, int ex_num,
                              const char *title, const char *sub,
                              const TypedQ *qs, int n,
                              const char *word_bank, gboolean answers_note);
void s01_fill_sample(GtkWidget *label);
void s01_reveal(GtkButton *button, gpointer data);
void s01_finish(GtkButton *button, gpointer data);
GtkWidget *build_profile(UnitCtx *unit, int ex_num, const char *title,
                                const char *sub, const ProfileQ *qs, int n);
int hm_char_index(gunichar ch);
void hm_update_slots(HangmanCtx *hm);
gboolean hm_word_solved(HangmanCtx *hm);
void hm_set_letters_enabled(HangmanCtx *hm, gboolean enabled);
void hm_redraw(HangmanCtx *hm);
void hm_clear_feedback(HangmanCtx *hm);
gboolean hm_advance(gpointer data);
void hm_reset_word(gpointer data);
gboolean hm_retry_word(gpointer data);
void hm_guess_letter(GtkButton *button, gpointer data);
void draw_hangman(GtkDrawingArea *area, cairo_t *cr,
                         int width, int height, gpointer data);
GtkWidget *build_hangman(UnitCtx *unit, int ex_num, const char *title,
                                const char *sub);
GtkWidget *u2ex1(UnitCtx *unit);
GtkWidget *u2ex2(UnitCtx *unit);
GtkWidget *u2ex3(UnitCtx *unit);
GtkWidget *u2ex4(UnitCtx *unit);
GtkWidget *u2ex5(UnitCtx *unit);
GtkWidget *u2ex6(UnitCtx *unit);
GtkWidget *u2ex7(UnitCtx *unit);
GtkWidget *u2ex8(UnitCtx *unit);
GtkWidget *u2ex9(UnitCtx *unit);
GtkWidget *u2ex10(UnitCtx *unit);
GtkWidget *u2ex11(UnitCtx *unit);
GtkWidget *u2ex12(UnitCtx *unit);
GtkWidget *u2ex13(UnitCtx *unit);
GtkWidget *u2ex14(UnitCtx *unit);
GtkWidget *u2ex15(UnitCtx *unit);
GtkWidget *u2ex16(UnitCtx *unit);
GtkWidget *u2ex17(UnitCtx *unit);
GtkWidget *u2ex18(UnitCtx *unit);
GtkWidget *u2ex19(UnitCtx *unit);
char *u3_fill_sentence(const U3Fill *f);
void u3_fill_row(ComboListCtx *ctx, GtkWidget *body, const U3Fill *f,
                        const char **pool, int pool_n, const char *gloss);
void u3_sample_line(GtkWidget *body, const char *text);
GtkWidget *u3_build_drop(UnitCtx *unit, int ex_num, const char *title,
                                const char *sub, const char *sample,
                                const U3Fill *rows, int n,
                                const char **pool, int pool_n,
                                const char **glosses, gboolean show_german);
GtkWidget *u3ex1(UnitCtx *unit);
const char *ex2_word_cs(const char *w);
void ex2_check(GtkButton *button, gpointer data);
GtkWidget *u3ex2(UnitCtx *unit);
GtkWidget *u3ex3(UnitCtx *unit);
U3LetCtx *u3_let_ctx_new(UnitCtx *unit, int ex_num,
                                GtkWidget *feedback);
char *u3_let_sentence(const U3Let *row);
void u3_let_check(GtkButton *button, gpointer data);
void u3_let_row(U3LetCtx *ctx, GtkWidget *body, const U3Let *row);
GtkWidget *u3_build_letters(UnitCtx *unit, int ex_num,
                                   const char *title, const char *sub,
                                   const U3Let *rows, int n);
void ex4_check(GtkButton *button, gpointer data);
GtkWidget *u3ex4(UnitCtx *unit);
GtkWidget *u3ex5(UnitCtx *unit);
GtkWidget *u3ex6(UnitCtx *unit);
void u3_table_check(GtkButton *button, gpointer data);
GtkWidget *u3ex7(UnitCtx *unit);
GtkWidget *u3ex8(UnitCtx *unit);
GtkWidget *u3ex9(UnitCtx *unit);
GtkWidget *u3ex10(UnitCtx *unit);
void u3_kw_check(GtkButton *button, gpointer data);
GtkWidget *u3_build_kw(UnitCtx *unit, int ex_num, const char *title,
                              const char *sub, const char *sample,
                              const U3Kw *qs, int n);
GtkWidget *u3ex11(UnitCtx *unit);
GtkWidget *u3ex12(UnitCtx *unit);
GtkWidget *u3ex13(UnitCtx *unit);
GtkWidget *u3ex14(UnitCtx *unit);
GtkWidget *u3ex15(UnitCtx *unit);
void draw_theme_swatch(GtkDrawingArea *area, cairo_t *cr,
                              int width, int height, gpointer data);
void on_mode_toggled(GtkToggleButton *btn, gpointer user_data);
void on_theme_toggled(GtkToggleButton *btn, gpointer user_data);
void on_lang_toggled(GtkToggleButton *btn, gpointer user_data);
GtkWidget *make_lang_chip(const char *label, UiLang lang,
                                 GtkToggleButton *group);
GtkWidget *make_mode_chip(const char *label, ColorMode mode,
                                 GtkToggleButton *group);
GtkWidget *make_theme_card(ThemeId id, GtkToggleButton *group);
void on_settings_clicked(GtkButton *button, gpointer user_data);
GtkWidget *build_stats_button(void);
GtkWidget *build_settings_button(void);
void unit_meta_init(void);
void unit_configure(int idx, const char *const *names, int n);
GtkWidget *build_exercise_page(UnitCtx *u, int n);
void net_paragraph(GtkWidget *box, const char *text);
void net_heading(GtkWidget *box, const char *text);
void net_note_font(GtkWidget *l, int px);
void net_rescale_notes_at(int w);
void net_rescale_notes(void);
gboolean net_note_tick(GtkWidget *w, GdkFrameClock *clock,
                              gpointer data);
GtkWidget *net_slide_card(const char *cls);
void net_note_kicker(GtkWidget *box, const char *text);
void net_note_title(GtkWidget *box, const char *text);
void net_note_line(GtkWidget *box, const char *text, gboolean tip);
void net_slide_apply(NetLesson *L);
void net_lessons_apply_lang(void);
void net_open_unit(GtkButton *button, gpointer data);
void net_slide_prev(GtkButton *button, gpointer data);
void net_slide_next(GtkButton *button, gpointer data);
void net_point(double t, double *ox, double *oy);
void net_path(cairo_t *cr, double t0, double t1);
void net_geometry(double avail);
void net_apply_layout(void);
void net_relayout(void);
gboolean net_relayout_idle(gpointer data);
void net_relayout_later(void);
void net_adjust_notify(GtkAdjustment *adj, GParamSpec *ps,
                              gpointer data);
void net_render_rail_to(cairo_t *cr);
void net_draw_rail(GtkDrawingArea *area, cairo_t *cr,
                          int width, int height, gpointer data);
void net_rail_theme_reset(void);
void net_add_node(GtkFixed *fixed, int index);
GtkWidget *build_netmap_page(void);
GtkWidget *build_net_unit_page(NetLesson *L, const char *title_key,
                               const char *sub_key, const NetSlide *slides,
                               guint n_slides);
GtkWidget *build_net_unit1_page(void);
GtkWidget *build_net_unit2_page(void);
GtkWidget *build_net_unit3_page(void);
GtkWidget *build_net_unit4_page(void);
GtkWidget *build_net_unit5_page(void);
GtkWidget *build_net_unit6_page(void);
GtkWidget *build_net_unit7_page(void);
GtkWidget *build_net_unit8_page(void);
GtkWidget *build_net_unit9_page(void);
GtkWidget *build_net_unit10_page(void);
GtkWidget *build_net_unit11_page(void);
GtkWidget *build_net_unit12_page(void);
GtkWidget *build_net_unit13_page(void);
GtkWidget *build_net_unit14_page(void);
GtkWidget *build_net_unit15_page(void);
GtkWidget *build_net_unit16_page(void);
GtkWidget *build_net_unit17_page(void);
GtkWidget *build_net_unit18_page(void);
GtkWidget *build_net_unit19_page(void);
GtkWidget *build_net_unit20_page(void);
GtkWidget *build_net_unit21_page(void);
GtkWidget *build_net_unit22_page(void);
GtkWidget *net_qz_combo(void);
void net_qz_set_prefix(GtkComboBoxText *c, int pfx);
int net_qz_prefix(GtkComboBoxText *c);
gboolean net_txt_eq(const char *a, const char *b);
gboolean net_qz_entry_ok(GtkWidget *entry, const char *want);
void net_qz_check(GtkButton *button, gpointer data);
void net_qz_fill(GtkButton *button, gpointer data);
void net_qz_header(GtkWidget *box, const char *text);
GtkWidget *net_qz_make_entry(void);
GtkWidget *build_net_exercise_page(void);
void net_mcq_check(GtkButton *button, gpointer data);
GtkWidget *build_net_mcq_page(int lesson_id, const char *back_page,
                              const char *title_key, const char *heading_key,
                              const ChoiceQ *qs, const char **hints, int n);
GtkWidget *build_net_unit2_exercise_page(void);
GtkWidget *build_net_unit3_exercise_page(void);
GtkWidget *build_net_unit4_exercise_page(void);
GtkWidget *build_net_unit5_exercise_page(void);
GtkWidget *build_net_unit6_exercise_page(void);
GtkWidget *build_net_unit7_exercise_page(void);
GtkWidget *build_net_unit8_exercise_page(void);
GtkWidget *build_net_unit9_exercise_page(void);
GtkWidget *build_net_unit10_exercise_page(void);
GtkWidget *build_net_unit11_exercise_page(void);
GtkWidget *build_net_unit12_exercise_page(void);
GtkWidget *build_net_unit13_exercise_page(void);
GtkWidget *build_net_unit14_exercise_page(void);
GtkWidget *build_net_unit15_exercise_page(void);
GtkWidget *build_net_unit16_exercise_page(void);
GtkWidget *build_net_unit17_exercise_page(void);
GtkWidget *build_net_unit18_exercise_page(void);
GtkWidget *build_net_unit19_exercise_page(void);
GtkWidget *build_net_unit20_exercise_page(void);
GtkWidget *build_net_unit21_exercise_page(void);
GtkWidget *build_net_unit22_exercise_page(void);
void activate(GtkApplication *app, gpointer user_data);
int main(int argc, char **argv);

#endif /* MATURITA_H */
