#include "graduately.h"

/* Biology for Základy přírodopisných věd.
 * Follows the secondary-school outline: general biology, a survey of
 * organisms, human biology, genetics and ecology. */

NetLesson bio_lessons[SCI_N] = {
    { .n_slides = 3, .unit_page = "biounit1", .ex_page = "bioex1" },
    { .n_slides = 3, .unit_page = "biounit2", .ex_page = "bioex2" },
    { .n_slides = 3, .unit_page = "biounit3", .ex_page = "bioex3" },
    { .n_slides = 3, .unit_page = "biounit4", .ex_page = "bioex4" },
    { .n_slides = 3, .unit_page = "biounit5", .ex_page = "bioex5" },
    { .n_slides = 3, .unit_page = "biounit6", .ex_page = "bioex6" },
    { .n_slides = 3, .unit_page = "biounit7", .ex_page = "bioex7" },
    { .n_slides = 3, .unit_page = "biounit8", .ex_page = "bioex8" },
};

static GtkWidget *build_bio_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Život", "Čím se živé liší",
            "Tip: metabolismus je přeměna látek a energie, ne jen dýchání.",
            {
                "Živé soustavy mají metabolismus, dráždivost, růst a vývin.",
                "Rozmnožují se a předávají dědičnou informaci.",
                "Udržují stálé vnitřní prostředí, homeostázu.",
                "Bez příjmu energie a látek z okolí život nepokračuje.",
                NULL,
            },
        },
        {
            "2 / 3   •   Řada", "Od buňky k ekosystému",
            "Tip: populace je jeden druh, společenstvo je víc druhů pohromadě.",
            {
                "Buňka je základní stavební a funkční jednotka živého.",
                "Tkáně tvoří orgány, orgány soustavy a soustavy organismus.",
                "Populace jsou jedinci téhož druhu na jednom místě.",
                "Společenstvo s neživým prostředím tvoří ekosystém.",
                NULL,
            },
        },
        {
            "3 / 3   •   Výživa", "Autotrof a heterotrof",
            "Tip: zelená rostlina je autotrof, živočich heterotrof.",
            {
                "Autotrof tvoří organické látky z anorganických, hlavně fotosyntézou.",
                "Heterotrof přijímá hotové organické látky.",
                "Život na Zemi je vázaný na vodu a na zdroj energie.",
                "Buňka vznikla dřív než mnohobuněčné organismy.",
                NULL,
            },
        },
    };

    return sci_unit_page(1, &bio_lessons[0], "bio_unit1", "bio_unit1_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_bio_unit1_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je základní jednotka živých organismů?",
         {"Atom", "Buňka", "Ekosystém", "Orgán"},
         4, 1},
        {"Co je metabolismus?",
         {"Jen pohyb živočicha",
          "Přeměna látek a energie v organismu",
          "Počet chromozomů",
          "Rozmnožování viru mimo buňku"},
         4, 1},
        {"Co je populace?",
         {"Jedinci jednoho druhu na určitém místě",
          "Všechny druhy v ekosystému",
          "Jedna buňka",
          "Neživé složky prostředí"},
         4, 0},
        {"Čím se autotrof liší od heterotrofa?",
         {"Nemá buňky",
          "Sám tvoří organické látky z anorganických",
          "Nepotřebuje energii",
          "Žije jen bez vody"},
         4, 1},
    };
    static const char *hints[] = {
        "Buněčná teorie: nová buňka vzniká z buňky.",
        "Metabolismus zahrnuje výstavbu i rozklad látek.",
        "Společenstvo už sdružuje populace různých druhů.",
        "Rostliny fotosyntetizují. Živočichové a houby jsou heterotrofní.",
    };

    return sci_mcq_page(1, 0, "biounit1", "bio_ex1_title", "bio_quiz1_head",
                        qs, hints, 4);
}

