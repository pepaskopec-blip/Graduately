#include "maturita.h"

/* ------------------------------------------------------------------ */
/* Unit 2 exercises ("Aus aller Welt")                                */
/* ------------------------------------------------------------------ */

/* ---- generic word-row helper (single blank, shared word bank) ----- */

void verb_rows_add(ComboListCtx *ctx, GtkWidget *body, int *counter,
                          const VerbQ *qs, int n,
                          const char **pool, int pool_n,
                          const char **meanings, gboolean pre_meaning) {
    for (int i = 0; i < n; i++) {
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        GtkWidget *before = gtk_label_new(qs[i].before);
        GtkWidget *after = gtk_label_new(qs[i].after);
        GtkWidget *combo;
        char *numtxt;

        (*counter)++;
        numtxt = g_strdup_printf("%d. ", *counter);
        GtkWidget *num = gtk_label_new(numtxt);
        g_free(numtxt);

        gtk_widget_set_halign(row, GTK_ALIGN_START);
        gtk_widget_set_valign(num, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(num, "ex-prompt");
        gtk_box_append(GTK_BOX(row), num);

        gtk_widget_set_valign(before, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(before, "ex-prompt");
        gtk_box_append(GTK_BOX(row), before);

        combo = make_word_combo(pool, pool_n);
        gtk_widget_set_size_request(combo, 150, -1);
        gtk_widget_set_valign(combo, GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(row), combo);
        combo_list_add(ctx, combo, qs[i].answer);

        if (qs[i].after && qs[i].after[0]) {
            gtk_widget_set_valign(after, GTK_ALIGN_CENTER);
            gtk_widget_add_css_class(after, "ex-prompt");
            gtk_box_append(GTK_BOX(row), after);
        }

        gtk_box_append(GTK_BOX(body), row);

        if (meanings && meanings[i]) {
            GtkWidget *mean = meaning_add(body, meanings[i]);
            if (pre_meaning || ctx->trans_upfront)
                gtk_widget_set_visible(mean, TRUE);
            combo_list_add_trans(ctx, mean);
        }

        if (ctx->reveal_german) {
            char *sentence = verbq_german(qs[i].before, qs[i].answer,
                                          qs[i].after);

            if (sentence && sentence[0]) {
                GtkWidget *w = model_answer_add(body, sentence);

                g_array_append_val(ctx->models, w);
            }
            g_free(sentence);
        }
    }
}


void verb_note_reveal(VerbNoteCtx *nc) {
    GString *s;
    int i;

    if (!nc->note || !nc->gloss)
        return;

    s = g_string_new(NULL);
    for (i = 0; i < nc->n; i++) {
        if (i > 0)
            g_string_append_c(s, '\n');
        g_string_append_printf(s, "%s – %s", nc->qs[i].answer,
                               tr(nc->gloss[i]));
    }
    gtk_label_set_text(GTK_LABEL(nc->note), s->str);
    g_string_free(s, TRUE);
    gtk_widget_set_visible(nc->note, TRUE);
}

void verb_ex_check(GtkButton *button, gpointer data) {
    VerbNoteCtx *nc = data;

    combo_list_check(button, nc->ctx);
    verb_note_reveal(nc);
}

GtkWidget *build_verb_ex(UnitCtx *unit, int ex_num,
                                const char *title, const char *sub,
                                const VerbQ *qs, int n,
                                const char **pool, int pool_n,
                                const char **meanings, gboolean pre_meaning,
                                const char **gloss) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, title, sub, "check",
                                    &body, &feedback, &check);
    ComboListCtx *ctx = combo_list_ctx_new(unit, ex_num, feedback);
    VerbNoteCtx *nc = NULL;
    int counter = 0;

    ctx->trans_upfront = TRUE;
    ctx->reveal_german = TRUE;

    verb_rows_add(ctx, body, &counter, qs, n, pool, pool_n, meanings,
                  pre_meaning);

    if (gloss) {
        GtkWidget *note = gtk_label_new(NULL);
        gtk_widget_set_halign(note, GTK_ALIGN_START);
        gtk_widget_set_margin_top(note, 6);
        gtk_widget_add_css_class(note, "meaning");
        gtk_label_set_wrap(GTK_LABEL(note), TRUE);
        gtk_widget_set_visible(note, FALSE);
        gtk_box_append(GTK_BOX(body), note);

        nc = g_new0(VerbNoteCtx, 1);
        nc->ctx = ctx;
        nc->note = note;
        nc->qs = qs;
        nc->n = n;
        nc->gloss = gloss;
        g_signal_connect(check, "clicked", G_CALLBACK(verb_ex_check), nc);
    } else {
        g_signal_connect(check, "clicked", G_CALLBACK(combo_list_check), ctx);
    }

    return page;
}

/* ---- typed-entry builder with auto-check --------------------------- */



void typed_check(GtkButton *button, gpointer data) {
    TypedCtx *ctx = data;
    int ok = 0;

    (void)button;

    for (int i = 0; i < ctx->n; i++) {
        const gchar *txt = gtk_editable_get_text(GTK_EDITABLE(ctx->entries[i]));
        gchar *norm = normalize_answer(txt);
        gboolean good = txt && txt[0] && answer_accepts(norm, ctx->qs[i].answers);
        g_free(norm);
        answer_mark(ctx->entries[i], good);
        if (good)
            ok++;
    }

    if (ctx->trans) {
        for (int i = 0; i < ctx->n; i++)
            if (ctx->trans[i])
                gtk_widget_set_visible(ctx->trans[i], TRUE);
    }

    if (ok == ctx->n) {
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        mark_done(ctx->unit, ctx->ex_num);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry"));
    }
}


/* Capitalise the first (canonical) answer alternative for display. */
char *typed_answer_display(const char *answers) {
    const char *pipe = strchr(answers, '|');
    char *base = pipe ? g_strndup(answers, (gsize)(pipe - answers))
                      : g_strdup(answers);

    if (base[0]) {
        gunichar ch = g_utf8_get_char(base);
        gunichar up = g_unichar_toupper(ch);
        if (up != ch) {
            char *first = g_ucs4_to_utf8(&up, 1, NULL, NULL, NULL);
            char *res = g_strconcat(first, g_utf8_next_char(base), NULL);
            g_free(first);
            g_free(base);
            return res;
        }
    }
    return base;
}

void typed_note_reveal(TypedNoteCtx *nc) {
    GString *s;
    int i;

    if (!nc->note)
        return;

    s = g_string_new(NULL);
    for (i = 0; i < nc->n; i++) {
        char *word = typed_answer_display(nc->qs[i].answers);
        if (i > 0)
            g_string_append_c(s, '\n');
        if (nc->qs[i].meaning)
            g_string_append_printf(s, "%s – %s", word, tr(nc->qs[i].meaning));
        else
            g_string_append(s, word);
        g_free(word);
    }
    gtk_label_set_text(GTK_LABEL(nc->note), s->str);
    g_string_free(s, TRUE);
    gtk_widget_set_visible(nc->note, TRUE);
}

void typed_ex_check(GtkButton *button, gpointer data) {
    TypedNoteCtx *nc = data;

    typed_check(button, nc->ctx);
    typed_note_reveal(nc);
}

