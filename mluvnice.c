#include "maturita.h"

#define MLUV_N_EX 20
#define MLUV_MAX_BLANK 5
#define MLUV_MAX_OPT 5

typedef struct {
    const char *prompt;
    const char *shown;
    int nblank;
    const char *hint[MLUV_MAX_BLANK];
    const char *answers[MLUV_MAX_BLANK];
} MluvItem;

typedef struct {
    const char *prompt;
    const char *solution;
} MluvRevealItem;

typedef struct {
    const MluvItem *items;
    int n;
    UnitCtx *unit;
    int ex_num;
    GtkWidget *feedback;
    GtkWidget **entries;
    GtkWidget **reveal;
} MluvCtx;

typedef struct {
    const MluvRevealItem *items;
    int n;
    UnitCtx *unit;
    int ex_num;
    GtkWidget *feedback;
    GtkWidget **sol;
} MluvRevealCtx;

typedef struct {
    const ChoiceQ *qs;
    const char **expls;
    int n;
    UnitCtx *unit;
    int ex_num;
    GtkWidget *feedback;
    GtkToggleButton **toggles;
    GtkWidget **expl_labels;
} MluvChoiceCtx;

static UnitCtx mluv_unit;

static const char *mluv_names[MLUV_N_EX] = {
    "i/y", "Velká písmena", "Slovní druhy", "Podst. jména", "Slovesa",
    "Slovotvorba", "Větné členy", "Vedlejší věty", "Synonyma", "Antonyma",
    "s/z", "Najdi chybu", "Tvary", "Test A–D", "Přímá řeč",
    "Rozdíly", "Interpunkce", "Obrazná pojmen.", "Slohové útvary",
    "Opakování",
};

void mluvnice_init(void) {
    mluv_unit.title = "Mluvnice";
    mluv_unit.page = "mluvnice";
    mluv_unit.ex_tag = "mluv";
    mluv_unit.sub_key = "Cvičení z mluvnice – styl maturita / "
                        "přijímačky z ČJL";
    mluv_unit.back_target = "czechmap";
    mluv_unit.progress_file = app_progress_mluvnice;
    mluv_unit.n_ex = MLUV_N_EX;
    for (int i = 0; i < MLUV_N_EX; i++)
        mluv_unit.ex_names[i + 1] = mluv_names[i];
    unit_load_progress(&mluv_unit);
}

void progress_for_mluvnice(ProgressSum *out) {
    int d;

    if (!out || mluv_unit.n_ex <= 0)
        return;
    d = unit_done_count(&mluv_unit);
    out->total_ex += mluv_unit.n_ex;
    out->open_units += 1;
    out->done_ex += d;
    if (d == mluv_unit.n_ex)
        out->done_units++;
}

void mluvnice_refresh_ui(void) {
    for (int i = 0; i < mluv_unit.n_ex; i++) {
        GtkWidget *btn = mluv_unit.ex_cells[i];

        if (!btn)
            continue;
        if (mluv_unit.done[i + 1]) {
            gtk_widget_add_css_class(btn, "done");
            if (mluv_unit.ex_icons[i + 1])
                gtk_widget_set_visible(mluv_unit.ex_icons[i + 1], TRUE);
        } else {
            gtk_widget_remove_css_class(btn, "done");
            if (mluv_unit.ex_icons[i + 1])
                gtk_widget_set_visible(mluv_unit.ex_icons[i + 1], FALSE);
        }
    }

    if (mluv_unit.ex_rail)
        gtk_widget_queue_draw(mluv_unit.ex_rail);
}

static void mluv_check(GtkButton *button, gpointer data) {
    MluvCtx *ctx = data;
    int total = 0;
    int ok = 0;

    (void)button;

    for (int i = 0; i < ctx->n; i++) {
        const MluvItem *it = &ctx->items[i];

        for (int j = 0; j < it->nblank; j++) {
            GtkWidget *e = ctx->entries[i * MLUV_MAX_BLANK + j];
            const gchar *txt = gtk_editable_get_text(GTK_EDITABLE(e));
            gchar *norm = normalize_answer(txt);
            gboolean good = txt && txt[0] &&
                            answer_accepts(norm, it->answers[j]);

            g_free(norm);
            answer_mark(e, good);
            total++;
            if (good)
                ok++;
        }
        if (ctx->reveal[i])
            gtk_widget_set_visible(ctx->reveal[i], TRUE);
    }

    if (total > 0 && ok == total) {
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        mark_done(ctx->unit, ctx->ex_num);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry"));
    }
}