static GtkWidget *build_bio_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Dva typy", "Prokaryota a eukaryota",
            "Tip: bakterie nemají jádro, živočišná a rostlinná buňka ano.",
            {
                "Prokaryotní buňka nemá pravé jádro. Patří sem bakterie.",
                "Eukaryotní buňka má jádro oddělené membránou a má organely.",
                "Plazmatická membrána ohraničuje každou buňku.",
                "Ribozomy vyrábějí bílkoviny v obou typech buněk.",
                NULL,
            },
        },
        {
            "2 / 3   •   Organely", "Co je v eukaryotní buňce",
            "Tip: chloroplast mají rostliny, mitochondrii mají i živočichové.",
            {
                "Jádro obsahuje DNA a řídí buňku.",
                "Mitochondrie uvolňují energii buněčným dýcháním.",
                "Chloroplasty provádějí fotosyntézu a jsou v rostlinách.",
                "Rostlinná buňka má navíc buněčnou stěnu a vakuolu.",
                NULL,
            },
        },
        {
            "3 / 3   •   Dělení", "Mitóza a meióza",
            "Tip: člověk má 46 chromozomů, pohlavní buňka 23.",
            {
                "Mitóza dává dvě tělní buňky se stejnou genetickou informací.",
                "Slouží k růstu, obnově a nepohlavnímu rozmnožování.",
                "Meióza tvoří pohlavní buňky s polovičním počtem chromozomů.",
                "Oplozením se počet chromozomů vrací na původní.",
                NULL,
            },
        },
    };

    return sci_unit_page(1, &bio_lessons[1], "bio_unit2", "bio_unit2_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_bio_unit2_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Která buňka je prokaryotní?",
         {"Bakterie", "Rostlinná buňka", "Živočišná buňka", "Buňka houby s jádrem"},
         4, 0},
        {"Kde probíhá fotosyntéza v rostlinné buňce?",
         {"V mitochondrii", "V chloroplastu", "V ribozomu", "Jen v jádře"},
         4, 1},
        {"K čemu slouží mitóza?",
         {"Ke vzniku pohlavních buněk s poloviční sadou",
          "Ke vzniku dvou geneticky stejných tělních buněk",
          "K tvorbě viru",
          "K odstranění jádra"},
         4, 1},
        {"Kolik chromozomů má lidská pohlavní buňka?",
         {"46", "23", "92", "2"},
         4, 1},
    };
    static const char *hints[] = {
        "Prokaryota nemají pravé jádro. Eukaryota ano.",
        "Chloroplast je zelený a je v rostlinách a některých protistech.",
        "Meióza, ne mitóza, snižuje počet chromozomů na polovinu.",
        "Tělní buňka má 46, spermie a vajíčko po 23.",
    };

    return sci_mcq_page(1, 1, "biounit2", "bio_ex2_title", "bio_quiz2_head",
                        qs, hints, 4);
}

static GtkWidget *build_bio_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Virus", "Na hranici živého",
            "Tip: virus není buňka a sám se mimo hostitele nemnoží.",
            {
                "Virus má nukleovou kyselinu v bílkovinném obalu.",
                "Nemá vlastní metabolismus a množí se jen v hostitelské buňce.",
                "Způsobuje třeba chřipku. Antibiotika na virus nepůsobí.",
                "Očkování učí imunitu poznat virus dřív, než nemoc propukne.",
                NULL,
            },
        },
        {
            "2 / 3   •   Bakterie a houby", "Rozkladači a původci",
            "Tip: houba není rostlina, živí se hotovými organickými látkami.",
            {
                "Bakterie jsou prokaryota. Některé jsou symbionti, jiné patogeny.",
                "V průmyslu kvasí jogurt, v přírodě rozkládají organické zbytky.",
                "Houby mají buněčnou stěnu z chitinu a nemají chlorofyl.",
                "Kvasinky, plísně i kloboukaté houby patří mezi houby.",
                NULL,
            },
        },
        {
            "3 / 3   •   Rostliny", "Kořen, stonek, list",
            "Tip: fotosyntéza běží hlavně v listech, v chloroplastech.",
            {
                "Rostliny jsou autotrofní a mají buněčnou stěnu z celulózy.",
                "Kořen přijímá vodu a minerály a upevňuje rostlinu.",
                "Stonek nese listy a vede látky, list je hlavní místo fotosyntézy.",
                "Květ slouží rozmnožování, semeno obsahuje zárodek.",
                NULL,
            },
        },
    };

    return sci_unit_page(1, &bio_lessons[2], "bio_unit3", "bio_unit3_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_bio_unit3_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Proč se virus množí jen v hostiteli?",
         {"Má vlastní chloroplast, ale bez světla",
          "Nemá buněčný metabolismus",
          "Je to prokaryotní buňka",
          "Má jádro a mitochondrie"},
         4, 1},
        {"Čím se houby liší od rostlin?",
         {"Nemají buňky",
          "Jsou heterotrofní a nemají chlorofyl",
          "Mají chloroplasty navíc",
          "Jsou vždy jednobuněčné prokaryota"},
         4, 1},
        {"Kde v rostlině probíhá většina fotosyntézy?",
         {"V kořeni", "V listu", "Jen v květu", "Ve dřevu bez chloroplastů"},
         4, 1},
        {"Z čeho je buněčná stěna rostlin?",
         {"Z chitinu", "Z celulózy", "Z cholesterolu", "Z peptidoglykanu jako u všech hub"},
         4, 1},
    };
    static const char *hints[] = {
        "Virus je nukleová kyselina v obalu. Buňku si půjčuje.",
        "Chitin je stěna hub. Rostliny mají celulózu a chlorofyl.",
        "List má nejvíc chloroplastů. Kořen je většinou pod zemí.",
        "Rostliny: celulóza. Houby: chitin. Bakterie: peptidoglykan.",
    };

    return sci_mcq_page(1, 2, "biounit3", "bio_ex3_title", "bio_quiz3_head",
                        qs, hints, 4);
}