GtkWidget *build_typed(UnitCtx *unit, int ex_num,
                              const char *title, const char *sub,
                              const TypedQ *qs, int n,
                              const char *word_bank, gboolean answers_note) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, title, sub, "check",
                                    &body, &feedback, &check);
    TypedCtx *ctx = g_new0(TypedCtx, 1);
    TypedNoteCtx *nc = NULL;

    ctx->qs = qs;
    ctx->n = n;
    ctx->unit = unit;
    ctx->ex_num = ex_num;
    ctx->feedback = feedback;
    ctx->entries = g_new0(GtkWidget *, n);
    ctx->trans = g_new0(GtkWidget *, n);

    {
        gboolean need = FALSE;

        for (int i = 0; i < n; i++) {
            if (text_has_german_umlaut(qs[i].prompt) ||
                text_has_german_umlaut(qs[i].answers)) {
                need = TRUE;
                break;
            }
        }
        if (need)
            add_umlaut_note(body);
    }

    if (word_bank) {
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        GtkWidget *hdr = gtk_label_new(NULL);
        GtkWidget *words = gtk_label_new(word_bank);

        gtk_widget_set_halign(row, GTK_ALIGN_START);
        i18n_bind(hdr, "wordbank", 0);
        gtk_widget_add_css_class(hdr, "hint");
        gtk_widget_add_css_class(words, "hint");
        gtk_box_append(GTK_BOX(row), hdr);
        gtk_box_append(GTK_BOX(row), words);
        gtk_box_append(GTK_BOX(body), row);
    }

    for (int i = 0; i < n; i++) {
        GtkWidget *prompt = gtk_label_new(qs[i].prompt);
        GtkWidget *entry;

        gtk_widget_set_halign(prompt, GTK_ALIGN_START);
        gtk_widget_add_css_class(prompt, "ex-prompt");
        gtk_label_set_wrap(GTK_LABEL(prompt), TRUE);
        gtk_box_append(GTK_BOX(body), prompt);

        entry = gtk_entry_new();
        gtk_widget_set_hexpand(entry, FALSE);
        gtk_widget_set_halign(entry, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(body), entry);
        ctx->entries[i] = entry;

        if (qs[i].meaning)
            ctx->trans[i] = meaning_add(body, qs[i].meaning);
    }

    if (answers_note) {
        GtkWidget *note = gtk_label_new(NULL);
        gtk_widget_set_halign(note, GTK_ALIGN_START);
        gtk_widget_set_margin_top(note, 6);
        gtk_widget_add_css_class(note, "meaning");
        gtk_label_set_wrap(GTK_LABEL(note), TRUE);
        gtk_widget_set_visible(note, FALSE);
        gtk_box_append(GTK_BOX(body), note);

        nc = g_new0(TypedNoteCtx, 1);
        nc->ctx = ctx;
        nc->note = note;
        nc->qs = qs;
        nc->n = n;
        g_signal_connect(check, "clicked", G_CALLBACK(typed_ex_check), nc);
    } else {
        g_signal_connect(check, "clicked", G_CALLBACK(typed_check), ctx);
    }

    return page;
}

/* ---- G05/G06/G07/G11/S03 typed content ----------------------------- */

const char *g05_bank =
    "Tscheche, Tschechin, Spanier, Spanierin, Türke, Türkin, Kroate, "
    "Kroatin, Deutscher, Deutsche, Österreicher, Österreicherin, "
    "Slowake, Slowakin";

const TypedQ g05_rows[] = {
    {"Er kommt aus Tschechien. Er ist _________.", "tscheche", "Čech"},
    {"Sie kommt aus Spanien. Sie ist _________.", "spanierin", "Španělka"},
    {"Er kommt aus der Türkei. Er ist _________.", "türke|turke", "Turek"},
    {"Sie kommt aus Kroatien. Sie ist _________.", "kroatin", "Chorvatka"},
    {"Sie kommt aus Deutschland. Sie ist _________.", "deutsche", "Němka"},
    {"Sie kommt aus Österreich. Sie ist _________.", "österreicherin|osterreicherin",
     "Rakušanka"},
    {"Er kommt aus der Slowakei. Er ist _________.", "slowake", "Slovák"},
};

const TypedQ g06_rows[] = {
    {"Ich heiße Thomas. Ich spreche Deutsch. Ich komme aus D________.",
     "deutschland|eutschland", "Deutschland – Německo"},
    {"Ich heiße Martina. Ich spreche Slowakisch. Ich komme aus der S________.",
     "slowakei|lowakei", "Slowakei – Slovensko"},
    {"Ich heiße Petra. Ich spreche Deutsch. Ich komme aus Ö________.",
     "österreich|osterreich|sterreich", "Österreich – Rakousko"},
    {"Ich heiße Miguel. Ich spreche Spanisch. Ich komme aus S________.",
     "spanien|panien", "Spanien – Španělsko"},
    {"Ich heiße George. Ich spreche Englisch. Ich komme aus E________.",
     "england|ngland", "England – Anglie"},
    {"Ich heiße Maria. Ich spreche Polnisch. Ich komme aus P________.",
     "polen|olen", "Polen – Polsko"},
};

const TypedQ g07_rows[] = {
    {"1.  Ru__land", "russland|ss", "Russland – Rusko"},
    {"2.  T_rkei", "türkei|turkei|ü|u", "Türkei – Turecko"},
    {"3.  Slo__kei", "slowakei|wa", "Slowakei – Slovensko"},
    {"4.  Kro__ien", "kroatien|at", "Kroatien – Chorvatsko"},
    {"5.  Deut___land", "deutschland|sch", "Deutschland – Německo"},
    {"6.  Öst__reich", "österreich|osterreich|er", "Österreich – Rakousko"},
    {"7.  Gr__chenland", "griechenland|ie", "Griechenland – Řecko"},
    {"8.  Ita__en", "italien|li", "Italien – Itálie"},
};

const TypedQ g11_rows[] = {
    {"tergehvatereikauwauzeh", "vater", "otec"},
    {"berelternnemrerhobschaz", "eltern", "rodiče"},
    {"nefrastschechinreiabtab", "tschechin", "Češka"},
    {"hejkosstudentaldwsahujoi", "student", "student"},
};

const TypedQ s03_rows[] = {
    {"1.  🇨🇿  Bist du Russe? – Nein, ich bin _________.", "tscheche",
     "Čech"},
    {"2.  🇦🇹  Bist du Tschechin? – Nein, ich bin _________.", "österreicherin|osterreicherin",
     "Rakušanka"},
    {"3.  🇨🇭  Bist du Deutsche? – Nein, ich bin _________.", "schweizerin",
     "Švýcarka"},
    {"4.  🇩🇪  Bist du Engländer? – Nein, ich bin _________.", "deutscher",
     "Němec"},
    {"5.  🇨🇭  Bist du Amerikanerin? – Nein, ich bin _________.", "schweizerin",
     "Švýcarka"},
};

/* ---- G01 content ---------------------------------------------------- */

const VerbQ g01_peter[] = {
    {"Er ", " aus Österreich. (kommen)", "kommt"},
    {"Er ", " in Graz. (wohnen)", "wohnt"},
    {"Er ", " gern Golf. (spielen)", "spielt"},
    {"Er ", " Deutsch, Englisch und Italienisch. (sprechen)", "spricht"},
};

const VerbQ g01_jana[] = {
    {"Sie ", " aus der Slowakei. (kommen)", "kommen"},
    {"Sie ", " in Bratislava. (wohnen)", "wohnen"},
    {"Sie ", " gern. (tanzen)", "tanzt"},
    {"Er ", " gern. (reisen)", "reist"},
    {"Sie ", " gute Freunde. (sein)", "sind"},
};

const char *g01_pool[] = {
    "kommt", "kommst", "komme", "kommen",
    "wohnt", "wohnst", "wohne", "wohnen",
    "spielt", "spielst", "spiele", "spielen",
    "spricht", "sprichst", "spreche", "sprechen",
    "tanzt", "tanze", "tanzen",
    "reist", "reise", "reisen",
    "ist", "bist", "bin", "sind", "seid",
};

const char *g01_peter_mean[] = {
    "Pochází z Rakouska.",
    "Bydlí ve Štýrském Hradci (Graz).",
    "Rád hraje golf.",
    "Mluví německy, anglicky a italsky.",
};

const char *g01_jana_mean[] = {
    "Pocházejí ze Slovenska.",
    "Bydlí v Bratislavě (ona).",
    "Ona ráda tancuje.",
    "On rád cestuje.",
    "Jsou dobří přátelé.",
};

/* ---- G04 sprechen --------------------------------------------------- */

const VerbQ g04_rows[] = {
    {"• Ich ", " Deutsch und du?", "spreche"},
    {"• Welche Sprachen ", " Sie?", "sprechen"},
    {"• ", " du Französisch?", "sprichst"},
    {"• Wir ", " Polnisch und ihr?", "sprechen"},
    {"• ", " ihr Englisch?", "sprecht"},
    {"• Er ", " kein Wort Türkisch.", "spricht"},
};

