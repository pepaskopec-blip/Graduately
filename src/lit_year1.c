#include "graduately.h"

/* Year 1: literary theory, antiquity, the Middle Ages, Renaissance,
 * Baroque, Classicism and the Enlightenment. */

NetLesson lit_lessons[LIT_N] = {
    { .n_slides = 3, .unit_page = "litunit1", .ex_page = "litex1" },
    { .n_slides = 3, .unit_page = "litunit2", .ex_page = "litex2" },
    { .n_slides = 3, .unit_page = "litunit3", .ex_page = "litex3" },
    { .n_slides = 3, .unit_page = "litunit4", .ex_page = "litex4" },
    { .n_slides = 3, .unit_page = "litunit5", .ex_page = "litex5" },
    { .n_slides = 3, .unit_page = "litunit6", .ex_page = "litex6" },
    { .n_slides = 3, .unit_page = "litunit7", .ex_page = "litex7" },
    { .n_slides = 3, .unit_page = "litunit8", .ex_page = "litex8" },
};

static GtkWidget *build_lit_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Úvod", "Literatura jako umění slova",
            "Tip: nejdřív druh a žánr, potom jazyk a dobový kontext.",
            {
                "Literatura sděluje zážitek uměleckým jazykem, nejen holou informaci.",
                "Od publicistiky ji odlišuje estetická funkce a práce s obrazností.",
                "Má i funkci poznávací, výchovnou a zábavnou.",
                "Ve škole se sleduje vývoj v kulturních a historických souvislostech.",
                NULL,
            },
        },
        {
            "2 / 3   •   Druhy", "Lyrika, epika a drama",
            "Tip: žánr je užší typ uvnitř literárního druhu.",
            {
                "Lyrika vyjadřuje niterný zážitek, epika vypráví příběh.",
                "Drama je určeno k jevištnímu provedení a stojí na dialogu.",
                "Epika: epos, román, povídka, novela, bajka. Lyrika: píseň, óda, elegie, sonet, balada.",
                "Drama: tragédie, komedie, činohra. Vypravěč bývá v ich-formě, nebo er-formě.",
                NULL,
            },
        },
        {
            "3 / 3   •   Jazyk", "Verš, tropy a figury",
            "Tip: metafora pojmenuje podobnost přímo, přirovnání použije jako.",
            {
                "Rým je sdružený (AABB), střídavý (ABAB), obkročný (ABBA) nebo přerývaný.",
                "Volný verš se neváže pravidelným rýmem ani stopou.",
                "Stopy: trochej, jamb a daktyl. Tropy: metafora, metonymie, personifikace, symbol, alegorie.",
                "Figury: anafora, epifora, gradace, inverze, oxymóron a apostrofa.",
                NULL,
            },
        },
    };

    return lit_unit_page(0, &lit_lessons[0], "lit_unit1", "lit_unit1_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit_unit1_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Které jsou tři literární druhy?",
         {"Lyrika, epika a drama", "Román, povídka a novela",
          "Metafora, metonymie a symbol", "Baroko, romantismus a realismus"},
         4, 0},
        {"Co je metafora?",
         {"Přímé přirovnání se slovem jako",
          "Pojmenování na základě podobnosti",
          "Opakování slov na začátku veršů", "Báseň bez rýmu"},
         4, 1},
        {"Který rým má schéma ABAB?",
         {"Sdružený", "Obkročný", "Střídavý", "Přerývaný"},
         4, 2},
        {"Co znamená ich-forma?",
         {"Vyprávění v první osobě", "Vyprávění v třetí osobě",
          "Drama beze slov", "Volný verš"},
         4, 0},
    };
    static const char *hints[] = {
        "Druh je základní členění, žánr je užší typ uvnitř druhu.",
        "Přirovnání používá jako, metafora podobnost pojmenuje přímo.",
        "Sdružený je AABB, obkročný ABBA, střídavý ABAB.",
        "Ich-forma je já, er-forma je on nebo ona.",
    };

    return lit_mcq_page(0, 0, "litunit1", "lit_ex1_title", "lit_quiz1_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Orient", "Nejstarší písemnictví",
            "Tip: Epos o Gilgamešovi je nejstarší dochovaný epos.",
            {
                "Mezopotámie zanechala Epos o Gilgamešovi, příběh o přátelství a hledání nesmrtelnosti.",
                "Egypt známe z Knihy mrtvých, hymnů a milostné lyriky.",
                "Indie má Védy, eposy Mahábhárata a Rámájana a divadelní hry Kálidásovy.",
                "Čínská Kniha písní sbírá lidovou i obřadní lyriku.",
                NULL,
            },
        },
        {
            "2 / 3   •   Bible", "Hebrejská a křesťanská tradice",
            "Tip: Bible je pramen motivů celé evropské kultury.",
            {
                "Starý zákon tvoří Tóra, historické a prorocké knihy, žalmy a mudrosloví.",
                "Kazatel, Job a Píseň písní patří k nejčtenějším starozákonním knihám.",
                "Nový zákon přináší evangelia, Skutky, epištoly a Zjevení.",
                "Biblické postavy a obrazy se vracejí v literatuře všech pozdějších epoch.",
                NULL,
            },
        },
        {
            "3 / 3   •   Paměť", "Co si z orientu odnést",
            "Tip: u eposu sledujte hrdinu, cestu a střet s osudem.",
            {
                "Společná témata jsou stvoření, potopa, zákon, láska a smrt.",
                "Texty často vznikaly ústně a teprve později byly zapsány.",
                "Anonymita a náboženský výklad světa převažují nad autorským já.",
                "Na maturitě stačí zařadit dílo do kultury a říct, čím ovlivnilo Evropu.",
                NULL,
            },
        },
    };

    return lit_unit_page(0, &lit_lessons[1], "lit_unit2", "lit_unit2_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit_unit2_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Odkud pochází Epos o Gilgamešovi?",
         {"Ze starého Egypta", "Z Mezopotámie", "Z antického Říma", "Ze středověké Francie"},
         4, 1},
        {"Které knihy patří k indické epice?",
         {"Ilias a Odyssea", "Mahábhárata a Rámájana",
          "Aeneis a Proměny", "Božská komedie a Dekameron"},
         4, 1},
        {"Co tvoří jádro Starého zákona?",
         {"Čtyři evangelia", "Tóra a další knihy hebrejské bible",
          "Korán", "Védy"},
         4, 1},
        {"Proč se Bible probírá v literatuře?",
         {"Je to učebnice zeměpisu",
          "Je pramenem motivů evropské kultury",
          "Vznikla v národním obrození",
          "Patří k absurdnímu dramatu"},
         4, 1},
    };
    static const char *hints[] = {
        "Gilgameš je nejstarší dochovaný epos, zapsaný klínovým písmem.",
        "Mahábhárata a Rámájana jsou velké indické eposy.",
        "Starý zákon je hebrejská bible, Nový zákon evangelia a epištoly.",
        "Biblické obrazy se vracejí od středověku po současnost.",
    };

    return lit_mcq_page(0, 1, "litunit2", "lit_ex2_title", "lit_quiz2_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Řecko", "Homér a attické drama",
            "Tip: katarze je očištění diváka soucitem a bázní.",
            {
                "Homérova Ilias a Odyssea jsou vrchol řeckého eposu, veršovaný hexametrem.",
                "Lyriku představují Sapfó, Anakreón a Pindaros.",
                "Tragédie: Aischylos, Sofoklés (Král Oidipus, Antigona) a Eurípidés (Médeia).",
                "Aristofanés psal komedie. Ideál kalokagathia spojoval krásu a dobro.",
                NULL,
            },
        },
        {
            "2 / 3   •   Řím", "Vergilius, Ovidius, Horatius",
            "Tip: Aeneis vědomě navazuje na Homéra a zakládá římský mýtus.",
            {
                "Vergiliova Aeneis vypráví o útěku z Tróje a založení římského rodu.",
                "Ovidiovy Proměny shrnují antické mýty, Umění milovat je milostný návod.",
                "Horatius proslul ódami a myšlenkou zlaté střední cesty.",
                "Římská literatura přejímá řecké vzory a předává je středověké Evropě.",
                NULL,
            },
        },
        {
            "3 / 3   •   Pojmy", "Co si z antiky pamatovat",
            "Tip: u tragédie hledejte vinu, osud a poznání.",
            {
                "Epos začíná často in medias res a vzývá Múzu.",
                "Tragický hrdina naráží na osud; divák prožívá katarzi.",
                "Mýtus vysvětluje svět příběhem bohů a hrdinů.",
                "Antika je stálý zdroj námětů pro renesanci, klasicismus i pozdější doby.",
                NULL,
            },
        },
    };

    return lit_unit_page(0, &lit_lessons[2], "lit_unit3", "lit_unit3_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit_unit3_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo napsal Iliadu a Odysseu?",
         {"Sofoklés", "Homér", "Vergilius", "Ovidius"},
         4, 1},
        {"Které dílo napsal Sofoklés?",
         {"Médeia", "Král Oidipus", "Aeneis", "Proměny"},
         4, 1},
        {"Co je katarze?",
         {"Začátek eposu uprostřed děje",
          "Očištění diváka soucitem a bázní",
          "Římský název pro komedii",
          "Volný verš bez stopy"},
         4, 1},
        {"Kdo složil Aeneis?",
         {"Horatius", "Homér", "Vergilius", "Aristofanés"},
         4, 2},
    };
    static const char *hints[] = {
        "Ilias a Odyssea jsou homérské eposy.",
        "Sofoklés: Král Oidipus a Antigona. Médeia je Eurípidés.",
        "Pojem katarze vychází z Aristotelovy Poetiky.",
        "Aeneis je římský epos Vergiliův, Proměny napsal Ovidius.",
    };

    return lit_mcq_page(0, 2, "litunit3", "lit_ex3_title", "lit_quiz3_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Doba", "Středověký pohled na svět",
            "Tip: texty bývají anonymní a často nábožensky alegorické.",
            {
                "Středověk vykládá svět křesťansky: pozemský život je cestou ke spáse.",
                "Převažuje latina, postupně se prosazují národní jazyky.",
                "Autor často ustupuje, dílo má sloužit víře, poučení nebo oslavě hrdiny.",
                "Vedle učené tvorby žije ústní lidová slovesnost.",
                NULL,
            },
        },
        {
            "2 / 3   •   Žánry", "Hrdinská, rytířská a městská literatura",
            "Tip: Dante uzavírá středověk a otevírá cestu k renesanci.",
            {
                "Hrdinská epika: Píseň o Rolandovi, Píseň o Nibelunzích, Beowulf.",
                "Rytířská epika a dvorská lyrika: artušovské romány, Tristan a Isolda, trubadúři.",
                "Náboženská literatura: legendy, hymny, exempla a duchovní drama.",
                "Městská satira a Danteho Božská komedie, psaná tercínou.",
                NULL,
            },
        },
        {
            "3 / 3   •   Paměť", "Jak středověk číst",
            "Tip: alegorie říká jednu věc a míní jinou, hlubší.",
            {
                "Hrdina ztělesňuje čest, věrnost, víru nebo rytířskou lásku.",
                "Symbol a alegorie jsou důležitější než psychologická kresba.",
                "Božská komedie provádí poutníka Peklem, Očistcem a Rájem.",
                "Středověké látky se vracejí v romantismu i v moderní kultuře.",
                NULL,
            },
        },
    };

    return lit_unit_page(0, &lit_lessons[3], "lit_unit4", "lit_unit4_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit_unit4_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Které dílo patří k hrdinské epice?",
         {"Dekameron", "Píseň o Rolandovi", "Hamlet", "Lakomec"},
         4, 1},
        {"Čím je typický středověký text?",
         {"Důrazem na individuální autorské já",
          "Náboženským výkladem a častou anonymitou",
          "Volným veršem avantgardy",
          "Naturalistickým popisem dědičnosti"},
         4, 1},
        {"Kdo napsal Božskou komedii?",
         {"Francesco Petrarca", "Dante Alighieri",
          "Giovanni Boccaccio", "William Shakespeare"},
         4, 1},
        {"Jakou strofou je psána Božská komedie?",
         {"Sonetem", "Tercínou", "Blankversem", "Hexametrem"},
         4, 1},
    };
    static const char *hints[] = {
        "Píseň o Rolandovi je francouzská chanson de geste.",
        "Středověk klade víru a poučení nad autorskou originalitu.",
        "Dante, Petrarca a Boccaccio jsou tři velcí Italové, Božská komedie je Dantova.",
        "Tercína je trojverší, které Dante v komedii používá.",
    };

    return lit_mcq_page(0, 3, "litunit4", "lit_ex4_title", "lit_quiz4_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Počátky", "Staroslověnština a latina",
            "Tip: rok 863 je příchod Konstantina a Metoděje na Velkou Moravu.",
            {
                "Konstantin a Metoděj přinesli staroslověnské písmo a překlady bohoslužebných textů.",
                "K památkám patří Proglas, Život Konstantinův a Metodějův a Kyjevské listy.",
                "Po vyhnání slovanské liturgie přebírá vzdělanost latina.",
                "Kosmova kronika je nejstarší česká kronika, psaná latinsky.",
                NULL,
            },
        },
        {
            "2 / 3   •   Čeština", "Literatura 14. století",
            "Tip: Dalimil píše česky a z hlediska české šlechty.",
            {
                "Dalimilova kronika je první veršovaná kronika v češtině.",
                "Alexandreida přenáší antický příběh o Alexandrovi do rytířského světa.",
                "Mastičkář je nejstarší české drama, Hradecký rukopis sbírá legendy a satiru.",
                "Doba Karla IV. posiluje češtinu vedle latiny a němčiny.",
                NULL,
            },
        },
        {
            "3 / 3   •   Husitství", "Hus, píseň a Chelčický",
            "Tip: husitství je vrchol starší české literatury v národním jazyce.",
            {
                "Jan Hus kritizuje církev v českých kázáních a latinském spise O církvi.",
                "Jistebnický kancionál uchoval píseň Ktož jsú boží bojovníci.",
                "Petr Chelčický v Sieti viery odmítá násilí a spojení víry s mocí.",
                "Husitská čeština zjednodušuje pravopis a posiluje srozumitelnost.",
                NULL,
            },
        },
    };

    return lit_unit_page(0, &lit_lessons[4], "lit_unit5", "lit_unit5_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit_unit5_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdy přišli Konstantin a Metoděj na Velkou Moravu?",
         {"Roku 863", "Roku 1415", "Roku 1348", "Roku 1620"},
         4, 0},
        {"Která kronika je první v češtině?",
         {"Kosmova kronika", "Dalimilova kronika",
          "Hájkova kronika", "Vita Caroli"},
         4, 1},
        {"Kterou píseň uchoval Jistebnický kancionál?",
         {"Kde domov můj", "Ktož jsú boží bojovníci",
          "Ach synku, synku", "Hej, Slované"},
         4, 1},
        {"Čím je Petr Chelčický?",
         {"Autorem renesančních sonetů",
          "Myslitelem, který odmítá násilí a spojení víry s mocí",
          "Barokním kazatelem protireformace",
          "Obrozenským jazykovědcem"},
         4, 1},
    };
    static const char *hints[] = {
        "863 je cyrilometodějská misie, 1415 upálení Husa.",
        "Kosmas psal latinsky, Dalimil česky.",
        "Ktož jsú boží bojovníci je husitská bojová píseň.",
        "Sieť viery pravé je Chelčického hlavní spis.",
    };

    return lit_mcq_page(0, 4, "litunit5", "lit_ex5_title", "lit_quiz5_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Doba", "Člověk se vrací do středu",
            "Tip: humanismus staví na antice, renesance na smyslovém světě.",
            {
                "Humanismus obnovuje zájem o antiku, jazyk a vzdělání člověka.",
                "Renesance oslavuje pozemský život, individualitu a smyslovou krásu.",
                "Itálie udává tón: Petrarcův sonet a Boccacciův Dekameron.",
                "Proti středověkému teocentrismu stojí zájem o člověka a přírodu.",
                NULL,
            },
        },
        {
            "2 / 3   •   Evropa", "Od Cervantese k Shakespearovi",
            "Tip: Hamlet je renesanční hrdina, který pochybuje a zkoumá.",
            {
                "Francie: Rabelais (Gargantua a Pantagruel) a Montaignovy Eseje.",
                "Španělsko: Cervantesův Důmyslný rytíř don Quijote de la Mancha.",
                "Anglie: Shakespeare, tragédie Hamlet, Romeo a Julie, Macbeth, Othello a komedie.",
                "Sonet má 14 veršů a pevné schéma; Shakespeare ho užívá i v milostné lyrice.",
                NULL,
            },
        },
        {
            "3 / 3   •   Čechy", "Veleslavín a Bible kralická",
            "Tip: Bible kralická je vrchol humanistické češtiny.",
            {
                "Jednota bratrská vydává Bibli kralickou, vzor spisovného jazyka.",
                "Daniel Adam z Veleslavína podporuje tisk, překlady a vzdělanost.",
                "Jan Blahoslav pečuje o jazyk a bratrský zpěvník.",
                "Český humanismus končí porážkou stavovského povstání a pobělohorskou změnou.",
                NULL,
            },
        },
    };

    return lit_unit_page(0, &lit_lessons[5], "lit_unit6", "lit_unit6_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit_unit6_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo je autorem Dekameronu?",
         {"Dante Alighieri", "Giovanni Boccaccio",
          "Francesco Petrarca", "Miguel de Cervantes"},
         4, 1},
        {"Které dílo napsal Shakespeare?",
         {"Don Quijote", "Hamlet", "Gargantua a Pantagruel", "Božská komedie"},
         4, 1},
        {"Co je sonet?",
         {"Báseň o 14 verších s pevnou stavbou",
          "Středověká hrdinská píseň",
          "Dramatická jednota času",
          "Barokní kázání"},
         4, 0},
        {"Která kniha je vrcholem humanistické češtiny?",
         {"Kosmova kronika", "Bible kralická", "Labyrint světa", "Máj"},
         4, 1},
    };
    static const char *hints[] = {
        "Petrarca je sonet, Boccaccio Dekameron, Dante Božská komedie.",
        "Hamlet, Romeo a Julie, Macbeth a Othello jsou Shakespearovy tragédie.",
        "Sonet má 14 veršů, obvykle dvě čtyřverší a dvě trojverší.",
        "Bibli kralickou vydala Jednota bratrská na přelomu 16. a 17. století.",
    };

    return lit_mcq_page(0, 5, "litunit6", "lit_ex6_title", "lit_quiz6_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Styl", "Kontrast a napětí",
            "Tip: baroko miluje protiklad těla a duše, světa a věčnosti.",
            {
                "Baroko vzniká v napětí reformace a protireformace.",
                "Vyhledává kontrast, nadsázku, emblém a smyslovou okázalost.",
                "Častý je motiv marnosti, smrti a touhy po Bohu.",
                "Ve světě sem patří Calderónův Život je sen a Miltonův Ztracený ráj.",
                NULL,
            },
        },
        {
            "2 / 3   •   Čechy", "Pobělohorská tvorba",
            "Tip: po roce 1620 se dělí domácí katolická tvorba a exil.",
            {
                "Bedřich Bridel v básni Co Bůh? Člověk? staví nicotu člověka proti Bohu.",
                "Bohuslav Balbín brání v latině český jazyk a dějiny.",
                "Lidová a pololidová tvorba, kramářské písně a legendy udržují češtinu.",
                "Oficiální kultura je katolická, mnoho vzdělanců odchází do exilu.",
                NULL,
            },
        },
        {
            "3 / 3   •   Komenský", "Labyrint světa a ráj srdce",
            "Tip: Komenský je vrchol českého baroka i evropského pedagogického myšlení.",
            {
                "Jan Amos Komenský, poslední biskup Jednoty bratrské, píše v exilu.",
                "Labyrint světa a ráj srdce je alegorické putování za smyslem života.",
                "Informatorium školy mateřské a Orbis pictus zakládají moderní pedagogiku.",
                "Kšaft umírající matky Jednoty bratrské je odkaz národu a jazyku.",
                NULL,
            },
        },
    };

    return lit_unit_page(0, &lit_lessons[6], "lit_unit7", "lit_unit7_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit_unit7_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Který rys vystihuje baroko?",
         {"Klidná vyváženost a tři jednoty",
          "Kontrast, nadsázka a napětí těla a duše",
          "Popis všedního dne bez obraznosti",
          "Odmítání náboženství"},
         4, 1},
        {"Kdo napsal Co Bůh? Člověk?",
         {"Jan Amos Komenský", "Bedřich Bridel",
          "Bohuslav Balbín", "Daniel Adam z Veleslavína"},
         4, 1},
        {"O čem je Labyrint světa a ráj srdce?",
         {"O alegorickém putování za smyslem života",
          "O válce s mloky",
          "O národním obrození",
          "O životě Švejka"},
         4, 0},
        {"Kam patří Komenského pedagogické spisy?",
         {"K poetismu", "K baroku a počátkům moderní pedagogiky",
          "K májovcům", "K existencialismu"},
         4, 1},
    };
    static const char *hints[] = {
        "Baroko pracuje s protikladem, světlem a stínem, tělem a věčností.",
        "Bridel je autor básně Co Bůh? Člověk?, Komenský napsal Labyrint.",
        "Poutník prochází marným světem a nalézá klid v srdci u Boha.",
        "Informatorium a Orbis pictus jsou Komenského učební spisy.",
    };

    return lit_mcq_page(0, 6, "litunit7", "lit_ex7_title", "lit_quiz7_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Klasicismus", "Rozum a pravidlo",
            "Tip: tři jednoty jsou jednota času, místa a děje.",
            {
                "Klasicismus 17. století navazuje na antiku a žádá kázeň, harmonii a pravidla.",
                "Vyvrcholil ve Francii: Corneille, Racine a Molièrovy komedie.",
                "Molièrův Lakomec a Tartuffe zesměšňují lidskou vadu jako typ.",
                "La Fontaine převyprávěl bajky, Boileau sepsal pravidla básnictví.",
                NULL,
            },
        },
        {
            "2 / 3   •   Osvícenství", "Rozum má měnit společnost",
            "Tip: Candide se vysmívá lehké víře, že žijeme v nejlepším z světů.",
            {
                "Osvícenství věří rozumu, vzdělání, toleranci a kritice předsudků.",
                "Voltairův Candide a Swiftovy Gulliverovy cesty jsou satirické prózy.",
                "Defoeův Robinson Crusoe oslavuje práci a praktický rozum.",
                "Encyklopedisté včetně Diderota chtějí zpřístupnit vědění.",
                NULL,
            },
        },
        {
            "3 / 3   •   Cit", "Preromantismus a cesta k obrození",
            "Tip: Werther otevírá literaturu citovému jednotlivci.",
            {
                "Preromantismus staví cit, přírodu a středověk proti chladnému rozumu.",
                "Goethovo Utrpení mladého Werthera a Faust spojují cit i filozofii.",
                "Schillerovy Loupežníci hájí svobodu proti útisku.",
                "V Čechách na osvícenskou učenost naváže národní obrození.",
                NULL,
            },
        },
    };

    return lit_unit_page(0, &lit_lessons[7], "lit_unit8", "lit_unit8_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit_unit8_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co jsou tři jednoty klasicistního dramatu?",
         {"Jednota času, místa a děje",
          "Jednota autora, hrdiny a čtenáře",
          "Jednota rýmu, stopy a sloky",
          "Jednota lyriky, epiky a dramatu"},
         4, 0},
        {"Kdo napsal Lakomce?",
         {"William Shakespeare", "Molière", "Johann Wolfgang Goethe", "Voltaire"},
         4, 1},
        {"Které dílo je osvícenská satira?",
         {"Kytice", "Candide", "Máj", "Babička"},
         4, 1},
        {"Čím preromantismus překonává klasicismus?",
         {"Důrazem na cit, přírodu a individuální prožitek",
          "Přísnějšími pravidly rýmu",
          "Návratem výhradně k latině",
          "Odmítnutím divadla"},
         4, 0},
    };
    static const char *hints[] = {
        "Klasicistní tragédie se má odehrát v jednom dni, na jednom místě a v jedné linii.",
        "Molière je autorem Lakomce a Tartuffa.",
        "Candide napsal Voltaire, Gulliverovy cesty Swift.",
        "Werther a Schillerovi Loupežníci už staví cit proti pravidlu.",
    };

    return lit_mcq_page(0, 7, "litunit8", "lit_ex8_title", "lit_quiz8_head",
                        qs, hints, 4);
}

void add_lit_pages(GtkStack *stack) {
    typedef GtkWidget *(*LitBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        LitBuilder build_unit;
        LitBuilder build_ex;
    } pages[] = {
        {"litunit1", "litex1", build_lit_unit1_page, build_lit_unit1_exercise_page},
        {"litunit2", "litex2", build_lit_unit2_page, build_lit_unit2_exercise_page},
        {"litunit3", "litex3", build_lit_unit3_page, build_lit_unit3_exercise_page},
        {"litunit4", "litex4", build_lit_unit4_page, build_lit_unit4_exercise_page},
        {"litunit5", "litex5", build_lit_unit5_page, build_lit_unit5_exercise_page},
        {"litunit6", "litex6", build_lit_unit6_page, build_lit_unit6_exercise_page},
        {"litunit7", "litex7", build_lit_unit7_page, build_lit_unit7_exercise_page},
        {"litunit8", "litex8", build_lit_unit8_page, build_lit_unit8_exercise_page},
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
