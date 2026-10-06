#include "graduately.h"

/* Chemistry for Základy přírodopisných věd.
 * Follows the secondary-school outline (RVP for vocational schools,
 * přírodovědné vzdělávání, and the gymnasium general-chemistry track):
 * general chemistry, inorganic chemistry, organic chemistry, biochemistry. */

NetLesson chem_lessons[SCI_N] = {
    { .n_slides = 3, .unit_page = "chemunit1", .ex_page = "chemex1" },
    { .n_slides = 3, .unit_page = "chemunit2", .ex_page = "chemex2" },
    { .n_slides = 3, .unit_page = "chemunit3", .ex_page = "chemex3" },
    { .n_slides = 3, .unit_page = "chemunit4", .ex_page = "chemex4" },
    { .n_slides = 3, .unit_page = "chemunit5", .ex_page = "chemex5" },
    { .n_slides = 3, .unit_page = "chemunit6", .ex_page = "chemex6" },
    { .n_slides = 3, .unit_page = "chemunit7", .ex_page = "chemex7" },
    { .n_slides = 3, .unit_page = "chemunit8", .ex_page = "chemex8" },
};

static GtkWidget *build_chem_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Atom", "Z čeho je látka",
            "Tip: protonové číslo je počet protonů, ne elektronů.",
            {
                "Atom má jádro z protonů a neutronů a obal z elektronů.",
                "Proton má kladný náboj, elektron záporný, neutron je bez náboje.",
                "Protonové číslo Z udává počet protonů a určuje prvek.",
                "Nukleonové číslo A je součet protonů a neutronů.",
                NULL,
            },
        },
        {
            "2 / 3   •   Tabulka", "Periody a skupiny",
            "Tip: perioda je řádek, skupina je sloupec.",
            {
                "Periodická soustava řadí prvky podle rostoucího protonového čísla.",
                "Prvky v jedné skupině mají podobné chemické vlastnosti.",
                "Číslo periody souvisí s počtem obsazených slupek.",
                "Vlevo jsou kovy, vpravo nekovy, mezi nimi polokovy.",
                NULL,
            },
        },
        {
            "3 / 3   •   Částice", "Atom, molekula, ion",
            "Tip: kation vzniká ztrátou elektronů, anion jejich přijetím.",
            {
                "Prvek tvoří atomy se stejným protonovým číslem.",
                "Molekula je částice ze dvou nebo více atomů spojených vazbou.",
                "Ion je nabitá částice: kation je kladný, anion záporný.",
                "Izotopy jednoho prvku se liší počtem neutronů, ne protonů.",
                NULL,
            },
        },
    };

    return sci_unit_page(0, &chem_lessons[0], "chem_unit1", "chem_unit1_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_chem_unit1_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co udává protonové číslo?",
         {"Počet protonů v jádře", "Počet neutronů v jádře",
          "Součet protonů a elektronů v molekule", "Hmotnost atomu v gramech"},
         4, 0},
        {"Jaký náboj má elektron?",
         {"Kladný", "Záporný", "Žádný", "Stejný jako neutron"},
         4, 1},
        {"Co mají společného prvky v jedné skupině?",
         {"Stejné nukleonové číslo", "Podobné chemické vlastnosti",
          "Stejný počet neutronů", "Stejné skupenství za pokojové teploty"},
         4, 1},
        {"Co je v periodické soustavě perioda?",
         {"Sloupec prvků", "Řádek prvků",
          "Jen skupina vzácných plynů", "Rozdíl protonů a neutronů"},
         4, 1},
    };
    static const char *hints[] = {
        "Z je počet protonů a zároveň pořadí prvku v tabulce.",
        "Proton je kladný, elektron záporný, neutron neutrální.",
        "Skupina je sloupec. Podobné vlastnosti plynou z podobného obalu.",
        "Perioda je řádek. Číslo periody souvisí s obsazenými slupkami.",
    };

    return sci_mcq_page(0, 0, "chemunit1", "chem_ex1_title", "chem_quiz1_head",
                        qs, hints, 4);
}