const char *g04_pool[] = {
    "spreche", "sprichst", "spricht", "sprechen", "sprecht",
};

const char *g04_mean[] = {
    "Mluvím německy. A ty?",
    "Kterými jazyky mluvíte?",
    "Mluvíš francouzsky? – Ano, trochu.",
    "My mluvíme polsky a vy?",
    "Mluvíte anglicky? – Ano, velmi dobře.",
    "Nemluví ani slovo turecky.",
};

/* ---- G08 Verben einsetzen ------------------------------------------- */

const VerbQ g08_rows[] = {
    {"Was ", " er?  →  Ein Buch. (lesen)", "liest"},
    {"Was ", " sie?  →  Medizin. (studieren)", "studiert"},
    {"Was ", " Jan gern?  →  Auto. (fahren)", "fährt"},
    {"Wo ", " ihr?  →  In Wien. (leben)", "lebt"},
    {"Wer ", " bei Siemens?  →  Meine Freundin Vera. (arbeiten)",
     "arbeitet"},
};

const char *g08_pool[] = {
    "liest", "lese", "lest", "lesen",
    "studiert", "studiere", "studiert", "studieren",
    "fährt", "fahre", "fahrt", "fahren",
    "lebt", "lebe", "lebt", "leben",
    "arbeitet", "arbeite", "arbeitet", "arbeiten",
};

const char *g08_mean[] = {
    "Co čte? – Knihu.",
    "Co studuje? – Medicínu.",
    "Čím Jan rád jezdí? – Autem.",
    "Kde bydlíte? – Ve Vídni.",
    "Kdo pracuje u Siemensu? – Moje kamarádka Věra.",
};

/* ---- G10 Euro -------------------------------------------------------- */

const VerbQ g10_rows[] = {
    {"Julia hat ", " Euro.", "dreiundsechzig"},
    {"Tina hat ", " Euro.", "achtunddreißig"},
    {"Peter hat ", " Euro.", "fünfundvierzig"},
    {"Hans hat ", " Euro.", "siebenundfünfzig"},
    {"Barbara hat ", " Euro.", "neunundneunzig"},
};

const char *g10_pool[] = {
    "dreiundsechzig", "achtunddreißig", "fünfundvierzig",
    "siebenundfünfzig", "neunundneunzig",
};

const char *g10_mean[] = {
    "šedesát tři eur",
    "třicet osm eur",
    "čtyřicet pět eur",
    "padesát sedm eur",
    "devadesát devět eur",
};

/* ---- G14 Lückentext -------------------------------------------------- */

const VerbQ g14_rows[] = {
    {"Agnieszka Kowalski kommt ", " Polen, aus Kraków.", "aus"},
    {"Sie ist 18 Jahre alt und ", " dieses Jahr Abitur.", "macht"},
    {"Sie chattet gerne mit ihrem deutschen ", ".", "Freund"},
    {"Er heißt Stefan Böhmermann und ", " in Dresden.", "lebt"},
    {"Agnieszka findet die ", " cool.", "Sprache"},
    {"Sie spricht schon sehr ", " Deutsch.", "gut"},
};

const char *g14_pool[] = {
    "aus", "macht", "Freund", "lebt", "Sprache", "gut",
};

const char *g14_mean[] = {
    "Agnieszka Kowalski pochází z Polska, z Krakova.",
    "Je jí 18 let a letos dělá maturitu.",
    "Ráda chatuje se svým německým přítelem.",
    "Jmenuje se Stefan Böhmermann a bydlí v Drážďanech.",
    "Agnieszce přijde ten jazyk super.",
    "Už mluví velmi dobře německy.",
};

const char *g14_gloss[] = {
    "z", "dělá", "kamarád", "žije", "jazyk", "dobře",
};

/* ---- G15 Verbinde ---------------------------------------------------- */

const VerbQ g15_rows[] = {
    {"ein Handy ", "", "brauchen"},
    {"bei BMW ", "", "arbeiten"},
    {"Auto ", "", "fahren"},
    {"das Gymnasium ", "", "besuchen"},
    {"Spanisch ", "", "sprechen"},
    {"Krimis ", "", "lesen"},
};

const char *g15_pool[] = {
    "brauchen", "arbeiten", "fahren", "besuchen", "sprechen", "lesen",
};

const char *g15_mean[] = {
    "potřebovat mobil",
    "pracovat ve BMW",
    "řídit auto",
    "navštěvovat gymnázium",
    "mluvit španělsky",
    "číst detektivky",
};

const char *g15_gloss[] = {
    "potřebovat", "pracovat", "řídit",
    "navštěvovat", "mluvit", "číst",
};

/* ---- G16 Zahlenpaare -------------------------------------------------- */

const VerbQ g16_rows[] = {
    {"54  =  ", "", "vierundfünfzig"},
    {"45  =  ", "", "fünfundvierzig"},
    {"369  =  ", "", "dreihundertneunundsechzig"},
    {"112  =  ", "", "hundertzwölf"},
    {"122  =  ", "", "hundertzweiundzwanzig"},
    {"68  =  ", "", "achtundsechzig"},
};

const char *g16_pool[] = {
    "vierundfünfzig", "fünfundvierzig", "dreihundertneunundsechzig",
    "hundertzwölf", "hundertzweiundzwanzig", "achtundsechzig",
};

const char *g16_mean[] = {
    "padesát čtyři",
    "čtyřicet pět",
    "tři sta šedesát devět",
    "sto dvanáct",
    "sto dvacet dva",
    "šedesát osm",
};

/* ---- G02 choice ------------------------------------------------------ */

const ChoiceQ g02_rows[] = {
    {"1.  Ich komme ___ Athen.", {"aus", "in"}, 2, 0},
    {"2.  Peter wohnt ___ Salzburg.", {"aus", "in"}, 2, 1},
    {"3.  Berlin liegt ___ Deutschland.", {"aus", "in"}, 2, 1},
    {"4.  Wir wohnen ___ Paris.", {"aus", "in"}, 2, 1},
    {"5.  Sie kommen ___ Griechenland.", {"aus", "in"}, 2, 0},
};

const char *g02_mean[] = {
    "Pocházím z Atén.",
    "Petr bydlí v Salcburku.",
    "Berlín leží v Německu.",
    "Bydlíme v Paříži.",
    "Pocházejí z Řecka.",
};

/* ---- G03 Fragewörter -------------------------------------------------- */

const ChoiceQ g03_rows[] = {
    {"1.  ___ kommt er?  →  Aus London.", {"Wer", "Wie", "Wo", "Woher", "Was"},
     5, 3},
    {"2.  ___ heißt sie?  →  Andrea.", {"Wer", "Wie", "Wo", "Woher", "Was"},
     5, 1},
    {"3.  ___ kommt aus Russland?  →  Alexander.",
     {"Wer", "Wie", "Wo", "Woher", "Was"}, 5, 0},
    {"4.  ___ liegt Prag?  →  In Tschechien.", {"Wer", "Wie", "Wo", "Woher", "Was"},
     5, 2},
    {"5.  ___ wohnen sie?  →  In Bratislava.",
     {"Wer", "Wie", "Wo", "Woher", "Was"}, 5, 2},
    {"6.  ___ macht ihr?  →  Wir studieren Germanistik.",
     {"Wer", "Wie", "Wo", "Woher", "Was"}, 5, 4},
};

const char *g03_mean[] = {
    "Odkud je? – Z Londýna.",
    "Jak se jmenuje? – Andrea.",
    "Kdo pochází z Ruska? – Alexandr.",
    "Kde leží Praha? – V Česku.",
    "Kde bydlí? – V Bratislavě.",
    "Co děláte? – Studujeme germanistiku.",
};

/* ---- G12 Unterstreiche ------------------------------------------------ */

const ChoiceQ g12_rows[] = {
    {"1.  Peter ___ gern Bücher.", {"liest", "lest", "lesen"}, 3, 0},
    {"2.  Filip ___ in Salzburg.", {"arbeit", "arbeitet", "arbeite"}, 3, 1},
    {"3.  Magda ___ gut Englisch.", {"spricht", "sprecht", "sprechen"}, 3, 0},
    {"4.  Marek ___ das Gymnasium.", {"besuchst", "besucht", "besuchen"}, 3, 1},
    {"5.  Dominika ___ 15 Jahre alt.", {"ist", "seid", "bin"}, 3, 0},
    {"6.  Meine Schwester ___ Marta.", {"heiße", "heißt", "heißen"}, 3, 1},
};