static GtkWidget *build_bio_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Bezobratlí", "Hmyz a další",
            "Tip: hmyz má tři páry nohou a tělo ze tří částí.",
            {
                "Bezobratlí nemají vnitřní kostru z obratlů.",
                "Kroužkovci, třeba žížala, mají článkované tělo.",
                "Měkkýši mají měkké tělo, často schránku. Patří sem hlemýžď.",
                "Hmyz má hlavu, hruď a zadeček a šest nohou.",
                NULL,
            },
        },
        {
            "2 / 3   •   Obratlovci", "Pět tříd, které škola probírá",
            "Tip: ptáci i savci mají stálou tělesnou teplotu.",
            {
                "Ryby dýchají žábrami a žijí ve vodě.",
                "Obojživelníci mají larvu ve vodě a vlhkou kůži, třeba žába.",
                "Plazi mají suchou kůži se šupinami a vejce s obalem.",
                "Ptáci mají peří, savci srst a mláďata krmí mlékem.",
                NULL,
            },
        },
        {
            "3 / 3   •   Vývoj", "Jak číst příbuznost",
            "Tip: vývoj není žebřík od horšího k dokonalému člověku.",
            {
                "Druhy se mění v čase. Společný předek vysvětluje podobnost.",
                "Obratlovci jsou jedna větev, ne vrchol všech živočichů.",
                "Hmyz je druhově nejbohatší skupina živočichů.",
                "Přizpůsobení prostředí se týká stavby těla i chování.",
                NULL,
            },
        },
    };

    return sci_unit_page(1, &bio_lessons[3], "bio_unit4", "bio_unit4_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_bio_unit4_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kolik nohou má hmyz?",
         {"Čtyři", "Šest", "Osm", "Deset"},
         4, 1},
        {"Čím dýchají ryby?",
         {"Plicemi jako savci", "Žábrami", "Jen kůží bez cév", "Vzdušnicemi jako hmyz"},
         4, 1},
        {"Který znak patří ptákům?",
         {"Mléčné žlázy", "Peří", "Žábry u dospělce", "Tělo bez kostry"},
         4, 1},
        {"Čím savci živí mláďata?",
         {"Vždy jen žloutkem mimo tělo", "Mlékem",
          "Fotosyntézou mláděte", "Filtrací vody žábrami"},
         4, 1},
    };
    static const char *hints[] = {
        "Tři páry nohou jsou šest. Pavouci mají osm a nejsou hmyz.",
        "Žábry berou kyslík z vody. Dospělá žába už dýchá plícemi a kůží.",
        "Peří je ptačí znak. Srst a mléko patří savcům.",
        "Mléčné žlázy jsou znak savců. Ptáci vejce zahřívají.",
    };

    return sci_mcq_page(1, 3, "biounit4", "bio_ex4_title", "bio_quiz4_head",
                        qs, hints, 4);
}