static GtkWidget *build_chem_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Ionty", "Iontová vazba",
            "Tip: NaCl je typická iontová sloučenina.",
            {
                "Iontová vazba vzniká přenosem elektronů mezi atomy.",
                "Kov odevzdá elektrony a stane se kationtem, nekov je přijme jako anion.",
                "Opačné náboje se přitahují a drží krystal pohromadě.",
                "Iontové látky mají vysoké teploty tání a v tavenině vedou proud.",
                NULL,
            },
        },
        {
            "2 / 3   •   Sdílení", "Kovalentní vazba",
            "Tip: ve vodě atomy elektrony sdílejí, nepředávají si je natrvalo.",
            {
                "Kovalentní vazba vzniká sdílením elektronového páru.",
                "Jednoduchá vazba je jeden pár, dvojná dva, trojná tři.",
                "Nepolární vazba spojuje atomy se stejnou elektronegativitou, třeba H2.",
                "Polární vazba vzniká mezi atomy s různou elektronegativitou, třeba v H2O.",
                NULL,
            },
        },
        {
            "3 / 3   •   Kovy", "Kovová vazba",
            "Tip: volné elektrony vysvětlují lesk i vodivost kovů.",
            {
                "V kovu jsou kationty obklopené sdílenými elektrony.",
                "Elektrony se mohou posouvat, proto kov vede proud a teplo.",
                "Elektronegativita je schopnost atomu přitahovat elektrony vazby.",
                "Velký rozdíl elektronegativit vede spíš k iontové vazbě.",
                NULL,
            },
        },
    };

    return sci_unit_page(0, &chem_lessons[1], "chem_unit2", "chem_unit2_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_chem_unit2_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jaká vazba drží krystal chloridu sodného?",
         {"Kovalentní nepolární", "Iontová", "Kovová", "Vodíkový můstek jako jediná"},
         4, 1},
        {"Co je kovalentní vazba?",
         {"Přenos elektronu z kovu na nekov",
          "Sdílení elektronového páru",
          "Pohyb volných elektronů mezi kationty kovu",
          "Přitažlivost jen mezi molekulami vody"},
         4, 1},
        {"Proč kovy vedou elektrický proud?",
         {"Mají pevné jádro bez elektronů",
          "Obsahují pohyblivé elektrony kovové vazby",
          "Jsou vždy kapalné",
          "Nemají protony"},
         4, 1},
        {"Co je elektronegativita?",
         {"Počet neutronů v jádře",
          "Schopnost atomu přitahovat elektrony vazby",
          "Hmotnost jednoho molu",
          "Teplota varu kovu"},
         4, 1},
    };
    static const char *hints[] = {
        "Sodík předá elektron chloru. Vznikne Na+ a Cl−.",
        "Kovalentní vazba je společný elektronový pár, ne trvalý přenos.",
        "Kovová vazba má volné elektrony, které přenášejí náboj.",
        "Čím větší rozdíl elektronegativit, tím polárnější nebo iontovější vazba.",
    };

    return sci_mcq_page(0, 1, "chemunit2", "chem_ex2_title", "chem_quiz2_head",
                        qs, hints, 4);
}

static GtkWidget *build_chem_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Rovnice", "Co se při reakci zachová",
            "Tip: vyčíslit znamená srovnat počty atomů, ne měnit vzorce.",
            {
                "Při chemické reakci se atomy přeskupují, ale neztrácejí.",
                "Zákon zachování hmotnosti: hmotnost reaktantů se rovná hmotnosti produktů.",
                "V rovnici musí být na obou stranách stejný počet atomů každého prvku.",
                "Koeficient před vzorcem násobí celou částici.",
                NULL,
            },
        },
        {
            "2 / 3   •   Mol", "Kolik je částic",
            "Tip: molární hmotnost vody je přibližně 18 g/mol.",
            {
                "Mol je jednotka látkového množství.",
                "Jeden mol obsahuje 6,022 × 10^23 částic, to je Avogadrova konstanta.",
                "Molární hmotnost M je hmotnost jednoho molu v g/mol.",
                "Látkové množství se počítá jako n = m / M.",
                NULL,
            },
        },
        {
            "3 / 3   •   Výpočet", "Od hmotnosti k molům",
            "Tip: nejdřív sečti relativní atomové hmotnosti ve vzorci.",
            {
                "Relativní atomová hmotnost vodíku je 1, kyslíku 16, uhlíku 12.",
                "Voda H2O má molární hmotnost 18 g/mol, oxid uhličitý CO2 má 44 g/mol.",
                "18 g vody je 1 mol, tedy 6,022 × 10^23 molekul.",
                "Stechiometrie porovnává moly podle koeficientů v rovnici.",
                NULL,
            },
        },
    };

    return sci_unit_page(0, &chem_lessons[2], "chem_unit3", "chem_unit3_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_chem_unit3_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co říká zákon zachování hmotnosti?",
         {"Hmotnost produktů je vždy menší",
          "Hmotnost reaktantů se rovná hmotnosti produktů",
          "Atomy se při reakci ničí",
          "Molární hmotnost se během reakce mění"},
         4, 1},
        {"Kolik částic je v jednom molu?",
         {"18", "44", "6,022 × 10^23", "1000"},
         4, 2},
        {"Jak se spočítá látkové množství?",
         {"n = m × M", "n = m / M", "n = M / m", "n = Z + A"},
         4, 1},
        {"Jaká je přibližně molární hmotnost vody?",
         {"2 g/mol", "16 g/mol", "18 g/mol", "44 g/mol"},
         4, 2},
    };
    static const char *hints[] = {
        "Atomy se přeskupují. Jejich celková hmotnost zůstává.",
        "Avogadrova konstanta je počet částic v jednom molu.",
        "Hmotnost dělíme molární hmotností. Výsledek je v molech.",
        "H2O: 1 + 1 + 16 = 18. CO2 je 12 + 16 + 16 = 44.",
    };

    return sci_mcq_page(0, 2, "chemunit3", "chem_ex3_title", "chem_quiz3_head",
                        qs, hints, 4);
}