static GtkWidget *mluv_typed_page(int ex_num, const char *title,
                                  const char *sub, const char *note,
                                  const MluvItem *items, int n) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell("mluvnice", title, sub, "check",
                                    &body, &feedback, &check);
    MluvCtx *ctx = g_new0(MluvCtx, 1);

    ctx->items = items;
    ctx->n = n;
    ctx->unit = &mluv_unit;
    ctx->ex_num = ex_num;
    ctx->feedback = feedback;
    ctx->entries = g_new0(GtkWidget *, n * MLUV_MAX_BLANK);
    ctx->reveal = g_new0(GtkWidget *, n);

    if (note) {
        GtkWidget *l = gtk_label_new(note);

        gtk_widget_set_halign(l, GTK_ALIGN_START);
        gtk_label_set_wrap(GTK_LABEL(l), TRUE);
        gtk_label_set_xalign(GTK_LABEL(l), 0.0);
        gtk_widget_add_css_class(l, "hint");
        gtk_widget_set_margin_bottom(l, 4);
        gtk_box_append(GTK_BOX(body), l);
    }

    for (int i = 0; i < n; i++) {
        const MluvItem *it = &items[i];
        GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        GtkWidget *prompt = gtk_label_new(it->prompt);
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        char *rev = g_strdup_printf("Řešení: %s", it->shown);
        GtkWidget *rl = gtk_label_new(rev);

        g_free(rev);
        gtk_widget_add_css_class(card, "notes-card");
        gtk_box_append(GTK_BOX(body), card);

        gtk_widget_set_halign(prompt, GTK_ALIGN_START);
        gtk_label_set_wrap(GTK_LABEL(prompt), TRUE);
        gtk_label_set_xalign(GTK_LABEL(prompt), 0.0);
        gtk_widget_add_css_class(prompt, "ex-prompt");
        gtk_box_append(GTK_BOX(card), prompt);

        gtk_widget_set_halign(row, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(card), row);

        for (int j = 0; j < it->nblank; j++) {
            GtkWidget *e = gtk_entry_new();

            gtk_editable_set_width_chars(GTK_EDITABLE(e), 18);
            if (it->hint[j])
                gtk_entry_set_placeholder_text(GTK_ENTRY(e), it->hint[j]);
            gtk_box_append(GTK_BOX(row), e);
            ctx->entries[i * MLUV_MAX_BLANK + j] = e;
        }

        gtk_widget_set_halign(rl, GTK_ALIGN_START);
        gtk_label_set_wrap(GTK_LABEL(rl), TRUE);
        gtk_label_set_xalign(GTK_LABEL(rl), 0.0);
        gtk_widget_add_css_class(rl, "meaning");
        gtk_widget_set_visible(rl, FALSE);
        gtk_box_append(GTK_BOX(card), rl);
        ctx->reveal[i] = rl;
    }

    g_signal_connect(check, "clicked", G_CALLBACK(mluv_check), ctx);
    return page;
}

static void mluv_reveal_check(GtkButton *button, gpointer data) {
    MluvRevealCtx *ctx = data;

    (void)button;

    for (int i = 0; i < ctx->n; i++)
        gtk_widget_set_visible(ctx->sol[i], TRUE);

    set_feedback(ctx->feedback, TRUE, "Zobrazili jste si vzorové řešení.");
    mark_done(ctx->unit, ctx->ex_num);
}

static GtkWidget *mluv_reveal_page(int ex_num, const char *title,
                                   const char *sub, const char *note,
                                   const MluvRevealItem *items, int n) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell("mluvnice", title, sub,
                                    "Zobrazit řešení",
                                    &body, &feedback, &check);
    MluvRevealCtx *ctx = g_new0(MluvRevealCtx, 1);

    ctx->items = items;
    ctx->n = n;
    ctx->unit = &mluv_unit;
    ctx->ex_num = ex_num;
    ctx->feedback = feedback;
    ctx->sol = g_new0(GtkWidget *, n);

    if (note) {
        GtkWidget *l = gtk_label_new(note);

        gtk_widget_set_halign(l, GTK_ALIGN_START);
        gtk_label_set_wrap(GTK_LABEL(l), TRUE);
        gtk_label_set_xalign(GTK_LABEL(l), 0.0);
        gtk_widget_add_css_class(l, "hint");
        gtk_widget_set_margin_bottom(l, 4);
        gtk_box_append(GTK_BOX(body), l);
    }

    for (int i = 0; i < n; i++) {
        GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        GtkWidget *prompt = gtk_label_new(items[i].prompt);
        GtkWidget *sol = gtk_label_new(items[i].solution);

        gtk_widget_add_css_class(card, "notes-card");
        gtk_box_append(GTK_BOX(body), card);

        gtk_widget_set_halign(prompt, GTK_ALIGN_START);
        gtk_label_set_wrap(GTK_LABEL(prompt), TRUE);
        gtk_label_set_xalign(GTK_LABEL(prompt), 0.0);
        gtk_widget_add_css_class(prompt, "ex-prompt");
        gtk_box_append(GTK_BOX(card), prompt);

        gtk_widget_set_halign(sol, GTK_ALIGN_START);
        gtk_label_set_wrap(GTK_LABEL(sol), TRUE);
        gtk_label_set_xalign(GTK_LABEL(sol), 0.0);
        gtk_widget_add_css_class(sol, "meaning");
        gtk_widget_set_visible(sol, FALSE);
        gtk_box_append(GTK_BOX(card), sol);
        ctx->sol[i] = sol;
    }

    g_signal_connect(check, "clicked", G_CALLBACK(mluv_reveal_check), ctx);
    return page;
}

