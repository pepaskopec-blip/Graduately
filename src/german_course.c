#include "graduately.h"

/* Skill course next to the three textbook units. Four years, same
 * listening / reading / writing player as English. */

#define DE_YEARS 4
#define DE_N     6

#include "german_lessons.inc"

static NetLesson de_rt[DE_YEARS][DE_N];
static char de_map_page[DE_YEARS][16];
static char de_unit_page[DE_YEARS][DE_N][16];
static char de_ex_page[DE_YEARS][DE_N][16];
static char de_title_key[DE_YEARS][DE_N][16];
static char de_sub_key[DE_YEARS][DE_N][20];
static gboolean de_ready;

static void de_prepare(void) {
    if (de_ready)
        return;
    de_ready = TRUE;
    for (int y = 0; y < DE_YEARS; y++) {
        g_snprintf(de_map_page[y], sizeof de_map_page[y], "de%dmap", y + 1);
        for (int i = 0; i < DE_N; i++) {
            g_snprintf(de_unit_page[y][i], sizeof de_unit_page[y][i],
                       "de%dunit%d", y + 1, i + 1);
            g_snprintf(de_ex_page[y][i], sizeof de_ex_page[y][i],
                       "de%dex%d", y + 1, i + 1);
            g_snprintf(de_title_key[y][i], sizeof de_title_key[y][i],
                       "de_y%d_u%d", y + 1, i + 1);
            g_snprintf(de_sub_key[y][i], sizeof de_sub_key[y][i],
                       "de_y%d_u%d_sub", y + 1, i + 1);
            de_rt[y][i].unit_page = de_unit_page[y][i];
            de_rt[y][i].ex_page = de_ex_page[y][i];
            de_rt[y][i].n_slides = (guint)de_lessons[y][i].n_slides;
        }
    }
}

static void de_save_progress(void) {
    GKeyFile *kf = g_key_file_new();
    gchar *data;
    GError *err = NULL;

    for (int y = 0; y < DE_YEARS; y++) {
        for (int i = 0; i < DE_N; i++) {
            char key[8];

            g_snprintf(key, sizeof key, "%d-%d", y + 1, i + 1);
            g_key_file_set_boolean(kf, "done", key, de_rt[y][i].done);
        }
    }
    data = g_key_file_to_data(kf, NULL, NULL);
    if (!g_file_set_contents(PROGRESS_DE, data, -1, &err)) {
        g_warning("german course: %s", err->message);
        g_error_free(err);
    }
    g_free(data);
    g_key_file_free(kf);
}

void de_load_progress(void) {
    GKeyFile *kf = g_key_file_new();
    GError *err = NULL;

    de_prepare();
    if (!g_key_file_load_from_file(kf, PROGRESS_DE, G_KEY_FILE_NONE, &err)) {
        if (err)
            g_error_free(err);
        g_key_file_free(kf);
        return;
    }
    for (int y = 0; y < DE_YEARS; y++) {
        for (int i = 0; i < DE_N; i++) {
            char key[8];

            g_snprintf(key, sizeof key, "%d-%d", y + 1, i + 1);
            de_rt[y][i].done = g_key_file_get_boolean(kf, "done", key, NULL);
        }
    }
    g_key_file_free(kf);
}

static void mark_de_done(int year, int index) {
    if (year < 0 || year >= DE_YEARS || index < 0 || index >= DE_N)
        return;
    if (de_rt[year][index].done)
        return;
    de_rt[year][index].done = TRUE;
    de_save_progress();
    refresh_stats_ui();
}

void progress_for_de(ProgressSum *out) {
    out->total_ex += DE_YEARS * DE_N;
    out->open_units += DE_YEARS * DE_N;
    for (int y = 0; y < DE_YEARS; y++) {
        for (int i = 0; i < DE_N; i++) {
            if (de_rt[y][i].done) {
                out->done_ex++;
                out->done_units++;
            }
        }
    }
}

void de_lessons_apply_lang(void) {
    for (int y = 0; y < DE_YEARS; y++)
        for (int i = 0; i < DE_N; i++)
            skill_slide_apply(&de_rt[y][i]);
}