static GtkWidget *build_chem_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Směsi", "Čistá látka a směs",
            "Tip: roztok je homogenní směs, suspenze heterogenní.",
            {
                "Čistá látka má stálé složení a stálé vlastnosti.",
                "Směs obsahuje dvě nebo více látek, které nejsou chemicky vázané.",
                "Homogenní směs, třeba roztok, vypadá v celém objemu stejně.",
                "Heterogenní směs má rozlišitelné složky, třeba suspenze nebo emulze.",
                NULL,
            },
        },
        {
            "2 / 3   •   Roztok", "Složení roztoku",
            "Tip: hmotnostní zlomek je podíl hmotnosti složky a hmotnosti směsi.",
            {
                "Rozpouštědlo je látka, v níž se rozpouští rozpuštěná látka.",
                "Hmotnostní zlomek w = hmotnost složky / hmotnost celé směsi.",
                "Nasycený roztok už při dané teplotě další látku nerozpustí.",
                "Rozpustnost většiny pevných látek s teplotou roste.",
                NULL,
            },
        },
        {
            "3 / 3   •   pH", "Kyseliny a zásady",
            "Tip: neutralizace kyseliny hydroxidem dává sůl a vodu.",
            {
                "Kyselina ve vodě zvyšuje množství H+, zásada množství OH−.",
                "pH menší než 7 je kyselé, 7 neutrální, větší než 7 zásadité.",
                "Silné kyseliny jsou třeba HCl a H2SO4, silná zásada je NaOH.",
                "Neutralizace: kyselina + hydroxid → sůl + voda.",
                NULL,
            },
        },
    };

    return sci_unit_page(0, &chem_lessons[3], "chem_unit4", "chem_unit4_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_chem_unit4_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je roztok?",
         {"Heterogenní směs dvou kovů", "Homogenní směs",
          "Čistý prvek", "Jen sraženina na dně"},
         4, 1},
        {"Jaké pH má kyselý roztok?",
         {"Přesně 7", "Menší než 7", "Větší než 7", "Vždy 14"},
         4, 1},
        {"Co vzniká neutralizací kyseliny hydroxidem?",
         {"Jen vodík a kyslík jako prvky", "Sůl a voda",
          "Uhlovodík", "Vzácný plyn"},
         4, 1},
        {"Co je hmotnostní zlomek?",
         {"Podíl hmotnosti složky a hmotnosti směsi",
          "Počet molů v litru",
          "Rozdíl pH a 7",
          "Hmotnost jednoho protonu"},
         4, 0},
    };
    static const char *hints[] = {
        "Roztok je stejnorodý. Suspenze a emulze jsou různorodé.",
        "Neutrální pH je 7. Pod ním je kyselina, nad ním zásada.",
        "H+ z kyseliny a OH− ze zásady dají vodu, zbytek iontů sůl.",
        "w = m(složky) / m(směsi). Nemá jednotku, nebo se uvádí v procentech.",
    };

    return sci_mcq_page(0, 3, "chemunit4", "chem_ex4_title", "chem_quiz4_head",
                        qs, hints, 4);
}

