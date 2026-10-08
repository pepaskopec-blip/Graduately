#include "graduately.h"

/* Later years of networks and hardware. Year 1 keeps its own path.
 * These years are a list of lessons, slides and a four-question quiz. */

#define QY_N 8
#define QY_COUNT 6

struct QuizYear {
    const char *prefix;
    const char *map_page;
    const char *back_page;
    const char *title_key;
    const char *sub_key;
    const char *file;
    NetLesson lessons[QY_N];
    char unit_page[QY_N][16];
    char ex_page[QY_N][16];
    char unit_key[QY_N][20];
    char sub_keybuf[QY_N][24];
    GtkWidget *nodes[QY_N];
};

static QuizYear years[QY_COUNT];
static gboolean qy_ready;

QuizYear *qy_net2;
QuizYear *qy_net3;
QuizYear *qy_net4;
QuizYear *qy_hw2;
QuizYear *qy_hw3;
QuizYear *qy_hw4;

static char map_store[QY_COUNT][16];
static char file_store[QY_COUNT][16];

void quizyear_boot(void) {
    static const char *prefix[] = {
        "net2", "net3", "net4", "hw2", "hw3", "hw4",
    };
    static const char *back[] = {
        "netyears", "netyears", "netyears",
        "hwyears", "hwyears", "hwyears",
    };
    static const char *title[] = {
        "net_year2", "net_year3", "net_year4",
        "hw_year2", "hw_year3", "hw_year4",
    };
    static const char *sub[] = {
        "net2_sub", "net3_sub", "net4_sub",
        "hw2_sub", "hw3_sub", "hw4_sub",
    };
    QuizYear **slot[] = {
        &qy_net2, &qy_net3, &qy_net4, &qy_hw2, &qy_hw3, &qy_hw4,
    };

    if (qy_ready)
        return;
    for (int y = 0; y < QY_COUNT; y++) {
        QuizYear *Y = &years[y];

        Y->prefix = prefix[y];
        Y->back_page = back[y];
        Y->title_key = title[y];
        Y->sub_key = sub[y];
        g_snprintf(map_store[y], sizeof map_store[y], "%smap", prefix[y]);
        g_snprintf(file_store[y], sizeof file_store[y], "%s.conf", prefix[y]);
        Y->map_page = map_store[y];
        Y->file = file_store[y];
        for (int i = 0; i < QY_N; i++) {
            g_snprintf(Y->unit_page[i], sizeof Y->unit_page[i],
                       "%sunit%d", prefix[y], i + 1);
            g_snprintf(Y->ex_page[i], sizeof Y->ex_page[i],
                       "%sex%d", prefix[y], i + 1);
            g_snprintf(Y->unit_key[i], sizeof Y->unit_key[i],
                       "%s_unit%d", prefix[y], i + 1);
            g_snprintf(Y->sub_keybuf[i], sizeof Y->sub_keybuf[i],
                       "%s_unit%d_sub", prefix[y], i + 1);
            Y->lessons[i].unit_page = Y->unit_page[i];
            Y->lessons[i].ex_page = Y->ex_page[i];
            Y->lessons[i].n_slides = 3;
        }
        *slot[y] = Y;
    }
    qy_ready = TRUE;
}

static char *qy_path(QuizYear *Y) {
    return g_build_filename(app_progress_dir, Y->file, NULL);
}

static void qy_save(QuizYear *Y) {
    GKeyFile *kf = g_key_file_new();
    char *path = qy_path(Y);
    gchar *data;
    GError *err = NULL;

    for (int i = 0; i < QY_N; i++) {
        char key[8];

        g_snprintf(key, sizeof key, "%d", i + 1);
        g_key_file_set_boolean(kf, "done", key, Y->lessons[i].done);
    }
    data = g_key_file_to_data(kf, NULL, NULL);
    if (!g_file_set_contents(path, data, -1, &err)) {
        g_warning("quiz year: %s", err->message);
        g_error_free(err);
    }
    g_free(data);
    g_free(path);
    g_key_file_free(kf);
}

static void qy_load_one(QuizYear *Y) {
    GKeyFile *kf = g_key_file_new();
    char *path = qy_path(Y);
    GError *err = NULL;

    if (!g_key_file_load_from_file(kf, path, G_KEY_FILE_NONE, &err)) {
        if (err)
            g_error_free(err);
        g_key_file_free(kf);
        g_free(path);
        return;
    }
    for (int i = 0; i < QY_N; i++) {
        char key[8];

        g_snprintf(key, sizeof key, "%d", i + 1);
        Y->lessons[i].done = g_key_file_get_boolean(kf, "done", key, NULL);
    }
    g_key_file_free(kf);
    g_free(path);
}

void quizyear_load_all(void) {
    quizyear_boot();
    for (int y = 0; y < QY_COUNT; y++)
        qy_load_one(&years[y]);
}

static void qy_refresh_node(QuizYear *Y, int index) {
    GtkWidget *node = Y->nodes[index];

    if (!node)
        return;
    if (Y->lessons[index].done) {
        gtk_widget_remove_css_class(node, "current");
        gtk_widget_add_css_class(node, "done");
    } else {
        gtk_widget_remove_css_class(node, "done");
        gtk_widget_add_css_class(node, "current");
    }
}

