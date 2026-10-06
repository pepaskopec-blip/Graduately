#include "graduately.h"

/* Year 4: literature after 1945, Czech writing under censorship and after
 * 1989, and how to interpret a text at the school-leaving exam. */

NetLesson lit4_lessons[LIT_N] = {
    { .n_slides = 3, .unit_page = "lit4unit1", .ex_page = "lit4ex1" },
    { .n_slides = 3, .unit_page = "lit4unit2", .ex_page = "lit4ex2" },
    { .n_slides = 3, .unit_page = "lit4unit3", .ex_page = "lit4ex3" },
    { .n_slides = 3, .unit_page = "lit4unit4", .ex_page = "lit4ex4" },
    { .n_slides = 3, .unit_page = "lit4unit5", .ex_page = "lit4ex5" },
    { .n_slides = 3, .unit_page = "lit4unit6", .ex_page = "lit4ex6" },
    { .n_slides = 3, .unit_page = "lit4unit7", .ex_page = "lit4ex7" },
    { .n_slides = 3, .unit_page = "lit4unit8", .ex_page = "lit4ex8" },
};

static GtkWidget *build_lit4_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Po válce", "Svět, který už nevěří velkým frázím",
            "Tip: antiutopie varuje před státem, který ovládne pravdu i soukromí.",
            {
                "Druhá světová válka a totalitní režimy mění téma viny, moci a paměti.",
                "George Orwell píše Farmu zvířat a román 1984.",
                "Ray Bradbury v 451 stupních Fahrenheita brání knihu proti cenzuře.",
                "William Golding v Pánu much ukazuje, jak rychle se zboří civilizovaný řád.",
                NULL,
            },
        },
        {
            "2 / 3   •   Hlasy", "Existencialismus, beatnici a magický realismus",
            "Tip: existencialismus říká, že člověk je svoboda a musí volit bez hotové opory.",
            {
                "Albert Camus píše Cizince a Mor, Jean-Paul Sartre Nevolnost.",
                "Jack Kerouac v románu Na cestě dává hlas beat generation.",
                "Gabriel García Márquez ve Sto rocích samoty mísí skutečnost a zázrak.",
                "Jorge Luis Borges staví povídku jako filozofickou hru s nekonečnem.",
                NULL,
            },
        },
        {
            "3 / 3   •   Východ", "Svědectví o nesvobodě",
            "Tip: Solženicyn a Bulgakov se čtou jako literatura i jako svědectví.",
            {
                "Alexandr Solženicyn v Jednom dni Ivana Děnisoviče otevírá téma lágru.",
                "Michail Bulgakov hájí v Mistrovi a Markétce umění proti strachu.",
                "Joseph Heller v Hlavě XXII zesměšňuje válečnou byrokracii.",
                "Poválečná próza tedy není jeden styl, ale několik odpovědí na stejné století.",
                NULL,
            },
        },
    };

    return lit_unit_page(3, &lit4_lessons[0], "lit4_unit1", "lit4_unit1_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit4_unit1_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo napsal román 1984?",
         {"George Orwell", "Ray Bradbury", "Albert Camus", "Umberto Eco"},
         4, 0},
        {"O čem je 451 stupňů Fahrenheita?",
         {"O cenzuře a pálení knih", "O národním obrození",
          "O antickém osudu", "O husitské písni"},
         4, 0},
        {"Který román patří Gabrielu Garcíovi Márquezovi?",
         {"Sto roků samoty", "Babička", "Evžen Oněgin", "R.U.R."},
         4, 0},
        {"Co je existencialismus v próze Camuse a Sartra?",
         {"Víra, že osud je předem dán bohy",
          "Důraz na svobodu volby, absurditu a odpovědnost",
          "Návrat ke třem jednotám",
          "Popis venkova bez psychologie"},
         4, 1},
    };
    static const char *hints[] = {
        "Orwell: Farma zvířat a 1984. Bradbury: 451 stupňů Fahrenheita.",
        "Hasiči u Bradburyho knihy pálí, lidé je zachraňují tím, že je umějí zpaměti.",
        "Sto roků samoty je základní román magického realismu.",
        "Cizinec a Mor jsou Camusovy, Nevolnost Sartrova.",
    };

    return lit_mcq_page(3, 0, "lit4unit1", "lit4_ex1_title", "lit4_quiz1_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit4_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Absurdita", "Čekání, které nekončí",
            "Tip: v absurdním dramatu se děj neuzavře, protože svět sám nedává smysl.",
            {
                "Samuel Beckett v Čekání na Godota nechává dva tuláky čekat na někoho, kdo nepřijde.",
                "Eugene Ionesco v Plešaté zpěvačce rozkládá obyčejnou řeč.",
                "Postavy jednají, ale jejich jednání cíl nenaplní.",
                "Smích a hrůza tu nejsou oddělené.",
                NULL,
            },
        },
        {
            "2 / 3   •   Myšlenka", "Sartre, Dürrenmatt a podobenství",
            "Tip: Návštěva staré dámy je podobenství o tom, zač je město ochotné prodat spravedlnost.",
            {
                "Sartrovy Mouchy a S vyloučením veřejnosti jsou existencialistická dramata.",
                "Friedrich Dürrenmatt píše Návštěvu staré dámy a Fyziky.",
                "Podobenství dovoluje mluvit o vině, aniž hra kopíruje noviny.",
                "Divadlo po roce 1945 často soudí diváka spolu s postavou.",
                NULL,
            },
        },
        {
            "3 / 3   •   Postmoderna", "Hra s tím, co už bylo napsáno",
            "Tip: postmoderna cituje starší texty a nedůvěřuje jedinému velkému výkladu.",
            {
                "Umberto Eco v Jménu růže spojuje detektivku, středověk a učenou hru.",
                "Intertextualita znamená, že dílo vědomě navazuje na jiná díla.",
                "Vypravěč může být nespolehlivý a závěr otevřený.",
                "Na maturitě postmoderu poznáte podle ironie, citace a pochybnosti o jedné pravdě.",
                NULL,
            },
        },
    };

    return lit_unit_page(3, &lit4_lessons[1], "lit4_unit2", "lit4_unit2_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit4_unit2_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo napsal Čekání na Godota?",
         {"Samuel Beckett", "Molière", "William Shakespeare", "Karel Čapek"},
         4, 0},
        {"Co je typické pro absurdní drama?",
         {"Jasný hrdinský konec",
          "Jednání, které nevede k smysluplnému završení",
          "Dodržení tří jednot",
          "Jen historický námět z antiky"},
         4, 1},
        {"Kdo napsal Návštěvu staré dámy?",
         {"Friedrich Dürrenmatt", "Bertolt Brecht",
          "Václav Havel", "Victor Hugo"},
         4, 0},
        {"Čím se vyznačuje Jméno růže?",
         {"Je to barokní kázání",
          "Postmoderní román, který spojuje detektivku a středověk",
          "Proletářská balada",
          "Klasicistní tragédie"},
         4, 1},
    };
    static const char *hints[] = {
        "Beckettovo Čekání na Godota je základní absurdní drama.",
        "Godot nepřijde a čekání se opakuje.",
        "Dürrenmattova hra zkoumá, za co obec prodá svědomí.",
        "Eco pracuje s citací, ironií a učenou hrou.",
    };

    return lit_mcq_page(3, 1, "lit4unit2", "lit4_ex2_title", "lit4_quiz2_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit4_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   1945–1948", "Krátká svoboda",
            "Tip: mezi květnem 1945 a únorem 1948 ještě vedle sebe žijí různé směry.",
            {
                "Po válce se vracejí zakázaní autoři a vycházejí odložené rukopisy.",
                "Skupina 42 ještě působí, katolická literatura má poslední veřejný prostor.",
                "Zároveň sílí požadavek, aby umění sloužilo nové moci.",
                "Únor 1948 tento spor rozhodne politicky.",
                NULL,
            },
        },
        {
            "2 / 3   •   Schematismus", "Budovatelský román",
            "Tip: schematická postava nemá pochybnost, protože dějiny už mají vyhráno.",
            {
                "Na počátku 50. let vládne socialistický realismus a budovatelský román.",
                "Hrdina je dělník nebo funkcionář, konflikt je mezi pokrokem a zpátečníkem.",
                "Jazyk je průhledný, závěr optimistický, kritika režimu nemožná.",
                "Mnoho autorů starších generací mlčí, překládá, nebo píše do zásuvky.",
                NULL,
            },
        },
        {
            "3 / 3   •   1956", "První trhlina",
            "Tip: rok 1956 je kritika kultu osobnosti a počátek opatrného uvolnění.",
            {
                "II. sjezd spisovatelů a světové události narušují jistotu schématu.",
                "Časopis Květen a poezie všedního dne vracejí do verše obyčejný život.",
                "Ještě to není svoboda 60. let, ale konec nejtvrdšího schématu.",
                "Na maturitu odlište budovatelský román od literatury, která přišla po něm.",
                NULL,
            },
        },
    };

    return lit_unit_page(3, &lit4_lessons[2], "lit4_unit3", "lit4_unit3_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit4_unit3_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co se v české literatuře děje po únoru 1948?",
         {"Návrat ke klasicismu",
          "Nástup socialistického realismu a budovatelského románu",
          "Vznik poetismu",
          "Konec cenzury"},
         4, 1},
        {"Čím je schematická postava?",
         {"Má jednoznačnou politickou roli a nepochybuje",
          "Je romantický vyvrženec",
          "Je vypravěč v ich-formě antického eposu",
          "Je autor Bible kralické"},
         4, 0},
        {"Co znamená rok 1956 pro českou literaturu?",
         {"Založení Devětsilu",
          "Kritiku kultu osobnosti a první uvolnění schématu",
          "Příchod Konstantina a Metoděje",
          "Vydání Máje"},
         4, 1},
        {"Kdy končí krátké poválečné období otevřenější kultury?",
         {"Únorem 1948", "Manifestem české moderny",
          "Almanachem Máj", "Rokem 863"},
         4, 0},
    };
    static const char *hints[] = {
        "Budovatelský román je oficiální žánr počátku 50. let.",
        "Schéma dělí postavy na kladné a záporné podle vztahu k režimu.",
        "1956 oslabuje kult osobnosti, svoboda 60. let přijde později.",
        "Únor 1948 je politický zlom, po němž mizí nezávislé proudy z veřejné scény.",
    };

    return lit_mcq_page(3, 2, "lit4unit3", "lit4_ex3_title", "lit4_quiz3_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit4_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Uvolnění", "Léta, kdy se smělo víc",
            "Tip: 60. léta nejsou bez cenzury, ale cenzura polevila a experiment se vrátil.",
            {
                "Literatura znovu zkouší formu, ironii a tabuizovaná témata.",
                "Válka, stalinismus i soukromí se dají pojmenovat otevřeněji.",
                "Vrcholí to Pražským jarem 1968 a končí srpnovou okupací.",
                "Mnoho knih, které tehdy vznikly, později vyjde jen v samizdatu nebo v exilu.",
                NULL,
            },
        },
        {
            "2 / 3   •   Próza", "Hrabal, Kundera, Škvorecký",
            "Tip: pábitel je Hrabalův vypravěč, který skutečnost přetaví v proud řeči.",
            {
                "Bohumil Hrabal píše Pábitelé, Ostře sledované vlaky a Taneční hodiny.",
                "Milan Kundera vydává Směšné lásky a Žert.",
                "Josef Škvorecký v Zbabělcích vypráví konec války očima mladíka.",
                "Arnošt Lustig a Ladislav Fuks vracejí téma šoa: Modlitba pro Kateřinu Horovitzovou, Spalovač mrtvol.",
                NULL,
            },
        },
        {
            "3 / 3   •   Kritika", "Vaculík, Páral a meze svobody",
            "Tip: Žert ukazuje, jak jedna věta a jeden režim zničí život.",
            {
                "Ludvík Vaculík píše Sekyru a později Dva tisíce slov.",
                "Vladimír Páral zachycuje konzumní stereotyp.",
                "Divadlo a film 60. let jdou s literaturou ruku v ruce.",
                "Okupace 1968 tuto vlnu utne a rozdělí autory na oficiální, samizdat a exil.",
                NULL,
            },
        },
    };

    return lit_unit_page(3, &lit4_lessons[3], "lit4_unit4", "lit4_unit4_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit4_unit4_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdo napsal Ostře sledované vlaky?",
         {"Bohumil Hrabal", "Milan Kundera", "Josef Škvorecký", "Ladislav Fuks"},
         4, 0},
        {"Který román napsal Milan Kundera v 60. letech?",
         {"Žert", "Babička", "Temno", "1984"},
         4, 0},
        {"O čem jsou Zbabělci Josefa Škvoreckého?",
         {"O konci války v českém městě očima mladíka",
          "O barokním exilu Komenského",
          "O antickém osudu",
          "O národním obrození"},
         4, 0},
        {"Kdo napsal Spalovače mrtvol?",
         {"Ladislav Fuks", "Jiří Wolker", "Karel Jaromír Erben", "Victor Hugo"},
         4, 0},
    };
    static const char *hints[] = {
        "Hrabal: Ostře sledované vlaky, Pábitelé, později Obsluhoval jsem anglického krále.",
        "Žert je Kunderův román o trestu za vtip.",
        "Zbabělci vyšli na konci 50. let a patří k uvolnění.",
        "Fuksův Spalovač mrtvol ukazuje, jak se obyčejný člověk stane nástrojem zla.",
    };

    return lit_mcq_page(3, 3, "lit4unit4", "lit4_ex4_title", "lit4_quiz4_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit4_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Tři proudy", "Normalizace",
            "Tip: po roce 1968 se česká literatura dělí na oficiální, samizdatovou a exilovou.",
            {
                "Normalizace vyřazuje nepohodlné autory z nakladatelství, novin i zaměstnání.",
                "Oficiální scéna tiskne jen to, co projde cenzurou.",
                "Samizdat opisuje rukopisy, známá je Vaculíkova Edice Petlice.",
                "Exil vydává v cizině, Škvoreckého nakladatelství 68 Publishers vzniká v Torontu.",
                NULL,
            },
        },
        {
            "2 / 3   •   Autoři", "Kdo kam patří",
            "Tip: jeden autor může v různých letech stát na různých stranách této hranice.",
            {
                "V exilu publikují Kundera, Škvorecký, Pavel Kohout a další.",
                "V samizdatu píšou Václav Havel, Ludvík Vaculík, Ivan Klíma a Jiří Gruša.",
                "Hrabal po nucených úpravách zčásti vychází oficiálně a zároveň koluje v opisech.",
                "Underground, Egon Bondy a Ivan Jirous, stojí mimo oficiální kulturu zcela.",
                NULL,
            },
        },
        {
            "3 / 3   •   Charta", "Literatura a občanský postoj",
            "Tip: Charta 77 není román, ale bez ní nelze normalizaci vysvětlit.",
            {
                "Charta 77 žádá dodržování práv, která stát sám podepsal.",
                "Mnoho signatářů jsou spisovatelé a zakázaní autoři.",
                "Čtení samizdatu je v té době čin, ne jen záliba.",
                "Na maturitu vždy řekněte, v kterém z tří proudů dílo vyšlo.",
                NULL,
            },
        },
    };

    return lit_unit_page(3, &lit4_lessons[4], "lit4_unit5", "lit4_unit5_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit4_unit5_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak se dělí česká literatura v normalizaci?",
         {"Na lyriku, epiku a drama",
          "Na oficiální tvorbu, samizdat a exil",
          "Na ruchovce a lumírovce",
          "Na antiku a středověk"},
         4, 1},
        {"Co je Edice Petlice?",
         {"Samizdatová edice Ludvíka Vaculíka",
          "Oficiální nakladatelství 50. let",
          "Barokní tiskárna",
          "Almanach májovců"},
         4, 0},
        {"Kde působilo nakladatelství 68 Publishers?",
         {"V Torontu", "V Praze jako oficiální dům",
          "Na Velké Moravě", "V Národním divadle"},
         4, 0},
        {"Kdo patří k undergroundu normalizace?",
         {"Egon Bondy a Ivan Jirous", "Josef Dobrovský a Josef Jungmann",
          "Homér a Sofoklés", "Božena Němcová a Karel Jaromír Erben"},
         4, 0},
    };
    static const char *hints[] = {
        "Tři okruhy publikování jsou základní schéma učebnic.",
        "Petlici řídil Vaculík, opisy šly z ruky do ruky.",
        "68 Publishers založili Škvorecký a Zdena Salivarová.",
        "Underground odmítá oficiální kulturu i její kompromisy.",
    };

    return lit_mcq_page(3, 4, "lit4unit5", "lit4_ex5_title", "lit4_quiz5_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit4_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Verš", "Poezie po roce 1945",
            "Tip: Seifert dostal Nobelovu cenu roku 1984, v době, kdy doma nebyl režimní básník.",
            {
                "Jaroslav Seifert píše po válce intimní a paměťovou lyriku, Morový sloup vychází v samizdatu.",
                "Jan Skácel a Miroslav Holub představují dvě polohy: něhu a věcnou metaforu.",
                "Oficiální poezie 50. let je agitační, později se vrací civilní tón.",
                "Písničkáři Karel Kryl, Jaromír Nohavica a Vladimír Merta nesou verš mimo knižní cenzuru.",
                NULL,
            },
        },
        {
            "2 / 3   •   Havel", "Drama, které mluví o moci řeči",
            "Tip: Zahradní slavnost ukazuje, jak fráze pohltí člověka.",
            {
                "Václav Havel debutuje Zahradní slavností a Vyrozuměním.",
                "Audience, Vernisáž a Protest jsou jednoaktovky o přizpůsobení.",
                "Havel navazuje na absurdní drama, ale dá mu konkrétní českou situaci.",
                "Hry kolují v samizdatu a hrají se v cizině, doma dlouho ne.",
                NULL,
            },
        },
        {
            "3 / 3   •   Jeviště", "Semafor, Cimrman a autorské divadlo",
            "Tip: Divadlo Járy Cimrmana si vymyslelo génia, aby se smělo smát dějinám.",
            {
                "Malá divadla 60. let, Semafor a další, vracela jevišti písničku a improvizaci.",
                "Zdeněk Svěrák a Ladislav Smoljak vytvářejí hry o Járovi Cimrmanovi.",
                "České nebe později shrne národní mýty s nadsázkou.",
                "Poezie a drama této doby často říkají víc mezi řádky než v hesle.",
                NULL,
            },
        },
    };

    return lit_unit_page(3, &lit4_lessons[5], "lit4_unit6", "lit4_unit6_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit4_unit6_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdy dostal Jaroslav Seifert Nobelovu cenu?",
         {"Roku 1895", "Roku 1984", "Roku 1836", "Roku 1415"},
         4, 1},
        {"Kterou hru napsal Václav Havel?",
         {"Zahradní slavnost", "Maryša", "Lakomec", "Noc na Karlštejně"},
         4, 0},
        {"Co jsou Audience a Vernisáž?",
         {"Havlovy jednoaktovky o přizpůsobení",
          "Erbenovy balady",
          "Legionářské romány",
          "Klasicistní tragédie"},
         4, 0},
        {"Kdo vytvořil Divadlo Járy Cimrmana?",
         {"Zdeněk Svěrák a Ladislav Smoljak",
          "Karel a Josef Čapkovi",
          "Alois a Vilém Mrštíkové",
          "Vítězslav Nezval a Karel Teige"},
         4, 0},
    };
    static const char *hints[] = {
        "Nobelova cena za literaturu 1984 patří Seifertovi.",
        "Zahradní slavnost je Havlova první velká hra.",
        "Audience se odehrává v pivovaře a ukazuje tlak na zakázaného spisovatele.",
        "Cimrman je mystifikace, která paroduje národní dějiny.",
    };

    return lit_mcq_page(3, 5, "lit4unit6", "lit4_ex6_title", "lit4_quiz6_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit4_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   1989", "Konec cenzury",
            "Tip: po listopadu se najednou tiskne to, co čtyřicet let chodilo v opisech.",
            {
                "Samizdat a exil se vracejí do knihkupectví.",
                "Čtenář dohání Kundera, Škvoreckého, Havla i zapadlé básníky.",
                "Zároveň vzniká trh: kniha se musí i prodat.",
                "Literatura ztrácí roli jediného místa, kde se říkala pravda.",
                NULL,
            },
        },
        {
            "2 / 3   •   Próza", "Viewegh, Topol a další",
            "Tip: Báječná léta pod psa jsou generační vzpomínka na normalizaci, napsaná už svobodně.",
            {
                "Michal Viewegh v Báječných letech pod psa spojuje rodinnou kroniku a humor.",
                "Jáchym Topol v Sestře píše syrovým, téměř mluveným jazykem o podzemí a svobodě.",
                "Michal Ajvaz, Miloš Urban a Petra Hůlová ukazují, jak se próza rozběhla do různých směrů.",
                "Autobiografie, groteska i náročný experiment stojí vedle sebe.",
                NULL,
            },
        },
        {
            "3 / 3   •   Dnešek", "Co z toho plyne pro školu",
            "Tip: současný text se čte stejně poctivě jako klasika: druh, jazyk, kontext.",
            {
                "Po roce 1989 už nelze mluvit o jednom oficiálním stylu.",
                "Poezie žije i mimo velká nakladatelství, na čteních a na webu.",
                "Drama se vrací na scény, které čtyřicet let nesměly hrát zakázané hry.",
                "Maturita žádá, abyste současnost uměli zařadit, ne jen převyprávět.",
                NULL,
            },
        },
    };

    return lit_unit_page(3, &lit4_lessons[6], "lit4_unit7", "lit4_unit7_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit4_unit7_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co se s literaturou stalo po roce 1989?",
         {"Cenzura zesílila",
          "Skončila cenzura a vrátily se samizdat a exil",
          "Zanikla veškerá próza",
          "Povinně se psal jen budovatelský román"},
         4, 1},
        {"Kdo napsal Báječná léta pod psa?",
         {"Michal Viewegh", "Bohumil Hrabal", "Karel Hynek Mácha", "Jan Neruda"},
         4, 0},
        {"Který román napsal Jáchym Topol?",
         {"Sestra", "Babička", "Máj", "Král Oidipus"},
         4, 0},
        {"Proč literatura po roce 1989 ztratila výsadní politickou roli?",
         {"Protože pravda se směla říkat i jinde než v opisech",
          "Protože zanikl český jazyk",
          "Protože se vrátila jen latina",
          "Protože maturita literaturu zrušila"},
         4, 0},
    };
    static const char *hints[] = {
        "Listopad 1989 otevírá nakladatelství i archivy.",
        "Vieweghova kniha je humoristická kronika normalizace.",
        "Sestra je syrový román o svobodě po roce 1989.",
        "Dokud byla cenzura, kniha často nahrazovala veřejnou debatu.",
    };

    return lit_mcq_page(3, 6, "lit4unit7", "lit4_ex7_title", "lit4_quiz7_head",
                        qs, hints, 4);
}