static void mluv_choice_check(GtkButton *button, gpointer data) {
    MluvChoiceCtx *ctx = data;
    int ok = 0;

    (void)button;

    for (int i = 0; i < ctx->n; i++) {
        const ChoiceQ *q = &ctx->qs[i];
        int active = -1;

        for (int o = 0; o < q->n_options; o++) {
            GtkToggleButton *tb = ctx->toggles[i * MLUV_MAX_OPT + o];

            gtk_widget_remove_css_class(GTK_WIDGET(tb), "ok");
            gtk_widget_remove_css_class(GTK_WIDGET(tb), "wrong");
            gtk_widget_set_sensitive(GTK_WIDGET(tb), FALSE);
            if (gtk_toggle_button_get_active(tb))
                active = o;
        }

        if (active == q->correct) {
            ok++;
            gtk_widget_add_css_class(
                GTK_WIDGET(ctx->toggles[i * MLUV_MAX_OPT + active]), "ok");
        } else if (active >= 0) {
            gtk_widget_add_css_class(
                GTK_WIDGET(ctx->toggles[i * MLUV_MAX_OPT + active]), "wrong");
            gtk_widget_add_css_class(
                GTK_WIDGET(ctx->toggles[i * MLUV_MAX_OPT + q->correct]), "ok");
        } else {
            gtk_widget_add_css_class(
                GTK_WIDGET(ctx->toggles[i * MLUV_MAX_OPT + q->correct]), "ok");
        }

        if (ctx->expl_labels && ctx->expl_labels[i])
            gtk_widget_set_visible(ctx->expl_labels[i], TRUE);
    }

    if (ok == ctx->n) {
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        mark_done(ctx->unit, ctx->ex_num);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry_short"));
    }
}

static GtkWidget *mluv_choice_page(int ex_num, const char *title,
                                   const char *sub, const char *note,
                                   const ChoiceQ *qs, const char **expls,
                                   int n) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell("mluvnice", title, sub, "check",
                                    &body, &feedback, &check);
    MluvChoiceCtx *ctx = g_new0(MluvChoiceCtx, 1);

    ctx->qs = qs;
    ctx->expls = expls;
    ctx->n = n;
    ctx->unit = &mluv_unit;
    ctx->ex_num = ex_num;
    ctx->feedback = feedback;
    ctx->toggles = g_new0(GtkToggleButton *, n * MLUV_MAX_OPT);
    ctx->expl_labels = g_new0(GtkWidget *, n);

    if (note) {
        GtkWidget *l = gtk_label_new(note);

        gtk_widget_set_halign(l, GTK_ALIGN_START);
        gtk_label_set_wrap(GTK_LABEL(l), TRUE);
        gtk_label_set_xalign(GTK_LABEL(l), 0.0);
        gtk_widget_add_css_class(l, "hint");
        gtk_widget_set_margin_bottom(l, 4);
        gtk_box_append(GTK_BOX(body), l);
    }

    for (int i = 0; i < n; i++) {
        GtkWidget *card = mcq_append_question(
            body, i + 1, &qs[i], &ctx->toggles[i * MLUV_MAX_OPT]);

        if (expls && expls[i]) {
            GtkWidget *e = gtk_label_new(expls[i]);

            gtk_widget_set_halign(e, GTK_ALIGN_START);
            gtk_label_set_wrap(GTK_LABEL(e), TRUE);
            gtk_label_set_xalign(GTK_LABEL(e), 0.0);
            gtk_widget_add_css_class(e, "meaning");
            gtk_widget_set_visible(e, FALSE);
            gtk_box_append(GTK_BOX(card), e);
            ctx->expl_labels[i] = e;
        }
    }

    g_signal_connect(check, "clicked", G_CALLBACK(mluv_choice_check), ctx);
    return page;
}

/* ---- Exercise content --------------------------------------------- */