static GtkWidget *build_chem_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Názvy", "Čtyři skupiny sloučenin",
            "Tip: oxid uhličitý je CO2, oxid uhelnatý je CO.",
            {
                "Oxidy jsou sloučeniny prvku s kyslíkem.",
                "Kyseliny ve vzorci obsahují vodík, který mohou odštěpit jako H+.",
                "Hydroxidy obsahují skupinu OH, soli vznikají z kyseliny náhradou vodíku.",
                "NaCl je chlorid sodný, NaOH hydroxid sodný, HCl kyselina chlorovodíková.",
                NULL,
            },
        },
        {
            "2 / 3   •   Nekovy", "Halogeny, kyslík, dusík",
            "Tip: halogeny jsou ve 17. skupině a jsou velmi reaktivní.",
            {
                "Kyslík O2 tvoří asi pětinu vzduchu a je nutný k dýchání i hoření.",
                "Halogeny fluor, chlor, brom a jod tvoří soli, halogenidy.",
                "Dusík N2 tvoří většinu vzduchu a je v bílkovinách i v hnojivech.",
                "Síra je v kyselině sírové H2SO4, důležité průmyslové kyselině.",
                NULL,
            },
        },
        {
            "3 / 3   •   Kovy", "Sodík, vápník, železo",
            "Tip: alkalické kovy reagují s vodou a musí se uchovávat pod petrolejem.",
            {
                "Alkalické kovy lithium, sodík a draslík jsou měkké a velmi reaktivní.",
                "Vápník a hořčík patří ke kovům alkalických zemin a jsou v kostech i chlorofylu.",
                "Železo je přechodný kov, základ oceli, a v těle je v hemoglobinu.",
                "Ušlechtilé kovy, třeba zlato, vzdorují korozi víc než železo.",
                NULL,
            },
        },
    };

    return sci_unit_page(0, &chem_lessons[4], "chem_unit5", "chem_unit5_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_chem_unit5_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Který vzorec patří oxidu uhličitému?",
         {"CO", "CO2", "H2CO3", "CaCO3"},
         4, 1},
        {"Co je hydroxid sodný?",
         {"NaCl", "NaOH", "HCl", "H2SO4"},
         4, 1},
        {"Které prvky jsou halogeny?",
         {"Li, Na, K", "F, Cl, Br, I", "Fe, Cu, Zn", "He, Ne, Ar"},
         4, 1},
        {"Kam patří sodík a draslík?",
         {"Mezi vzácné plyny", "Mezi alkalické kovy",
          "Mezi halogeny", "Mezi polokovy"},
         4, 1},
    };
    static const char *hints[] = {
        "CO je oxid uhelnatý, CO2 oxid uhličitý. H2CO3 je kyselina uhličitá.",
        "Hydroxidy končí na OH. NaOH je silná zásada.",
        "Halogeny jsou 17. skupina. Alkalické kovy jsou 1. skupina.",
        "Li, Na a K jsou alkalické kovy. S vodou reagují bouřlivě.",
    };

    return sci_mcq_page(0, 4, "chemunit5", "chem_ex5_title", "chem_quiz5_head",
                        qs, hints, 4);
}

