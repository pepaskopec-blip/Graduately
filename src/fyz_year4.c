#include "graduately.h"

/* Year 4: wave and geometric optics, special relativity, quantum
 * physics, the atom, the nucleus, nuclear reactions and astrophysics. */

NetLesson fyz4_lessons[FYZ_N] = {
    { .n_slides = 3, .unit_page = "fyz4unit1", .ex_page = "fyz4ex1" },
    { .n_slides = 3, .unit_page = "fyz4unit2", .ex_page = "fyz4ex2" },
    { .n_slides = 3, .unit_page = "fyz4unit3", .ex_page = "fyz4ex3" },
    { .n_slides = 3, .unit_page = "fyz4unit4", .ex_page = "fyz4ex4" },
    { .n_slides = 3, .unit_page = "fyz4unit5", .ex_page = "fyz4ex5" },
    { .n_slides = 3, .unit_page = "fyz4unit6", .ex_page = "fyz4ex6" },
    { .n_slides = 3, .unit_page = "fyz4unit7", .ex_page = "fyz4ex7" },
    { .n_slides = 3, .unit_page = "fyz4unit8", .ex_page = "fyz4ex8" },
};

static GtkWidget *build_fyz4_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Paprsek", "Odraz a lom",
            "Tip: úhel odrazu se rovná úhlu dopadu.",
            {
                "Světlo se v homogenním prostředí šíří přímočaře.",
                "Úhel odrazu se rovná úhlu dopadu. Oba se měří od kolmice.",
                "Index lomu je n = c / v. Ve skle je větší než 1.",
                "Snellův zákon: n₁·sin α = n₂·sin β.",
                NULL,
            },
        },
        {
            "2 / 3   •   Mez", "Úplný odraz",
            "Tip: úplný odraz nastane při přechodu do opticky řidšího prostředí.",
            {
                "Opticky hustší prostředí má větší index lomu.",
                "Při přechodu do řidšího prostředí se paprsek láme od kolmice.",
                "Od mezního úhlu se už neláme a nastane úplný odraz.",
                "Optická vlákna vedou světlo opakovaným úplným odrazem.",
                NULL,
            },
        },
        {
            "3 / 3   •   Vlna", "Interference a ohyb",
            "Tip: světlo je příčné elektromagnetické vlnění.",
            {
                "Interference je skládání vln. Světlý pruh vzniká při dráhovém rozdílu k·λ.",
                "Tmavý pruh vzniká při rozdílu liché poloviny vlnové délky.",
                "Ohyb je patrný, když je překážka srovnatelná s vlnovou délkou.",
                "Polarizace dokazuje, že světlo je příčné vlnění.",
                NULL,
            },
        },
    };

    return fyz_unit_page(3, &fyz4_lessons[0], "fyz4_unit1", "fyz4_unit1_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz4_unit1_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak zní zákon odrazu?",
         {"Úhel odrazu je dvojnásobek úhlu dopadu",
          "Úhel odrazu se rovná úhlu dopadu",
          "Úhel odrazu je vždy 90°",
          "Odraz nezávisí na kolmici"},
         4, 1},
        {"Co je index lomu?",
         {"n = v / c", "n = c / v", "n = λ·f v látce dělené c²", "n = sin α"},
         4, 1},
        {"Kdy nastane úplný odraz?",
         {"Při přechodu do opticky řidšího prostředí nad mezním úhlem",
          "Při každém dopadu na zrcadlo",
          "Jen ve vakuu",
          "Při přechodu do hustšího prostředí"},
         4, 0},
        {"Kdy vznikne interferencí světlý pruh?",
         {"Při dráhovém rozdílu k·λ",
          "Při dráhovém rozdílu vždy λ/2",
          "Když se vlny nepotkají",
          "Jen u podélného zvuku"},
         4, 0},
    };
    static const char *hints[] = {
        "Úhly se měří od kolmice dopadu, ne od plochy.",
        "V látce je světlo pomalejší než ve vakuu, proto n > 1.",
        "Mezní úhel splňuje sin αm = n₂ / n₁ pro n₁ > n₂.",
        "Podmínka pro minimum je (2k+1)·λ/2.",
    };

    return fyz_mcq_page(3, 0, "fyz4unit1", "fyz4_ex1_title", "fyz4_quiz1_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz4_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Zrcadla", "Dutá a vypuklá",
            "Tip: zobrazovací rovnice je 1/a + 1/a' = 1/f.",
            {
                "Rovinné zrcadlo dává vzpřímený obraz stejné velikosti, zdánlivě za zrcadlem.",
                "Duté zrcadlo může dát skutečný převrácený obraz.",
                "Vypuklé zrcadlo dává zmenšený vzpřímený zdánlivý obraz.",
                "Ohnisková vzdálenost kulového zrcadla je f = r / 2.",
                NULL,
            },
        },
        {
            "2 / 3   •   Čočky", "Spojka a rozptylka",
            "Tip: spojka soustředí rovnoběžné paprsky do ohniska.",
            {
                "Spojka je uprostřed tlustší a má kladnou ohniskovou vzdálenost.",
                "Rozptylka je uprostřed tenčí a paprsky rozbíhá.",
                "Zobrazovací rovnice je 1/a + 1/a' = 1/f.",
                "a je vzdálenost předmětu, a' vzdálenost obrazu.",
                NULL,
            },
        },
        {
            "3 / 3   •   Oko", "Vady a přístroje",
            "Tip: krátkozrakost se koriguje rozptylkou.",
            {
                "Oko zobrazuje spojnou soustavou na sítnici.",
                "Krátkozraké oko zobrazuje před sítnici. Pomůže rozptylka.",
                "Dalekozraké oko zobrazuje za sítnici. Pomůže spojka.",
                "Lupa je spojka a předmět je mezi čočkou a ohniskem.",
                NULL,
            },
        },
    };

    return fyz_unit_page(3, &fyz4_lessons[1], "fyz4_unit2", "fyz4_unit2_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz4_unit2_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jaká je ohnisková vzdálenost kulového zrcadla?",
         {"f = r", "f = r / 2", "f = 2·r", "f = 1 / r²"},
         4, 1},
        {"Jak zní zobrazovací rovnice čočky?",
         {"a + a' = f", "1/a + 1/a' = 1/f", "a·a' = f", "1/a − 1/f = a'"},
         4, 1},
        {"Čím se koriguje krátkozrakost?",
         {"Spojkou", "Rozptylkou", "Dutým zrcadlem v oku", "Clonou bez čočky"},
         4, 1},
        {"Jaký obraz dává lupa?",
         {"Skutečný převrácený, předmět je za ohniskem",
          "Zvětšený vzpřímený zdánlivý, předmět je mezi čočkou a ohniskem",
          "Zmenšený obraz v nekonečnu vždy",
          "Obraz stejný jako rovinné zrcadlo"},
         4, 1},
    };
    static const char *hints[] = {
        "Poloměr křivosti je dvojnásobek ohniskové vzdálenosti.",
        "Když je a = 2f, je i a' = 2f a obraz je stejně velký.",
        "Dalekozrakost koriguje spojka. Obraz pak padne na sítnici.",
        "Zdánlivý obraz spojky vzniká, když je předmět uvnitř ohniska.",
    };

    return fyz_mcq_page(3, 1, "fyz4unit2", "fyz4_ex2_title", "fyz4_quiz2_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz4_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Postuláty", "Dva principy",
            "Tip: rychlost světla ve vakuu je ve všech inerciálních soustavách stejná.",
            {
                "Fyzikální zákony mají ve všech inerciálních soustavách stejný tvar.",
                "Rychlost světla ve vakuu je ve všech inerciálních soustavách c.",
                "c je mezní rychlost pro přenos informace a pro částice s hmotností.",
                "Klasické skládání rychlostí u rychlostí blízkých c selhává.",
                NULL,
            },
        },
        {
            "2 / 3   •   Čas a délka", "Dilatace a kontrakce",
            "Tip: pohybující se hodiny jdou z hlediska klidné soustavy pomaleji.",
            {
                "Dilatace času: pohyblivé hodiny jdou pomaleji.",
                "Kontrakce délky zkracuje rozměr ve směru pohybu.",
                "Rozměry kolmé ke směru pohybu se nemění.",
                "Efekty jsou patrné až při rychlostech srovnatelných s c.",
                NULL,
            },
        },
        {
            "3 / 3   •   Energie", "E = m·c²",
            "Tip: klidová energie je energie tělesa v soustavě, kde je v klidu.",
            {
                "Klidová energie je E₀ = m·c².",
                "Celková energie roste s rychlostí.",
                "Hmotnost a energie jsou dvě stránky téže veličiny.",
                "Částice s nenulovou klidovou hmotností nikdy nedosáhne rychlosti c.",
                NULL,
            },
        },
    };

    return fyz_unit_page(3, &fyz4_lessons[2], "fyz4_unit3", "fyz4_unit3_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz4_unit3_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co platí o rychlosti světla ve vakuu?",
         {"Závisí na rychlosti zdroje klasickým součtem",
          "Je ve všech inerciálních soustavách stejná",
          "Je 340 m/s",
          "Je různá podle barvy jako hlavní postulát"},
         4, 1},
        {"Co je dilatace času?",
         {"Pohyblivé hodiny jdou z hlediska klidného pozorovatele pomaleji",
          "Pohyblivé hodiny jdou rychleji",
          "Čas se zastaví při každé rychlosti",
          "Délka se prodlužuje"},
         4, 0},
        {"Který rozměr se při kontrakci zkracuje?",
         {"Rozměr kolmý ke směru pohybu",
          "Rozměr ve směru pohybu",
          "Všechny rozměry stejně",
          "Žádný, zkracuje se jen hmotnost"},
         4, 1},
        {"Jak se značí klidová energie?",
         {"E₀ = m·v² / 2", "E₀ = m·c²", "E₀ = m·g·h", "E₀ = h·f"},
         4, 1},
    };
    static const char *hints[] = {
        "Druhý postulát nahrazuje klasické skládání rychlostí světla.",
        "Vzorec obsahuje √(1 − v²/c²) ve jmenovateli vlastního času.",
        "Tyč kolmá k pohybu má stejnou délku v obou soustavách.",
        "c² je obrovské číslo, proto i malá hmotnost znamená velkou energii.",
    };

    return fyz_mcq_page(3, 2, "fyz4unit3", "fyz4_ex3_title", "fyz4_quiz3_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz4_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Foton", "Energie kvanta",
            "Tip: E = h·f. h je Planckova konstanta.",
            {
                "Záření si vyměňuje energii po kvantech, fotonech.",
                "Energie fotonu je E = h·f.",
                "h je asi 6,63·10⁻³⁴ J·s.",
                "Větší frekvence znamená energičtější foton.",
                NULL,
            },
        },
        {
            "2 / 3   •   Fotoefekt", "Světlo vyráží elektrony",
            "Tip: pod mezní frekvencí elektrony nevyletí, i když je světlo silné.",
            {
                "Fotoelektrický jev: foton předá energii elektronu v kovu.",
                "Výstupní práce Wv je energie potřebná k opuštění kovu.",
                "Einsteinova rovnice: h·f = Wv + Ek.",
                "Počet elektronů roste s intenzitou, jejich maximální energie s frekvencí.",
                NULL,
            },
        },
        {
            "3 / 3   •   Dualita", "Vlna i částice",
            "Tip: de Broglieho vlnová délka je λ = h / p.",
            {
                "Světlo se v interferenci chová jako vlna a ve fotoefektu jako částice.",
                "I elektrony mají vlnové vlastnosti.",
                "De Broglie: λ = h / p, kde p je hybnost.",
                "Čím větší hybnost, tím kratší vlnová délka.",
                NULL,
            },
        },
    };

    return fyz_unit_page(3, &fyz4_lessons[3], "fyz4_unit4", "fyz4_unit4_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz4_unit4_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak se počítá energie fotonu?",
         {"E = m·c² jen pro foton v klidu", "E = h·f", "E = h / f", "E = f / h"},
         4, 1},
        {"Co rozhoduje o tom, zda fotoefekt nastane?",
         {"Jen intenzita světla",
          "Frekvence musí být aspoň mezní",
          "Jen barva kovu bez frekvence",
          "Teplota laboratoře jako jediná podmínka"},
         4, 1},
        {"Jak zní Einsteinova rovnice fotoefektu?",
         {"h·f = Wv + Ek", "h·f = Wv − Ek vždy záporně",
          "E = m·g·h", "h·f = R·I"},
         4, 0},
        {"Co říká de Broglieho vztah?",
         {"λ = h·p", "λ = h / p", "λ = p / h", "λ = c·p"},
         4, 1},
    };
    static const char *hints[] = {
        "Foton nemá klidovou hmotnost. Jeho energie je h·f, hybnost h/λ.",
        "Silnější světlo stejné nízké frekvence elektrony neuvolní.",
        "Zbytek energie fotonu po výstupní práci je kinetická energie elektronu.",
        "Makroskopická tělesa mají obrovskou hybnost, proto nepatrnou vlnovou délku.",
    };

    return fyz_mcq_page(3, 3, "fyz4unit4", "fyz4_ex4_title", "fyz4_quiz4_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz4_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Modely", "Od Rutherforda k Bohrovi",
            "Tip: Rutherfordův pokus ukázal malé kladné jádro.",
            {
                "Thomsonův model měl náboj rozptýlený v celém atomu.",
                "Rutherford: atom má malé kladné jádro a obal z elektronů.",
                "Většina objemu atomu je prázdná.",
                "Bohr doplnil, že elektron je jen na určitých drahách bez vyzařování.",
                NULL,
            },
        },
        {
            "2 / 3   •   Hladiny", "Emise a absorpce",
            "Tip: atom vyzáří foton, když elektron klesne na nižší hladinu.",
            {
                "Energie elektronu v atomu je kvantovaná.",
                "Při přeskoku dolů atom vyzáří foton, při přeskoku nahoru ho pohltí.",
                "Rozdíl hladin je E = h·f.",
                "Spektrum plynu je čárové, spektrum pevné látky je spojité.",
                NULL,
            },
        },
        {
            "3 / 3   •   Obal", "Slupky",
            "Tip: slupky se značí K, L, M a odpovídají hlavnímu kvantovému číslu.",
            {
                "Hlavní kvantové číslo n určuje slupku a energii ve vodíku.",
                "Slupka K je n = 1, L je n = 2, M je n = 3.",
                "Pauliho princip: v atomu nejsou dva elektrony se všemi kvantovými čísly stejnými.",
                "Chemická vazba vzniká hlavně elektrony ve vnější slupce.",
                NULL,
            },
        },
    };

    return fyz_unit_page(3, &fyz4_lessons[4], "fyz4_unit5", "fyz4_unit5_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz4_unit5_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co ukázal Rutherfordův pokus?",
         {"Že atom nemá jádro",
          "Že kladný náboj je soustředěn v malém jádře",
          "Že elektron má vlnovou délku nula",
          "Že spektrum pevné látky je čárové"},
         4, 1},
        {"Kdy atom vyzáří foton?",
         {"Když elektron přejde na nižší hladinu",
          "Když elektron přejde na vyšší hladinu",
          "Když se jádro nepohne a elektron stojí",
          "Při každém oběhu podle klasické fyziky trvale"},
         4, 0},
        {"Jaká je energie fotonu při přeskoku?",
         {"Součet obou hladin", "Rozdíl hladin",
          "Vždy h·c bez rozdílu", "Klidová energie elektronu"},
         4, 1},
        {"Která slupka je nejblíž jádru?",
         {"M", "L", "K", "Libovolná se stejným n"},
         4, 2},
    };
    static const char *hints[] = {
        "Odraz mála částic alfa o velký úhel nešel vysvětlit rozptýleným nábojem.",
        "Přeskok nahoru je absorpce. Elektron energii fotonu přijme.",
        "E₂ − E₁ = h·f. Větší skok znamená větší frekvenci.",
        "n = 1 je K. Vyšší n je dál od jádra a má vyšší energii.",
    };

    return fyz_mcq_page(3, 4, "fyz4unit5", "fyz4_ex5_title", "fyz4_quiz5_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz4_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Jádro", "Protonové a nukleonové číslo",
            "Tip: nukleonové číslo A je počet protonů a neutronů.",
            {
                "Jádro tvoří protony a neutrony, souhrnně nukleony.",
                "Protonové číslo Z je počet protonů a určuje prvek.",
                "Nukleonové číslo A je počet nukleonů. Neutronů je A − Z.",
                "Izotopy mají stejné Z a různé A.",
                NULL,
            },
        },
        {
            "2 / 3   •   Vazba", "Vazebná energie",
            "Tip: jádro je lehčí než součet volných nukleonů.",
            {
                "Jaderné síly drží nukleony pohromadě a působí na krátkou vzdálenost.",
                "Vazebná energie je energie, kterou je třeba dodat na rozbití jádra na nukleony.",
                "Hmotnostní úbytek odpovídá vazebné energii podle E = m·c².",
                "Nejstabilnější jsou jádra kolem železa.",
                NULL,
            },
        },
        {
            "3 / 3   •   Rozpad", "Alfa, beta, gama",
            "Tip: poločas je doba, za kterou se rozpadne polovina jader.",
            {
                "Záření alfa je jádro helia, má malý dolet a silné ionizační účinky.",
                "Záření beta jsou elektrony nebo pozitrony z jádra.",
                "Záření gama je elektromagnetické a proniká nejvíc.",
                "Poločas rozpadu je pro daný nuklid stálý a nezávisí na okolí.",
                NULL,
            },
        },
    };

    return fyz_unit_page(3, &fyz4_lessons[5], "fyz4_unit6", "fyz4_unit6_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz4_unit6_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je nukleonové číslo A?",
         {"Počet protonů", "Počet protonů a neutronů",
          "Počet elektronů v obalu", "Počet slupek"},
         4, 1},
        {"Co jsou izotopy?",
         {"Jádra se stejným Z a různým A",
          "Jádra se stejným A a různým Z",
          "Jádra bez neutronů",
          "Atomy bez elektronů"},
         4, 0},
        {"Co je záření alfa?",
         {"Elektromagnetická vlna", "Jádro helia",
          "Volný neutron", "Foton z obalu"},
         4, 1},
        {"Co je poločas rozpadu?",
         {"Doba, za kterou se rozpadne polovina jader",
          "Doba jednoho oběhu elektronu",
          "Doba, za kterou vznikne jádro železa",
          "Doba života jednoho neutronu mimo každou látku jako jediný údaj"},
         4, 0},
    };
    static const char *hints[] = {
        "Zapiš uhlík 14 jako A = 14, Z = 6, neutronů 8.",
        "Stejné protonové číslo znamená stejný prvek.",
        "Beta je elektron nebo pozitron. Gama je foton.",
        "Po dvou poločasech zbývá čtvrtina původních jader.",
    };

    return fyz_mcq_page(3, 5, "fyz4unit6", "fyz4_ex6_title", "fyz4_quiz6_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz4_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Štěpení", "Těžká jádra",
            "Tip: štěpení uranu uvolní neutrony, které mohou štěpit dál.",
            {
                "Štěpení rozdělí těžké jádro na dvě středně těžká.",
                "Uvolní se energie, protože produkty mají větší vazebnou energii na nukleon.",
                "Při štěpení uranu 235 vznikají i neutrony.",
                "Řetězová reakce pokračuje, když každý krok uvolní aspoň jeden další neutron.",
                NULL,
            },
        },
        {
            "2 / 3   •   Reaktor", "Řízená reakce",
            "Tip: moderátor neutrony zpomaluje, regulační tyče je pohlcují.",
            {
                "V reaktoru je řetězová reakce řízená.",
                "Moderátor, třeba voda, zpomaluje neutrony.",
                "Regulační tyče neutrony zachycují a výkon tím klesá.",
                "Teplo z reaktoru ohřívá vodu a pohání turbínu.",
                NULL,
            },
        },
        {
            "3 / 3   •   Syntéza", "Slučování lehkých jader",
            "Tip: fúze pohání Slunce. Potřebuje vysokou teplotu.",
            {
                "Jaderná syntéza slučuje lehká jádra, třeba vodík na helium.",
                "Uvolní víc energie na nukleon než štěpení.",
                "K překonání odpudivé síly je potřeba velmi vysoká teplota.",
                "Ochrana před zářením stojí na čase, vzdálenosti a stínění.",
                NULL,
            },
        },
    };

    return fyz_unit_page(3, &fyz4_lessons[6], "fyz4_unit7", "fyz4_unit7_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz4_unit7_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je řetězová reakce?",
         {"Jedno štěpení, které se dál nešíří",
          "Štěpení udržované neutrony z předchozích štěpení",
          "Slučování elektronů",
          "Rozpad alfa bez neutronů"},
         4, 1},
        {"K čemu je v reaktoru moderátor?",
         {"Aby neutrony zpomalil",
          "Aby všechna jádra okamžitě rozbil",
          "Aby pohltil všechno teplo bez užitku",
          "Aby zvýšil protonové číslo paliva"},
         4, 0},
        {"Co dělají regulační tyče?",
         {"Neutrony zachycují a reakci brzdí",
          "Neutrony jen vyrábějí",
          "Slouží jako palivo místo uranu",
          "Zvyšují teplotu fúze"},
         4, 0},
        {"Proč fúze potřebuje vysokou teplotu?",
         {"Aby jádra překonala elektrické odpuzování",
          "Aby se jádra ochladila",
          "Aby vznikl moderátor",
          "Aby poločas klesl k nule"},
         4, 0},
    };
    static const char *hints[] = {
        "Kritické množství znamená, že neutrony nestačí utéct a reakce běží.",
        "Pomalý neutron štěpí uran 235 ochotněji než rychlý.",
        "Zasunutí tyčí výkon sníží. Vysunutí ho zvýší.",
        "Ve Slunci fúzi pomáhá i obrovský tlak. Záření se stíní olovem a betonem.",
    };

    return fyz_mcq_page(3, 6, "fyz4unit7", "fyz4_ex7_title", "fyz4_quiz7_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz4_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Slunce", "Hvězda",
            "Tip: Slunce získává energii fúzí vodíku na helium.",
            {
                "Slunce je hvězda. Svítí díky jaderné syntéze v jádře.",
                "Světlo ze Slunce letí na Zemi asi 8 minut.",
                "Svítivost a teplota povrchu řadí hvězdy do Hertzsprungova–Russellova diagramu.",
                "Většina hvězd leží na hlavní posloupnosti.",
                NULL,
            },
        },
        {
            "2 / 3   •   Vývoj", "Od oblaku k závěru",
            "Tip: hmotnější hvězda žije kratší dobu.",
            {
                "Hvězda vzniká smršťováním oblaku plynu a prachu.",
                "Po vyčerpání vodíku se mění stavba hvězdy.",
                "Hvězda podobná Slunci skončí jako bílý trpaslík.",
                "Velmi hmotná hvězda může skončit výbuchem supernovy a zbyde neutronová hvězda nebo černá díra.",
                NULL,
            },
        },
        {
            "3 / 3   •   Vesmír", "Galaxie a rozpínání",
            "Tip: světelný rok je vzdálenost, ne čas.",
            {
                "Galaxie je soustava hvězd, plynu a prachu. Slunce je v Mléčné dráze.",
                "Světelný rok je dráha, kterou světlo urazí za rok.",
                "Vzdálené galaxie mají spektrum posunuté k červené. To znamená, že se vzdalují.",
                "Rudý posuv podporuje obraz rozpínajícího se vesmíru.",
                NULL,
            },
        },
    };

    return fyz_unit_page(3, &fyz4_lessons[7], "fyz4_unit8", "fyz4_unit8_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz4_unit8_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Odkud bere Slunce energii?",
         {"Z hoření uhlí", "Z fúze vodíku na helium",
          "Z štěpení uranu v kůře", "Z odrazu světla planet"},
         4, 1},
        {"Co je hlavní posloupnost?",
         {"Pás, na kterém leží většina hvězd v HR diagramu",
          "Dráha Země kolem Slunce",
          "Řada planet podle velikosti",
          "Spektrum jen bílých trpaslíků"},
         4, 0},
        {"Co je světelný rok?",
         {"Doba oběhu Země", "Vzdálenost, kterou světlo urazí za rok",
          "Rychlost světla", "Stáří Slunce"},
         4, 1},
        {"Co znamená rudý posuv vzdálených galaxií?",
         {"Galaxie se přibližují",
          "Galaxie se vzdalují",
          "Galaxie přestaly svítit",
          "Světlo změnilo rychlost ve vakuu"},
         4, 1},
    };
    static const char *hints[] = {
        "Fúze v jádře Slunce mění hmotnost na energii podle E = m·c².",
        "Obři a bílí trpaslíci leží mimo hlavní posloupnost.",
        "Rok je tu doba letu světla, výsledek je délka.",
        "Posuv k delší vlnové délce je Dopplerův jev rozpínání.",
    };

    return fyz_mcq_page(3, 7, "fyz4unit8", "fyz4_ex8_title", "fyz4_quiz8_head",
                        qs, hints, 4);
}

void add_fyz4_pages(GtkStack *stack) {
    typedef GtkWidget *(*FyzBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        FyzBuilder build_unit;
        FyzBuilder build_ex;
    } pages[] = {
        {"fyz4unit1", "fyz4ex1", build_fyz4_unit1_page, build_fyz4_unit1_exercise_page},
        {"fyz4unit2", "fyz4ex2", build_fyz4_unit2_page, build_fyz4_unit2_exercise_page},
        {"fyz4unit3", "fyz4ex3", build_fyz4_unit3_page, build_fyz4_unit3_exercise_page},
        {"fyz4unit4", "fyz4ex4", build_fyz4_unit4_page, build_fyz4_unit4_exercise_page},
        {"fyz4unit5", "fyz4ex5", build_fyz4_unit5_page, build_fyz4_unit5_exercise_page},
        {"fyz4unit6", "fyz4ex6", build_fyz4_unit6_page, build_fyz4_unit6_exercise_page},
        {"fyz4unit7", "fyz4ex7", build_fyz4_unit7_page, build_fyz4_unit7_exercise_page},
        {"fyz4unit8", "fyz4ex8", build_fyz4_unit8_page, build_fyz4_unit8_exercise_page},
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