static const MluvItem e1_items[] = {
    {"Stará bab_čka sed_la na lav_čce.", "babička, seděla, lavičce", 3,
     {"i/í/y/ý", "i/í/y/ý", "i/í/y/ý"},
     {"babička", "seděla|sedla", "lavičce"}},
    {"Na obloze sv_tily hv_zdy.", "svítily, hvězdy", 2,
     {"i/í/y/ý", "i/í/y/ý"},
     {"svítily", "hvězdy"}},
    {"M_sl_m, že m_š pravdu.", "Myslím, myš", 2,
     {"i/í/y/ý", "i/í/y/ý"},
     {"Myslím", "myš"}},
    {"V_soký muž nes_l těžký kuf_r.", "Vysoký, nesl, kufr", 3,
     {"i/í/y/ý", "i/í/y/ý", "i/í/y/ý"},
     {"Vysoký", "nesl", "kufr"}},
    {"Ryb_ář čekal cel_ den u řek_.", "Rybář, celý, řeky", 3,
     {"i/í/y/ý", "i/í/y/ý", "i/í/y/ý"},
     {"Rybář", "celý", "řeky"}},
};

static const ChoiceQ e2_qs[] = {
    {"Narodil se v ______ v malé vesnici.",
     {"Čechách", "čechách"}, 2, 0},
    {"Přečetl jsem román od ______.",
     {"Boženy Němcové", "boženy němcové"}, 2, 0},
    {"Jedeme na ______.", {"Moravu", "moravu"}, 2, 0},
    {"V ______ žije přes milion lidí.", {"Praze", "praze"}, 2, 0},
    {"Navštívili jsme ______.",
     {"Národní muzeum", "národní muzeum"}, 2, 0},
};

static const MluvItem e3_items[] = {
    {"Krásný den se chýlil ke konci. — slovo „Krásný“",
     "přídavné jméno", 1, {"slovní druh"},
     {"přídavné jméno|přídavné jmeno|adjektivum"}},
    {"Šel rychle domů. — slovo „rychle“",
     "příslovce", 1, {"slovní druh"},
     {"příslovce|adverbium"}},
    {"Sedím před školou. — slovo „před“",
     "předložka", 1, {"slovní druh"},
     {"předložka|prepozice"}},
    {"To je jeho kniha. — slovo „jeho“",
     "zájmeno", 1, {"slovní druh"},
     {"zájmeno|zájmeno|pronomen"}},
    {"Ach, to je škoda! — slovo „Ach“",
     "citoslovce", 1, {"slovní druh"},
     {"citoslovce|interjekce"}},
    {"Přišel jsem, ale bylo pozdě. — slovo „ale“",
     "spojka", 1, {"slovní druh"},
     {"spojka|konjunkce"}},
    {"Napočítal jsem pět jablek. — slovo „pět“",
     "číslovka", 1, {"slovní druh"},
     {"číslovka|cislovka|numerale"}},
    {"Učitel vysvětloval látku. — slovo „Učitel“",
     "podstatné jméno", 1, {"slovní druh"},
     {"podstatné jméno|podstatné jmeno|substantivum"}},
};

static const MluvItem e4_items[] = {
    {"Kniha leží na stole. — slovo „stole“ (stůl)",
     "mužský rod, jednotné číslo, 6. pád (lokál)", 3,
     {"rod", "číslo", "pád"},
     {"mužský|mužský rod|maskulinum",
      "jednotné|jednotné číslo|singulár",
      "6|lokál|šestý|6. pád"}},
    {"Povídám si s kamarády. — slovo „kamarády“ (kamarád)",
     "mužský rod, množné číslo, 7. pád (instrumentál)", 3,
     {"rod", "číslo", "pád"},
     {"mužský|mužský rod|maskulinum",
      "množné|množné číslo|plurál",
      "7|instrumentál|sedmý|7. pád"}},
    {"Vidím krásnou horu. — slovo „horu“ (hora)",
     "ženský rod, jednotné číslo, 4. pád (akuzativ)", 3,
     {"rod", "číslo", "pád"},
     {"ženský|ženský rod|femininum",
      "jednotné|jednotné číslo|singulár",
      "4|akuzativ|čtvrtý|4. pád"}},
    {"To je dům mého souseda. — slovo „souseda“ (soused)",
     "mužský rod, jednotné číslo, 2. pád (genitiv)", 3,
     {"rod", "číslo", "pád"},
     {"mužský|mužský rod|maskulinum",
      "jednotné|jednotné číslo|singulár",
      "2|genitiv|druhý|2. pád"}},
    {"Děti si hrají na zahradě. — slovo „Děti“ (dítě)",
     "střední rod, množné číslo, 1. pád (nominativ)", 3,
     {"rod", "číslo", "pád"},
     {"střední|střední rod|neutrum",
      "množné|množné číslo|plurál",
      "1|nominativ|první|1. pád"}},
};