static GtkWidget *build_chem_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Uhlík", "Proč existuje organická chemie",
            "Tip: organické sloučeniny staví řetězce a kruhy z uhlíku.",
            {
                "Organická chemie zkoumá sloučeniny uhlíku, kromě oxidů a uhličitanů.",
                "Uhlík tvoří čtyři vazby a může se řetězit.",
                "Uhlovodíky obsahují jen uhlík a vodík.",
                "Nasycené mají jen jednoduché vazby, nenasycené i dvojné nebo trojné.",
                NULL,
            },
        },
        {
            "2 / 3   •   Řady", "Alkany, alkeny a alkyny",
            "Tip: obecný vzorec alkanů je CnH2n+2.",
            {
                "Alkany jsou nasycené. Methan CH4 je nejjednodušší.",
                "Alkeny mají dvojnou vazbu. Ethen C2H4 je surovina pro plasty.",
                "Alkyny mají trojnou vazbu. Ethyn, acetylen, se používá ke svařování.",
                "Hořením uhlovodíků vzniká oxid uhličitý a voda.",
                NULL,
            },
        },
        {
            "3 / 3   •   Areny", "Benzen",
            "Tip: benzen není alken se třemi obyčejnými dvojnými vazbami.",
            {
                "Areny jsou aromatické uhlovodíky s zvláštně stálým kruhem.",
                "Benzen má vzorec C6H6.",
                "Aromatický kruh je stálejší než obyčejné střídání dvojných vazeb.",
                "Z arenů se vyrábějí plasty, léčiva i barviva.",
                NULL,
            },
        },
    };

    return sci_unit_page(0, &chem_lessons[5], "chem_unit6", "chem_unit6_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_chem_unit6_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Který uhlovodík je alkan?",
         {"Ethen", "Ethyn", "Methan", "Benzen"},
         4, 2},
        {"Jaký je obecný vzorec alkanů?",
         {"CnH2n", "CnH2n+2", "CnH2n−2", "CnH2n−6"},
         4, 1},
        {"Čím se ethen liší od ethanu?",
         {"Nemá uhlík", "Má dvojnou vazbu",
          "Je to kov", "Má trojnou vazbu"},
         4, 1},
        {"Jaký vzorec má benzen?",
         {"CH4", "C2H4", "C2H2", "C6H6"},
         4, 3},
    };
    static const char *hints[] = {
        "Methan a ethan jsou alkany. Ethen je alken, ethyn alkyn, benzen aren.",
        "Alkany CnH2n+2, alkeny CnH2n, alkyny CnH2n−2.",
        "Ethan je nasycený. Ethen má jednu dvojnou vazbu.",
        "Benzen C6H6 je základní aren. Methan je CH4.",
    };

    return sci_mcq_page(0, 5, "chemunit6", "chem_ex6_title", "chem_quiz6_head",
                        qs, hints, 4);
}

static GtkWidget *build_chem_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Skupiny", "Deriváty uhlovodíků",
            "Tip: funkční skupina rozhoduje o vlastnostech víc než délka řetězce.",
            {
                "Derivát vzniká náhradou vodíku jinou skupinou atomů.",
                "Alkoholy mají hydroxyl −OH. Ethanol je v alkoholických nápojích.",
                "Karboxylové kyseliny mají −COOH. Kyselina octová je v octu.",
                "Aldehydy a ketony obsahují karbonylovou skupinu C=O.",
                NULL,
            },
        },
        {
            "2 / 3   •   Cukry", "Sacharidy",
            "Tip: glukóza je monosacharid, škrob a celulóza jsou její polymery.",
            {
                "Sacharidy jsou přírodní látky z uhlíku, vodíku a kyslíku.",
                "Glukóza C6H12O6 je základní cukr a palivo buněk.",
                "Sacharosa je řepný a třtinový cukr, škrob je zásoba rostlin.",
                "Celulóza tvoří buněčné stěny rostlin a člověk ji netráví.",
                NULL,
            },
        },
        {
            "3 / 3   •   Tuky a bílkoviny", "Z čeho je tělo",
            "Tip: bílkovina je řetězec aminokyselin, tuk je ester glycerolu.",
            {
                "Lipidy jsou tuky a oleje, estery glycerolu a mastných kyselin.",
                "Tuky jsou zásoba energie a součást membrán.",
                "Bílkoviny vznikají spojením aminokyselin peptidovou vazbou.",
                "Pořadí aminokyselin určuje tvar a funkci bílkoviny.",
                NULL,
            },
        },
    };

    return sci_unit_page(0, &chem_lessons[6], "chem_unit7", "chem_unit7_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_chem_unit7_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kterou skupinu mají alkoholy?",
         {"−COOH", "−OH", "−NH2 jako jedinou v alkanech", "Jen dvojnou vazbu uhlík–uhlík"},
         4, 1},
        {"Co je kyselina octová?",
         {"Alkan", "Karboxylová kyselina", "Halogen", "Hydroxid"},
         4, 1},
        {"Která látka je sacharid?",
         {"Škrob", "Chlorid sodný", "Ethyn", "Oxid vápenatý"},
         4, 0},
        {"Z čeho jsou bílkoviny?",
         {"Z glycerolu a nic jiného", "Z aminokyselin",
          "Jen z glukózy", "Z vzácných plynů"},
         4, 1},
    };
    static const char *hints[] = {
        "Hydroxyl −OH je alkohol. Karboxyl −COOH je kyselina.",
        "Octová kyselina CH3COOH má karboxyl. V octu je její zředěný roztok.",
        "Glukóza, sacharosa, škrob a celulóza jsou sacharidy.",
        "Aminokyseliny se pojí peptidovou vazbou do bílkoviny.",
    };

    return sci_mcq_page(0, 6, "chemunit7", "chem_ex7_title", "chem_quiz7_head",
                        qs, hints, 4);
}