static void qy_mark(QuizYear *Y, int index) {
    if (index < 0 || index >= QY_N || Y->lessons[index].done)
        return;
    Y->lessons[index].done = TRUE;
    qy_save(Y);
    qy_refresh_node(Y, index);
    refresh_stats_ui();
}

static void qy_add(ProgressSum *out, int first, int n) {
    for (int y = first; y < first + n; y++) {
        out->total_ex += QY_N;
        out->open_units += QY_N;
        for (int i = 0; i < QY_N; i++) {
            if (years[y].lessons[i].done) {
                out->done_ex++;
                out->done_units++;
            }
        }
    }
}

void progress_for_net_years(ProgressSum *out) {
    quizyear_boot();
    qy_add(out, 0, 3);
}

void progress_for_hw_years(ProgressSum *out) {
    quizyear_boot();
    qy_add(out, 3, 3);
}

static void qy_slide_prev(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (L && L->idx > 0) {
        L->idx--;
        skill_slide_apply(L);
    }
}

static void qy_slide_next(GtkButton *button, gpointer data) {
    NetLesson *L = data;

    (void)button;
    if (!L)
        return;
    if (L->idx + 1 < L->n_slides) {
        L->idx++;
        skill_slide_apply(L);
    } else if (L->ex_page) {
        show_page(L->ex_page, 1);
    }
}

void quizyear_apply_lang(void) {
    if (!qy_ready)
        return;
    for (int y = 0; y < QY_COUNT; y++)
        for (int i = 0; i < QY_N; i++)
            skill_slide_apply(&years[y].lessons[i]);
}

GtkWidget *quizyear_unit(QuizYear *Y, int index, const char *title_key,
                         const char *sub_key, const NetSlide *slides,
                         int n_slides) {
    NetLesson *L = &Y->lessons[index];
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *scroll = gtk_scrolled_window_new();
    GtkWidget *nav;

    net_lesson_notes_ensure(L);
    net_notes_target = L;
    L->n_slides = (guint)n_slides;
    gtk_widget_set_margin_start(page, 28);
    gtk_widget_set_margin_end(page, 28);
    gtk_widget_set_margin_top(page, 20);
    gtk_widget_set_margin_bottom(page, 20);
    gtk_box_append(GTK_BOX(page), top_bar(Y->map_page, title_key, sub_key));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 10);
    gtk_box_append(GTK_BOX(page), scroll);
    L->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(L->stack),
                                  GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), L->stack);
    for (int s = 0; s < n_slides; s++) {
        GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
        char name[16];

        g_snprintf(name, sizeof name, "slide%d", s);
        gtk_widget_set_margin_top(card, 8);
        gtk_widget_set_margin_bottom(card, 8);
        net_note_kicker(card, slides[s].kicker);
        net_note_title(card, slides[s].title);
        for (int i = 0; i < 8 && slides[s].line[i]; i++)
            net_note_line(card, slides[s].line[i], FALSE);
        if (slides[s].tip)
            net_note_line(card, slides[s].tip, TRUE);
        gtk_stack_add_named(GTK_STACK(L->stack), card, name);
    }
    nav = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(nav), TRUE);
    gtk_widget_set_margin_top(nav, 10);
    gtk_box_append(GTK_BOX(page), nav);
    L->prev_btn = gtk_button_new_with_label(tr("net_slide_prev"));
    gtk_widget_add_css_class(L->prev_btn, "btn-primary");
    gtk_box_append(GTK_BOX(nav), L->prev_btn);
    g_signal_connect(L->prev_btn, "clicked", G_CALLBACK(qy_slide_prev), L);
    L->next_btn = gtk_button_new_with_label(tr("net_slide_next"));
    gtk_widget_add_css_class(L->next_btn, "btn-primary");
    gtk_box_append(GTK_BOX(nav), L->next_btn);
    g_signal_connect(L->next_btn, "clicked", G_CALLBACK(qy_slide_next), L);
    net_rescale_lesson_notes(L, 18, 32, 15);
    net_notes_target = NULL;
    L->idx = 0;
    skill_slide_apply(L);
    return page;
}

typedef struct {
    QuizYear *year;
    int index;
    const ChoiceQ *qs;
    int n;
    int n_opts;
    GtkWidget *feedback;
    GtkToggleButton **toggles;
    GtkWidget **hints;
} QyQuiz;