static const MluvItem e5_items[] = {
    {"čteme", "1. osoba, množné číslo, přítomný čas, oznamovací způsob", 4,
     {"osoba", "číslo", "čas", "způsob"},
     {"1|1.|první|první osoba|1. osoba",
      "množné|množné číslo|plurál",
      "přítomný|přítomný čas|přítomný čas",
      "oznamovací|oznamovací způsob|indikativ"}},
    {"běžel", "3. osoba, jednotné číslo, minulý čas, oznamovací způsob", 4,
     {"osoba", "číslo", "čas", "způsob"},
     {"3|3.|třetí|třetí osoba|3. osoba",
      "jednotné|jednotné číslo|singulár",
      "minulý|minulý čas|minulý čas",
      "oznamovací|oznamovací způsob|indikativ"}},
    {"přečtěte",
     "2. osoba, množné číslo, rozkazovací způsob (čas se neurčuje)", 4,
     {"osoba", "číslo", "čas", "způsob"},
     {"2|2.|druhý|druhá osoba|2. osoba",
      "množné|množné číslo|plurál",
      "neurčitý|nestanovený|neurčený",
      "rozkazovací|rozkazovací způsob|imperativ"}},
    {"by zpíval",
     "3. osoba, jednotné číslo, podmiňovací způsob přítomný", 4,
     {"osoba", "číslo", "čas", "způsob"},
     {"3|3.|třetí|třetí osoba|3. osoba",
      "jednotné|jednotné číslo|singulár",
      "neurčitý|nestanovený|přítomný",
      "podmiňovací|podmiňovací způsob|kondicionál"}},
    {"budou pracovat",
     "3. osoba, množné číslo, budoucí čas, oznamovací způsob", 4,
     {"osoba", "číslo", "čas", "způsob"},
     {"3|3.|třetí|třetí osoba|3. osoba",
      "množné|množné číslo|plurál",
      "budoucí|budoucí čas|futurum",
      "oznamovací|oznamovací způsob|indikativ"}},
};

static const MluvRevealItem e6_items[] = {
    {"napsali", "na- / ps- / -a- / -li"},
    {"přečíst", "pře- / číst"},
    {"zahradník", "za- / hrad- / -ník / -Ø"},
    {"nepřátelský", "ne- / přátel- / -ský / -Ø"},
    {"výběr", "vý- / bér- / -Ø / -Ø"},
};

static const MluvItem e7_items[] = {
    {"Malá holčička plakala. — větný člen „Malá holčička“",
     "podmět", 1, {"větný člen"}, {"podmět|subjekt"}},
    {"Otec pracuje v garáži. — větný člen „v garáži“",
     "příslovečné určení místa", 1, {"větný člen"},
     {"příslovečné určení místa|příslovečné určení místa (PU místa)|"
      "příslovečné určení|adverbiále místa"}},
    {"Sestra je učitelka. — větný člen „je učitelka“",
     "přísudek jmenný se sponou", 1, {"větný člen"},
     {"přísudek jmenný se sponou|přísudek jmenný|jmenný přísudek se sponou|"
      "přísudek se sponou"}},
    {"Dal jsem knihu bratrovi. — větný člen „knihu“",
     "předmět", 1, {"větný člen"}, {"předmět|objekt"}},
    {"Přišel unavený. — větný člen „unavený“",
     "doplněk", 1, {"větný člen"}, {"doplněk"}},
    {"Pomalu šel domů. — větný člen „Pomalu“",
     "příslovečné určení způsobu", 1, {"větný člen"},
     {"příslovečné určení způsobu|příslovečné určení způsobu (PU způsobu)|"
      "příslovečné určení|adverbiále způsobu"}},
};

static const MluvItem e8_items[] = {
    {"Věřím, že uspěješ.", "vedlejší věta předmětná", 1, {"druh vedlejší věty"},
     {"vedlejší věta předmětná|věta předmětná|předmětná"}},
    {"Udělám to, jak budu moci.", "vedlejší věta příslovečná způsobová", 1,
     {"druh vedlejší věty"},
     {"vedlejší věta příslovečná způsobová|příslovečná způsobová|způsobová"}},
    {"Přišel, protože chtěl pomoci.", "vedlejší věta příslovečná příčinná", 1,
     {"druh vedlejší věty"},
     {"vedlejší věta příslovečná příčinná|příslovečná příčinná|příčinná"}},
    {"Řekni mi, kde bydlíš.", "vedlejší věta předmětná", 1,
     {"druh vedlejší věty"},
     {"vedlejší věta předmětná|věta předmětná|předmětná"}},
    {"Byl tak unavený, že usnul okamžitě.",
     "vedlejší věta příslovečná následková", 1, {"druh vedlejší věty"},
     {"vedlejší věta příslovečná následková|příslovečná následková|následková"}},
};

