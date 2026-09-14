#include "maturita.h"

/* ------------------------------------------------------------------ */
/* Maturitní četba: George Orwell – 1984                              */
/* ------------------------------------------------------------------ */

typedef struct {
    const char *q;
    const char *opts[4];
    int correct;
    const char *expl;
} LitQ;

static const LitQ lit_qs[] = {
    {
        "Kdo je hlavní hrdina románu a co dělá?",
        {
            "Winston Smith – přepisuje historii na Ministerstvu pravdy",
            "O'Brien – řídí Ministerstvo lásky",
            "Emmanuel Goldstein – vede Bratrstvo",
            "Winston Smith – velí Policii myšlení",
        },
        0,
        "Winston Smith je vnější člen strany, který na Ministerstvu pravdy "
        "upravuje staré záznamy a novinové články.",
    },
    {
        "Ve kterém fiktivním státě se děj odehrává?",
        {"V Eurasii", "V Oceánii", "Ve Východasii", "V Angsocu"},
        1,
        "Příběh se odehrává v Oceánii, v provincii Letopočet 1 (bývalá "
        "Anglie), jejímž hlavním městem je Londýn.",
    },
    {
        "Co znamená slogan „VÁLKA JE MÍR, SVOBODA JE OTROCTVÍ, "
        "NEVĚDOMOST JE SÍLA“?",
        {
            "Jsou to tři zásady Bratrstva",
            "Jsou to názvy ministerstev",
            "Jde o hesla prolety",
            "Jsou to paradoxní hesla, kterými strana ospravedlňuje útlak",
        },
        3,
        "Strana používá dvojité myšlení: hesla znějí vznešeně, ale ve "
        "skutečnosti obhajují válku, otroctví a nevědomost.",
    },
    {
        "Kdo je Velký bratr a je reálnou osobou?",
        {
            "Skutečný vůdce Bratrstva",
            "Winstonův nadřízený na ministerstvu",
            "Symbol všemocné moci a kultu osobnosti; možná vůbec neexistuje",
            "O'Brienův pseudonym",
        },
        2,
        "Velký bratr je tvář režimu. Není jasné, zda jde o živou osobu, "
        "nebo jen o symbol, který drží strach a poslušnost.",
    },
    {
        "Co je Newspeak (novořeč) a proč ho strana zavádí?",
        {
            "Tajná řeč Bratrstva",
            "Zjednodušená angličtina omezující slovní zásobu, aby nešlo "
            "vyslovit zakázané myšlenky",
            "Úřední jazyk prolety",
            "Šifra Policie myšlení",
        },
        1,
        "Newspeak neustále ubírá slova. Když pro svobodu nebo odpor "
        "neexistuje slovo, nelze o nich ani myslet.",
    },
    {
        "Co je teleobrazovka (telescreen) a jak funguje?",
        {
            "Televize určená jen pro zábavu",
            "Zařízení, které vysílá i sleduje; nelze ji úplně vypnout",
            "Rozhlasová stanice",
            "Počítač Ministerstva pravdy",
        },
        1,
        "Telescreen funguje obousměrně – neustále sleduje občany. Vypnout "
        "lze jen zvuk, obraz běží dál.",
    },
    {
        "Jak se jmenují tři supervelmoci světa 1984?",
        {
            "Oceánie, Eurasie a Východasie",
            "Oceánie, Evropa a Asie",
            "Anglie, Eurasie a Ostasie",
            "Oceánie, Východasie a Afrika",
        },
        0,
        "Svět je rozdělen na tři státy ve věčném, ale nerozhodném "
        "konfliktu: Oceánii, Eurasii a Východasii.",
    },
    {
        "Čím se zabývá Ministerstvo pravdy?",
        {
            "Vede válku",
            "Dohlíží na morálku a sňatky",
            "Upravuje a přepisuje historii, noviny a archivy",
            "Rozděluje potraviny",
        },
        2,
        "Ministerstvo pravdy vyrábí „pravdu“ – Winston v něm přepisuje "
        "staré záznamy tak, aby vždy odpovídaly současné linii strany.",
    },
    {
        "K čemu slouží Ministerstvo lásky?",
        {
            "Zajišťuje sňatky a lásku",
            "Je to budova bez oken, kde se mučí a likvidují nepřátelé strany",
            "Vyrábí zbraně",
            "Školí děti",
        },
        1,
        "Ministerstvo lásky je opakem svého názvu – sídlí tam mučírny a "
        "nachází se v něm i obávaná místnost 101.",
    },
    {
        "V čem je paradox Ministerstva hojnosti?",
        {
            "Má na starosti hospodářství, ale ve skutečnosti drží lidi "
            "v nedostatku",
            "Rozdává jídlo a zboží zdarma",
            "Zajišťuje dostatek pro všechny",
            "Starají se o něj prolety",
        },
        0,
        "Ministerstvo hojnosti plánuje výrobu, ale ekonomika je záměrně "
        "nastavena tak, aby lidé žili v trvalém nedostatku.",
    },
    {
        "Co je Ministerstvo míru a čím se zabývá?",
        {
            "Uzavírá mírové smlouvy",
            "Dojednává příměří",
            "Ve skutečnosti vede válku",
            "Velí Policii myšlení",
        },
        2,
        "Název je přesně opačný: Ministerstvo míru řídí válku, protože "
        "trvalý válečný stav pomáhá udržet moc strany.",
    },
    {
        "Kdo je Julie a jaký vztah s ní Winston naváže?",
        {
            "Winstonova žena",
            "Mladá členka strany, s níž Winston prožije tajnou milostnou "
            "aféru",
            "O'Brienova asistentka",
            "Proletka a obchodnice",
        },
        1,
        "Julie pracuje v oddělení beletrie a naoko je vzornou členkou "
        "strany. S Winstonem vytvoří tajný milostný vztah.",
    },
    {
        "Proč je vztah Winstona a Julie zakázaný?",
        {
            "Strana zakazuje lásku a sexualitu jako zločin; vztah je navíc "
            "tajný",
            "Julie je vdaná za člena vnitřní strany",
            "Winston je jejím nadřízeným",
            "Julie patří k proletům",
        },
        0,
        "Strana potlačuje citové a sexuální vazby, protože oddanost má "
        "patřit jen straně. Tajný vztah je proto dvojnásob nebezpečný.",
    },
    {
        "Kdo je O'Brien a jakou roli hraje v příběhu?",
        {
            "Skutečný vůdce Bratrstva",
            "Vysoce postavený člen vnitřní strany, který Winstona nakonec "
            "mučí a „vyléčí“",
            "Winstonův přítel z dětství",
            "Proletářský básník",
        },
        1,
        "Winston věří, že O'Brien je odbojář. Ve skutečnosti je to věrný "
        "člen vnitřní strany, který ho dostane do místnosti 101.",
    },
    {
        "Co je Bratrstvo a existuje vůbec?",
        {
            "Skutečná armáda Oceánie",
            "Náboženská organizace",
            "Údajná tajná organizace odporu; není jisté, zda existuje",
            "Odbory prolety",
        },
        2,
        "Bratrstvo zmiňuje Goldstein i O'Brien, ale kniha nikdy nepotvrdí, "
        "že doopravdy existuje – může jít o past strany.",
    },
    {
        "Kdo je Emmanuel Goldstein?",
        {
            "Ministr pravdy",
            "Oficiální nepřítel státu a údajný vůdce Bratrstva",
            "Winstonův kolega",
            "Velký bratrův dvojník",
        },
        1,
        "Goldstein je terčem dvouminutovky nenávisti. Má ztělesňovat "
        "nepřítele, proti kterému strana sjednocuje občany.",
    },
    {
        "Co je dvojité myšlení (doublethink)?",
        {
            "Schopnost myslet ve dvou jazycích",
            "Zdvojení osobnosti",
            "Schopnost věřit dvěma protikladným tvrzením zároveň",
            "Tajné zapisování myšlenek",
        },
        2,
        "Dvojité myšlení je jádro stranické ideologie – člověk přijme i "
        "zjevné rozpory, jako „válka je mír“.",
    },
    {
        "Proč strana neustále přepisuje minulost?",
        {
            "Aby opravila chyby v archivech",
            "Aby měla vždy pravdu a nepřítel jí nemohl dokázat opak",
            "Aby ušetřila papír",
            "Aby potěšila prolety",
        },
        1,
        "Kdo ovládá minulost, ovládá i budoucnost. Přepisováním historie "
        "strana udržuje absolutní kontrolu nad pravdou.",
    },
    {
        "Co je zločin myšlení (thoughtcrime)?",
        {
            "Pouhá myšlenka nebo názor, který se nelíbí straně",
            "Krádež státního majetku",
            "Nelegální obchod",
            "Opomenutí dvouminutovky",
        },
        0,
        "Strana trestá už samotné myšlenky. Winston se proviní tím, že si "
        "do deníku napíše „Dolů s Velkým bratrem“.",
    },
    {
        "Kdo je pan Charrington a jaké je jeho skutečné postavení?",
        {
            "Winstonův přítel a majitel hospody",
            "Proletářský básník",
            "Zdánlivě starý majitel obchodu, ve skutečnosti agent Policie "
            "myšlení",
            "Člen Bratrstva",
        },
        2,
        "Charrington Winstonovi pronajme pokoj bez teleobrazovky, ale "
        "právě on nakonec oba milence udá.",
    },
    {
        "Co je místnost 101?",
        {
            "Winstonova cela na policii",
            "Skladiště knih",
            "Místnost, ve které každého čeká jeho největší strach",
            "Zasedací místnost vnitřní strany",
        },
        2,
        "Místnost 101 je vrcholný mučicí nástroj. Winstonovi v ní hrozí "
        "krysy – jeho největší fobie.",
    },
    {
        "Jak skončí vztah Winstona a Julie?",
        {
            "Utečou spolu do Eurasie",
            "Vezmou se a žijí svobodně",
            "Winston se stane členem Bratrstva",
            "Zradí se a nakonec se přestanou milovat",
        },
        3,
        "Oba jsou po mučení zlomení a vzájemně se zradí. Na konci už "
        "k sobě nic necítí.",
    },
    {
        "Co znamená závěrečná věta „Miloval Velkého bratra“?",
        {
            "Winstonův vzdor se změnil v pokornou lásku k režimu",
            "Winston se stal novým vůdcem",
            "Byl to jen Winstonův sen",
            "Julie mu to řekla",
        },
        0,
        "Winston byl vnitřně zlomen. Přestal být sám sebou a nakonec "
        "režim opravdu přijal za svůj.",
    },
    {
        "Které tvrzení NENÍ jedním z hlavních témat románu?",
        {
            "Moc a absolutní kontrola",
            "Pravda a přepisování historie",
            "Svoboda a identita",
            "Idylický venkovský život",
        },
        3,
        "1984 je temná antiutopie. Hlavními tématy jsou moc, kontrola, "
        "manipulace s pravdou a ztráta svobody, ne venkovská idyla.",
    },
    {
        "V jakém roce byl román 1984 poprvé vydán?",
        {"1936", "1949", "1954", "1984"},
        1,
        "Román vyšel v roce 1949; letopočet 1984 označuje blízkou "
        "budoucnost, do níž Orwell příběh zasadil.",
    },
    {
        "Které další Orwellovo dílo se dá s 1984 srovnat?",
        {"Farma zvířat", "Velký Gatsby", "Zločin a trest", "Babička"},
        0,
        "Obě Orwellova díla varují před totalitou. Farma zvířat ji líčí "
        "alegoricky pomocí zvířat, 1984 očima jedince.",
    },
    {
        "Co je „dvouminutovka nenávisti“?",
        {
            "Krátká každodenní akce, při níž občané společně nenávidí "
            "Goldsteina a nepřítele",
            "Pravidelná modlitba ke Velkému bratrovi",
            "Sportovní přestávka ve škole",
            "Porada vedení ministerstva",
        },
        0,
        "Slouží k uvolnění a přesměrování agrese občanů na nepřítele "
        "státu, čímž strana posiluje svou moc.",
    },
    {
        "Proč je podle knihy válka pro systém nezbytná?",
        {
            "Přináší mír a blahobyt",
            "Udržuje strach, spotřebovává výrobu a zachovává chudobu "
            "i hierarchii",
            "Rozvíjí vědu a techniku",
            "Sjednocuje vládu s prolety",
        },
        1,
        "Trvalá válka pohlcuje nadbytečnou produkci a udržuje lidi ve "
        "strachu i chudobě, takže se nemohou vzbouřit.",
    },
    {
        "Jaký je rozdíl mezi vnějšími členy strany a prolety?",
        {
            "Proleti mají více práv než členové strany",
            "Vnější členové jsou dozíráni, kdežto proleti žijí v chudobě "
            "a strana o ně nestojí",
            "Není mezi nimi žádný rozdíl",
            "Proleti tvoří vedení strany",
        },
        1,
        "Proleti tvoří asi 85 % obyvatel. Žijí v bídě a jsou považováni "
        "za neškodné, dokud si neuvědomí svou sílu.",
    },
    {
        "Jak strana kontroluje sexualitu a osobní vztahy?",
        {
            "Podporuje volnou lásku a nezávislost",
            "Potlačuje lásku a sexualitu, děti vede k oddanosti straně",
            "Zakazuje veškerá manželství",
            "Povoluje vztahy jen s prolety",
        },
        1,
        "Strana chce, aby veškerá oddanost a energie patřily jí. Proto "
        "podporuje zdrženlivost a vede děti v Antisexuální lize mládeže.",
    },
    {
        "Co symbolizuje papírové těžítko, které si Winston koupí?",
        {
            "Jeho nenávist ke Goldsteinovi",
            "Kousek krásy a minulosti, kterou už nelze vrátit",
            "Moc Velkého bratra",
            "Lásku k Julii",
        },
        1,
        "Těžítko je drobný, krásný předmět z minulosti. Pro Winstona "
        "ztělesňuje svět, který strana zničila.",
    },
    {
        "Jaké poselství chtěl Orwell románem nejspíš předat?",
        {
            "Varování před totalitou a ztrátou svobody i pravdy",
            "Oslavu technického pokroku",
            "Návod, jak založit stát",
            "Kritiku sportu a zábavy",
        },
        0,
        "1984 je varování: když lidé přestanou bránit pravdu a svobodu, "
        "mohou o ně nenávratně přijít.",
    },
};