static void qy_check(GtkButton *button, gpointer data) {
    QyQuiz *ctx = data;
    int ok = 0;

    (void)button;
    for (int i = 0; i < ctx->n; i++) {
        int active = -1;

        for (int o = 0; o < ctx->n_opts; o++) {
            GtkToggleButton *tb = ctx->toggles[i * ctx->n_opts + o];

            gtk_widget_remove_css_class(GTK_WIDGET(tb), "ok");
            gtk_widget_remove_css_class(GTK_WIDGET(tb), "wrong");
            if (gtk_toggle_button_get_active(tb))
                active = o;
        }
        if (active == ctx->qs[i].correct) {
            ok++;
            if (active >= 0)
                gtk_widget_add_css_class(
                    GTK_WIDGET(ctx->toggles[i * ctx->n_opts + active]), "ok");
        } else if (active >= 0) {
            gtk_widget_add_css_class(
                GTK_WIDGET(ctx->toggles[i * ctx->n_opts + active]), "wrong");
        }
        if (ctx->hints && ctx->hints[i])
            gtk_widget_set_visible(ctx->hints[i], TRUE);
    }
    if (ok == ctx->n) {
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        qy_mark(ctx->year, ctx->index);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry_short"));
    }
}

GtkWidget *quizyear_quiz(QuizYear *Y, int index, const char *title_key,
                         const char *head_key, const ChoiceQ *qs,
                         const char **hints, int n) {
    QyQuiz *ctx = g_new0(QyQuiz, 1);
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *scroll = gtk_scrolled_window_new();
    GtkWidget *body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    GtkWidget *head = gtk_label_new(NULL);
    GtkWidget *intro = gtk_label_new(NULL);
    GtkWidget *check;
    int n_opts = qs[0].n_options;

    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);
    gtk_box_append(GTK_BOX(page),
                   top_bar(Y->lessons[index].unit_page, title_key, NULL));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 12);
    gtk_box_append(GTK_BOX(page), scroll);
    gtk_widget_set_hexpand(body, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), body);
    i18n_bind(head, head_key, 0);
    gtk_widget_set_halign(head, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(head), TRUE);
    gtk_widget_add_css_class(head, "quiz-heading");
    gtk_box_append(GTK_BOX(body), head);
    i18n_bind(intro, "net_quiz_intro", 0);
    gtk_widget_set_halign(intro, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(intro), TRUE);
    gtk_widget_add_css_class(intro, "quiz-intro");
    gtk_box_append(GTK_BOX(body), intro);

    ctx->year = Y;
    ctx->index = index;
    ctx->qs = qs;
    ctx->n = n;
    ctx->n_opts = n_opts;
    ctx->toggles = g_new0(GtkToggleButton *, n * n_opts);
    ctx->hints = g_new0(GtkWidget *, n);
    for (int i = 0; i < n; i++) {
        GtkWidget *card = mcq_append_question(body, i + 1, &qs[i],
                                              &ctx->toggles[i * n_opts]);

        ctx->hints[i] = meaning_add(card, hints ? hints[i] : NULL);
    }
    check = gtk_button_new();
    i18n_bind(check, "check", 1);
    gtk_widget_add_css_class(check, "btn-primary");
    gtk_widget_set_halign(check, GTK_ALIGN_START);
    gtk_widget_set_margin_top(check, 8);
    gtk_box_append(GTK_BOX(body), check);
    g_signal_connect(check, "clicked", G_CALLBACK(qy_check), ctx);
    ctx->feedback = gtk_label_new("");
    gtk_widget_set_halign(ctx->feedback, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(ctx->feedback), TRUE);
    gtk_box_append(GTK_BOX(body), ctx->feedback);
    return page;
}

GtkWidget *quizyear_map(QuizYear *Y) {
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *scroll = gtk_scrolled_window_new();
    GtkWidget *list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);

    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);
    gtk_box_append(GTK_BOX(page), top_bar(Y->back_page, Y->title_key, Y->sub_key));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 18);
    gtk_box_append(GTK_BOX(page), scroll);
    gtk_widget_set_halign(list, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(list, 520, -1);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list);
    for (int i = 0; i < QY_N; i++) {
        GtkWidget *btn = gtk_button_new();
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 14);
        GtkWidget *texts = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        GtkWidget *title = gtk_label_new(NULL);
        GtkWidget *sub = gtk_label_new(NULL);
        char badge[8];
        GtkWidget *num;

        g_snprintf(badge, sizeof badge, "%d", i + 1);
        num = gtk_label_new(badge);
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
        i18n_bind(title, Y->unit_key[i], 0);
        gtk_widget_set_halign(title, GTK_ALIGN_START);
        gtk_widget_add_css_class(title, "unit-name");
        gtk_box_append(GTK_BOX(texts), title);
        i18n_bind(sub, Y->sub_keybuf[i], 0);
        gtk_widget_set_halign(sub, GTK_ALIGN_START);
        gtk_label_set_wrap(GTK_LABEL(sub), TRUE);
        gtk_label_set_xalign(GTK_LABEL(sub), 0);
        gtk_widget_add_css_class(sub, "quiz-intro");
        gtk_box_append(GTK_BOX(texts), sub);
        g_object_set_data_full(G_OBJECT(btn), "target",
                               g_strdup(Y->unit_page[i]), g_free);
        g_signal_connect(btn, "clicked", G_CALLBACK(on_nav_clicked), NULL);
        Y->nodes[i] = btn;
        qy_refresh_node(Y, i);
        gtk_box_append(GTK_BOX(list), btn);
    }
    return page;
}