static const MluvItem e9_items[] = {
    {"rychlý", "hbitý, svižný", 1, {"synonymum"},
     {"hbitý|svižný|rychlý|čilý|bystrý|prudký"}},
    {"smutný", "nešťastný, zarmoucený", 1, {"synonymum"},
     {"nešťastný|zarmoucený|tesklivý|skličený|žalostný"}},
    {"začít", "zahájit, započít", 1, {"synonymum"},
     {"zahájit|započít|začínat|rozpoutat|zahajovat"}},
    {"krásný", "nádherný, překrásný", 1, {"synonymum"},
     {"nádherný|překrásný|kouzelný|půvabný|okouzlující"}},
    {"mluvit", "hovořit, promlouvat", 1, {"synonymum"},
     {"hovořit|promlouvat|vyprávět|říkat|povídat"}},
};

static const MluvItem e10_items[] = {
    {"starý ×", "nový", 1, {"opak"}, {"nový|mladý"}},
    {"začátek ×", "konec", 1, {"opak"}, {"konec|závěr"}},
    {"přijít ×", "odejít", 1, {"opak"}, {"odejít|odjet|odcházet"}},
    {"radost ×", "smutek", 1, {"opak"}, {"smutek|žal|zármutek"}},
    {"vysoký ×", "nízký", 1, {"opak"}, {"nízký|malý"}},
};

static const MluvItem e11_items[] = {
    {"(s/z)kočit dolů ze stromu", "skočit", 1, {"s-/z-"}, {"skočit"}},
    {"(s/z)epsat úkol", "sepsat", 1, {"s-/z-"}, {"sepsat"}},
    {"(s/z)budovat nový dům", "zbudovat", 1, {"s-/z-"}, {"zbudovat"}},
    {"(s/z)vednout těžký kámen", "zvednout", 1, {"s-/z-"}, {"zvednout"}},
    {"(s/z)pravit auto", "spravit", 1, {"s-/z-"}, {"spravit"}},
};

static const MluvRevealItem e12_items[] = {
    {"Koupil jsem novej kabát.", "nový („novej“ je nespisovné)"},
    {"Přišli jsme tam, kdy jsme měli.",
     "kdy → když: „Přišli jsme tam, když jsme měli.“"},
    {"Ona mě o tom neřekla nic.",
     "mě → mi (dativ): „Ona mi o tom neřekla nic.“"},
    {"Je to nejlepší film, který jsem viděl vůbec.",
     "„vůbec nejlepší film“: „Je to vůbec nejlepší film, který jsem viděl.“"},
    {"Byl jsem na nákupu a koupil chléb.",
     "„Byl jsem nakupovat a koupil jsem chléb.“"},
};

static const MluvItem e13_items[] = {
    {"Viděl jsem (ten starý muž) ______.", "toho starého muže", 1,
     {"správný tvar"}, {"toho starého muže|toho starého muže,"}},
    {"Mluvil jsem s (moje sestra) ______.", "mojí sestrou", 1,
     {"správný tvar"}, {"mojí sestrou|mou sestrou|svou sestrou"}},
    {"Jdu do (škola) ______.", "školy", 1, {"správný tvar"}, {"školy"}},
    {"Sedím vedle (kamarád) ______.", "kamaráda", 1,
     {"správný tvar"}, {"kamaráda"}},
    {"Vzpomínám na (dětství) ______.", "dětství", 1,
     {"správný tvar"}, {"dětství"}},
};

static const ChoiceQ e14_qs[] = {
    {"Slovo „záříjový“ je:", {"správně", "chybně"}, 2, 1},
    {"Které slovo je přídavné jméno?",
     {"rychle", "rychlost", "rychlý", "zrychlení"}, 4, 2},
    {"Věta „Petr čte knihu.“ je:",
     {"souvětí", "věta jednoduchá", "věta bez podmětu", "věta zvolací"}, 4, 1},
    {"Slovo „příroda“ je utvořeno:",
     {"odvozením", "skládáním", "zkracováním", "přejímáním"}, 4, 0},
    {"Která dvojice jsou synonyma?",
     {"rychlý – pomalý", "jít – přijít", "smutný – nešťastný",
      "velký – malý"}, 4, 2},
};