static GtkWidget *build_bio_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Pohyb", "Kostra a svaly",
            "Tip: kost je živá tkáň, ne jen minerál.",
            {
                "Kostra dává oporu, chrání orgány a je zásobárna vápníku.",
                "Sval se upíná na kost šlachou a pohyb vzniká jeho stahem.",
                "Kloub spojuje kosti pohyblivě.",
                "Páteř chrání míchu.",
                NULL,
            },
        },
        {
            "2 / 3   •   Oběh", "Srdce a krev",
            "Tip: malý oběh vede krev do plic, velký do těla.",
            {
                "Srdce člověka má dvě síně a dvě komory.",
                "Pravá komora žene krev do plic, levá do těla.",
                "Tepny vedou krev ze srdce, žíly do srdce.",
                "V plicních sklípcích se do krve dostává kyslík a odchází oxid uhličitý.",
                NULL,
            },
        },
        {
            "3 / 3   •   Trávení", "Od úst k ledvinám",
            "Tip: živiny se vstřebávají hlavně v tenkém střevě.",
            {
                "V ústech začíná mechanické a chemické zpracování potravy.",
                "Žaludek kyselinou a enzymy pokračuje v trávení bílkovin.",
                "Tenké střevo živiny vstřebá, tlusté hlavně vodu.",
                "Ledviny filtrují krev a tvoří moč, játra zpracovávají živiny.",
                NULL,
            },
        },
    };

    return sci_unit_page(1, &bio_lessons[4], "bio_unit5", "bio_unit5_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_bio_unit5_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kolik dutin má srdce člověka?",
         {"Dvě", "Tři", "Čtyři", "Jednu"},
         4, 2},
        {"Kam vede malý krevní oběh?",
         {"Z levé komory do celého těla", "Z pravé komory do plic",
          "Z jater do žaludku", "Z ledvin do střeva"},
         4, 1},
        {"Kde se vstřebává většina živin?",
         {"V jícnu", "V tenkém střevě", "V tlustém střevě jako cukry", "Ve slezině"},
         4, 1},
        {"Co tvoří ledviny?",
         {"Žluč", "Moč", "Insulin", "Slinu"},
         4, 1},
    };
    static const char *hints[] = {
        "Dvě síně a dvě komory. Mezi pravou a levou polovinou je přepážka.",
        "Malý oběh okysličuje krev. Velký ji rozvádí tělem.",
        "Klky tenkého střeva zvětšují plochu. Tlusté střevo hlavně zahušťuje.",
        "Játra tvoří žluč. Slinivka insulin. Ledviny moč.",
    };

    return sci_mcq_page(1, 4, "biounit5", "bio_ex5_title", "bio_quiz5_head",
                        qs, hints, 4);
}