static GtkWidget *build_chem_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Enzymy", "Biokatalyzátory",
            "Tip: enzym je bílkovina a urychluje jednu určitou reakci.",
            {
                "Biochemie zkoumá chemické děje v živých organismech.",
                "Enzym je bílkovinný katalyzátor. Snižuje aktivační energii.",
                "Každý enzym působí na určitý substrát, jako klíč do zámku.",
                "Teplo a nevhodné pH enzym denaturují a reakce se zpomalí.",
                NULL,
            },
        },
        {
            "2 / 3   •   Energie", "Fotosyntéza a dýchání",
            "Tip: fotosyntéza váže energii, buněčné dýchání ji uvolňuje.",
            {
                "Fotosyntéza: 6 CO2 + 6 H2O → C6H12O6 + 6 O2, potřebuje světlo.",
                "Probíhá v chloroplastech a je zdrojem kyslíku i organických látek.",
                "Buněčné dýchání glukózu oxiduje a uvolňuje energii pro buňku.",
                "Zjednodušeně je dýchání opačný směr než fotosyntéza.",
                NULL,
            },
        },
        {
            "3 / 3   •   DNA", "Chemický zápis dědičnosti",
            "Tip: v RNA je uracil tam, kde má DNA thymin.",
            {
                "Nukleové kyseliny jsou DNA a RNA.",
                "DNA nese genetickou informaci. Báze jsou adenin, thymin, guanin a cytosin.",
                "Adenin se páruje s thyminem, guanin s cytosinem.",
                "RNA má místo thyminu uracil a v buňce pomáhá číst geny.",
                NULL,
            },
        },
    };

    return sci_unit_page(0, &chem_lessons[7], "chem_unit8", "chem_unit8_sub",
                          slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_chem_unit8_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co je enzym?",
         {"Anorganická sůl", "Bílkovinný biokatalyzátor",
          "Vzácný plyn", "Nasycený uhlovodík"},
         4, 1},
        {"Co při fotosyntéze vzniká spolu s glukózou?",
         {"Dusík", "Kyslík", "Chlorid sodný", "Železo"},
         4, 1},
        {"Která báze je v RNA místo thyminu?",
         {"Adenin", "Uracil", "Guanin navíc bez cytosinu", "Sodík"},
         4, 1},
        {"Co dělá buněčné dýchání s glukózou?",
         {"Ukládá ji beze změny do jádra",
          "Oxiduje ji a uvolňuje energii",
          "Mění ji na vzácný plyn",
          "Zastavuje všechny enzymy"},
         4, 1},
    };
    static const char *hints[] = {
        "Enzym je bílkovina. Katalyzátor reakci urychlí a sám se nespotřebuje.",
        "Zjednodušená rovnice končí glukózou a kyslíkem.",
        "DNA: A, T, G, C. RNA má U místo T.",
        "Dýchání je opak fotosyntézy v tom smyslu, že energii z glukózy uvolní.",
    };

    return sci_mcq_page(0, 7, "chemunit8", "chem_ex8_title", "chem_quiz8_head",
                        qs, hints, 4);
}

void add_chem_pages(GtkStack *stack) {
    typedef GtkWidget *(*SciBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        SciBuilder build_unit;
        SciBuilder build_ex;
    } pages[] = {
        {"chemunit1", "chemex1", build_chem_unit1_page, build_chem_unit1_exercise_page},
        {"chemunit2", "chemex2", build_chem_unit2_page, build_chem_unit2_exercise_page},
        {"chemunit3", "chemex3", build_chem_unit3_page, build_chem_unit3_exercise_page},
        {"chemunit4", "chemex4", build_chem_unit4_page, build_chem_unit4_exercise_page},
        {"chemunit5", "chemex5", build_chem_unit5_page, build_chem_unit5_exercise_page},
        {"chemunit6", "chemex6", build_chem_unit6_page, build_chem_unit6_exercise_page},
        {"chemunit7", "chemex7", build_chem_unit7_page, build_chem_unit7_exercise_page},
        {"chemunit8", "chemex8", build_chem_unit8_page, build_chem_unit8_exercise_page},
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
