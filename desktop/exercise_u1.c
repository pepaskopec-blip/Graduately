#include "maturita.h"

/* ------------------------------------------------------------------ */
/* Choice exercises (3 + 9)                                           */
/* ------------------------------------------------------------------ */


const ChoiceQ ex3_questions[] = {
    {"Welche Zahl ist „vierzehn“?", {"4", "14", "40"}, 3, 1},
    {"Welche Zahl ist „siebzehn“?", {"7", "17", "70"}, 3, 1},
    {"Welche Zahl ist „zwanzig“?", {"2", "12", "20"}, 3, 2},
    {"Welche Zahl ist „sechs“?", {"6", "16", "60"}, 3, 0},
};

const char *ex3_meaning[] = {
    "„vierzehn“ = čtrnáct (14)",
    "„siebzehn“ = sedmnáct (17)",
    "„zwanzig“ = dvacet (20)",
    "„sechs“ = šest (6)",
};

const ChoiceQ ex9_questions[] = {
    {"___ heißt du?", {"Wer", "Wie", "Wo"}, 3, 1},
    {"___ kommst du?", {"Woher", "Wie", "Wer"}, 3, 0},
    {"___ wohnst du?", {"Wo", "Wer", "Was"}, 3, 0},
    {"___ ist das?", {"Wer", "Wo", "Wie"}, 3, 0},
    {"___ alt bist du?", {"Wie", "Woher", "Was"}, 3, 0},
};

const char *ex9_meaning[] = {
    "Jak se jmenuješ?",
    "Odkud jsi?",
    "Kde bydlíš?",
    "Kdo to je?",
    "Kolik je ti let?",
};


void choice_check(GtkButton *button, gpointer data) {
    ChoiceCtx *ctx = data;
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
            gtk_widget_add_css_class(GTK_WIDGET(ctx->toggles[i * ctx->n_opts + active]), "ok");
        } else if (active >= 0) {
            gtk_widget_add_css_class(GTK_WIDGET(ctx->toggles[i * ctx->n_opts + active]), "wrong");
        }
    }

    if (ok == ctx->n) {
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        mark_done(ctx->unit, ctx->ex_num);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry_short"));
    }

    meaning_reveal_all(ctx->trans, ctx->n);
}

GtkWidget *build_choice(UnitCtx *unit, const char *title,
                               const char *subtitle, int ex_num,
                               const ChoiceQ *qs, const char **meanings, int n) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, title, subtitle, "check",
                                    &body, &feedback, &check);
    ChoiceCtx *ctx = g_new0(ChoiceCtx, 1);
    int n_opts = qs[0].n_options;

    ctx->qs = qs;
    ctx->n = n;
    ctx->n_opts = n_opts;
    ctx->unit = unit;
    ctx->ex_num = ex_num;
    ctx->feedback = feedback;
    ctx->toggles = g_new0(GtkToggleButton *, n * n_opts);
    ctx->trans = g_new0(GtkWidget *, n);

    for (int i = 0; i < n; i++) {
        GtkWidget *card = mcq_append_question(body, i + 1, &qs[i],
                                              &ctx->toggles[i * n_opts]);

        if (meanings[i])
            ctx->trans[i] = meaning_add(card, meanings[i]);
    }

    g_signal_connect(check, "clicked", G_CALLBACK(choice_check), ctx);
    return page;
}

/* ------------------------------------------------------------------ */
/* Exercise 4: Freie Antwort                                          */
/* ------------------------------------------------------------------ */


const FreeQ ex4_questions[] = {
    {"Wie heißt du?", "Jak se jmenuješ?",
     "Ich heiße Anna.", "Jmenuji se Anna."},
    {"Woher kommst du?", "Odkud pocházíš?",
     "Ich komme aus Tschechien.", "Pocházím z Česka."},
    {"Wie alt bist du?", "Kolik je ti let?",
     "Ich bin sechzehn Jahre alt.", "Je mi šestnáct let."},
    {"Wie geht es dir?", "Jak se máš?",
     "Mir geht es gut, danke.", "Mám se dobře, děkuji."},
};

void ex4_fill_sample(GtkWidget *label) {
    const FreeQ *q = g_object_get_data(G_OBJECT(label), "fq");
    char *txt;

    if (!q)
        return;
    txt = g_strdup_printf(tr("sample_fmt"), tr(q->q_cs), q->sample,
                          tr(q->a_cs));
    gtk_label_set_text(GTK_LABEL(label), txt);
    g_free(txt);
}