static GtkWidget *build_bio_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Nervy", "Řízení organismu",
            "Tip: mozek a mícha jsou ústřední nervová soustava.",
            {
                "Nervová soustava vede vzruchy rychle a cíleně.",
                "Mozek a mícha tvoří ústřední část, nervy obvodovou.",
                "Reflex je odpověď na podnět, třeba ucuknutí od horka.",
                "Smysly převádějí podněty z okolí na vzruch.",
                NULL,
            },
        },
        {
            "2 / 3   •   Hormony", "Pomalejší řízení",
            "Tip: insulin ze slinivky snižuje hladinu glukózy v krvi.",
            {
                "Hormony jsou signální látky přenášené krví.",
                "Štítná žláza ovlivňuje metabolismus, nadledviny stresovou odpověď.",
                "Insulin snižuje glykemii, glukagon ji zvyšuje.",
                "Pohlavní hormony řídí dospívání a rozmnožování.",
                NULL,
            },
        },
        {
            "3 / 3   •   Obrana", "Rozmnožování a imunita",
            "Tip: očkování připraví imunitu, aniž by člověk nemoc prodělal.",
            {
                "Spermie a vajíčko mají po 23 chromozomech, zygota 46.",
                "Oplození spojí genetickou informaci obou rodičů.",
                "Bílé krvinky a protilátky patří k imunitě.",
                "Imunita rozliší vlastní buňky od cizích a od patogenů.",
                NULL,
            },
        },
    };

    return sci_unit_page(1, &bio_lessons[5], "bio_unit6", "bio_unit6_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_bio_unit6_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co tvoří ústřední nervovou soustavu?",
         {"Jen obvodové nervy", "Mozek a mícha",
          "Jen štítná žláza", "Srdce a plíce"},
         4, 1},
        {"Co dělá insulin?",
         {"Zvyšuje množství glukózy v krvi",
          "Snižuje hladinu glukózy v krvi",
          "Tvoří moč",
          "Nahrazuje červené krvinky"},
         4, 1},
        {"Kolik chromozomů má lidská zygota?",
         {"23", "46", "92 v každé běžné tělní buňce navíc", "2"},
         4, 1},
        {"K čemu je očkování?",
         {"Aby virus získal buňku",
          "Aby se imunita naučila poznat patogen předem",
          "Aby nahradilo všechny bílé krvinky natrvalo",
          "Aby zastavilo mitózu"},
         4, 1},
    };
    static const char *hints[] = {
        "Obvodové nervy spojují ústředí se smysly a svaly.",
        "Insulin vyrábí slinivka. Jeho nedostatek je podstatou cukrovky 1. typu.",
        "23 + 23 = 46. Pohlavní buňky jsou haploidní, zygota diploidní.",
        "Vakcína je antigen, který imunitní paměť připraví.",
    };

    return sci_mcq_page(1, 5, "biounit6", "bio_ex6_title", "bio_quiz6_head",
                        qs, hints, 4);
}

static GtkWidget *build_bio_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   DNA", "Gen a alela",
            "Tip: gen je úsek DNA, alela je jeho konkrétní forma.",
            {
                "DNA nese dědičnou informaci a je v chromozomech.",
                "Gen je úsek DNA, který ovlivňuje určitý znak.",
                "Alely jsou varianty téhož genu.",
                "Dominantní alela se projeví i v jedné kopii, recesivní až ve dvou.",
                NULL,
            },
        },
        {
            "2 / 3   •   Mendel", "Genotyp a fenotyp",
            "Tip: fenotyp je to, co vidíme, genotyp je zápis alel.",
            {
                "Johann Gregor Mendel sledoval dědičnost na hrachu.",
                "Při křížení se alely rozcházejí do pohlavních buněk.",
                "Genotyp je sestava alel, fenotyp je jejich projev.",
                "Ne každý znak řídí jediný gen a prostředí projev také mění.",
                NULL,
            },
        },
        {
            "3 / 3   •   Změna", "Mutace a pohlaví",
            "Tip: u člověka je typická žena XX a typický muž XY.",
            {
                "Mutace je změna DNA. Může být neutrální, škodlivá, nebo vzácně užitečná.",
                "Mutace v pohlavní buňce se může přenést na potomka.",
                "Pohlavní chromozomy člověka jsou X a Y.",
                "Kombinace XX odpovídá ženě, XY muži.",
                NULL,
            },
        },
    };

    return sci_unit_page(1, &bio_lessons[6], "bio_unit7", "bio_unit7_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_bio_unit7_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je gen?",
         {"Celá buňka", "Úsek DNA ovlivňující znak",
          "Druh bílkoviny v potravě", "Hormon štítné žlázy"},
         4, 1},
        {"Co je fenotyp?",
         {"Sestava alel v buňce", "Pozorovatelný projev znaku",
          "Počet ribozomů", "Vzorec glukózy"},
         4, 1},
        {"Kdy se projeví recesivní alela?",
         {"Vždycky, i když je v páru s dominantní",
          "Až když jsou recesivní obě alely",
          "Jen u bakterií",
          "Jen jako mutace viru"},
         4, 1},
        {"Která sestava chromozomů odpovídá muži?",
         {"XX", "XY", "YY jako jediná možnost u savců", "Jen autozomy bez X a Y"},
         4, 1},
    };
    static const char *hints[] = {
        "Alela je varianta genu. Chromozom nese mnoho genů.",
        "Genotyp je zápis, fenotyp je výsledek genů a prostředí.",
        "Heterozygot s dominantní alelou má dominantní fenotyp.",
        "XX je žena, XY muž. Y nese gen pro vývoj varlat.",
    };

    return sci_mcq_page(1, 6, "biounit7", "bio_ex7_title", "bio_quiz7_head",
                        qs, hints, 4);
}