#define LIT_NQ ((int)G_N_ELEMENTS(lit_qs))

typedef struct {
    const LitQ *qs;
    int n;
    int idx;
    int score;
    int answered;
    int finished;
    GtkWidget *progress;
    GtkWidget *score_lbl;
    GtkWidget *prompt;
    GtkWidget *grid;
    GtkWidget *opts[4];
    GtkWidget *opt_lbl[4];
    GtkWidget *expl;
    GtkWidget *next;
} LitQuiz;

static void lit_quiz_show(LitQuiz *z) {
    const LitQ *q = &z->qs[z->idx];
    char buf[128];

    g_snprintf(buf, sizeof(buf), tr("lit_question_fmt"), z->idx + 1, z->n);
    gtk_label_set_text(GTK_LABEL(z->progress), buf);
    g_snprintf(buf, sizeof(buf), tr("lit_score_fmt"), z->score, z->n);
    gtk_label_set_text(GTK_LABEL(z->score_lbl), buf);

    gtk_label_set_text(GTK_LABEL(z->prompt), q->q);
    for (int i = 0; i < 4; i++) {
        gtk_label_set_text(GTK_LABEL(z->opt_lbl[i]), q->opts[i]);
        gtk_widget_remove_css_class(z->opts[i], "ok");
        gtk_widget_remove_css_class(z->opts[i], "wrong");
        gtk_widget_set_sensitive(z->opts[i], TRUE);
    }
    gtk_widget_set_visible(z->grid, TRUE);
    gtk_widget_set_visible(z->expl, FALSE);
    gtk_widget_set_visible(z->next, FALSE);
    z->answered = 0;
}