void ex4_reveal(GtkButton *button, gpointer data) {
    GtkWidget *sample = data;
    (void)button;
    ex4_fill_sample(sample);
    gtk_widget_set_visible(sample, !gtk_widget_get_visible(sample));
}


void ex4_finish(GtkButton *button, gpointer data) {
    Ex4Ctx *ctx = data;
    (void)button;
    set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
    for (int i = 0; i < ctx->n; i++) {
        ex4_fill_sample(ctx->samples[i]);
        gtk_widget_set_visible(ctx->samples[i], TRUE);
    }
    mark_done(ctx->unit, ctx->ex_num);
}

GtkWidget *build_free_answer(UnitCtx *unit, int ex_num, const char *title,
                                    const char *subtitle, const char *tip_key,
                                    const FreeQ *qs, int n) {
    GtkWidget *body, *feedback, *check;
    Ex4Ctx *ctx;
    GtkWidget *page = ex_page_shell(unit->page, title, subtitle, "finish",
                                    &body, &feedback, &check);

    ctx = g_new0(Ex4Ctx, 1);
    ctx->unit = unit;
    ctx->ex_num = ex_num;
    ctx->feedback = feedback;
    ctx->n = n;
    ctx->samples = g_new0(GtkWidget *, n);

    if (tip_key) {
        GtkWidget *tip = gtk_label_new(NULL);
        i18n_bind(tip, tip_key, 0);
        gtk_widget_set_halign(tip, GTK_ALIGN_START);
        gtk_widget_add_css_class(tip, "hint");
        gtk_box_append(GTK_BOX(body), tip);
    }

    for (int i = 0; i < n; i++) {
        const FreeQ *q = &qs[i];
        GtkWidget *ql = gtk_label_new(q->question);
        GtkWidget *entry;
        GtkWidget *reveal;
        GtkWidget *sample;

        gtk_widget_set_halign(ql, GTK_ALIGN_START);
        gtk_widget_add_css_class(ql, "ex-prompt");
        gtk_box_append(GTK_BOX(body), ql);

        entry = gtk_entry_new();
        i18n_bind(entry, "your_answer", 3);
        gtk_box_append(GTK_BOX(body), entry);

        reveal = gtk_button_new();
        i18n_bind(reveal, "show_sample", 1);
        gtk_widget_add_css_class(reveal, "pill");
        gtk_widget_set_halign(reveal, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(body), reveal);

        sample = gtk_label_new(NULL);
        g_object_set_data(G_OBJECT(sample), "fq", (gpointer)q);
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

GtkWidget *build_ex4(UnitCtx *unit) {
    return build_free_answer(unit, 4, "Freie Antwort", "sub_free", "tip_ss",
                             ex4_questions,
                             (int)G_N_ELEMENTS(ex4_questions));
}

/* ------------------------------------------------------------------ */
/* Exercise 5: Zahlen (digits -> word)                                */
/* ------------------------------------------------------------------ */


const NumberQ ex5_data[] = {
    {"14", "vierzehn"},
    {"17", "siebzehn"},
    {"20", "zwanzig"},
    {"6", "sechs"},
    {"13", "dreizehn"},
};

const char *ex5_pool[] = {
    "vierzehn", "siebzehn", "zwanzig", "sechs", "dreizehn",
};

const char *ex5_meaning[] = {
    "čtrnáct",
    "sedmnáct",
    "dvacet",
    "šest",
    "třináct",
};

GtkWidget *build_ex5(UnitCtx *unit) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, "Zahlen",
                                    "sub_zahlen",
                                    "check", &body, &feedback, &check);
    ComboListCtx *ctx = combo_list_ctx_new(unit, 5, feedback);

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


const CountQ ex6_data[] = {
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

const char *ex6_pool[] = {
    "zwei", "fünf", "sieben", "neun", "drei", "elf", "vier", "acht", "sechs",
};

const char *ex6_meaning[] = {
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

GtkWidget *build_ex6(UnitCtx *unit) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, "Wie viel?",
                                    "sub_wieviel",
                                    "check", &body, &feedback, &check);
    ComboListCtx *ctx = combo_list_ctx_new(unit, 6, feedback);

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


const SeqQ ex7_data[] = {
    {"sechs, sieben, ", ", neun", "acht"},
    {"zwölf, dreizehn, ", ", fünfzehn", "vierzehn"},
    {"achtzehn, neunzehn, ", ", einundzwanzig", "zwanzig"},
    {"vier, fünf, ", ", sieben", "sechs"},
    {"zehn, elf, ", ", dreizehn", "zwölf"},
};

const char *ex7_pool[] = {
    "acht", "vierzehn", "zwanzig", "sechs", "zwölf",
};

const char *ex7_meaning[] = {
    "šest, sedm, osm, devět",
    "dvanáct, třináct, čtrnáct, patnáct",
    "osmnáct, devatenáct, dvacet, dvacet jedna",
    "čtyři, pět, šest, sedm",
    "deset, jedenáct, dvanáct, třináct",
};

GtkWidget *build_ex7(UnitCtx *unit) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, "Zahlenreihe",
                                    "sub_reihe",
                                    "check", &body, &feedback, &check);
    ComboListCtx *ctx = combo_list_ctx_new(unit, 7, feedback);

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


/* Complete German sentence for a before/after gap row (with any trailing
 * "(…)" hint such as "(kommen)" removed) – shown after Check. */
char *verbq_german(const char *before, const char *answer,
                          const char *after) {
    GString *s;
    const char *p;
    gboolean inside = FALSE;
    char *out;

    s = g_string_new(NULL);
    if (before && before[0])
        g_string_append(s, before);
    if (answer && answer[0])
        g_string_append(s, answer);
    if (after && after[0])
        g_string_append(s, after);

    out = s->str;
    s->str = NULL;
    g_string_free(s, TRUE);
    s = g_string_new(NULL);
    for (p = out; *p; p++) {
        if (*p == '(') {
            inside = TRUE;
            continue;
        }
        if (*p == ')') {
            inside = FALSE;
            continue;
        }
        if (!inside)
            g_string_append_c(s, *p);
    }
    g_free(out);

    while (s->len > 0 && g_ascii_isspace(s->str[s->len - 1]))
        g_string_truncate(s, s->len - 1);
    if (s->len == 0) {
        g_string_free(s, TRUE);
        return NULL;
    }
    return g_string_free(s, FALSE);
}

const VerbQ ex8_data[] = {
    {"Ich ", " Peter.", "heiße"},
    {"Woher ", " du?", "kommst"},
    {"Ich ", " in Prag.", "wohne"},
    {"Wie alt ", " du?", "bist"},
    {"Ich ", " Fußball.", "spiele"},
    {"Was ", " du gern?", "machst"},
};

const char *ex8_pool[] = {
    "heißen", "kommen", "wohnen", "sein", "spielen", "machen",
    "heiße", "kommst", "wohne", "bist", "spiele", "machst",
};

const char *ex8_meaning[] = {
    "Jmenuji se Petr.",
    "Odkud jsi?",
    "Bydlím v Praze.",
    "Kolik je ti let?",
    "Hraji fotbal.",
    "Co rád/a děláš?",
};

GtkWidget *build_ex8(UnitCtx *unit) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, "Verb einsetzen",
                                    "sub_verb",
                                    "check", &body, &feedback, &check);
    ComboListCtx *ctx = combo_list_ctx_new(unit, 8, feedback);

    ctx->trans_upfront = TRUE;
    ctx->reveal_german = TRUE;

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

        {
            GtkWidget *mean = meaning_add(body, ex8_meaning[i]);
            GtkWidget *w;
            char *sentence;

            gtk_widget_set_visible(mean, TRUE);
            combo_list_add_trans(ctx, mean);

            sentence = verbq_german(q->before, q->answer, q->after);
            if (sentence && sentence[0]) {
                w = model_answer_add(body, sentence);
                g_array_append_val(ctx->models, w);
            }
            g_free(sentence);
        }
    }

    g_signal_connect(check, "clicked", G_CALLBACK(combo_list_check), ctx);
    return page;
}