const char *g12_mean[] = {
    "Petr rád čte knihy.",
    "Filip pracuje v Salcburku.",
    "Magda dobře mluví anglicky.",
    "Marek navštěvuje gymnázium.",
    "Dominice je 15 let.",
    "Moje sestra se jmenuje Marta.",
};

/* ---- G13 Ordne zu ----------------------------------------------------- */


const OrdneRow g13_rows[] = {
    {"heißen sie?", "Wie", "Jana und Michael."},
    {"spricht Polnisch?", "Wer", "Jacek."},
    {"wohnt Eva?", "Wo", "In Plzeň."},
    {"kommt Markus?", "Woher", "Aus Deutschland."},
    {"macht ihr?", "Was", "Wir studieren Germanistik."},
    {"spricht Jana?", "Was", "Slowakisch."},
};

const char *g13_fw_pool[] = {"Was", "Wer", "Wie", "Wo", "Woher"};

const char *g13_ans_pool[] = {
    "Jana und Michael.", "Jacek.", "In Plzeň.", "Aus Deutschland.",
    "Wir studieren Germanistik.", "Slowakisch.",
};

const char *g13_mean[] = {
    "Jak se jmenují? – Jana a Michael.",
    "Kdo mluví polsky? – Jacek.",
    "Kde bydlí Eva? – V Plzni.",
    "Odkud pochází Markus? – Z Německa.",
    "Co děláte? – Studujeme germanistiku.",
    "Jakým jazykem mluví Jana? – Slovensky.",
};

/* ---- G09 content (Freie Antwort) ------------------------------------- */

const FreeQ ex9_free_qs[] = {
    {"Wie heißt du?", "Jak se jmenuješ?",
     "Ich heiße Lukas.", "Jmenuji se Lukáš."},
    {"Wo wohnst du?", "Kde bydlíš?",
     "Ich wohne in Prag.", "Bydlím v Praze."},
    {"Wohin fährst du?", "Kam jedeš?",
     "Ich fahre nach Deutschland.", "Jedu do Německa."},
    {"Was liest du gern?", "Co rád/a čteš?",
     "Ich lese gern Krimis.", "Rád/a čtu detektivky."},
    {"Was spielst du gern?", "Co rád/a hraješ?",
     "Ich spiele gern Golf.", "Rád/a hraju golf."},
};

/* ---- S01 Steckbrief --------------------------------------------------- */


const ProfileQ s01_rows[] = {
    {"Ich heiße …", "Ich heiße Petra."},
    {"Ich komme …", "Ich komme aus Tschechien."},
    {"Ich lebe …", "Ich lebe in Prag."},
    {"Ich bin …", "Ich bin fünfzehn Jahre alt."},
    {"Ich spreche …", "Ich spreche Tschechisch und Deutsch."},
    {"Ich besuche …", "Ich besuche das Gymnasium."},
    {"Ich finde …", "Ich finde Deutsch toll."},
    {"Ich arbeite …", "Ich arbeite noch nicht."},
};

void s01_fill_sample(GtkWidget *label) {
    const ProfileQ *q = g_object_get_data(G_OBJECT(label), "pq");
    char *txt;

    if (!q)
        return;
    txt = g_strdup_printf(tr("sample_line"), q->sample);
    gtk_label_set_text(GTK_LABEL(label), txt);
    g_free(txt);
}


void s01_reveal(GtkButton *button, gpointer data) {
    GtkWidget *sample = data;
    (void)button;
    s01_fill_sample(sample);
    gtk_widget_set_visible(sample, !gtk_widget_get_visible(sample));
}

void s01_finish(GtkButton *button, gpointer data) {
    ProfileCtx *ctx = data;
    (void)button;
    set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
    for (int i = 0; i < ctx->n; i++) {
        s01_fill_sample(ctx->samples[i]);
        gtk_widget_set_visible(ctx->samples[i], TRUE);
    }
    mark_done(ctx->unit, ctx->ex_num);
}

GtkWidget *build_profile(UnitCtx *unit, int ex_num, const char *title,
                                const char *sub, const ProfileQ *qs, int n) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, title, sub, "finish",
                                    &body, &feedback, &check);
    ProfileCtx *ctx = g_new0(ProfileCtx, 1);

    ctx->unit = unit;
    ctx->ex_num = ex_num;
    ctx->feedback = feedback;
    ctx->n = n;
    ctx->samples = g_new0(GtkWidget *, n);

    for (int i = 0; i < n; i++) {
        GtkWidget *prompt = gtk_label_new(qs[i].stem);
        GtkWidget *entry;
        GtkWidget *reveal;
        GtkWidget *sample;

        gtk_widget_set_halign(prompt, GTK_ALIGN_START);
        gtk_widget_add_css_class(prompt, "ex-prompt");
        gtk_box_append(GTK_BOX(body), prompt);

        entry = gtk_entry_new();
        i18n_bind(entry, "your_answer", 3);
        gtk_box_append(GTK_BOX(body), entry);

        reveal = gtk_button_new();
        i18n_bind(reveal, "show_sample", 1);
        gtk_widget_add_css_class(reveal, "pill");
        gtk_widget_set_halign(reveal, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(body), reveal);

        sample = gtk_label_new(NULL);
        g_object_set_data(G_OBJECT(sample), "pq", (gpointer)&qs[i]);
        gtk_widget_set_halign(sample, GTK_ALIGN_START);
        gtk_widget_add_css_class(sample, "hint");
        gtk_label_set_wrap(GTK_LABEL(sample), TRUE);
        gtk_widget_set_visible(sample, FALSE);
        gtk_box_append(GTK_BOX(body), sample);
        ctx->samples[i] = sample;

        g_signal_connect(reveal, "clicked", G_CALLBACK(s01_reveal), sample);
    }

    g_signal_connect(check, "clicked", G_CALLBACK(s01_finish), ctx);
    return page;
}

/* ---- S02 Hangman (Berufe) --------------------------------------------- */


const char *hm_words[HM_WORDS] = {
    "KOCH", "ARZT", "LEHRER", "POLIZIST", "VERKÄUFER",
};

const char *hm_tips[HM_WORDS] = {
    "vaří v restauraci",
    "léčí nemocné lidi",
    "učí ve škole",
    "pracuje u policie",
    "prodává v obchodě",
};

/* QWERTY order, Ä/Ö/Ü in their own bottom row. */
const char *hm_letters[] = {
    "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
    "A", "S", "D", "F", "G", "H", "J", "K", "L",
    "Z", "X", "C", "V", "B", "N", "M",
    "Ä", "Ö", "Ü",
};

const gunichar hm_letters_u[] = {
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M',
    0x00c4, 0x00d6, 0x00dc,
};


int hm_char_index(gunichar ch) {
    for (int i = 0; i < HM_N_LETTERS; i++)
        if (hm_letters_u[i] == ch)
            return i;
    return -1;
}

void hm_update_slots(HangmanCtx *hm) {
    const char *w = hm_words[hm->word_index];
    const char *p = w;
    GString *out = g_string_new(NULL);

    while (*p) {
        gunichar ch = g_utf8_get_char(p);
        p = g_utf8_next_char(p);
        if (out->len > 0)
            g_string_append_c(out, ' ');
        int idx = hm_char_index(ch);
        if (idx >= 0 && hm->guessed[idx])
            g_string_append_unichar(out, ch);
        else
            g_string_append_c(out, '_');
    }
    gtk_label_set_text(GTK_LABEL(hm->slots), out->str);
    g_string_free(out, TRUE);
}

gboolean hm_word_solved(HangmanCtx *hm) {
    const char *w = hm_words[hm->word_index];
    const char *p = w;

    while (*p) {
        gunichar ch = g_utf8_get_char(p);
        p = g_utf8_next_char(p);
        int idx = hm_char_index(ch);
        if (idx < 0 || !hm->guessed[idx])
            return FALSE;
    }
    return TRUE;
}