static void lit_quiz_finish(LitQuiz *z) {
    char buf[128];

    z->finished = 1;
    gtk_label_set_text(GTK_LABEL(z->progress), tr("lit_finished"));
    gtk_label_set_text(GTK_LABEL(z->prompt), tr("lit_finished_text"));
    g_snprintf(buf, sizeof(buf), tr("lit_score_fmt"), z->score, z->n);
    gtk_label_set_text(GTK_LABEL(z->score_lbl), buf);
    gtk_widget_set_visible(z->grid, FALSE);
    gtk_widget_set_visible(z->expl, FALSE);
    gtk_widget_set_visible(z->next, TRUE);
    gtk_button_set_label(GTK_BUTTON(z->next), tr("lit_again"));
}

static void lit_opt_clicked(GtkButton *button, gpointer data) {
    LitQuiz *z = data;
    int chosen = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "idx"));
    const LitQ *q;
    char buf[128];

    if (z->answered)
        return;
    z->answered = 1;
    q = &z->qs[z->idx];

    for (int i = 0; i < 4; i++)
        gtk_widget_set_sensitive(z->opts[i], FALSE);

    gtk_widget_add_css_class(z->opts[q->correct], "ok");
    if (chosen == q->correct)
        z->score++;
    else
        gtk_widget_add_css_class(z->opts[chosen], "wrong");

    g_snprintf(buf, sizeof(buf), tr("lit_score_fmt"), z->score, z->n);
    gtk_label_set_text(GTK_LABEL(z->score_lbl), buf);

    gtk_label_set_text(GTK_LABEL(z->expl), q->expl);
    gtk_widget_set_visible(z->expl, TRUE);

    gtk_button_set_label(GTK_BUTTON(z->next),
                         z->idx + 1 >= z->n ? tr("lit_show_result")
                                            : tr("lit_next"));
    gtk_widget_set_visible(z->next, TRUE);
}

