#include "graduately.h"

/* Year 2: National Revival, Romanticism, Realism, the Máj generation,
 * ruchovci and lumírovci, Czech realism, the modern movement. */

NetLesson lit2_lessons[LIT_N] = {
    { .n_slides = 3, .unit_page = "lit2unit1", .ex_page = "lit2ex1" },
    { .n_slides = 3, .unit_page = "lit2unit2", .ex_page = "lit2ex2" },
    { .n_slides = 3, .unit_page = "lit2unit3", .ex_page = "lit2ex3" },
    { .n_slides = 3, .unit_page = "lit2unit4", .ex_page = "lit2ex4" },
    { .n_slides = 3, .unit_page = "lit2unit5", .ex_page = "lit2ex5" },
    { .n_slides = 3, .unit_page = "lit2unit6", .ex_page = "lit2ex6" },
    { .n_slides = 3, .unit_page = "lit2unit7", .ex_page = "lit2ex7" },
    { .n_slides = 3, .unit_page = "lit2unit8", .ex_page = "lit2ex8" },
};

static GtkWidget *build_lit2_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Smysl", "Proč obrození vzniklo",
            "Tip: obrození zachraňuje češtinu jako jazyk vzdělanosti a národa.",
            {
                "Na konci 18. století je čeština hlavně jazykem venkova a kuchyně.",
                "Osvícenství a herderovská myšlenka národa dávají podnět k obraně jazyka.",
                "Cíl je vrátit češtinu do vědy, školy, divadla a krásné literatury.",
                "Obrození se obvykle dělí na fázi obrannou a ofenzivní.",
                NULL,
            },
        },
        {
            "2 / 3   •   Obrana", "Dobrovský a první generace",
            "Tip: Dobrovský češtinu vědecky popisuje, ale o její budoucnosti pochybuje.",
            {
                "Josef Dobrovský píše Zevrubnou mluvnici a zkoumá staroslověnštinu.",
                "Václav Thám a Kramerius vydávají české knihy a noviny pro lid.",
                "Vzniká české divadlo, Bouda a snaha hrát v národním jazyce.",
                "První fáze jazyk brání a popisuje, ještě ho plně nerozvíjí v umění.",
                NULL,
            },
        },
        {
            "3 / 3   •   Útok", "Jungmann, Kollár, Čelakovský",
            "Tip: Rukopisy královédvorský a zelenohorský měly dodat národu starou epiku.",
            {
                "Josef Jungmann vytváří česko-německý slovník a překládá z evropských literatur.",
                "František Palacký píše dějiny jako příběh národa.",
                "Kollárova Slávy dcera a Čelakovského Ohlasy spojují slovanskou myšlenku s lidovou písní.",
                "Rukopisy z let 1817 a 1818 literaturu na čas ovlivnily, později byly odhaleny jako padělky.",
                NULL,
            },
        },
    };

    return lit_unit_page(1, &lit2_lessons[0], "lit2_unit1", "lit2_unit1_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit2_unit1_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jaký byl hlavní cíl národního obrození?",
         {"Nahradit češtinu latinou",
          "Obnovit češtinu jako jazyk kultury a národa",
          "Zavést romantický román podle Francie",
          "Zrušit divadlo"},
         4, 1},
        {"Čím je Josef Dobrovský?",
         {"Autorem Máje", "Jazykovědcem obranné fáze",
          "Lumírovcem", "Autorem Bible kralické"},
         4, 1},
        {"Kdo sestavil velký česko-německý slovník?",
         {"Josef Jungmann", "Karel Hynek Mácha",
          "Jan Neruda", "Jaroslav Vrchlický"},
         4, 0},
        {"Co jsou Rukopisy královédvorský a zelenohorský?",
         {"Díla Kosmy", "Padělky, které měly dokázat starobylost české epiky",
          "Husitské kroniky", "Barokní kancionály"},
         4, 1},
    };
    static const char *hints[] = {
        "Obrození brání a potom rozvíjí národní jazyk.",
        "Dobrovský patří k obranné fázi, Jungmann k ofenzivní.",
        "Jungmannův slovník a překlady otevírají češtinu vysoké literatuře.",
        "RKZ vznikly 1817 a 1818 a později byly rozpoznány jako podvrhy.",
    };

    return lit_mcq_page(1, 0, "lit2unit1", "lit2_ex1_title", "lit2_quiz1_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit2_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Postoj", "Jedinec proti světu",
            "Tip: romantik hledá v minulosti, přírodě a výjimečném hrdinovi to, co současnost nedává.",
            {
                "Romantismus staví cit, obraznost a svobodu jednotlivce proti pravidlům klasicismu.",
                "Hrdina je výjimečný, často vyvržený, a střetává se se společností.",
                "Oblíbené je středověk, lidová slovesnost, exotika a tajemno.",
                "Rozpor mezi snem a skutečností je základní romantické napětí.",
                NULL,
            },
        },
        {
            "2 / 3   •   Evropa", "Byron, Hugo, Puškin",
            "Tip: Byronův hrdina je unavený, hrdý a v rozporu se světem.",
            {
                "Anglie: Byron, Shelley, Scottovy historické romány a jezerní básníci.",
                "Francie: Victor Hugo, Chrám Matky Boží v Paříži a Bídníci, předmluva ke Cromwellovi.",
                "Německo: bratři Grimmové, Novalis a Heinrich Heine.",
                "Rusko: Puškinův Evžen Oněgin a Lermontov. Polsko: Mickiewicz.",
                NULL,
            },
        },
        {
            "3 / 3   •   Amerika", "Poe a romantické tajemno",
            "Tip: Poe stojí u hororu, detektivky i teorie básně.",
            {
                "Edgar Allan Poe píše Havrana, hrůzostrašné povídky a detektivní záhady.",
                "Romantismus otevírá i gotický román a téma dvojnictví.",
                "Historický román učí číst dějiny jako osud národa.",
                "Tyto vzory silně působí na český romantismus v čele s Máchou.",
                NULL,
            },
        },
    };

    return lit_unit_page(1, &lit2_lessons[1], "lit2_unit2", "lit2_unit2_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit2_unit2_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je pro romantismus typické?",
         {"Tři jednoty a rozumová kázeň",
          "Cit, rozpor jedince se společností a zájem o minulost",
          "Přesný popis všední práce",
          "Odmítnutí obraznosti"},
         4, 1},
        {"Kdo napsal Evžena Oněgina?",
         {"Victor Hugo", "Alexandr Sergejevič Puškin",
          "George Gordon Byron", "Edgar Allan Poe"},
         4, 1},
        {"Které dílo patří Victoru Hugovi?",
         {"Bídníci", "Lakomec", "Candide", "Král Oidipus"},
         4, 0},
        {"Čím je Edgar Allan Poe?",
         {"Autorem realistického románu o venkově",
          "Romantikem hororu, tajemna a detektivní povídky",
          "Klasicistním dramatikem",
          "Autorem husitské kroniky"},
         4, 1},
    };
    static const char *hints[] = {
        "Romantický hrdina stojí proti obyčejnému světu.",
        "Evžen Oněgin je veršovaný román Puškinův.",
        "Hugo napsal Bídníky a Chrám Matky Boží v Paříži.",
        "Havran a povídky tajemna jsou Poeova romantická tvorba.",
    };

    return lit_mcq_page(1, 1, "lit2unit2", "lit2_ex2_title", "lit2_quiz2_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit2_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Máj", "Karel Hynek Mácha",
            "Tip: Máj vyšel 1836 a doba mu vytýkala, že není dost vlastenecký.",
            {
                "Máj je lyrickoepická skladba o vině, času a rozporu snu se skutečností.",
                "Vilém zabije svůdce milenky a neví, že zabil vlastního otce.",
                "Příroda je krásná a lhostejná k lidskému utrpení.",
                "Báseň je psána jambem a patří k vrcholům české poezie.",
                NULL,
            },
        },
        {
            "2 / 3   •   Balada", "Erben, Tyl a Němcová",
            "Tip: v Kytici trestá osud vinu, často vinu rodičů na dětech.",
            {
                "Karel Jaromír Erben vydal 1853 Kytici, soubor balad z lidové víry.",
                "Josef Kajetán Tyl píše Strakonického dudáka a hru Fidlovačka s písní Kde domov můj.",
                "Božena Němcová v Babičce staví ideál lidskosti a venkovského řádu.",
                "Její povídky, třeba Divá Bára, hájí právo ženy na vlastní cestu.",
                NULL,
            },
        },
        {
            "3 / 3   •   Satira", "Havlíček a vyvrcholení obrození",
            "Tip: Havlíček už romantický patos ironizuje a měří ho politikou.",
            {
                "Karel Havlíček Borovský píše epigramy, Tyrolské elegie a Křest svatého Vladimíra.",
                "Satira míří na absolutismus, církevní moc a národní pózu.",
                "Třetí fáze obrození splývá s romantismem a politickým ruchem roku 1848.",
                "Mácha, Erben, Tyl, Němcová a Havlíček jsou jádro českého romantismu.",
                NULL,
            },
        },
    };

    return lit_unit_page(1, &lit2_lessons[2], "lit2_unit3", "lit2_unit3_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit2_unit3_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdy vyšel Máchův Máj?",
         {"Roku 1836", "Roku 1853", "Roku 1858", "Roku 1895"},
         4, 0},
        {"O čem jsou Erbenovy balady v Kytici?",
         {"O budování továren", "O vině a trestu podle lidové víry",
          "O legionářích", "O absurdním čekání"},
         4, 1},
        {"Kde zazní píseň Kde domov můj?",
         {"V Máji", "Ve Fidlovačce Josefa Kajetána Tyla",
          "V Babičce", "V Labyrintu světa"},
         4, 1},
        {"Které dílo napsal Karel Havlíček Borovský?",
         {"Tyrolské elegie", "Povídky malostranské",
          "Psohlavci", "R.U.R."},
         4, 0},
    };
    static const char *hints[] = {
        "Máj 1836, Kytice 1853, almanach Máj 1858.",
        "Erbenova balada spojuje lidový příběh s mravním řádem.",
        "Tylova Fidlovačka obsahuje píseň, z níž vznikla hymna.",
        "Havlíček: epigramy, Tyrolské elegie, Křest svatého Vladimíra.",
    };

    return lit_mcq_page(1, 2, "lit2unit3", "lit2_ex3_title", "lit2_quiz3_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit2_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Metoda", "Vidět společnost takovou, jaká je",
            "Tip: realismus typizuje, naturalismus přidává dědičnost a prostředí.",
            {
                "Realismus popisuje současnost, společenské vrstvy a všední konflikty.",
                "Postava je typ: nese rysy své doby, třídy a prostředí.",
                "Vypravěč bývá střízlivý a dává prostor detailu.",
                "Naturalismus jde dál: člověka určuje dědičnost, pudy a hmotné podmínky.",
                NULL,
            },
        },
        {
            "2 / 3   •   Francie a Anglie", "Od Balzaca k Zolovi",
            "Tip: Paní Bovaryová ukazuje střet romantického snu s provincií.",
            {
                "Honoré de Balzac v Lidské komedii mapuje francouzskou společnost.",
                "Stendhalův Červený a černý sleduje ctižádost v porevoluční Francii.",
                "Flaubertova Paní Bovaryová je vrchol psychologického realismu.",
                "Zola (Zabiják) je naturalista. Dickens (Oliver Twist) kritizuje bídu průmyslové Anglie.",
                NULL,
            },
        },
        {
            "3 / 3   •   Rusko", "Tolstoj, Dostojevskij, Čechov",
            "Tip: Dostojevskij zkoumá vinu svědomí, Tolstoj široký obraz doby.",
            {
                "Nikolaj Gogol spojuje satiru a grotesku, Revizor je komedie o strachu z moci.",
                "Lev Tolstoj píše Vojnu a mír a Annu Kareninu.",
                "Fjodor Michajlovič Dostojevskij ve Zločinu a trestu řeší vinu a trest svědomí.",
                "Anton Pavlovič Čechov zklidňuje drama i povídku do zdánlivě obyčejného života.",
                NULL,
            },
        },
    };

    return lit_unit_page(1, &lit2_lessons[3], "lit2_unit4", "lit2_unit4_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit2_unit4_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Čím se realismus liší od romantismu?",
         {"Hledá výjimečného hrdinu mimo společnost",
          "Zobrazuje současnou společnost a typické postavy",
          "Drží se tří jednot klasicismu",
          "Píše jen duchovní legendy"},
         4, 1},
        {"Kdo je autorem Paní Bovaryové?",
         {"Émile Zola", "Gustave Flaubert", "Charles Dickens", "Lev Tolstoj"},
         4, 1},
        {"Které dílo napsal Dostojevskij?",
         {"Zločin a trest", "Oliver Twist", "Evžen Oněgin", "Bídníci"},
         4, 0},
        {"Co zdůrazňuje naturalismus?",
         {"Jen šťastný konec pohádky",
          "Vliv dědičnosti, pudů a prostředí",
          "Pravidla antické tragédie",
          "Slovanskou vzájemnost"},
         4, 1},
    };
    static const char *hints[] = {
        "Realistický hrdina je typ své vrstvy a doby.",
        "Flaubert napsal Paní Bovaryovou, Zola je naturalista.",
        "Zločin a trest je román o vině Raskolnikova.",
        "Zola chápe člověka jako určeného tělem a prostředím.",
    };

    return lit_mcq_page(1, 3, "lit2unit4", "lit2_ex4_title", "lit2_quiz4_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit2_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Program", "Almanach Máj 1858",
            "Tip: májovci chtějí literaturu světovou, pravdivou a současnou.",
            {
                "Mladá generace se hlásí k Máchovi a odmítá vlasteneckou frázi.",
                "Almanach Máj žádá umění otevřené Evropě a skutečnému životu.",
                "K okruhu patří Neruda, Hálek, Světlá a Pfleger.",
                "Generace reaguje na Bachův absolutismus a politické zklamání.",
                NULL,
            },
        },
        {
            "2 / 3   •   Neruda", "Poezie, fejeton a Malá Strana",
            "Tip: Povídky malostranské jsou galerie typů, ne idyla.",
            {
                "Jan Neruda píše Hřbitovní kvítí, Knihy veršů, Písně kosmické a Balady a romance.",
                "Povídky malostranské zachycují pražskou čtvrť s ironií i soucitem.",
                "Fejeton v Národních listech je Nerudova škola pohledu na den.",
                "Balady a romance spojují legendu, lidový tón a moderní skepsi.",
                NULL,
            },
        },
        {
            "3 / 3   •   Vedle něj", "Hálek, Světlá, Arbes",
            "Tip: Arbesovo romaneto je napínavý příběh s racionálním vysvětlením.",
            {
                "Vítězslav Hálek je lyrik májovců, autor Večerních písní.",
                "Karolina Světlá píše Vesnický román a prosazuje ženskou vzdělanost.",
                "Jakub Arbes vytváří romaneto, třeba Newtonův mozek.",
                "Májovci otevírají cestu pozdějšímu realismu.",
                NULL,
            },
        },
    };

    return lit_unit_page(1, &lit2_lessons[4], "lit2_unit5", "lit2_unit5_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit2_unit5_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdy vyšel almanach Máj?",
         {"Roku 1836", "Roku 1858", "Roku 1868", "Roku 1895"},
         4, 1},
        {"Kdo napsal Povídky malostranské?",
         {"Jan Neruda", "Božena Němcová", "Alois Jirásek", "Karel Čapek"},
         4, 0},
        {"Co je romaneto?",
         {"Lidová balada", "Napínavý příběh s racionálním jádrem",
          "Klasicistní tragédie", "Husitská píseň"},
         4, 1},
        {"Jaký program měli májovci?",
         {"Návrat výhradně k latině",
          "Literatura světová, pravdivá a současná",
          "Jen oslavné ódy na panovníka",
          "Odmítnutí Máje"},
         4, 1},
    };
    static const char *hints[] = {
        "Almanach Máj je 1858, Máchův Máj je 1836, Ruch 1868.",
        "Neruda v nich kreslí typy z Malé Strany.",
        "Romaneto je žánr Jakuba Arbesa.",
        "Májovci navazují na Máchu a chtějí umění bez fráze.",
    };

    return lit_mcq_page(1, 4, "lit2unit5", "lit2_ex5_title", "lit2_quiz5_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit2_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Spor", "Dvě odpovědi na smysl literatury",
            "Tip: ruchovci brání národní tradici, lumírovci otevírají okno do Evropy.",
            {
                "Po májovcích se generace rozchází v tom, čemu má literatura sloužit.",
                "Ruchovci kolem almanachu Ruch 1868 zdůrazňují národ a domácí látky.",
                "Lumírovci kolem časopisu Lumír chtějí světovost a dokonalou formu.",
                "Spor není osobní hádka, ale dvě představy o úkolu českého umění.",
                NULL,
            },
        },
        {
            "2 / 3   •   Ruch", "Svatopluk Čech",
            "Tip: Písně otroka jsou politická alegorie nesvobody.",
            {
                "Svatopluk Čech píše Písně otroka, Evropy a satiru o panu Broučkovi.",
                "Eliška Krásnohorská hájí srozumitelnost a národní úkol poezie.",
                "Ruchovci navazují na obrozenskou představu básníka jako mluvčího národa.",
                "Forma bývá rétorická, téma často vlastenecké a slovanské.",
                NULL,
            },
        },
        {
            "3 / 3   •   Lumír", "Vrchlický, Sládek, Zeyer",
            "Tip: Vrchlický přináší do češtiny formy i látky z celé Evropy.",
            {
                "Jaroslav Vrchlický je mimořádně plodný lyrik, epik i dramatik, autor Noci na Karlštejně.",
                "Josef Václav Sládek píše intimní a venkovskou lyriku a překládá.",
                "Julius Zeyer čerpá z mýtů a legend, Radúz a Mahulena je pohádkové drama.",
                "Lumírovci připravují půdu pro českou modernu.",
                NULL,
            },
        },
    };

    return lit_unit_page(1, &lit2_lessons[5], "lit2_unit6", "lit2_unit6_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit2_unit6_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Čím se lumírovci liší od ruchovců?",
         {"Odmítají veškerou poezii",
          "Kladou důraz na světovost a uměleckou formu",
          "Píšou jen latinsky",
          "Patří k baroku"},
         4, 1},
        {"Kdo napsal Písně otroka?",
         {"Svatopluk Čech", "Jaroslav Vrchlický", "Jan Neruda", "Karel Hynek Mácha"},
         4, 0},
        {"Které drama napsal Vrchlický?",
         {"Maryša", "Noc na Karlštejně", "Lakomec", "Audience"},
         4, 1},
        {"Kam patří Julius Zeyer?",
         {"K májovcům", "K lumírovcům", "K proletářské poezii", "K samizdatu"},
         4, 1},
    };
    static const char *hints[] = {
        "Lumír znamená otevření české literatury Evropě.",
        "Svatopluk Čech je hlavní básník ruchovců.",
        "Noc na Karlštejně je veršovaná komedie Vrchlického.",
        "Zeyer, Vrchlický a Sládek tvoří jádro lumírovců.",
    };

    return lit_mcq_page(1, 5, "lit2unit6", "lit2_ex6_title", "lit2_quiz6_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit2_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Venkov", "Realistická próza",
            "Tip: venkovský realismus neidealizuje, ukazuje bídu, víru i krutost zvyku.",
            {
                "Český realismus se obrací k venkovu, městu i historii.",
                "Karel Václav Rais v Kalibově zločinu sleduje chudobu a vinu.",
                "Teréza Nováková a Ignát Herrmann přidávají region a město.",
                "Postava je určena prostředím, majetkem a mravem obce.",
                NULL,
            },
        },
        {
            "2 / 3   •   Historie", "Jirásek a historická próza",
            "Tip: Jirásek vykládá dějiny jako posilu národního vědomí.",
            {
                "Alois Jirásek píše Psohlavce, Temno, F. L. Věka a Staré pověsti české.",
                "Zikmund Winter přidává kulturněhistorický detail městského života.",
                "Historická próza navazuje na Palackého pojetí českých dějin.",
                "Na jevišti ji doplňuje drama z venkova a z dějin.",
                NULL,
            },
        },
        {
            "3 / 3   •   Drama", "Maryša a naturalismus",
            "Tip: Maryša je tragédie nevolené svatby a bezmoci ženy.",
            {
                "Alois a Vilém Mrštíkové napsali Maryšu, hru o vynuceném sňatku.",
                "Gabriela Preissová v Gazdině robě otevírá ženský osud na vesnici.",
                "Naturalismus u nás zastupuje Karel Matěj Čapek-Chod.",
                "Konec století už připravuje modernu, symbol a individuální senzibilitu.",
                NULL,
            },
        },
    };

    return lit_unit_page(1, &lit2_lessons[6], "lit2_unit7", "lit2_unit7_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit2_unit7_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo napsal Kalibův zločin?",
         {"Karel Václav Rais", "Alois Jirásek", "Jan Neruda", "Julius Zeyer"},
         4, 0},
        {"Které dílo patří Aloisi Jiráskovi?",
         {"Temno", "Máj", "Babička", "R.U.R."},
         4, 0},
        {"O čem je Maryša?",
         {"O kosmických písních",
          "O vynuceném sňatku a tragédii venkovské ženy",
          "O cestě do Itálie",
          "O robotovi"},
         4, 1},
        {"Kam patří historická próza Jiráska a Wintra?",
         {"K českému realismu druhé poloviny 19. století",
          "K antice", "K poetismu", "K literatuře po roce 1989"},
         4, 0},
    };
    static const char *hints[] = {
        "Rais je představitel venkovského realismu.",
        "Temno, Psohlavci a Staré pověsti české jsou Jiráskovy.",
        "Maryšu napsali bratři Mrštíkové.",
        "Historický román tu slouží i národní paměti.",
    };

    return lit_mcq_page(1, 6, "lit2unit7", "lit2_ex7_title", "lit2_quiz7_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit2_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Evropa", "Prokletí básníci a dekadence",
            "Tip: Baudelairovy Květy zla otevírají moderní městskou obraznost.",
            {
                "Konec století hledá nový jazyk: symbol, náladu, smyslovost a skepsi.",
                "Charles Baudelaire, Paul Verlaine a Arthur Rimbaud jsou prokletí básníci.",
                "Oscar Wilde v Obrazu Doriana Graye spojuje krásu, morálku a rozklad.",
                "Symbol nesděluje věc přímo, nýbrž ji naznačuje.",
                NULL,
            },
        },
        {
            "2 / 3   •   Čechy", "Česká moderna",
            "Tip: Manifest české moderny vyšel roku 1895.",
            {
                "Manifest žádá individualitu, kritičnost a konec vlastenecké rutiny.",
                "K modernistům patří Antonín Sova, Otokar Březina a Karel Hlaváček.",
                "Jiří Karásek ze Lvovic reprezentuje dekadenci, Josef Svatopluk Machar skepsi a satiru.",
                "Impresionismus v básni zachycuje prchavý dojem, symbolismus skrytý smysl.",
                NULL,
            },
        },
        {
            "3 / 3   •   Buřiči", "Generace anarchistů",
            "Tip: buřiči spojují vzdor, erotiku a soucit se sociálním dnem.",
            {
                "Fráňa Šrámek píše Stříbrný vítr a drama Měsíc nad řekou.",
                "Viktor Dyk je autorem Krysaře a vzdorné politické lyriky.",
                "Petr Bezruč ve Slezských písních mluví hlasem utiskovaného regionu.",
                "S modernou končí 19. století a otevírá se literatura století dvacátého.",
                NULL,
            },
        },
    };

    return lit_unit_page(1, &lit2_lessons[7], "lit2_unit8", "lit2_unit8_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit2_unit8_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdy vyšel Manifest české moderny?",
         {"Roku 1858", "Roku 1895", "Roku 1918", "Roku 1948"},
         4, 1},
        {"Kdo napsal Květy zla?",
         {"Charles Baudelaire", "Oscar Wilde", "Antonín Sova", "Jan Neruda"},
         4, 0},
        {"Které dílo napsal Viktor Dyk?",
         {"Krysař", "Babička", "Maryša", "Edison"},
         4, 0},
        {"Čím jsou Slezské písně Petra Bezruče?",
         {"Oslavou pražského salonu",
          "Hlasem sociálního a národního útisku ve Slezsku",
          "Barokní legendou",
          "Antickým eposem"},
         4, 1},
    };
    static const char *hints[] = {
        "1895 je programový rok české moderny.",
        "Baudelaire je ústřední prokletý básník.",
        "Krysař je Dykova novela o moci a pomstě.",
        "Bezruč mluví za chudý a utlačovaný kraj.",
    };

    return lit_mcq_page(1, 7, "lit2unit8", "lit2_ex8_title", "lit2_quiz8_head",
                        qs, hints, 4);
}

void add_lit2_pages(GtkStack *stack) {
    typedef GtkWidget *(*LitBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        LitBuilder build_unit;
        LitBuilder build_ex;
    } pages[] = {
        {"lit2unit1", "lit2ex1", build_lit2_unit1_page, build_lit2_unit1_exercise_page},
        {"lit2unit2", "lit2ex2", build_lit2_unit2_page, build_lit2_unit2_exercise_page},
        {"lit2unit3", "lit2ex3", build_lit2_unit3_page, build_lit2_unit3_exercise_page},
        {"lit2unit4", "lit2ex4", build_lit2_unit4_page, build_lit2_unit4_exercise_page},
        {"lit2unit5", "lit2ex5", build_lit2_unit5_page, build_lit2_unit5_exercise_page},
        {"lit2unit6", "lit2ex6", build_lit2_unit6_page, build_lit2_unit6_exercise_page},
        {"lit2unit7", "lit2ex7", build_lit2_unit7_page, build_lit2_unit7_exercise_page},
        {"lit2unit8", "lit2ex8", build_lit2_unit8_page, build_lit2_unit8_exercise_page},
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