/* ------------------------------------------------------------------ */
/* Exercise 10: Wörter trennen (word arrangement)                     */
/* ------------------------------------------------------------------ */

const AssemblyItem ex10_items[] = {
    {"wiespätistes", {"wie", "spät", "ist", "es"}, 4},
    {"ichheißeanna", {"ich", "heiße", "anna"}, 3},
    {"woherkommstdu", {"woher", "kommst", "du"}, 3},
    {"aufwiedersehen", {"auf", "wiedersehen"}, 2},
    {"dankeschön", {"danke", "schön"}, 2},
};

const char *ex10_meaning[] = {
    "Kolik je hodin?",
    "Jmenuji se Anna.",
    "Odkud jsi?",
    "Na shledanou.",
    "Moc děkuji.",
};

/* ------------------------------------------------------------------ */
/* Exercise 12: Was macht er/sie?                                     */
/* ------------------------------------------------------------------ */


const VerbClueQ ex12_data[] = {
    {"🏊", "Er ", " im See.", "schwimmt"},
    {"🎤", "Sie ", " ein Lied.", "singt"},
    {"🍳", "Er ", " Suppe.", "kocht"},
    {"📖", "Sie ", " ein Buch.", "liest"},
    {"🚗", "Er ", " Auto.", "fährt"},
    {"🎨", "Sie ", " ein Bild.", "malt"},
    {"⚽", "Er ", " Fußball.", "spielt"},
    {"🛏️", "Sie ", " .", "schläft"},
};