static const char *e14_expl[] = {
    "Správně je „zářijový“ (nikoli „záříjový“).",
    "„rychlý“ je přídavné jméno; „rychle“ je příslovce.",
    "Věta má jen jeden přísudek („čte“), jde o větu jednoduchou.",
    "„příroda“ je utvořena odvozením, není to složenina.",
    "Synonyma vyjadřují stejný význam: „smutný“ a „nešťastný“.",
};

static const MluvRevealItem e15_items[] = {
    {"Matka řekla: „Pojď se najíst.“",
     "Matka řekla, abych přišel se najíst."},
    {"Učitel řekl: „Zítra bude písemka.“",
     "Učitel řekl, že zítra bude písemka."},
    {"Kamarád se zeptal: „Kde bydlíš?“",
     "Kamarád se zeptal, kde bydlím."},
};

static const MluvRevealItem e16_items[] = {
    {"bydlet × žít",
     "bydlet = mít domov někde; žít = existovat, prožívat život"},
    {"říkat × vyprávět",
     "říkat = sdělovat informaci; vyprávět = podrobně popisovat příběh"},
    {"dívat se × vidět",
     "dívat se = záměrně sledovat; vidět = vnímat zrakem (i neúmyslně)"},
    {"učit × učit se",
     "učit = předávat znalosti druhým; učit se = sám nabývat znalosti"},
    {"přinést × donést",
     "přinést = přinést sem (k mluvčímu); donést = odnést tam (od mluvčího)"},
};

static const MluvRevealItem e17_items[] = {
    {"Přišel jsem domů unavený hladový a promrzlý.",
     "Přišel jsem domů unavený, hladový a promrzlý."},
    {"Když dorazil vlak nastoupili všichni cestující.",
     "Když dorazil vlak, nastoupili všichni cestující."},
    {"Myslím že dnes bude pršet.", "Myslím, že dnes bude pršet."},
    {"Jana která je moje nejlepší kamarádka bydlí vedle.",
     "Jana, která je moje nejlepší kamarádka, bydlí vedle."},
    {"Pes štěkal ale nikdo nepřicházel.",
     "Pes štěkal, ale nikdo nepřicházel."},
};

static const MluvItem e18_items[] = {
    {"Slunce se usmívalo na celé město.", "personifikace", 1,
     {"druh pojmenování"}, {"personifikace|zosobnění"}},
    {"Je silný jako býk.", "přirovnání", 1,
     {"druh pojmenování"}, {"přirovnání|komparace"}},
    {"Moře hněvu ho zaplavilo.", "metafora", 1,
     {"druh pojmenování"}, {"metafora"}},
    {"Čekal jsem na tebe věčnost.", "hyperbola", 1,
     {"druh pojmenování"}, {"hyperbola|nadsázka"}},
    {"Přečetl celého Shakespeara.", "metonymie", 1,
     {"druh pojmenování"}, {"metonymie"}},
};

static const MluvItem e19_items[] = {
    {"Narodil se roku 1890 v Praze. Vystudoval práva a poté pracoval jako "
     "úředník…", "životopis", 1, {"slohový útvar"}, {"životopis|biografie"}},
    {"Byl to chlapec střední postavy, s hnědýma očima a klidným výrazem…",
     "charakteristika", 1, {"slohový útvar"}, {"charakteristika"}},
    {"Jednoho dne jsem šel lesem, když tu najednou jsem zaslechl podivný "
     "zvuk…", "vyprávění", 1, {"slohový útvar"}, {"vyprávění"}},
    {"Mnozí se ptají, zda má smysl číst knihy v době internetu. Domnívám "
     "se…", "úvaha", 1, {"slohový útvar"}, {"úvaha"}},
    {"Místnost byla malá, se dvěma okny na jih. U zdi stála stará skříň…",
     "popis", 1, {"slohový útvar"}, {"popis"}},
};

static const ChoiceQ e20_qs[] = {
    {"Které slovo není příslovce?",
     {"rychle", "tam", "hezký", "brzy"}, 4, 2},
    {"Ve větě „Petr dal Pavlovi knihu“ je slovo „Pavlovi“:",
     {"podmět", "přísudek", "předmět", "příslovečné určení"}, 4, 2},
    {"Slovo „nejhezčí“ obsahuje:",
     {"předponu a příponu", "pouze předponu", "kořen a příponu",
      "pouze příponu"}, 4, 0},
    {"Která věta je souvětí souřadné slučovací?",
     {"Přišel, protože chtěl pomoci.", "Šel domů a spal.",
      "Věřím, že přijdeš.", "Udělám to, jak budeš chtít."}, 4, 1},
    {"Příslovce „rychle“ je stupňováno jako:",
     {"rychle – rychleji – nejrychleji",
      "rychle – více rychle – nejvíce rychle",
      "rychle – rychlé – nejrychlejší", "nelze stupňovat"}, 4, 0},
};