static GtkWidget *de_nav_button(const char *title_key, const char *sub_key,
                                const char *target, const char *badge) {
    GtkWidget *btn = gtk_button_new();
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 14);
    GtkWidget *num = gtk_label_new(badge);
    GtkWidget *texts = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    GtkWidget *title = gtk_label_new(NULL);
    GtkWidget *sub = gtk_label_new(NULL);

    gtk_widget_add_css_class(btn, "unit-node");
    gtk_widget_add_css_class(btn, "current");
    gtk_widget_set_hexpand(btn, TRUE);
    gtk_widget_set_margin_start(row, 18);
    gtk_widget_set_margin_end(row, 18);
    gtk_widget_set_margin_top(row, 10);
    gtk_widget_set_margin_bottom(row, 10);
    gtk_button_set_child(GTK_BUTTON(btn), row);
    gtk_widget_add_css_class(num, "unit-number");
    gtk_widget_set_valign(num, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(row), num);
    gtk_widget_set_hexpand(texts, TRUE);
    gtk_box_append(GTK_BOX(row), texts);
    i18n_bind(title, title_key, 0);
    gtk_widget_set_halign(title, GTK_ALIGN_START);
    gtk_widget_add_css_class(title, "unit-name");
    gtk_box_append(GTK_BOX(texts), title);
    i18n_bind(sub, sub_key, 0);
    gtk_widget_set_halign(sub, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(sub), TRUE);
    gtk_label_set_xalign(GTK_LABEL(sub), 0);
    gtk_widget_add_css_class(sub, "quiz-intro");
    gtk_box_append(GTK_BOX(texts), sub);
    g_object_set_data_full(G_OBJECT(btn), "target", g_strdup(target), g_free);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_nav_clicked), NULL);
    return btn;
}

static GtkWidget *de_list_page(const char *back, const char *title,
                               const char *sub, GtkWidget *list) {
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *scroll = gtk_scrolled_window_new();

    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);
    gtk_box_append(GTK_BOX(page), top_bar(back, title, sub));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 18);
    gtk_box_append(GTK_BOX(page), scroll);
    gtk_widget_set_halign(list, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(list, 520, -1);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list);
    return page;
}

GtkWidget *build_dehome_page(void) {
    GtkWidget *list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    static const char *years[] = {
        "de_year1", "de_year2", "de_year3", "de_year4",
    };
    static const char *subs[] = {
        "de_y1_sub", "de_y2_sub", "de_y3_sub", "de_y4_sub",
    };

    de_prepare();
    gtk_box_append(GTK_BOX(list),
                   de_nav_button("de_book_title", "de_book_sub", "roadmap", "U"));
    for (int y = 0; y < DE_YEARS; y++) {
        char badge[8];

        g_snprintf(badge, sizeof badge, "%d", y + 1);
        gtk_box_append(GTK_BOX(list),
                       de_nav_button(years[y], subs[y], de_map_page[y], badge));
    }
    return de_list_page("subjects", "Deutsch", "de_home_sub", list);
}

static GtkWidget *build_de_map(int year) {
    GtkWidget *list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    static const char *year_keys[] = {
        "de_year1", "de_year2", "de_year3", "de_year4",
    };
    static const char *sub_keys[] = {
        "de_y1_sub", "de_y2_sub", "de_y3_sub", "de_y4_sub",
    };

    de_prepare();
    for (int i = 0; i < DE_N; i++) {
        char badge[8];

        g_snprintf(badge, sizeof badge, "%d", i + 1);
        gtk_box_append(GTK_BOX(list),
                       de_nav_button(de_title_key[year][i],
                                     de_sub_key[year][i],
                                     de_unit_page[year][i], badge));
    }
    return de_list_page("dehome", year_keys[year], sub_keys[year], list);
}

void add_de_pages(GtkStack *stack) {
    de_prepare();
    for (int y = 0; y < DE_YEARS; y++) {
        gtk_stack_add_named(stack, build_de_map(y), de_map_page[y]);
        for (int i = 0; i < DE_N; i++) {
            gtk_stack_add_named(stack,
                skill_unit_page(&de_rt[y][i], &de_lessons[y][i],
                                de_map_page[y], de_title_key[y][i],
                                de_sub_key[y][i]),
                de_unit_page[y][i]);
            gtk_stack_add_named(stack,
                skill_ex_page(&de_lessons[y][i], y, i, mark_de_done,
                              de_unit_page[y][i], de_title_key[y][i], "de"),
                de_ex_page[y][i]);
        }
    }
}