static GtkWidget *build_bio_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Úrovně", "Od jedince k biosféře",
            "Tip: ekosystém zahrnuje živé i neživé.",
            {
                "Ekologie zkoumá vztahy organismů mezi sebou a k prostředí.",
                "Populace, společenstvo a ekosystém jsou tři navazující úrovně.",
                "Biosféra je všechny ekosystémy Země.",
                "Abiotické faktory jsou světlo, voda, teplota, vzduch a půda.",
                NULL,
            },
        },
        {
            "2 / 3   •   Potravní vztahy", "Kdo koho živí",
            "Tip: na každý další článek řetězce zbývá méně energie.",
            {
                "Producenti, hlavně rostliny, vážou energii fotosyntézou.",
                "Konzumenti se živí jinými organismy.",
                "Rozkladači, bakterie a houby, vracejí látky do koloběhu.",
                "V potravním řetězci se většina energie ztrácí jako teplo.",
                NULL,
            },
        },
        {
            "3 / 3   •   Člověk", "Prostředí a ochrana",
            "Tip: fotosyntéza váže CO2, dýchání a spalování ho vracejí.",
            {
                "Člověk mění krajinu, klima i biodiverzitu.",
                "Znečištění, kácení a nadměrný lov snižují rozmanitost druhů.",
                "Koloběh uhlíku spojuje fotosyntézu, dýchání a spalování.",
                "Ochrana přírody drží životní podmínky i pro člověka.",
                NULL,
            },
        },
    };

    return sci_unit_page(1, &bio_lessons[7], "bio_unit8", "bio_unit8_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_bio_unit8_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je ekosystém?",
         {"Jen jedinci jednoho druhu",
          "Společenstvo spolu s neživým prostředím",
          "Jedna buňka",
          "Jen atmosféra bez organismů"},
         4, 1},
        {"Kdo je v ekosystému producent?",
         {"Dravec", "Zelená rostlina", "Houba rozkladač", "Virus"},
         4, 1},
        {"Co dělají rozkladači?",
         {"Vyrábějí kyslík fotosyntézou jako hlavní producenti",
          "Rozkládají organické zbytky a vracejí látky do koloběhu",
          "Jsou vždy obratlovci",
          "Nahrazují producenty v moři i na souši jako rostliny"},
         4, 1},
        {"Proč je na vrcholu potravního řetězce málo energie?",
         {"Protože se na každém článku část energie ztratí",
          "Protože producenti energii nevytvářejí",
          "Protože rozkladači energii zdvojnásobí",
          "Protože slunce ekosystém neovlivňuje"},
         4, 0},
    };
    static const char *hints[] = {
        "Společenstvo jsou živé složky. Ekosystém k nim přidává neživé.",
        "Producent je autotrof. Konzument je heterotrof, který někoho žere.",
        "Bakterie a houby mineralizují zbytky. Rostliny je znovu využijí.",
        "Škola často uvádí, že na další hladinu přejde jen malá část energie.",
    };

    return sci_mcq_page(1, 7, "biounit8", "bio_ex8_title", "bio_quiz8_head",
                        qs, hints, 4);
}

void add_bio_pages(GtkStack *stack) {
    typedef GtkWidget *(*SciBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        SciBuilder build_unit;
        SciBuilder build_ex;
    } pages[] = {
        {"biounit1", "bioex1", build_bio_unit1_page, build_bio_unit1_exercise_page},
        {"biounit2", "bioex2", build_bio_unit2_page, build_bio_unit2_exercise_page},
        {"biounit3", "bioex3", build_bio_unit3_page, build_bio_unit3_exercise_page},
        {"biounit4", "bioex4", build_bio_unit4_page, build_bio_unit4_exercise_page},
        {"biounit5", "bioex5", build_bio_unit5_page, build_bio_unit5_exercise_page},
        {"biounit6", "bioex6", build_bio_unit6_page, build_bio_unit6_exercise_page},
        {"biounit7", "bioex7", build_bio_unit7_page, build_bio_unit7_exercise_page},
        {"biounit8", "bioex8", build_bio_unit8_page, build_bio_unit8_exercise_page},
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