static void lit_next_clicked(GtkButton *button, gpointer data) {
    LitQuiz *z = data;

    (void)button;

    if (z->finished) {
        z->finished = 0;
        z->idx = 0;
        z->score = 0;
        lit_quiz_show(z);
        return;
    }
    if (z->idx + 1 >= z->n) {
        lit_quiz_finish(z);
        return;
    }
    z->idx++;
    lit_quiz_show(z);
}

GtkWidget *build_cetba1984_quiz_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *box;
    GtkWidget *head;
    LitQuiz *z = g_new0(LitQuiz, 1);

    z->qs = lit_qs;
    z->n = LIT_NQ;

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("cetba1984", "lit_quiz_title", "lit_quiz_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_widget_set_margin_start(box, 6);
    gtk_widget_set_margin_end(box, 6);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), box);

    head = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append(GTK_BOX(box), head);

    z->progress = gtk_label_new("");
    gtk_widget_set_halign(z->progress, GTK_ALIGN_START);
    gtk_widget_set_hexpand(z->progress, TRUE);
    gtk_label_set_xalign(GTK_LABEL(z->progress), 0.0);
    gtk_widget_add_css_class(z->progress, "lit-progress");
    gtk_box_append(GTK_BOX(head), z->progress);

    z->score_lbl = gtk_label_new("");
    gtk_widget_set_halign(z->score_lbl, GTK_ALIGN_END);
    gtk_widget_add_css_class(z->score_lbl, "lit-score");
    gtk_box_append(GTK_BOX(head), z->score_lbl);

    z->prompt = gtk_label_new("");
    gtk_widget_set_halign(z->prompt, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(z->prompt), TRUE);
    gtk_widget_set_margin_top(z->prompt, 4);
    gtk_widget_add_css_class(z->prompt, "lit-prompt");
    gtk_box_append(GTK_BOX(box), z->prompt);

    z->grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(z->grid), 12);
    gtk_grid_set_column_spacing(GTK_GRID(z->grid), 12);
    gtk_grid_set_row_homogeneous(GTK_GRID(z->grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(z->grid), TRUE);
    gtk_widget_set_vexpand(z->grid, TRUE);
    gtk_box_append(GTK_BOX(box), z->grid);

    for (int i = 0; i < 4; i++) {
        GtkWidget *btn = gtk_button_new();
        GtkWidget *lbl;
        char cls[16];

        lbl = gtk_label_new("");
        gtk_label_set_wrap(GTK_LABEL(lbl), TRUE);
        gtk_label_set_justify(GTK_LABEL(lbl), GTK_JUSTIFY_CENTER);
        gtk_label_set_max_width_chars(GTK_LABEL(lbl), 26);

        g_snprintf(cls, sizeof(cls), "kahoot-%d", i);
        gtk_widget_add_css_class(btn, "kahoot-opt");
        gtk_widget_add_css_class(btn, cls);
        gtk_widget_set_hexpand(btn, TRUE);
        gtk_widget_set_vexpand(btn, TRUE);
        gtk_button_set_child(GTK_BUTTON(btn), lbl);
        g_object_set_data(G_OBJECT(btn), "idx", GINT_TO_POINTER(i));
        g_signal_connect(btn, "clicked", G_CALLBACK(lit_opt_clicked), z);

        z->opts[i] = btn;
        z->opt_lbl[i] = lbl;
        gtk_grid_attach(GTK_GRID(z->grid), btn, i % 2, i / 2, 1, 1);
    }

    z->expl = gtk_label_new("");
    gtk_widget_set_halign(z->expl, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(z->expl), TRUE);
    gtk_label_set_xalign(GTK_LABEL(z->expl), 0.0);
    gtk_widget_add_css_class(z->expl, "lit-expl");
    gtk_widget_set_visible(z->expl, FALSE);
    gtk_box_append(GTK_BOX(box), z->expl);

    z->next = gtk_button_new();
    i18n_bind(z->next, "lit_next", 1);
    gtk_widget_add_css_class(z->next, "btn-primary");
    gtk_widget_set_halign(z->next, GTK_ALIGN_END);
    gtk_widget_set_margin_top(z->next, 6);
    gtk_widget_set_visible(z->next, FALSE);
    gtk_box_append(GTK_BOX(box), z->next);
    g_signal_connect(z->next, "clicked", G_CALLBACK(lit_next_clicked), z);

    lit_quiz_show(z);

    return page;
}