GtkWidget *build_mluvnice_exercise_page(int n) {
    switch (n) {
        case 1:
            return mluv_typed_page(1,
                "Pravopis – i/y po obojetných souhláskách",
                "Doplňte i/í nebo y/ý.", NULL,
                e1_items, G_N_ELEMENTS(e1_items));
        case 2:
            return mluv_choice_page(2, "Pravopis – velká/malá písmena",
                "Vyberte správnou variantu.", NULL,
                e2_qs, NULL, G_N_ELEMENTS(e2_qs));
        case 3:
            return mluv_typed_page(3, "Slovní druhy",
                "Určete slovní druh podtržených slov.",
                "Do pole napište název slovního druhu.",
                e3_items, G_N_ELEMENTS(e3_items));
        case 4:
            return mluv_typed_page(4,
                "Mluvnické kategorie – podstatná jména",
                "Určete rod, číslo a pád podtržených slov.",
                "Vyplňte tři pole: rod, číslo a pád.",
                e4_items, G_N_ELEMENTS(e4_items));
        case 5:
            return mluv_typed_page(5, "Mluvnické kategorie – slovesa",
                "Určete osobu, číslo, čas a způsob u těchto sloves.",
                "U rozkazovacího a podmiňovacího způsobu napište do pole "
                "čas „neurčitý“.",
                e5_items, G_N_ELEMENTS(e5_items));
        case 6:
            return mluv_reveal_page(6, "Slovotvorba",
                "Rozeberte slova na morfémy.",
                "Určete předponu, kořen, příponu a koncovku.",
                e6_items, G_N_ELEMENTS(e6_items));
        case 7:
            return mluv_typed_page(7, "Větné členy",
                "Určete větný člen podtržených výrazů.", NULL,
                e7_items, G_N_ELEMENTS(e7_items));
        case 8:
            return mluv_typed_page(8, "Druhy vedlejších vět",
                "Určete druh vedlejší věty.", NULL,
                e8_items, G_N_ELEMENTS(e8_items));
        case 9:
            return mluv_typed_page(9, "Synonyma",
                "Ke každému slovu napište synonymum.", NULL,
                e9_items, G_N_ELEMENTS(e9_items));
        case 10:
            return mluv_typed_page(10, "Antonyma", "Napište opak.", NULL,
                e10_items, G_N_ELEMENTS(e10_items));
        case 11:
            return mluv_typed_page(11,
                "Pravopis – s/z na začátku slova",
                "Vyberte správnou předponu s-/z-.", NULL,
                e11_items, G_N_ELEMENTS(e11_items));
        case 12:
            return mluv_reveal_page(12, "Skladba – najděte chybu",
                "Každá věta obsahuje jednu chybu. Najděte ji a opravte.",
                NULL, e12_items, G_N_ELEMENTS(e12_items));
        case 13:
            return mluv_typed_page(13, "Tvarosloví – správný tvar",
                "Dejte slovo do správného tvaru.", NULL,
                e13_items, G_N_ELEMENTS(e13_items));
        case 14:
            return mluv_choice_page(14, "Výběr ze čtyř možností",
                "Vyberte správnou odpověď.", NULL,
                e14_qs, e14_expl, G_N_ELEMENTS(e14_qs));
        case 15:
            return mluv_reveal_page(15, "Přímá a nepřímá řeč",
                "Přepište přímou řeč na nepřímou.", NULL,
                e15_items, G_N_ELEMENTS(e15_items));
        case 16:
            return mluv_reveal_page(16, "Slovní zásoba – rozdíly",
                "Vysvětlete rozdíl mezi těmito dvojicemi slov.", NULL,
                e16_items, G_N_ELEMENTS(e16_items));
        case 17:
            return mluv_reveal_page(17, "Interpunkce",
                "Doplňte čárky tam, kde patří.", NULL,
                e17_items, G_N_ELEMENTS(e17_items));
        case 18:
            return mluv_typed_page(18, "Obrazná pojmenování",
                "Určete, o jaký druh obrazného pojmenování jde.", NULL,
                e18_items, G_N_ELEMENTS(e18_items));
        case 19:
            return mluv_typed_page(19, "Stylistika – slohové útvary",
                "Přiřaďte ukázku ke správnému slohovému útvaru.", NULL,
                e19_items, G_N_ELEMENTS(e19_items));
        case 20:
            return mluv_choice_page(20, "Souhrnné opakování",
                "Vyberte správnou odpověď.", NULL,
                e20_qs, NULL, G_N_ELEMENTS(e20_qs));
        default:
            return NULL;
    }
}

GtkWidget *build_mluvnice_page(void) {
    return build_unit_page(&mluv_unit);
}