const char *ex12_pool[] = {
    "schwimmt", "singt", "kocht", "liest", "fährt", "malt", "spielt", "schläft",
};

const char *ex12_meaning[] = {
    "Plave v jezeře.",
    "Zpívá píseň.",
    "Vaří polévku.",
    "Čte knihu.",
    "Řídí auto.",
    "Maluje obraz.",
    "Hraje fotbal.",
    "Spí.",
};

GtkWidget *build_ex12(UnitCtx *unit) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, "Was macht er/sie?",
                                    "sub_bild",
                                    "check", &body, &feedback, &check);
    ComboListCtx *ctx = combo_list_ctx_new(unit, 12, feedback);

    ctx->trans_upfront = TRUE;
    ctx->reveal_german = TRUE;

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

        {
            GtkWidget *mean = meaning_add(body, ex12_meaning[i]);
            GtkWidget *w;
            char *sentence;

            gtk_widget_set_visible(mean, TRUE);
            combo_list_add_trans(ctx, mean);

            sentence = verbq_german(q->before, q->answer, q->after);
            if (sentence && sentence[0]) {
                w = model_answer_add(body, sentence);
                g_array_append_val(ctx->models, w);
            }
            g_free(sentence);
        }
    }

    g_signal_connect(check, "clicked", G_CALLBACK(combo_list_check), ctx);
    return page;
}

/* ------------------------------------------------------------------ */
/* Assign exercises (11 + 13)                                         */
/* ------------------------------------------------------------------ */