/* ------------------------------------------------------------------ */
/* Sestavte děj (drag & drop)                                         */
/* ------------------------------------------------------------------ */

static const AssemblyItem lit_plot_items[] = {
    {
        "Přetáhněte části příběhu do správného pořadí:",
        {
            "Winston upravuje historii na Ministerstvu pravdy",
            "Seznámí se s Julií a začnou se tajně scházet",
            "Pronajme si pokoj nad obchodem pana Charringtona",
            "Přečte Goldsteinovu knihu u O'Briena",
            "Jsou zatčeni Policií myšlení",
            "Winston se zlomí v místnosti 101",
        },
        6,
    },
};

static const char *lit_plot_meaning[] = {
    "Správné pořadí: přepisování historie → seznámení s Julií → "
    "pronájem pokoje → četba Goldsteinovy knihy → zatčení → místnost 101.",
};

static UnitCtx lit_plot_unit;

GtkWidget *build_cetba1984_plot_page(void) {
    lit_plot_unit.page = "cetba1984";
    lit_plot_unit.progress_file = "progress/cetba.conf";
    return build_assembly(&lit_plot_unit, tr("lit_plot_title"),
                          "lit_plot_sub", 1, lit_plot_items,
                          lit_plot_meaning, 1);
}

/* ------------------------------------------------------------------ */
/* Reading list and 1984 overview                                     */
/* ------------------------------------------------------------------ */