static GtkWidget *build_lit4_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Postup", "Jak číst text k maturitě",
            "Tip: nejdřív řekněte, co text je, teprve potom, co si o něm myslíte.",
            {
                "Určete literární druh a žánr a dobu, do které text patří.",
                "Pojmenujte téma a hlavní motivy, ne jen děj větu po větě.",
                "U prózy si všimněte vypravěče, kompozice a postav.",
                "U lyriky rozlište autora a lyrický subjekt.",
                NULL,
            },
        },
        {
            "2 / 3   •   Jazyk", "Čím text působí",
            "Tip: jeden konkrétní prostředek, který umíte pojmenovat, je víc než obecná chvála.",
            {
                "Hledejte metaforu, přirovnání, symbol, kontrast nebo opakování.",
                "U verše určete rým, stopu, nebo řekněte, že jde o volný verš.",
                "Všimněte si slovníku: knižní, hovorový, biblický, ironický.",
                "Kompozice může být chronologická, retrospektivní, nebo pásmo obrazů.",
                NULL,
            },
        },
        {
            "3 / 3   •   Kontext", "Text není ostrov",
            "Tip: vlastní názor má stát na textu, ne místo něj.",
            {
                "Zařaďte ukázku k autorovi, směru a historické situaci.",
                "Srovnání s jiným dílem ukáže, že látce rozumíte.",
                "Řekněte, co text sděluje dnes, a opřete to o konkrétní místo.",
                "Struktura odpovědi: zařazení, téma, jazyk, kontext, vlastní soud.",
                NULL,
            },
        },
    };

    return lit_unit_page(3, &lit4_lessons[7], "lit4_unit8", "lit4_unit8_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_lit4_unit8_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Čím má začít rozbor textu?",
         {"Určením druhu, žánru a doby",
          "Převyprávěním celého dějepisu",
          "Vlastním dojmem bez textu",
          "Seznamem všech autorů století"},
         4, 0},
        {"Koho nesmíme zaměnit v lyrice?",
         {"Autora a lyrický subjekt",
          "Nakladatele a tiskaře",
          "Čtenáře a cenzora",
          "Rým a stopu"},
         4, 0},
        {"Co je volný verš?",
         {"Verš vázaný povinným sdruženým rýmem",
          "Verš bez pravidelného rýmu a stopy",
          "Jen antický hexametr",
          "Dramatická replika"},
         4, 1},
        {"Jak má vypadat vlastní názor u maturity?",
         {"Má nahradit rozbor",
          "Má vycházet z konkrétního místa v textu",
          "Má být co nejdelší bez argumentu",
          "Má zůstat nevysloven"},
         4, 1},
    };
    static const char *hints[] = {
        "Zařazení je rám, bez kterého jazykový rozbor visí ve vzduchu.",
        "Lyrický subjekt je hlas básně, ne automaticky občanský průkaz autora.",
        "Volný verš je běžný od avantgardy po současnost.",
        "Soud bez citace z textu maturita nepočítá jako rozbor.",
    };

    return lit_mcq_page(3, 7, "lit4unit8", "lit4_ex8_title", "lit4_quiz8_head",
                        qs, hints, 4);
}

void add_lit4_pages(GtkStack *stack) {
    typedef GtkWidget *(*LitBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        LitBuilder build_unit;
        LitBuilder build_ex;
    } pages[] = {
        {"lit4unit1", "lit4ex1", build_lit4_unit1_page, build_lit4_unit1_exercise_page},
        {"lit4unit2", "lit4ex2", build_lit4_unit2_page, build_lit4_unit2_exercise_page},
        {"lit4unit3", "lit4ex3", build_lit4_unit3_page, build_lit4_unit3_exercise_page},
        {"lit4unit4", "lit4ex4", build_lit4_unit4_page, build_lit4_unit4_exercise_page},
        {"lit4unit5", "lit4ex5", build_lit4_unit5_page, build_lit4_unit5_exercise_page},
        {"lit4unit6", "lit4ex6", build_lit4_unit6_page, build_lit4_unit6_exercise_page},
        {"lit4unit7", "lit4ex7", build_lit4_unit7_page, build_lit4_unit7_exercise_page},
        {"lit4unit8", "lit4ex8", build_lit4_unit8_page, build_lit4_unit8_exercise_page},
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