const AssignItem ex11_items[] = {
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

const AssignItem ex13_items[] = {
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

const char *ex11_groups[] = {"Begrüßung", "Verabschiedung"};
const char *ex13_groups[] = {"Deutschland (D)", "Österreich (A)", "Schweiz (CH)"};

const char *ex11_meaning[] = {
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

const char *ex13_meaning[] = {
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



void assign_rebuild(AssignCtx *ac) {
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

void assign_chip_clicked(GtkButton *button, gpointer data) {
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

void assign_group_toggled(GtkToggleButton *button, gpointer data) {
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

void assign_check(GtkButton *button, gpointer data) {
    AssignCtx *ac = data;
    int ok = 0;

    (void)button;

    for (int i = 0; i < ac->n_items; i++)
        if (ac->current_group[i] == ac->items_data[i].correct_group)
            ok++;

    if (ok == ac->n_items) {
        set_feedback(ac->feedback, TRUE, tr("feedback_ok"));
        mark_done(ac->unit, ac->ex_num);
    } else {
        set_feedback(ac->feedback, FALSE, tr("feedback_retry"));
    }

    for (int i = 0; i < ac->n_items; i++)
        if (ac->trans[i])
            gtk_widget_set_visible(ac->trans[i], TRUE);
}

GtkWidget *build_assign(UnitCtx *unit, const char *title,
                               const char *subtitle, int ex_num,
                               const AssignItem *items, int n_items,
                               const char **group_labels, int n_groups,
                               const char **meanings) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, title, subtitle, "check",
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
    ac->unit = unit;
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
/* Exercise 14: Wörter übersetzen                                     */
/* ------------------------------------------------------------------ */

const TypedQ trans_p10[] = {
    {"der Abend, -e", "večer|evening", "večer"},
    {"Guten Abend!", "dobrý večer|good evening", "Dobrý večer!"},
    {"du", "ty|you", "ty"},
    {"(Es) freut mich.", "těší mě|nice to meet you|pleased to meet you",
     "Těší mě."},
    {"gut", "dobrý|dobře|good", "dobrý"},
    {"Hallo!", "ahoj|hello", "Ahoj!"},
    {"heißen (Infinitiv)",
     "jmenovat se|jmenuje se|to be called|to be named", "jmenovat se"},
    {"Ich heiße Jens.",
     "jmenuji se jens|jmenuju se jens|my name is jens", "Jmenuji se Jens."},
    {"Hi!", "ahoj|čau|hi", "Ahoj! (neformálně)"},
    {"ich", "já|i", "já"},
    {"ihr", "vy|you", "vy"},
    {"ja", "ano|jo|yes", "ano"},
    {"der Morgen", "ráno|morning", "ráno"},
    {"Guten Morgen!", "dobré ráno|good morning", "Dobré ráno!"},
    {"der Name, -n", "jméno|name", "jméno"},
    {"nein", "ne|no", "ne"},
    {"(er/sie/es) ist", "je|is", "je"},
    {"der Tag, -e", "den|day", "den"},
    {"Guten Tag!", "dobrý den|good day", "Dobrý den!"},
    {"und", "a|and", "a"},
    {"wer", "kdo|who", "kdo"},
    {"Wer bist du?", "kdo jsi|kdo jste|who are you", "Kdo jsi?"},
    {"wie", "jak|how", "jak"},
    {"Wie heißt du?",
     "jak se jmenuješ|jak se jmenujete|what is your name|what's your name",
     "Jak se jmenuješ?"},
    {"wir", "my|we", "my"},
};

const TypedQ trans_p11[] = {
    {"in", "v|ve|in", "v"},
    {"in der Nähe (von)",
     "poblíž|v blízkosti|nedaleko|blízko|near|nearby|close to",
     "poblíž, v blízkosti"},
    {"das Spiel, -e", "hra|game", "hra"},
    {"wo", "kde|where", "kde"},
    {"wohnen (Infinitiv)", "bydlet|bydlí|to live", "bydlet"},
    {"Wo wohnst du?",
     "kde bydlíš|kde bydlíte|where do you live", "Kde bydlíš?"},
    {"Ich wohne in Prag.", "bydlím v praze|i live in prague", "Bydlím v Praze."},
    {"null", "nula|zero", "nula"},
    {"eins", "jedna|jeden|one", "jedna"},
    {"zwei", "dvě|dva|two", "dvě"},
    {"drei", "tři|three", "tři"},
    {"vier", "čtyři|four", "čtyři"},
    {"fünf", "pět|five", "pět"},
    {"sechs", "šest|six", "šest"},
    {"sieben", "sedm|seven", "sedm"},
    {"acht", "osm|eight", "osm"},
    {"neun", "devět|nine", "devět"},
    {"zehn", "deset|ten", "deset"},
    {"elf", "jedenáct|eleven", "jedenáct"},
    {"zwölf", "dvanáct|twelve", "dvanáct"},
    {"dreizehn", "třináct|thirteen", "třináct"},
    {"vierzehn", "čtrnáct|fourteen", "čtrnáct"},
    {"fünfzehn", "patnáct|fifteen", "patnáct"},
    {"sechzehn", "šestnáct|sixteen", "šestnáct"},
    {"siebzehn", "sedmnáct|seventeen", "sedmnáct"},
    {"achtzehn", "osmnáct|eighteen", "osmnáct"},
    {"neunzehn", "devatenáct|nineteen", "devatenáct"},
    {"zwanzig", "dvacet|twenty", "dvacet"},
};

const TypedQ trans_p12[] = {
    {"alt", "starý|old", "starý"},
    {"er", "on|he", "on"},
    {"der Monat, -e", "měsíc|month", "měsíc"},
    {"sie (Singular)", "ona|she", "ona"},
    {"sie (Plural)", "oni|they", "oni"},
};

const TypedQ trans_p13[] = {
    {"aber", "ale|but", "ale"},
    {"das Alter", "věk|age", "věk"},
    {"auch", "také|taky|also|too", "také"},
    {"aus", "z|ze|from", "z"},
    {"die Band, -s", "skupina|hudební skupina|kapela|hudební kapela|band", "(hudební) skupina"},
    {"chatten (Infinitiv)", "chatovat|chatuje|to chat", "chatovat"},
    {"das Computerspiel, -e", "počítačová hra|computer game", "počítačová hra"},
    {"die/das E-Mail, -s", "e-mail|email", "e-mail"},
    {"das Englisch", "angličtina|anglicky|english", "angličtina, anglicky"},
    {"der Fan, -s", "fanoušek|fanda|fan", "fanoušek"},
    {"das Französisch", "francouzština|francouzsky|french",
     "francouzština, francouzsky"},
    {"der Freund, -e", "kamarád|přítel|friend", "kamarád"},
    {"die Freundin, -nen", "kamarádka|přítelkyně|friend", "kamarádka"},
    {"beste Freundin", "nejlepší kamarádka|nejlepší přítelkyně|best friend",
     "nejlepší kamarádka"},
    {"der Fußball", "fotbal|football|soccer", "fotbal"},
    {"Er ist Fußballfanatiker.", "je fotbalový fanatik|he is a football fanatic",
     "Je fotbalový fanatik."},
    {"gehen (Infinitiv)", "jít|jde|chodit|to go", "jít"},
    {"Ich gehe gern auf Konzerte.",
     "rád chodím na koncerty|ráda chodím na koncerty|chodím rád na koncerty|chodím ráda na koncerty|i like going to concerts",
     "Rád(a) chodím na koncerty."},
    {"gern", "rád|ráda|gladly", "rád/a"},
    {"die Gitarre, -n", "kytara|guitar", "kytara"},
    {"das Handy, -s", "mobil|mobilní telefon|mobile phone", "mobil"},
    {"das Hobby, -s", "koníček|hobby", "koníček"},
    {"hören (Infinitiv)", "poslouchat|poslouchá|slyšet|to listen|to hear",
     "poslouchat"},
    {"Er hört gern Musik.", "rád poslouchá hudbu|he likes listening to music",
     "Rád poslouchá hudbu."},
    {"im Internet surfen (Infinitiv)",
     "surfovat na internetu|to surf the internet", "surfovat na internetu"},
    {"immer", "pořád|neustále|vždy|vždycky|always", "pořád, neustále"},
    {"das Jahr, -e", "rok|year", "rok"},
    {"kein/keine/kein", "žádný|žádná|žádné|no|none", "žádný/žádná/žádné"},
    {"das Klavier, -e", "klavír|piano", "klavír"},
    {"kommen (Infinitiv, aus)",
     "přicházet|přijít|přichází|pocházet|pochází|to come|to come from",
     "přicházet, pocházet (z)"},
    {"Er kommt aus Wien.",
     "pochází z vídně|he comes from vienna|he is from vienna",
     "Pochází z Vídně."},
    {"das Konzert, -e", "koncert|concert", "koncert"},
    {"lernen (Infinitiv)", "učit se|učí se|to learn", "učit se"},
    {"liegen (Infinitiv)", "ležet|leží|to lie|to be located", "ležet"},
    {"der Nachname, -n", "příjmení|surname|last name", "příjmení"},
    {"nicht weit von", "nedaleko od|not far from", "nedaleko od"},
    {"reisen (Infinitiv)", "cestovat|cestuje|to travel", "cestovat"},
    {"schreiben (Infinitiv)", "psát|píše|to write", "psát"},
    {"die Schule, -n", "škola|school", "škola"},
    {"spielen (Infinitiv)", "hrát|hraje|to play", "hrát"},
    {"Sie spielt Gitarre.", "hraje na kytaru|she plays the guitar",
     "Hraje na kytaru."},
    {"tanzen (Infinitiv)", "tancovat|tančí|tancuje|to dance", "tancovat"},
    {"der Vorname, -n", "křestní jméno|first name", "křestní jméno"},
    {"der Wohnort, -e", "bydliště|místo bydliště|place of residence",
     "bydliště"},
};

const TypedQ trans_p14[] = {
    {"falsch", "nesprávně|chybně|špatně|nepravda|wrong|incorrect", "nesprávně, chybně"},
    {"die Flöte, -n", "flétna|flute", "flétna"},
    {"haben (Infinitiv)", "mít|má|to have", "mít"},
    {"das Instrument, -e", "nástroj|hudební nástroj|instrument", "(hudební) nástroj"},
    {"jetzt", "teď|nyní|now", "teď"},
    {"machen (Infinitiv)", "dělat|dělá|to do|to make", "dělat"},
    {"richtig", "správně|správný|correct|right", "správně"},
    {"der Sport", "sport", "sport"},
    {"Sport treiben (Infinitiv)",
     "sportovat|sportuje|provozovat sport|dělat sport|to do sport", "sportovat"},
    {"das Squash", "squash", "squash"},
    {"das/der Yoga", "jóga|yoga", "jóga"},
};

const TypedQ trans_p15[] = {
    {"bald", "brzy|brzo|soon", "brzy"},
    {"Bis bald!", "brzy na shledanou|brzo na shledanou|brzy se uvidíme|brzo se uvidíme|see you soon",
     "Brzy na shledanou!"},
    {"bis", "do|až|until", "do"},
    {"Bis zum nächsten Mal!", "do příštího setkání|do příště|until next time",
     "Do příštího setkání!"},
    {"die Frau, -en", "paní|žena|manželka|woman|mrs|wife", "paní"},
    {"der Herr, -en", "pan|muž|man|mr", "pan"},
    {"Mach's gut!", "měj se hezky|měj se|mějte se hezky|mějte se|take care", "Měj se hezky!"},
    {"das Museum, Museen", "muzeum|museum", "muzeum"},
    {"Sie gehen ins Museum.",
     "jdou do muzea|they are going to the museum|they go to the museum",
     "Jdou do muzea."},
    {"schön", "hezký|krásný|pěkný|beautiful|nice", "hezký"},
    {"Einen schönen Tag noch!",
     "hezký zbytek dne|měj hezký den|have a nice day", "Hezký zbytek dne!"},
    {"Servus!", "ahoj|servus|hi", "Ahoj! (servus)"},
    {"Tschüs!", "ahoj|čau|bye", "Ahoj! (při loučení)"},
    {"das Wiedersehen", "shledání|reunion", "shledání"},
    {"Auf Wiedersehen!", "na shledanou|na viděnou|goodbye", "Na shledanou!"},
};

const TransSection u1_trans_sections[] = {
    {"Strana 10", trans_p10, (int)G_N_ELEMENTS(trans_p10)},
    {"Strana 11", trans_p11, (int)G_N_ELEMENTS(trans_p11)},
    {"Strana 12", trans_p12, (int)G_N_ELEMENTS(trans_p12)},
    {"Strana 13", trans_p13, (int)G_N_ELEMENTS(trans_p13)},
    {"Strana 14", trans_p14, (int)G_N_ELEMENTS(trans_p14)},
    {"Strana 15", trans_p15, (int)G_N_ELEMENTS(trans_p15)},
};
const int u1_trans_sections_n = (int)G_N_ELEMENTS(u1_trans_sections);

typedef struct {
    UnitCtx *unit;
    int ex_num;
    GtkWidget *feedback;
    GtkWidget *header;
    GtkWidget *progress;
    GtkWidget *prompt;
    GtkWidget *entry;
    GtkWidget *check;
    TypedQ *qs;           /* flattened words, in page order            */
    const char **headers; /* section header i18n key per word          */
    int total;
    int done;
    int *order;           /* queue of word indices, order[0] = current */
    int qn;               /* words still in the queue                  */
    gboolean warned;      /* current word already got an "incomplete" warning */
} TransCtx;

/* Canonical translation of a word, in the current UI language. */
static const char *trans_answer(const TypedQ *q) {
    if (q->meaning)
        return tr(q->meaning);
    return q->answers ? q->answers : "";
}

static void trans_update_progress(TransCtx *ctx) {
    gchar *txt = g_strdup_printf(tr("trans_progress"), ctx->done, ctx->total);

    gtk_label_set_text(GTK_LABEL(ctx->progress), txt);
    g_free(txt);
}

static void trans_show(TransCtx *ctx) {
    ctx->warned = FALSE;

    if (ctx->qn <= 0) {
        gtk_label_set_text(GTK_LABEL(ctx->header), "");
        gtk_label_set_text(GTK_LABEL(ctx->prompt), tr("trans_done"));
        gtk_widget_set_visible(ctx->entry, FALSE);
        gtk_widget_set_visible(ctx->check, FALSE);
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        trans_update_progress(ctx);
        mark_done(ctx->unit, ctx->ex_num);
        return;
    }

    {
        int idx = ctx->order[0];
        const TypedQ *q = &ctx->qs[idx];

        gtk_label_set_text(GTK_LABEL(ctx->header), tr(ctx->headers[idx]));
        gtk_label_set_text(GTK_LABEL(ctx->prompt), q->prompt);
    }

    gtk_editable_set_text(GTK_EDITABLE(ctx->entry), "");
    gtk_widget_set_visible(ctx->entry, TRUE);
    trans_update_progress(ctx);
    if (gtk_widget_get_mapped(ctx->entry))
        gtk_widget_grab_focus(ctx->entry);
}

static void trans_on_map(GtkWidget *widget, gpointer data) {
    TransCtx *ctx = data;

    (void)widget;
    if (ctx->qn > 0 && gtk_widget_get_visible(ctx->entry))
        gtk_widget_grab_focus(ctx->entry);
}

static void translate_check(GtkButton *button, gpointer data) {
    TransCtx *ctx = data;
    int idx;
    const TypedQ *q;
    const gchar *txt;
    gchar *norm;
    gboolean good;

    (void)button;

    if (ctx->qn <= 0)
        return;

    idx = ctx->order[0];
    q = &ctx->qs[idx];
    txt = gtk_editable_get_text(GTK_EDITABLE(ctx->entry));
    norm = normalize_answer(txt ? txt : "");
    good = txt && txt[0] && answer_accepts(norm, q->answers);

    /* Nearly there (e.g. "jmenovat" for "jmenovat se"): warn in yellow and let
     * the student finish the answer. A second Check on the same word counts. */
    if (!good && !ctx->warned && txt && txt[0] &&
        answer_is_incomplete(norm, q->answers)) {
        g_free(norm);
        ctx->warned = TRUE;
        set_feedback_warn(ctx->feedback, tr("trans_incomplete"));
        return;
    }
    g_free(norm);

    /* Pop the current word off the front of the queue. */
    for (int i = 0; i < ctx->qn - 1; i++)
        ctx->order[i] = ctx->order[i + 1];
    ctx->qn--;

    if (good) {
        ctx->done++;
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
    } else {
        gchar *msg = g_strdup_printf(tr("trans_wrong"), trans_answer(q));

        set_feedback(ctx->feedback, FALSE, msg);
        g_free(msg);
        ctx->order[ctx->qn++] = idx;   /* failed words retry at the end */
    }

    trans_show(ctx);
}

GtkWidget *build_translate(UnitCtx *unit) {
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *wrap, *card, *bottom, *feedback, *check;
    TransCtx *ctx = g_new0(TransCtx, 1);
    int total = 0;
    int k = 0;

    for (int s = 0; s < unit->n_trans_sections; s++)
        total += unit->trans_sections[s].n;

    ctx->unit = unit;
    ctx->ex_num = unit->branch_ex;
    ctx->total = total;
    ctx->qn = total;
    ctx->qs = g_new0(TypedQ, total);
    ctx->headers = g_new0(const char *, total);
    ctx->order = g_new0(int, total);

    for (int s = 0; s < unit->n_trans_sections; s++) {
        const TransSection *sec = &unit->trans_sections[s];

        for (int i = 0; i < sec->n; i++, k++) {
            ctx->qs[k] = sec->rows[i];
            ctx->headers[k] = sec->header;
            ctx->order[k] = k;
        }
    }

    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);
    gtk_box_append(GTK_BOX(page),
                   top_bar(unit->page, "Vokabeltraining", "sub_translate"));

    wrap = gtk_center_box_new();
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_box_append(GTK_BOX(page), wrap);

    card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_size_request(card, 380, -1);
    gtk_widget_set_valign(card, GTK_ALIGN_CENTER);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), card);

    ctx->header = gtk_label_new(NULL);
    gtk_widget_set_halign(ctx->header, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(ctx->header, "ex-sub");
    gtk_box_append(GTK_BOX(card), ctx->header);

    ctx->progress = gtk_label_new(NULL);
    gtk_widget_set_halign(ctx->progress, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(ctx->progress, "hint");
    gtk_box_append(GTK_BOX(card), ctx->progress);

    ctx->prompt = gtk_label_new(NULL);
    gtk_widget_set_halign(ctx->prompt, GTK_ALIGN_CENTER);
    gtk_label_set_wrap(GTK_LABEL(ctx->prompt), TRUE);
    gtk_label_set_justify(GTK_LABEL(ctx->prompt), GTK_JUSTIFY_CENTER);
    gtk_widget_add_css_class(ctx->prompt, "trans-word");
    gtk_widget_set_margin_top(ctx->prompt, 10);
    gtk_widget_set_margin_bottom(ctx->prompt, 10);
    gtk_box_append(GTK_BOX(card), ctx->prompt);

    ctx->entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ctx->entry), tr("your_answer"));
    gtk_widget_set_halign(ctx->entry, GTK_ALIGN_FILL);
    gtk_box_append(GTK_BOX(card), ctx->entry);

    bottom = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_margin_top(bottom, 4);
    gtk_box_append(GTK_BOX(page), bottom);

    feedback = gtk_label_new("");
    gtk_widget_set_halign(feedback, GTK_ALIGN_START);
    gtk_widget_set_valign(feedback, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(feedback, TRUE);
    gtk_box_append(GTK_BOX(bottom), feedback);
    ctx->feedback = feedback;

    check = gtk_button_new();
    i18n_bind(check, "check", 1);
    gtk_widget_add_css_class(check, "btn-primary");
    gtk_widget_set_valign(check, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(bottom), check);
    ctx->check = check;

    g_signal_connect(check, "clicked", G_CALLBACK(translate_check), ctx);
    g_signal_connect(ctx->entry, "activate", G_CALLBACK(translate_check), ctx);
    g_signal_connect(page, "map", G_CALLBACK(trans_on_map), ctx);

    trans_show(ctx);
    return page;
}