static GtkWidget *lit_link_card(const char *target, const char *title,
                                const char *sub, GtkDrawingAreaDrawFunc icon) {
    GtkWidget *btn = gtk_button_new();
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 14);
    GtkWidget *ic;
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    GtkWidget *t = gtk_label_new(NULL);
    GtkWidget *s = gtk_label_new(NULL);

    gtk_widget_add_css_class(btn, "lit-link");
    gtk_widget_set_hexpand(btn, TRUE);

    ic = icon_area_new(icon, 0, 0, 0, 34);
    gtk_widget_set_valign(ic, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(row), ic);

    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_halign(t, GTK_ALIGN_START);
    gtk_label_set_xalign(GTK_LABEL(t), 0.0);
    gtk_label_set_wrap(GTK_LABEL(t), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(t), 40);
    gtk_widget_add_css_class(t, "lit-link-title");
    i18n_bind(t, title, 0);
    gtk_box_append(GTK_BOX(box), t);

    gtk_widget_set_halign(s, GTK_ALIGN_START);
    gtk_label_set_xalign(GTK_LABEL(s), 0.0);
    gtk_label_set_wrap(GTK_LABEL(s), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(s), 46);
    gtk_widget_add_css_class(s, "lit-link-sub");
    i18n_bind(s, sub, 0);
    gtk_box_append(GTK_BOX(box), s);

    gtk_box_append(GTK_BOX(row), box);
    gtk_button_set_child(GTK_BUTTON(btn), row);
    g_object_set_data_full(G_OBJECT(btn), "target", g_strdup(target), g_free);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_nav_clicked), NULL);
    return btn;
}

