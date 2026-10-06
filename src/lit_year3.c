#include "graduately.h"

/* Year 3: the first half of the 20th century, world and Czech. */

NetLesson lit3_lessons[LIT_N] = {
    { .n_slides = 3, .unit_page = "lit3unit1", .ex_page = "lit3ex1" },
    { .n_slides = 3, .unit_page = "lit3unit2", .ex_page = "lit3ex2" },
    { .n_slides = 3, .unit_page = "lit3unit3", .ex_page = "lit3ex3" },
    { .n_slides = 3, .unit_page = "lit3unit4", .ex_page = "lit3ex4" },
    { .n_slides = 3, .unit_page = "lit3unit5", .ex_page = "lit3ex5" },
    { .n_slides = 3, .unit_page = "lit3unit6", .ex_page = "lit3ex6" },
    { .n_slides = 3, .unit_page = "lit3unit7", .ex_page = "lit3ex7" },
    { .n_slides = 3, .unit_page = "lit3unit8", .ex_page = "lit3ex8" },
};

static GtkWidget *build_lit3_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Zlom", "Umění po roce 1900",
            "Tip: avantgarda chce tvořit budoucnost, ne jen zdobit přítomnost.",
            {
                "Nové směry odmítají popisnou iluzi 19. století.",
                "Futurismus oslavuje rychlost, stroj a rozchod s muzeem.",
                "Expresionismus křičí vnitřní úzkost, dadaismus se vysmívá smyslu války.",
                "Surrealismus, vedený Bretonem, čerpá ze snu a nevědomí.",
                NULL,
            },
        },
        {
            "2 / 3   •   Báseň", "Apollinaire a pásmo",
            "Tip: Pásmo ruší pravidelnou sloku a skládá báseň z volných obrazů.",
            {
                "Guillaume Apollinaire píše Pásmo a dává jméno kubismu i novému lyrismu.",
                "Báseň může být bez interpunkce, jako proud vjemů velkoměsta.",
                "Vladimir Majakovskij spojuje futurismus s politickým gestem.",
                "Volný verš a asociace se stávají běžným nástrojem moderny.",
                NULL,
            },
        },
        {
            "3 / 3   •   Smysl", "Proč avantgardu znát",
            "Tip: český poetismus a surrealismus z těchto podnětů přímo vyrůstají.",
            {
                "Avantgarda je mezinárodní a časopisy i manifesty putují přes hranice.",
                "Umění se chápe jako experiment, ne jako hotový vzor.",
                "Válka 1914–1918 tyto směry radikalizuje.",
                "Bez avantgardy nelze číst Nezvala, Seiferta ani evropskou poezii 20. století.",
                NULL,
            },
        },
    };

    return lit_unit_page(2, &lit3_lessons[0], "lit3_unit1", "lit3_unit1_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit3_unit1_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co chce avantgarda?",
         {"Vrátit se k třem jednotám",
          "Experimentovat a tvořit nové umění",
          "Psát jen historické romány",
          "Obnovit latinskou kroniku"},
         4, 1},
        {"Kdo napsal Pásmo?",
         {"Guillaume Apollinaire", "Franz Kafka",
          "Ernest Hemingway", "Karel Čapek"},
         4, 0},
        {"Odkud čerpá surrealismus?",
         {"Z úředního jazyka", "Ze snu a nevědomí",
          "Z antické jednoty času", "Z kramářské písně"},
         4, 1},
        {"Který směr oslavuje rychlost a stroj?",
         {"Klasicismus", "Futurismus", "Venkovský realismus", "Baroko"},
         4, 1},
    };
    static const char *hints[] = {
        "Avantgarda je programová moderna počátku 20. století.",
        "Apollinairovo Pásmo je vzor moderní volné básně.",
        "Bretonův surrealismus navazuje na Freudovo nevědomí.",
        "Futurismus vyhlašuje Marinetti, v Rusku Majakovskij.",
    };

    return lit_mcq_page(2, 0, "lit3unit1", "lit3_ex1_title", "lit3_quiz1_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit3_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Válka", "Ztracená generace a Remarque",
            "Tip: ztracená generace jsou autoři, které první válka připravila o iluze.",
            {
                "Ernest Hemingway píše Sbohem, armádo a později Stařec a moře.",
                "Francis Scott Fitzgerald zachycuje Velkého Gatsbyho a lesk i prázdnotu úspěchu.",
                "Erich Maria Remarque v románu Na západní frontě klid ukazuje zákop očima vojáka.",
                "John Steinbeck přidává sociální prózu, třeba O myších a lidech.",
                NULL,
            },
        },
        {
            "2 / 3   •   Experiment", "Kafka, Joyce, Proust",
            "Tip: Proměna začíná tím, že se člověk probudí jako hmyz, a nikdo se nad tím nepozastaví dost.",
            {
                "Franz Kafka v Proměně a Procesu zobrazuje úzkost a nepřehlednou moc.",
                "James Joyce v Odysseovi rozbíjí tradiční vyprávění proudem vědomí.",
                "Marcel Proust staví román na paměti a čase.",
                "Moderní próza už nespoléhá na vševědoucího vypravěče a lineární děj.",
                NULL,
            },
        },
        {
            "3 / 3   •   Varování", "Antiutopie a svědomí",
            "Tip: antiutopie ukazuje společnost, která štěstí vynucuje a svobodu ruší.",
            {
                "Aldous Huxley v Konci civilizace líčí svět řízený technikou a konzumem.",
                "Thomas Mann v Kouzelném vrchu spojuje nemoc, čas a evropskou krizi.",
                "Michail Bulgakov v Mistrovi a Markétce hájí umění proti násilné moci.",
                "Tyto prózy připravují poválečné antiutopie Orwella a Bradburyho.",
                NULL,
            },
        },
    };

    return lit_unit_page(2, &lit3_lessons[1], "lit3_unit2", "lit3_unit2_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit3_unit2_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo patří ke ztracené generaci?",
         {"Homér", "Ernest Hemingway", "Dante Alighieri", "Molière"},
         4, 1},
        {"Které dílo napsal Franz Kafka?",
         {"Proměna", "Babička", "Evžen Oněgin", "Maryša"},
         4, 0},
        {"O čem je Remarquův román Na západní frontě klid?",
         {"O salonu 18. století",
          "O zážitku vojáka v první světové válce",
          "O husitském zpěvu",
          "O národním obrození"},
         4, 1},
        {"Co je proud vědomí?",
         {"Lidová balada",
          "Záznam vnitřní řeči a vjemů bez tradičního děje",
          "Klasicistní prolog",
          "Barokní kázání"},
         4, 1},
    };
    static const char *hints[] = {
        "Hemingway, Fitzgerald a další píšou po první válce o ztrátě jistot.",
        "Proměna a Proces jsou Kafkovy klíčové prózy.",
        "Remarque bourá představu války jako hrdinského dobrodružství.",
        "Proud vědomí užívá Joyce a další modernisté.",
    };

    return lit_mcq_page(2, 1, "lit3unit2", "lit3_ex2_title", "lit3_quiz2_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit3_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Jeviště", "Drama se rozchází s iluzí",
            "Tip: epické divadlo Brechtovo chce, aby divák přemýšlel, ne jen prožíval.",
            {
                "Moderní drama opouští měšťanskou konverzaci jako jediný vzor.",
                "George Bernard Shaw spojuje komedii s myšlenkou a kritikou.",
                "Luigi Pirandello tematizuje, že člověk hraje role a pravda se tříští.",
                "Expresionistické drama křičí úzkost rozbitého světa.",
                NULL,
            },
        },
        {
            "2 / 3   •   Brecht", "Epické divadlo",
            "Tip: zcizovací efekt brání divákovi, aby se v postavě jen rozplynul.",
            {
                "Bertolt Brecht píše Matku Kuráž a Kavkazský křídový kruh.",
                "Písně, komentář a přerušení děje nutí zaujmout postoj.",
                "Divadlo má být politické a rozumové, ne jen dojemné.",
                "Brecht silně ovlivnil české divadlo druhé poloviny století.",
                NULL,
            },
        },
        {
            "3 / 3   •   Amerika", "O'Neill a počátek absurdity",
            "Tip: absurdní drama v plné podobě přijde až po druhé válce.",
            {
                "Eugene O'Neill přináší do Ameriky psychologické a tragické drama.",
                "Meziválečné jeviště zkouší masku, sbor i sen.",
                "Po roce 1945 na to naváže Beckett a Ionesco.",
                "Na maturitu patří Shaw, Pirandello a Brecht jako tři různé moderní cesty.",
                NULL,
            },
        },
    };

    return lit_unit_page(2, &lit3_lessons[2], "lit3_unit3", "lit3_unit3_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit3_unit3_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je epické divadlo?",
         {"Divadlo, které chce diváka dojmout až k slzám a nic víc",
          "Brechtovo divadlo, které nutí přemýšlet a zaujímat postoj",
          "Středověký mysterijní cyklus",
          "Opera bez textu"},
         4, 1},
        {"Kdo napsal Matku Kuráž?",
         {"Bertolt Brecht", "William Shakespeare",
          "Molière", "Václav Havel"},
         4, 0},
        {"Čím se Pirandello zabývá?",
         {"Husitskou písní", "Tím, že člověk hraje role a skutečnost se tříští",
          "Venkovským realismem", "Eposem o Gilgamešovi"},
         4, 1},
        {"Kdy se plně rozvine absurdní drama?",
         {"V antice", "Až po druhé světové válce",
          "V národním obrození", "Ve 14. století"},
         4, 1},
    };
    static const char *hints[] = {
        "Zcizovací efekt je hlavní nástroj Brechtova epického divadla.",
        "Matka Kuráž je brechtovská hra o válce a obchodu.",
        "Pirandello patří k moderně, která pochybuje o jedné pravdě postavy.",
        "Beckettovo Čekání na Godota je 1952, tedy po válce.",
    };

    return lit_mcq_page(2, 2, "lit3unit3", "lit3_ex3_title", "lit3_quiz3_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit3_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Švejk", "Jaroslav Hašek",
            "Tip: Švejk není jen vtip, je to obrana malého člověka proti mašinerii.",
            {
                "Osudy dobrého vojáka Švejka za světové války vyšly 1921–1923.",
                "Hašek zesměšňuje rakouskou armádu, úřad a velkou frázi.",
                "Švejk poslouchá tak doslova, až rozkaz ztrácí smysl.",
                "Román zůstal nedokončený a přesto se stal světovým.",
                NULL,
            },
        },
        {
            "2 / 3   •   Legie", "Válka a legionářská próza",
            "Tip: legionářská literatura vypráví anabázi československých legií.",
            {
                "Rudolf Medek a Josef Kopta píšou o legiích v Rusku.",
                "Válka vstupuje i do Vančurova Pole orná a válečná.",
                "Proti hrdinské legendě stojí Haškova groteska.",
                "Oba póly, patos i výsměch, patří k české paměti první války.",
                NULL,
            },
        },
        {
            "3 / 3   •   Poezie", "Wolker a proletářská poezie",
            "Tip: Wolker chce, aby báseň sloužila chudým a byla srozumitelná.",
            {
                "Jiří Wolker vydává Host do domu a Těžkou hodinu.",
                "Balady, třeba Balada o nenarozeném dítěti, spojují sociální téma s lidovým tónem.",
                "Proletářská poezie věří kolektivu a změně světa.",
                "Devětsil, založený 1920, z ní brzy přejde k poetismu.",
                NULL,
            },
        },
    };

    return lit_unit_page(2, &lit3_lessons[3], "lit3_unit4", "lit3_unit4_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit3_unit4_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo napsal Osudy dobrého vojáka Švejka?",
         {"Jaroslav Hašek", "Karel Čapek", "Jiří Wolker", "Viktor Dyk"},
         4, 0},
        {"Jak Švejk vzdoruje armádě?",
         {"Hrdinským útokem v čele legií",
          "Doslovnou poslušností, která rozkaz zesměšní",
          "Útěkem do exilu po roce 1968",
          "Psaním manifestu moderny"},
         4, 1},
        {"Které sbírky napsal Jiří Wolker?",
         {"Host do domu a Těžká hodina", "Kytice a Máj",
          "Květy zla a Havran", "Edison a Pásmo"},
         4, 0},
        {"Co je Devětsil?",
         {"Barokní řád", "Umělecký svaz založený roku 1920",
          "Husitská píseň", "Nakladatelství exilu v Torontu"},
         4, 1},
    };
    static const char *hints[] = {
        "Švejk je Haškův nedokončený román z první války.",
        "Groteskní poslušnost je Švejkova zbraň.",
        "Wolker je hlavní básník proletářské poezie.",
        "Z Devětsilu vyroste poetismus.",
    };

    return lit_mcq_page(2, 3, "lit3unit4", "lit3_ex4_title", "lit3_quiz4_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit3_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Poetismus", "Radost, město a asociace",
            "Tip: poetismus je český směr, teoretikem je Karel Teige.",
            {
                "Poetismus chce báseň jako hru obrazů, cestu a oslavu moderního života.",
                "Vítězslav Nezval píše Podivuhodného kouzelníka a Edison.",
                "Jaroslav Seifert debutuje Na vlnách TSF a zůstane u konkrétního, smyslového verše.",
                "Konstantin Biebl přidává exotiku a později smutek.",
                NULL,
            },
        },
        {
            "2 / 3   •   Surrealismus", "Sen a úzkost",
            "Tip: Nezvalův Absolutní hrobař už není radostný poetismus.",
            {
                "Ve 30. letech část poetistů přechází k surrealismu.",
                "Skupina surrealistů v ČSR vzniká 1934 kolem Nezvala.",
                "Obraz se stává snovějším a úzkostnějším.",
                "Vedle toho roste meditativní poezie, která avantgardu přijímá jen zčásti.",
                NULL,
            },
        },
        {
            "3 / 3   •   Jiný hlas", "Halas, Holan, Zahradníček",
            "Tip: Halasova Torzo naděje je odpověď na Mnichov, ne na karneval města.",
            {
                "František Halas píše temnou, zhuštěnou lyriku a Torzo naděje.",
                "Vladimír Holan je básník meditace a později svědectví.",
                "Jan Zahradníček představuje spirituální a katolickou poezii.",
                "Meziválečná poezie tedy není jen Devětsil: má i pól víry a úzkosti.",
                NULL,
            },
        },
    };

    return lit_unit_page(2, &lit3_lessons[4], "lit3_unit5", "lit3_unit5_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit3_unit5_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo je teoretikem poetismu?",
         {"Karel Teige", "Josef Dobrovský", "Jan Hus", "Bertolt Brecht"},
         4, 0},
        {"Kterou skladbu napsal Vítězslav Nezval?",
         {"Edison", "Máj", "Kytice", "Babička"},
         4, 0},
        {"Kdy vznikla Skupina surrealistů v ČSR?",
         {"Roku 1858", "Roku 1934", "Roku 863", "Roku 1989"},
         4, 1},
        {"Čím je Halasovo Torzo naděje?",
         {"Poetistickou oslavou velkoměsta",
          "Básnickou odpovědí na ohrožení republiky",
          "Legionářským románem",
          "Klasicistní komedií"},
         4, 1},
    };
    static const char *hints[] = {
        "Teige formuluje poetismus jako umění života.",
        "Edison je Nezvalova pásmová báseň o vynálezci a noci.",
        "1934 je rok českého surrealistického seskupení.",
        "Torzo naděje reaguje na krizi konce 30. let.",
    };

    return lit_mcq_page(2, 4, "lit3unit5", "lit3_ex5_title", "lit3_quiz5_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit3_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Čapek", "Demokratický proud",
            "Tip: Čapek brání obyčejného člověka a varuje před mocí, která se vymkne.",
            {
                "Karel Čapek píše R.U.R., kde zazní slovo robot, Krakatit a Válku s mloky.",
                "Bílá nemoc a Matka jsou dramata o válce a odpovědnosti.",
                "Hordubal, Povětroň a Obyčejný život jsou noetická trilogie: pravda není jedna.",
                "Povídky z jedné a druhé kapsy a fejetony drží lidský rozměr.",
                NULL,
            },
        },
        {
            "2 / 3   •   Vančura", "Jazyk jako událost",
            "Tip: Vančura píše obrazně a vědomě proti všednímu novinovému stylu.",
            {
                "Vladislav Vančura je autorem Rozmarného léta a Markéty Lazarové.",
                "Jeho věta je rytmická, metaforická a často archaicky zbarvená.",
                "Patří k levicové avantgardě, ale není schematický.",
                "Vedle něj stojí reportážní a novinářská próza meziválečné demokracie.",
                NULL,
            },
        },
        {
            "3 / 3   •   Smích", "Poláček, Olbracht a další",
            "Tip: Bylo nás pět je dětská perspektiva, která odhaluje svět dospělých.",
            {
                "Karel Poláček píše Bylo nás pět a Muže v offsidu.",
                "Ivan Olbracht: Nikola Šuhaj loupežník a Golet v údolí.",
                "Eduard Bass a Karel Poláček představují humoristickou a novinářskou linii.",
                "Demokratický proud věří republice, rozumu a občanské slušnosti.",
                NULL,
            },
        },
    };

    return lit_unit_page(2, &lit3_lessons[5], "lit3_unit6", "lit3_unit6_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit3_unit6_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Ve kterém díle zazní slovo robot?",
         {"V R.U.R. Karla Čapka", "V Máji", "V Kytici", "V Švejkovi"},
         4, 0},
        {"Kdo napsal Rozmarné léto?",
         {"Vladislav Vančura", "Jaroslav Hašek", "Božena Němcová", "Alois Jirásek"},
         4, 0},
        {"Kterou knihu napsal Karel Poláček?",
         {"Bylo nás pět", "Temno", "Labyrint světa", "Krysař"},
         4, 0},
        {"O čem varuje Válka s mloky?",
         {"O nebezpečí moci, která využije lidskou chtivost",
          "O pravidlech sonetu",
          "O příchodu věrozvěstů",
          "O barokním emblému"},
         4, 0},
    };
    static const char *hints[] = {
        "Slovo robot vymyslel Josef Čapek, svět je zná z dramatu R.U.R.",
        "Rozmarné léto a Markéta Lazarová jsou Vančurovy prózy.",
        "Poláček je autor Bylo nás pět a Mužů v offsidu.",
        "Mloci jsou podobenství o civilizaci, která si vypěstuje zhoubu.",
    };

    return lit_mcq_page(2, 5, "lit3unit6", "lit3_ex6_title", "lit3_quiz6_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit3_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Duše", "Psychologická próza",
            "Tip: psychologická próza sleduje vnitřní motiv, ne jen vnější děj.",
            {
                "Jaroslav Havlíček píše Petrolejové lampy a Neviditelného.",
                "Jarmila Glazarová v Adventu zobrazuje krutost a závislost na vesnici.",
                "Egon Hostovský a Václav Řezáč přidávají úzkost a mravní selhání.",
                "Člověk tu není typ vrstvy, ale spleť nutkání a vin.",
                NULL,
            },
        },
        {
            "2 / 3   •   Víra", "Katolický proud",
            "Tip: katolická literatura není jen pobožnost, je to svébytný umělecký názor.",
            {
                "Jaroslav Durych píše historický román Bloudění.",
                "Jakub Deml tvoří deníkovou a spirituální prózu.",
                "Jan Čep a Jan Zahradníček drží venkov a víru jako řád.",
                "Proud stojí vedle avantgardy a často s ní polemizuje.",
                NULL,
            },
        },
        {
            "3 / 3   •   Venkov", "Ruralismus",
            "Tip: ruralismus idealizuje venkov jako zdroj řádu proti městu.",
            {
                "Ruralisté, třeba Josef Knap a František Křelina, brání selskou tradici.",
                "Liší se od kritického realismu, který vesnici nešetřil.",
                "Ve 30. letech je to vlivný, dnes diskutovaný proud.",
                "Maturita má umět tyto proudy rozlišit, ne je slepit v jednu meziválečnou prózu.",
                NULL,
            },
        },
    };

    return lit_unit_page(2, &lit3_lessons[6], "lit3_unit7", "lit3_unit7_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit3_unit7_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo napsal Petrolejové lampy?",
         {"Jaroslav Havlíček", "Karel Čapek", "Ivan Olbracht", "Jiří Wolker"},
         4, 0},
        {"Kam patří Jaroslav Durych?",
         {"Ke katolickému proudu", "K poetismu",
          "Ke ztracené generaci", "K ruchovcům"},
         4, 0},
        {"Co zdůrazňuje ruralismus?",
         {"Velkoměstský kaleidoskop",
          "Venkov a selský řád",
          "Antickou tragédii",
          "Absenci děje"},
         4, 1},
        {"Čím se psychologická próza zabývá především?",
         {"Vnitřním motivem a vinou postavy",
          "Překladem antických eposů",
          "Pravidly klasicistního verše",
          "Kronikou Velké Moravy"},
         4, 0},
    };
    static const char *hints[] = {
        "Havlíček je autor Petrolejových lamp a Neviditelného.",
        "Durychovo Bloudění je historický román katolického proudu.",
        "Ruralismus staví vesnici proti rozkladnému městu.",
        "Děj je tu hlavně cesta do nitra.",
    };

    return lit_mcq_page(2, 6, "lit3unit7", "lit3_ex7_title", "lit3_quiz7_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit3_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Okupace", "Literatura za protektorátu",
            "Tip: po roce 1939 mizí svobodný tisk a řada autorů píše do šuplíku nebo umírá.",
            {
                "Německá okupace zavírá českou kulturu do cenzury a perzekuce.",
                "Mladí básníci kolem Jiřího Ortena píšou elegii a deník ohroženého života.",
                "Ortenovy sbírky, třeba Ohnice, vycházejí pod cizím jménem.",
                "Halas, Holan a Seifert reagují na válku zhuštěným veršem.",
                NULL,
            },
        },
        {
            "2 / 3   •   Skupina", "Skupina 42",
            "Tip: Skupina 42 bere město, periferii a civilní řeč, ne snový symbol.",
            {
                "Skupina 42 vzniká za války a programově se dívá na civilizaci města.",
                "Patří k ní Josef Kainar, Jiří Kolář, Ivan Blatný a teoretik Jindřich Chalupecký.",
                "Báseň i próza zaznamenávají banalitu, která je pod tlakem dějin přízračná.",
                "Skupina působí i po válce, než ji politický režim umlčí.",
                NULL,
            },
        },
        {
            "3 / 3   •   Svědectví", "Co se o válce píše hned a co později",
            "Tip: velké prózy o šoa často vzniknou až po roce 1945.",
            {
                "Za války vzniká poezie, deník a ilegální text, ne velký románový odstup.",
                "Julius Fučík píše Reportáž psanou na oprátce ve vězení.",
                "Lustig, Fuks, Hrabal nebo Otčenášek válku zpracují až v dalších desetiletích.",
                "Na maturitu oddělte dobu vzniku od doby, o které dílo vypráví.",
                NULL,
            },
        },
    };

    return lit_unit_page(2, &lit3_lessons[7], "lit3_unit8", "lit3_unit8_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit3_unit8_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo je básník Ohnice?",
         {"Jiří Orten", "Jaroslav Seifert", "Vítězslav Nezval", "Petr Bezruč"},
         4, 0},
        {"Čím se vyznačuje Skupina 42?",
         {"Návratem ke klasicistní tragédii",
          "Civilním pohledem na město a periferii",
          "Venkovským ruralismem",
          "Psaním jen v latině"},
         4, 1},
        {"Kdo patří ke Skupině 42?",
         {"Josef Kainar a Jiří Kolář", "Karel Hynek Mácha a Karel Jaromír Erben",
          "Homér a Vergilius", "Jungmann a Dobrovský"},
         4, 0},
        {"Proč velké romány o válce často nejsou z let okupace?",
         {"Protože za cenzury a bez odstupu vznikala spíš poezie, deník a svědectví",
          "Protože se o válce nesmělo psát ani po roce 1945",
          "Protože próza vznikla až ve středověku",
          "Protože autoři neznali téma města"},
         4, 0},
    };
    static const char *hints[] = {
        "Orten publikoval za okupace pod pseudonymem a zahynul roku 1941.",
        "Program Skupiny 42 formuluje civilizační a městský pohled.",
        "Kainar, Kolář a Blatný jsou její básníci.",
        "Lustig, Fuks a další válečné prózy jsou poválečné.",
    };

    return lit_mcq_page(2, 7, "lit3unit8", "lit3_ex8_title", "lit3_quiz8_head",
                        qs, hints, 4);
}

void add_lit3_pages(GtkStack *stack) {
    typedef GtkWidget *(*LitBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        LitBuilder build_unit;
        LitBuilder build_ex;
    } pages[] = {
        {"lit3unit1", "lit3ex1", build_lit3_unit1_page, build_lit3_unit1_exercise_page},
        {"lit3unit2", "lit3ex2", build_lit3_unit2_page, build_lit3_unit2_exercise_page},
        {"lit3unit3", "lit3ex3", build_lit3_unit3_page, build_lit3_unit3_exercise_page},
        {"lit3unit4", "lit3ex4", build_lit3_unit4_page, build_lit3_unit4_exercise_page},
        {"lit3unit5", "lit3ex5", build_lit3_unit5_page, build_lit3_unit5_exercise_page},
        {"lit3unit6", "lit3ex6", build_lit3_unit6_page, build_lit3_unit6_exercise_page},
        {"lit3unit7", "lit3ex7", build_lit3_unit7_page, build_lit3_unit7_exercise_page},
        {"lit3unit8", "lit3ex8", build_lit3_unit8_page, build_lit3_unit8_exercise_page},
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