void hm_set_letters_enabled(HangmanCtx *hm, gboolean enabled) {
    for (int i = 0; i < HM_N_LETTERS; i++) {
        if (hm->letter_btns[i])
            gtk_widget_set_sensitive(hm->letter_btns[i], enabled);
    }
}

void hm_redraw(HangmanCtx *hm) {
    if (hm->draw)
        gtk_widget_queue_draw(hm->draw);
}

void hm_clear_feedback(HangmanCtx *hm) {
    gtk_widget_remove_css_class(hm->feedback, "feedback-ok");
    gtk_widget_remove_css_class(hm->feedback, "feedback-err");
    gtk_label_set_text(GTK_LABEL(hm->feedback), "");
}

gboolean hm_advance(gpointer data) {
    HangmanCtx *hm = data;

    hm->locked = FALSE;
    if (hm->word_index + 1 >= HM_WORDS) {
        hm->finished = TRUE;
        set_feedback(hm->feedback, TRUE, tr("hm_done"));
        mark_done(hm->unit, hm->ex_num);
        return G_SOURCE_REMOVE;
    }
    hm->word_index++;
    hm->misses = 0;
    for (int i = 0; i < HM_N_LETTERS; i++)
        hm->guessed[i] = FALSE;
    hm_set_letters_enabled(hm, TRUE);
    hm_update_slots(hm);
    gtk_label_set_text(GTK_LABEL(hm->hint_lbl), tr(hm_tips[hm->word_index]));
    {
        char *t = g_strdup_printf(tr("hm_progress"), hm->word_index + 1,
                                  HM_WORDS);
        gtk_label_set_text(GTK_LABEL(hm->counter_lbl), t);
        g_free(t);
    }
    hm_clear_feedback(hm);
    hm_redraw(hm);
    return G_SOURCE_REMOVE;
}

void hm_reset_word(gpointer data) {
    HangmanCtx *hm = data;
    hm->misses = 0;
    for (int i = 0; i < HM_N_LETTERS; i++)
        hm->guessed[i] = FALSE;
    hm->locked = FALSE;
    hm_set_letters_enabled(hm, TRUE);
    hm_update_slots(hm);
    hm_clear_feedback(hm);
    hm_redraw(hm);
}

gboolean hm_retry_word(gpointer data) {
    hm_reset_word(data);
    return G_SOURCE_REMOVE;
}

void hm_guess_letter(GtkButton *button, gpointer data) {
    HangmanCtx *hm = data;
    int idx = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "li"));
    const char *w = hm_words[hm->word_index];
    const char *p = w;
    gboolean correct = FALSE;

    (void)button;
    if (hm->finished || hm->locked)
        return;
    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
    hm->guessed[idx] = TRUE;

    while (*p) {
        gunichar ch = g_utf8_get_char(p);
        p = g_utf8_next_char(p);
        if (hm_char_index(ch) == idx) {
            correct = TRUE;
            break;
        }
    }

    if (correct) {
        hm_update_slots(hm);
        if (hm_word_solved(hm)) {
            hm->locked = TRUE;
            hm_set_letters_enabled(hm, FALSE);
            set_feedback(hm->feedback, TRUE, tr("hm_wrong"));
            g_timeout_add(1100, hm_advance, hm);
        }
    } else {
        hm->misses++;
        hm_redraw(hm);
        if (hm->misses >= HM_MAX_MISSES) {
            char *msg = g_strdup_printf(tr("hm_fail"), w);
            hm->locked = TRUE;
            hm_set_letters_enabled(hm, FALSE);
            set_feedback(hm->feedback, FALSE, msg);
            g_free(msg);
            /* reveal the word, then restart it with empty letters */
            for (int i = 0; i < HM_N_LETTERS; i++)
                hm->guessed[i] = TRUE;
            hm_update_slots(hm);
            hm_redraw(hm);
            g_timeout_add(1600, hm_retry_word, hm);
        }
    }
}

void draw_hangman(GtkDrawingArea *area, cairo_t *cr,
                         int width, int height, gpointer data) {
    HangmanCtx *hm = data;
    Rgb c = color_from_hex(app_theme.subtext);
    double cx = width / 2.0 - 30.0;
    double ground = height - 10.0;
    int parts = hm->misses;

    (void)area;
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_source_rgb(cr, c.r, c.g, c.b);
    cairo_set_line_width(cr, 3.0);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

    /* scaffold */
    cairo_new_path(cr);
    cairo_move_to(cr, width * 0.15, ground);
    cairo_line_to(cr, width * 0.85, ground);
    cairo_move_to(cr, cx, ground);
    cairo_line_to(cr, cx, 20.0);
    cairo_line_to(cr, cx + 70.0, 20.0);
    cairo_move_to(cr, cx + 70.0, 20.0);
    cairo_line_to(cr, cx + 70.0, 42.0);
    cairo_stroke(cr);

    if (parts >= 1) {            /* head */
        cairo_new_path(cr);
        cairo_arc(cr, cx + 70.0, 54.0, 12.0, 0.0, 2.0 * G_PI);
        cairo_stroke(cr);
    }
    if (parts >= 2) {            /* body */
        cairo_new_path(cr);
        cairo_move_to(cr, cx + 70.0, 66.0);
        cairo_line_to(cr, cx + 70.0, 108.0);
        cairo_stroke(cr);
    }
    if (parts >= 3) {            /* left arm */
        cairo_new_path(cr);
        cairo_move_to(cr, cx + 70.0, 76.0);
        cairo_line_to(cr, cx + 50.0, 96.0);
        cairo_stroke(cr);
    }
    if (parts >= 4) {            /* right arm */
        cairo_new_path(cr);
        cairo_move_to(cr, cx + 70.0, 76.0);
        cairo_line_to(cr, cx + 90.0, 96.0);
        cairo_stroke(cr);
    }
    if (parts >= 5) {            /* left leg */
        cairo_new_path(cr);
        cairo_move_to(cr, cx + 70.0, 108.0);
        cairo_line_to(cr, cx + 50.0, 132.0);
        cairo_stroke(cr);
    }
    if (parts >= 6) {            /* right leg */
        cairo_new_path(cr);
        cairo_move_to(cr, cx + 70.0, 108.0);
        cairo_line_to(cr, cx + 90.0, 132.0);
        cairo_stroke(cr);
    }
}