static void lit_note_card(GtkWidget *parent, const char *title,
                          GtkDrawingAreaDrawFunc icon,
                          const char *const *lines) {
    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *head;
    GtkWidget *ic;
    GtkWidget *t;

    gtk_widget_add_css_class(card, "notes-card");
    gtk_widget_set_margin_bottom(card, 14);
    gtk_box_append(GTK_BOX(parent), card);

    head = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append(GTK_BOX(card), head);

    ic = icon_area_new(icon, 0, 0, 0, 26);
    gtk_widget_set_valign(ic, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(head), ic);

    t = gtk_label_new(title);
    gtk_widget_set_halign(t, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(t), TRUE);
    gtk_widget_set_valign(t, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(t, "notes-title");
    gtk_box_append(GTK_BOX(head), t);

    for (int i = 0; lines[i]; i++) {
        GtkWidget *l = gtk_label_new(lines[i]);

        gtk_widget_set_halign(l, GTK_ALIGN_START);
        gtk_label_set_wrap(GTK_LABEL(l), TRUE);
        gtk_label_set_xalign(GTK_LABEL(l), 0.0);
        gtk_widget_add_css_class(l, "notes-body");
        gtk_box_append(GTK_BOX(card), l);
    }
}

GtkWidget *build_readinglist_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *col;
    GtkWidget *hero;
    GtkWidget *card;
    GtkWidget *row;
    GtkWidget *ic;
    GtkWidget *texts;
    GtkWidget *title;
    GtkWidget *author;
    GtkWidget *sub;

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("czechmap", "Maturitní četba", "reading_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);

    wrap = gtk_center_box_new();
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), wrap);

    col = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);
    gtk_widget_set_valign(col, GTK_ALIGN_CENTER);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), col);

    hero = icon_area_new(draw_book_icon, 0, 0, 0, 92);
    gtk_widget_add_css_class(hero, "reading-hero");
    gtk_box_append(GTK_BOX(col), hero);

    card = gtk_button_new();
    gtk_widget_add_css_class(card, "book-card");
    gtk_widget_set_hexpand(card, TRUE);

    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
    gtk_button_set_child(GTK_BUTTON(card), row);

    ic = icon_area_new(draw_book_icon, 0, 0, 0, 54);
    gtk_widget_set_valign(ic, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(row), ic);

    texts = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_valign(texts, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(texts, TRUE);
    gtk_box_append(GTK_BOX(row), texts);

    title = gtk_label_new("1984");
    gtk_widget_set_halign(title, GTK_ALIGN_START);
    gtk_widget_add_css_class(title, "book-title");
    gtk_box_append(GTK_BOX(texts), title);

    author = gtk_label_new("George Orwell");
    gtk_widget_set_halign(author, GTK_ALIGN_START);
    gtk_widget_add_css_class(author, "book-author");
    gtk_box_append(GTK_BOX(texts), author);

    sub = gtk_label_new(NULL);
    gtk_widget_set_halign(sub, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(sub), TRUE);
    gtk_widget_add_css_class(sub, "book-sub");
    i18n_bind(sub, "book_1984_sub", 0);
    gtk_box_append(GTK_BOX(texts), sub);

    g_object_set_data_full(G_OBJECT(card), "target", g_strdup("cetba1984"),
                           g_free);
    g_signal_connect(card, "clicked", G_CALLBACK(on_nav_clicked), NULL);
    gtk_box_append(GTK_BOX(col), card);

    return page;
}