GtkWidget *build_hangman(UnitCtx *unit, int ex_num, const char *title,
                                const char *sub) {
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *wrap = gtk_center_box_new();
    GtkWidget *content;
    HangmanCtx *hm = g_new0(HangmanCtx, 1);
    GtkWidget *hint_row;
    GtkWidget *counter;
    GtkWidget *slots;
    GtkWidget *tip_hdr;
    GtkWidget *hint_lbl;
    GtkWidget *letters;
    GtkWidget *head;
    GtkWidget *guess_lbl;
    char *tmp;

    hm->unit = unit;
    hm->ex_num = ex_num;

    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);
    gtk_box_append(GTK_BOX(page), top_bar(unit->page, title, sub));

    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_box_append(GTK_BOX(page), wrap);

    content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_halign(content, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(content, GTK_ALIGN_CENTER);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), content);

    counter = gtk_label_new(NULL);
    hm->counter_lbl = counter;
    gtk_widget_add_css_class(counter, "ex-sub");
    gtk_box_append(GTK_BOX(content), counter);

    head = gtk_drawing_area_new();
    gtk_widget_set_size_request(head, 220, 170);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(head), draw_hangman,
                                   hm, NULL);
    hm->draw = head;
    gtk_box_append(GTK_BOX(content), head);

    slots = gtk_label_new(NULL);
    gtk_widget_add_css_class(slots, "hm-word");
    hm->slots = slots;
    gtk_box_append(GTK_BOX(content), slots);

    hint_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_halign(hint_row, GTK_ALIGN_CENTER);
    tip_hdr = gtk_label_new(NULL);
    i18n_bind(tip_hdr, "hm_hint", 0);
    gtk_widget_add_css_class(tip_hdr, "hint");
    hint_lbl = gtk_label_new(NULL);
    gtk_widget_add_css_class(hint_lbl, "hint");
    hm->hint_lbl = hint_lbl;
    gtk_box_append(GTK_BOX(hint_row), tip_hdr);
    gtk_box_append(GTK_BOX(hint_row), hint_lbl);
    gtk_box_append(GTK_BOX(content), hint_row);

    guess_lbl = gtk_label_new(NULL);
    i18n_bind(guess_lbl, "hm_guess", 0);
    gtk_widget_add_css_class(guess_lbl, "ex-sub");
    gtk_box_append(GTK_BOX(content), guess_lbl);

    letters = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_append(GTK_BOX(content), letters);

    hm->letter_btns = g_new0(GtkWidget *, HM_N_LETTERS);
    {
        static const int kb_start[4] = { 0, HM_ROW2, HM_ROW3, HM_ROW4 };
        static const int kb_end[4] = { HM_ROW2, HM_ROW3, HM_ROW4,
                                       HM_N_LETTERS };
        for (int r = 0; r < 4; r++) {
            GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
            gtk_widget_set_halign(row, GTK_ALIGN_CENTER);
            gtk_widget_set_margin_top(row, r == 3 ? 4 : 0);
            for (int i = kb_start[r]; i < kb_end[r]; i++) {
                GtkWidget *b = gtk_button_new_with_label(hm_letters[i]);
                gtk_widget_add_css_class(b, "pill");
                gtk_widget_add_css_class(b, "hm-key");
                g_object_set_data(G_OBJECT(b), "li", GINT_TO_POINTER(i));
                g_signal_connect(b, "clicked", G_CALLBACK(hm_guess_letter), hm);
                gtk_box_append(GTK_BOX(row), b);
                hm->letter_btns[i] = b;
            }
            gtk_box_append(GTK_BOX(letters), row);
        }
    }

    hm->feedback = gtk_label_new(NULL);
    gtk_widget_set_halign(hm->feedback, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(hm->feedback, 6);
    gtk_box_append(GTK_BOX(content), hm->feedback);

    gtk_label_set_text(GTK_LABEL(hint_lbl), tr(hm_tips[0]));
    tmp = g_strdup_printf(tr("hm_progress"), 1, HM_WORDS);
    gtk_label_set_text(GTK_LABEL(counter), tmp);
    g_free(tmp);
    hm_update_slots(hm);

    return page;
}

/* ---- per-exercise builders for unit 2 ------------------------------- */

GtkWidget *u2ex1(UnitCtx *unit) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, "Verben konjugieren",
                                    "sub_verben", "check",
                                    &body, &feedback, &check);
    ComboListCtx *ctx = combo_list_ctx_new(unit, 1, feedback);
    int counter = 0;

    ctx->trans_upfront = TRUE;
    ctx->reveal_german = TRUE;

    GtkWidget *h1 = gtk_label_new("Peter Fritsch:");
    gtk_widget_set_halign(h1, GTK_ALIGN_START);
    gtk_widget_add_css_class(h1, "ex-sub");
    gtk_box_append(GTK_BOX(body), h1);
    verb_rows_add(ctx, body, &counter, g01_peter, G_N_ELEMENTS(g01_peter),
                  g01_pool, G_N_ELEMENTS(g01_pool), g01_peter_mean, FALSE);

    GtkWidget *h2 = gtk_label_new("Jana Nová und Pavol Korčák:");
    gtk_widget_set_halign(h2, GTK_ALIGN_START);
    gtk_widget_add_css_class(h2, "ex-sub");
    gtk_widget_set_margin_top(h2, 10);
    gtk_box_append(GTK_BOX(body), h2);
    verb_rows_add(ctx, body, &counter, g01_jana, G_N_ELEMENTS(g01_jana),
                  g01_pool, G_N_ELEMENTS(g01_pool), g01_jana_mean, FALSE);

    g_signal_connect(check, "clicked", G_CALLBACK(combo_list_check), ctx);
    return page;
}

GtkWidget *u2ex2(UnitCtx *unit) {
    return build_choice(unit, "aus oder in", "sub_ausin", 2,
                        g02_rows, g02_mean, G_N_ELEMENTS(g02_rows));
}

GtkWidget *u2ex3(UnitCtx *unit) {
    return build_choice(unit, "Fragewörter", "sub_wer", 3,
                        g03_rows, g03_mean, G_N_ELEMENTS(g03_rows));
}

GtkWidget *u2ex4(UnitCtx *unit) {
    return build_verb_ex(unit, 4, "sprechen", "sub_sprich",
                         g04_rows, G_N_ELEMENTS(g04_rows),
                         g04_pool, G_N_ELEMENTS(g04_pool), g04_mean, FALSE,
                         NULL);
}

GtkWidget *u2ex5(UnitCtx *unit) {
    return build_typed(unit, 5, "Nationalitäten", "sub_nation",
                       g05_rows, G_N_ELEMENTS(g05_rows), g05_bank, FALSE);
}

GtkWidget *u2ex6(UnitCtx *unit) {
    return build_typed(unit, 6, "Woher?", "sub_woher",
                       g06_rows, G_N_ELEMENTS(g06_rows), NULL, FALSE);
}

GtkWidget *u2ex7(UnitCtx *unit) {
    return build_typed(unit, 7, "Länder schreiben", "sub_laender",
                       g07_rows, G_N_ELEMENTS(g07_rows), NULL, FALSE);
}

GtkWidget *u2ex8(UnitCtx *unit) {
    return build_verb_ex(unit, 8, "Verben einsetzen", "sub_verb2",
                         g08_rows, G_N_ELEMENTS(g08_rows),
                         g08_pool, G_N_ELEMENTS(g08_pool), g08_mean, FALSE,
                         NULL);
}

GtkWidget *u2ex9(UnitCtx *unit) {
    return build_free_answer(unit, 9, "Freie Antwort", "sub_free", "tip_ss",
                             ex9_free_qs, (int)G_N_ELEMENTS(ex9_free_qs));
}

GtkWidget *u2ex10(UnitCtx *unit) {
    return build_verb_ex(unit, 10, "Euro", "sub_euro",
                         g10_rows, G_N_ELEMENTS(g10_rows),
                         g10_pool, G_N_ELEMENTS(g10_pool), g10_mean, TRUE,
                         NULL);
}

GtkWidget *u2ex11(UnitCtx *unit) {
    return build_typed(unit, 11, "Wörter suchen", "sub_wortsuchen",
                       g11_rows, G_N_ELEMENTS(g11_rows), NULL, FALSE);
}

GtkWidget *u2ex12(UnitCtx *unit) {
    return build_choice(unit, "Was ist richtig?", "sub_verb2", 12,
                        g12_rows, g12_mean, G_N_ELEMENTS(g12_rows));
}

GtkWidget *u2ex13(UnitCtx *unit) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, "Ordne zu", "sub_ordne",
                                    "check", &body, &feedback, &check);
    ComboListCtx *ctx = combo_list_ctx_new(unit, 13, feedback);

    for (guint i = 0; i < G_N_ELEMENTS(g13_rows); i++) {
        const OrdneRow *r = &g13_rows[i];
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        GtkWidget *num = gtk_label_new(NULL);
        GtkWidget *mid = gtk_label_new(r->mid);
        GtkWidget *arrow = gtk_label_new("→");
        GtkWidget *fw = make_word_combo(g13_fw_pool,
                                        G_N_ELEMENTS(g13_fw_pool));
        GtkWidget *ans = make_word_combo(g13_ans_pool,
                                         G_N_ELEMENTS(g13_ans_pool));
        char *numtxt = g_strdup_printf("%d.", (int)(i + 1));

        gtk_widget_set_halign(row, GTK_ALIGN_START);
        gtk_label_set_text(GTK_LABEL(num), numtxt);
        g_free(numtxt);
        gtk_widget_set_valign(num, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(num, "ex-prompt");
        gtk_box_append(GTK_BOX(row), num);

        gtk_widget_set_valign(fw, GTK_ALIGN_CENTER);
        gtk_widget_set_size_request(fw, 110, -1);
        gtk_box_append(GTK_BOX(row), fw);
        combo_list_add(ctx, fw, r->fw);

        gtk_widget_set_valign(mid, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(mid, "ex-prompt");
        gtk_box_append(GTK_BOX(row), mid);

        gtk_widget_set_valign(arrow, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(arrow, "ex-prompt");
        gtk_box_append(GTK_BOX(row), arrow);

        gtk_widget_set_valign(ans, GTK_ALIGN_CENTER);
        gtk_widget_set_size_request(ans, 190, -1);
        gtk_box_append(GTK_BOX(row), ans);
        combo_list_add(ctx, ans, r->ans);

        gtk_box_append(GTK_BOX(body), row);
        combo_list_add_trans(ctx, meaning_add(body, g13_mean[i]));
    }

    g_signal_connect(check, "clicked", G_CALLBACK(combo_list_check), ctx);
    return page;
}

GtkWidget *u2ex14(UnitCtx *unit) {
    return build_verb_ex(unit, 14, "Lückentext", "sub_luecke",
                         g14_rows, G_N_ELEMENTS(g14_rows),
                         g14_pool, G_N_ELEMENTS(g14_pool), g14_mean, TRUE,
                         g14_gloss);
}

GtkWidget *u2ex15(UnitCtx *unit) {
    return build_verb_ex(unit, 15, "Verbinde", "sub_verbinde",
                         g15_rows, G_N_ELEMENTS(g15_rows),
                         g15_pool, G_N_ELEMENTS(g15_pool), g15_mean, TRUE,
                         g15_gloss);
}

GtkWidget *u2ex16(UnitCtx *unit) {
    return build_verb_ex(unit, 16, "Zahlen", "sub_zahlpaar",
                         g16_rows, G_N_ELEMENTS(g16_rows),
                         g16_pool, G_N_ELEMENTS(g16_pool), g16_mean, FALSE,
                         NULL);
}

GtkWidget *u2ex17(UnitCtx *unit) {
    return build_profile(unit, 17, "Steckbrief", "sub_steckbrief",
                         s01_rows, G_N_ELEMENTS(s01_rows));
}

GtkWidget *u2ex18(UnitCtx *unit) {
    return build_hangman(unit, 18, "Berufe", "sub_berufe");
}

GtkWidget *u2ex19(UnitCtx *unit) {
    return build_typed(unit, 19, "Nationalität", "sub_bistdu",
                       s03_rows, G_N_ELEMENTS(s03_rows), NULL, TRUE);
}

/* ------------------------------------------------------------------ */
/* Unit 2 vocabulary (off-path Vokabeltraining branch)                */
/* ------------------------------------------------------------------ */

const TypedQ u2_trans_p22[] = {
    {"arbeiten (Infinitiv)", "pracovat|pracuje|to work", "pracovat"},
    {"das Auto, -s", "auto|car", "auto"},
    {"brauchen (Infinitiv)", "potřebovat|potřebuje|to need", "potřebovat"},
    {"die Britin, -nen", "britka|british woman", "Britka"},
    {"britisch", "britský|british", "britský"},
    {"deutsch", "německý|german", "německý"},
    {"das Deutsch",
     "němčina|německy|němčina, německy|german", "němčina, německy"},
    {"die Eisschnelläuferin, -nen",
     "rychlobruslařka|speed skater", "rychlobruslařka"},
    {"fahren (Infinitiv)", "jezdit|jet|řídit|to drive|to go", "jezdit"},
    {"Er fährt gern Auto.",
     "rád řídí auto|rád jezdí autem|he likes driving a car",
     "Rád řídí auto."},
    {"Frankreich", "francie|france", "Francie"},
    {"für", "pro|za|for", "pro, za"},
    {"der Fußballspieler", "fotbalista|footballer|football player",
     "fotbalista"},
    {"das Jahrhunderttalent, -e",
     "talent století|talent of the century", "talent století"},
    {"das Kind, -er", "dítě|child", "dítě"},
    {"lesen (Infinitiv)", "číst|to read", "číst"},
    {"das Model, -s", "model|modelka|model, modelka", "model, modelka"},
    {"die Nummer, -n", "číslo|number", "číslo"},
    {"die Schauspielerin, -nen", "herečka|actress", "herečka"},
    {"der Schüler, -", "žák|pupil|schoolboy", "žák"},
    {"die Sportinitiative, -n",
     "sportovní iniciativa|sports initiative", "sportovní iniciativa"},
    {"sprechen (Infinitiv)", "mluvit|to speak", "mluvit"},
    {"Sie spricht Englisch.",
     "mluví anglicky|ona mluví anglicky|she speaks english",
     "Mluví anglicky."},
    {"der Trainer, -", "trenér|coach|trainer", "trenér"},
};

const TypedQ u2_trans_p23[] = {
    {"besuchen (Infinitiv)", "navštěvovat|navštívit|to visit",
     "navštěvovat, navštívit"},
    {"Er besucht das Gymnasium.",
     "chodí na gymnázium|navštěvuje gymnázium|he attends grammar school",
     "Chodí na gymnázium."},
    {"dort", "tam|there", "tam"},
    {"dreifach", "trojnásobný|threefold|triple", "trojnásobný"},
    {"gewinnen (Infinitiv)", "vyhrát|vyhrávat|to win", "vyhrát, vyhrávat"},
    {"die Goldmedaille, -n", "zlatá medaile|gold medal", "zlatá medaile"},
    {"das Gymnasium, Gymnasien", "gymnázium|grammar school", "gymnázium"},
    {"heute", "dnes|dneska|today", "dnes"},
    {"das Kroatisch",
     "chorvatština|chorvatsky|chorvatština, chorvatsky|croatian",
     "chorvatština, chorvatsky"},
    {"leben (Infinitiv)", "žít|to live", "žít"},
    {"Sie lebt in Österreich.", "žije v rakousku|she lives in austria",
     "Žije v Rakousku."},
    {"die Muttersprache, -n", "mateřský jazyk|mother tongue",
     "mateřský jazyk"},
    {"oft", "často|often", "často"},
    {"Österreich", "rakousko|austria", "Rakousko"},
    {"die Rad(renn)fahrerin, -nen", "cyklistka|cyclist", "cyklistka"},
    {"das Spanisch",
     "španělština|španělsky|španělština, španělsky|spanish",
     "španělština, španělsky"},
    {"die Sportlerin, -nen", "sportovkyně|sportswoman|athlete",
     "sportovkyně"},
    {"sportlich", "sportovní|sporty", "sportovní"},
    {"tschechisch", "český|czech", "český"},
    {"das Tschechisch",
     "čeština|česky|čeština, česky|czech", "čeština, česky"},
};

const TypedQ u2_trans_p24[] = {
    {"das Amerika", "amerika|america", "Amerika"},
    {"die Assistentin, -nen", "asistentka|assistant", "asistentka"},
    {"der Beruf, -e", "povolání|profese|job|profession", "povolání"},
    {"Was ist er von Beruf?",
     "jaké má povolání|čím je|what is his job", "Jaké má povolání?"},
    {"Deutschland", "německo|germany", "Německo"},
    {"England", "anglie|england", "Anglie"},
    {"die Flagge, -n", "vlajka|flag", "vlajka"},
    {"Griechenland", "řecko|greece", "Řecko"},
    {"das Griechisch",
     "řečtina|řecky|řečtina, řecky|greek", "řečtina, řecky"},
    {"Großbritannien", "velká británie|great britain", "Velká Británie"},
    {"die Herkunft", "původ|origin", "původ"},
    {"Italien", "itálie|italy", "Itálie"},
    {"das Italienisch",
     "italština|italsky|italština, italsky|italian", "italština, italsky"},
    {"Kroatien", "chorvatsko|croatia", "Chorvatsko"},
    {"das Land, Länder", "země|stát|country", "země, stát"},
    {"der Lehrer, -", "učitel|teacher", "učitel"},
    {"die Lehrerin, -nen", "učitelka|teacher", "učitelka"},
    {"der Manager, -", "manažer|manager", "manažer"},
    {"Polen", "polsko|poland", "Polsko"},
    {"das Polnisch",
     "polština|polsky|polština, polsky|polish", "polština, polsky"},
    {"das Russisch",
     "ruština|rusky|ruština, rusky|russian", "ruština, rusky"},
    {"Russland", "rusko|russia", "Rusko"},
    {"die Schweiz", "švýcarsko|switzerland", "Švýcarsko"},
    {"die Slowakei", "slovensko|slovakia", "Slovensko"},
    {"das Slowakisch",
     "slovenština|slovensky|slovenština, slovensky|slovak",
     "slovenština, slovensky"},
    {"Spanien", "španělsko|spain", "Španělsko"},
    {"die Sprache, -n", "jazyk|řeč|language", "jazyk, řeč"},
    {"der Student, -en", "student|student", "student"},
    {"die Studentin, -nen", "studentka|student", "studentka"},
    {"Tschechien", "česko|czechia", "Česko"},
    {"die Türkei", "turecko|turkey", "Turecko"},
    {"das Türkisch",
     "turečtina|turecky|turečtina, turecky|turkish", "turečtina, turecky"},
    {"das Ungarisch",
     "maďarština|maďarsky|maďarština, maďarsky|hungarian",
     "maďarština, maďarsky"},
    {"Ungarn", "maďarsko|hungary", "Maďarsko"},
    {"die Verkäuferin, -nen", "prodavačka|shop assistant|saleswoman",
     "prodavačka"},
    {"welcher/welche/welches", "jaký|jaká|jaké|which", "jaký/jaká/jaké"},
    {"Welche Sprachen spricht sie?",
     "jakými jazyky mluví|které jazyky mluví|which languages does she speak",
     "Jakými jazyky mluví?"},
    {"super", "super|skvělý|great|super, skvělý", "super, skvělý"},
    {"wenig", "málo|little|few", "málo"},
    {"ein wenig", "trochu|a little|a bit", "trochu"},
    {"das Wort, Wörter", "slovo|word", "slovo"},
};

const TypedQ u2_trans_p25[] = {
    {"das Abitur, -e", "maturita|a-levels", "maturita"},
    {"der Austausch, -e", "výměna|exchange", "výměna"},
    {"ein bisschen", "trochu|a bit|a little", "trochu"},
    {"finden (Infinitiv)",
     "najít|považovat|shledávat|myslet si|to find",
     "najít, považovat"},
    {"Sie findet den Kurs...",
     "kurz se jí líbí|kurz shledává skvělým|she likes the course",
     "Kurz se jí líbí. (Kurz shledává skvělým.)"},
    {"interessant", "zajímavý|interesting", "zajímavý"},
    {"der Kurs, -e", "kurz|course", "kurz"},
    {"leider", "bohužel|unfortunately", "bohužel"},
    {"mein/meine/mein", "můj|moje|my", "můj/moje/moje"},
    {"der Mensch, -en", "člověk|human|person", "člověk"},
    {"die Mutter, Mütter", "matka|mother", "matka"},
    {"noch", "ještě|still", "ještě"},
    {"Portugal", "portugalsko|portugal", "Portugalsko"},
    {"das Portugiesisch",
     "portugalština|portugalsky|portugalština, portugalsky|portuguese",
     "portugalština, portugalsky"},
    {"Schweden", "švédsko|sweden", "Švédsko"},
    {"das Schwedisch",
     "švédština|švédsky|švédština, švédsky|swedish", "švédština, švédsky"},
    {"sehr", "velmi|very", "velmi"},
    {"nicht sehr gut", "ne moc dobře|not very well", "ne moc dobře"},
    {"studieren (Infinitiv)", "studovat|to study", "studovat"},
    {"das Südamerika", "jižní amerika|south america", "Jižní Amerika"},
    {"die USA (mn. č.)", "usa|united states|spojené státy", "USA"},
    {"viel", "hodně|much|a lot", "hodně"},
    {"wie viel", "kolik|how much|how many", "kolik"},
};

const TypedQ u2_trans_p26a[] = {
    {"Deutscher/Deutsche", "němec|němka|němec/němka", "Němec/Němka"},
    {"Engländer/Engländerin",
     "angličan|angličanka|angličan/angličanka", "Angličan/Angličanka"},
    {"Franzose/Französin",
     "francouz|francouzka|francouz/francouzka", "Francouz/Francouzka"},
    {"Italiener/Italienerin", "ital|italka|ital/italka", "Ital/Italka"},
    {"die Nationalität, -en", "národnost|nationality", "národnost"},
    {"Österreicher/Österreicherin",
     "rakušan|rakušanka|rakušan/rakušanka", "Rakušan/Rakušanka"},
    {"Russe/Russin", "rus|ruska|rus/ruska", "Rus/Ruska"},
    {"Schweizer/Schweizerin",
     "švýcar|švýcarka|švýcar/švýcarka", "Švýcar/Švýcarka"},
    {"Slowake/Slowakin", "slovák|slovenka|slovák/slovenka",
     "Slovák/Slovenka"},
    {"Spanier/Spanierin", "španěl|španělka|španěl/španělka",
     "Španěl/Španělka"},
    {"Tscheche/Tschechin", "čech|češka|čech/češka", "Čech/Češka"},
};

const TypedQ u2_trans_p26b[] = {
    {"dreißig", "30|třicet", "třicet (30)"},
    {"vierzig", "40|čtyřicet", "čtyřicet (40)"},
    {"fünfzig", "50|padesát", "padesát (50)"},
    {"sechzig", "60|šedesát", "šedesát (60)"},
    {"siebzig", "70|sedmdesát", "sedmdesát (70)"},
    {"achtzig", "80|osmdesát", "osmdesát (80)"},
    {"neunzig", "90|devadesát", "devadesát (90)"},
    {"(ein)hundert", "100|sto", "sto (100)"},
    {"(ein)tausend", "1000|tisíc", "tisíc (1000)"},
};

const TypedQ u2_trans_p27[] = {
    {"das Afrika", "afrika|africa", "Afrika"},
    {"die Arbeit, -en", "práce|work|job", "práce"},
    {"Asien", "asie|asia", "Asie"},
    {"das Australien", "austrálie|australia", "Austrálie"},
    {"beginnen (Infinitiv)", "začít|začínat|to begin|to start",
     "začít, začínat"},
    {"danke", "děkuji|díky|thanks|thank you", "děkuji, díky"},
    {"das Europa", "evropa|europe", "Evropa"},
    {"ganz", "celý|whole", "celý"},
    {"gehen (Infinitiv)", "jít|chodit|to go", "jít"},
    {"Wie geht es Ihnen?", "jak se máte|how are you", "Jak se máte?"},
    {"Wie geht es dir?", "jak se máš|how are you", "Jak se máš?"},
    {"genau", "přesně|exactly", "přesně"},
    {"morgen", "zítra|tomorrow", "zítra"},
    {"nur", "jen|only", "jen"},
    {"der Rekord, -e", "rekord|record", "rekord"},
    {"schon", "již|už|already", "již, už"},
    {"das Semester, -", "semestr|semester", "semestr"},
    {"viel", "hodně|much|a lot", "hodně"},
    {"wie viel", "kolik|how much|how many", "kolik"},
    {"die Welt", "svět|world", "svět"},
    {"aus der ganzen Welt", "z celého světa|from all over the world",
     "z celého světa"},
};

const TransSection u2_trans_sections[] = {
    {"Strana 22", u2_trans_p22, (int)G_N_ELEMENTS(u2_trans_p22)},
    {"Strana 23", u2_trans_p23, (int)G_N_ELEMENTS(u2_trans_p23)},
    {"Strana 24", u2_trans_p24, (int)G_N_ELEMENTS(u2_trans_p24)},
    {"Strana 25", u2_trans_p25, (int)G_N_ELEMENTS(u2_trans_p25)},
    {"Strana 26", u2_trans_p26a, (int)G_N_ELEMENTS(u2_trans_p26a)},
    {"Strana 26", u2_trans_p26b, (int)G_N_ELEMENTS(u2_trans_p26b)},
    {"Strana 27", u2_trans_p27, (int)G_N_ELEMENTS(u2_trans_p27)},
};
const int u2_trans_sections_n = (int)G_N_ELEMENTS(u2_trans_sections);