GtkWidget *build_cetba1984_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *box;
    GtkWidget *links;
    static const char *about[] = {
        "Autor: George Orwell (vl. jm. Eric Arthur Blair)",
        "Žánr: antiutopický (dystopický) román",
        "Poprvé vydáno: 1949",
        "Děj: Londýn, provincie Letopočet 1, stát Oceánie",
        "Vyprávění: v er-formě, po deníkových zápiscích Winstona Smithe",
        NULL,
    };
    static const char *world[] = {
        "Svět je rozdělen na tři supervelmoci: Oceánii, Eurasii a Východasii, "
        "které spolu vedou trvalou válku.",
        "Vládne jediná strana (Angsoc) v čele s kultem Velkého bratra.",
        "Ministerstvo pravdy – přepisuje historii a vyrábí „pravdu“.",
        "Ministerstvo lásky – mučírny a likvidace nepohodlných.",
        "Ministerstvo hojnosti – plánuje hospodářství a drží lidi v "
        "nedostatku.",
        "Ministerstvo míru – vede válku.",
        NULL,
    };
    static const char *people[] = {
        "Winston Smith – hlavní hrdina, přepisuje historii, touží po svobodě.",
        "Julie – mladá členka strany, Winstonova tajná láska.",
        "O'Brien – člen vnitřní strany, předstírá odboj a Winstona zničí.",
        "Emmanuel Goldstein – nepřítel státu, údajný vůdce Bratrstva.",
        "Velký bratr – symbol všemocné moci strany.",
        "Pan Charrington – zdánlivý obchodník, ve skutečnosti agent.",
        NULL,
    };
    static const char *terms[] = {
        "Newspeak – uměle zjednodušovaný jazyk, který omezuje myšlení.",
        "Dvojité myšlení – ochota věřit dvěma protikladným pravdám.",
        "Zločin myšlení – už samotná zakázaná myšlenka.",
        "Policie myšlení – tajná policie, která zatýká a mučí.",
        "Bratrstvo – možná neexistující odbojová organizace.",
        "Místnost 101 – nejhlubší mučírna, pro každého jeho největší strach.",
        NULL,
    };

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("readinglist", "George Orwell – 1984",
                           "cetba1984_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_widget_set_margin_bottom(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), box);

    links = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_bottom(links, 14);
    gtk_box_append(GTK_BOX(box), links);
    gtk_box_append(GTK_BOX(links),
                   lit_link_card("cetba1984quiz", "lit_quiz_title",
                                 "lit_quiz_sub", draw_quiz_icon));
    gtk_box_append(GTK_BOX(links),
                   lit_link_card("cetba1984dej", "lit_plot_title",
                                 "lit_plot_sub", draw_order_icon));

    lit_note_card(box, "O knize", draw_book_icon, about);
    lit_note_card(box, "Svět a strana", draw_globe_icon, world);
    lit_note_card(box, "Postavy", draw_people_icon, people);
    lit_note_card(box, "Klíčové pojmy", draw_bulb_icon, terms);

    return page;
}
