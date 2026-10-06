#include "graduately.h"

/* Year 2: kinetic theory, heat, the structure of matter, changes of
 * state, mechanical oscillation and waves, sound. */

NetLesson fyz2_lessons[FYZ_N] = {
    { .n_slides = 3, .unit_page = "fyz2unit1", .ex_page = "fyz2ex1" },
    { .n_slides = 3, .unit_page = "fyz2unit2", .ex_page = "fyz2ex2" },
    { .n_slides = 3, .unit_page = "fyz2unit3", .ex_page = "fyz2ex3" },
    { .n_slides = 3, .unit_page = "fyz2unit4", .ex_page = "fyz2ex4" },
    { .n_slides = 3, .unit_page = "fyz2unit5", .ex_page = "fyz2ex5" },
    { .n_slides = 3, .unit_page = "fyz2unit6", .ex_page = "fyz2ex6" },
    { .n_slides = 3, .unit_page = "fyz2unit7", .ex_page = "fyz2ex7" },
    { .n_slides = 3, .unit_page = "fyz2unit8", .ex_page = "fyz2ex8" },
};

static GtkWidget *build_fyz2_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Částice", "Z čeho je látka",
            "Tip: částice se neustále neuspořádaně pohybují.",
            {
                "Látky se skládají z atomů, molekul nebo iontů.",
                "Částice se neuspořádaně pohybují. Tomu říkáme tepelný pohyb.",
                "Mezi částicemi působí přitažlivé i odpudivé síly.",
                "Brownův pohyb a difuze jsou důkazy pohybu částic.",
                NULL,
            },
        },
        {
            "2 / 3   •   Skupenství", "Pevné, kapalné, plynné",
            "Tip: v plynu jsou částice daleko od sebe a nevyplňují stálý tvar.",
            {
                "V pevné látce částice kmitají kolem rovnovážných poloh.",
                "V kapalině jsou částice blízko, ale mohou se přesouvat.",
                "V plynu se částice pohybují volně a narážejí do stěn.",
                "Plazma je ionizovaný plyn a bývá čtvrté skupenství.",
                NULL,
            },
        },
        {
            "3 / 3   •   Energie", "Vnitřní energie",
            "Tip: vnitřní energie roste s teplotou.",
            {
                "Vnitřní energie je součet kinetické a potenciální energie částic.",
                "Teplo je energie předaná díky rozdílu teplot.",
                "Práce i teplo mohou vnitřní energii změnit.",
                "Teplota popisuje stav, teplo popisuje děj.",
                NULL,
            },
        },
    };

    return fyz_unit_page(1, &fyz2_lessons[0], "fyz2_unit1", "fyz2_unit1_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz2_unit1_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co dokazuje Brownův pohyb?",
         {"Že částice stojí", "Že se částice neustále pohybují",
          "Že světlo je vlna", "Že tlak nezávisí na teplotě"},
         4, 1},
        {"Jak se částice chovají v pevné látce?",
         {"Volně létají nádobou",
          "Kmitají kolem rovnovážných poloh",
          "Nemají žádnou energii",
          "Jsou od sebe jako v řídkém plynu"},
         4, 1},
        {"Co je teplo?",
         {"Stavová veličina stejná jako teplota",
          "Energie předaná díky rozdílu teplot",
          "Hmotnost částic",
          "Tlak nasycené páry"},
         4, 1},
        {"Co je vnitřní energie?",
         {"Jen polohová energie celého tělesa m·g·h",
          "Součet kinetické a potenciální energie částic",
          "Práce tíhové síly",
          "Výkon ohřívače"},
         4, 1},
    };
    static const char *hints[] = {
        "Pylová zrnka ve vodě poskakují, protože do nich narážejí molekuly.",
        "Kapalina teče, plyn vyplní nádobu. Pevná látka drží tvar.",
        "Teplota je stav. Teplo je přenos energie.",
        "Zahřátí zvýší kinetickou energii částic, a tím vnitřní energii.",
    };

    return fyz_mcq_page(1, 0, "fyz2unit1", "fyz2_ex1_title", "fyz2_quiz1_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz2_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Stupnice", "Celsiova a termodynamická",
            "Tip: 0 °C je 273,15 K. Velikost jednoho stupně je stejná.",
            {
                "Celsiova teplota má nulu při tání ledu a 100 °C při varu vody za normálního tlaku.",
                "Termodynamická teplota T se měří v kelvinech.",
                "T = t + 273,15. Absolutní nula je 0 K, tedy −273,15 °C.",
                "Částice se při absolutní nule nezastaví úplně, ale klasická teorie tam končí.",
                NULL,
            },
        },
        {
            "2 / 3   •   Teplo", "Kalorimetrická rovnice",
            "Tip: měrná tepelná kapacita vody je asi 4180 J/(kg·K).",
            {
                "Teplo přijaté tělesem je Q = m·c·Δt.",
                "c je měrná tepelná kapacita, u vody přibližně 4180 J/(kg·K).",
                "Tepelná kapacita C = m·c říká, kolik tepla ohřeje těleso o 1 K.",
                "V izolované soustavě je teplo odevzdané rovno teplu přijatému.",
                NULL,
            },
        },
        {
            "3 / 3   •   Přenos", "Vedení, proudění, záření",
            "Tip: vakuum nevede teplo částicemi, záření jím projde.",
            {
                "Vedení přenáší energii srážkami částic, hlavně v pevných látkách.",
                "Proudění přenáší teplo pohybem kapaliny nebo plynu.",
                "Záření přenáší energii elektromagnetickými vlnami i ve vakuu.",
                "Kovy vedou teplo dobře, vzduch a pěnové izolace špatně.",
                NULL,
            },
        },
    };

    return fyz_unit_page(1, &fyz2_lessons[1], "fyz2_unit2", "fyz2_unit2_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz2_unit2_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kolik kelvinů je 0 °C?",
         {"0 K", "100 K", "273,15 K", "373,15 K"},
         4, 2},
        {"Jak se počítá přijaté teplo?",
         {"Q = m·c·Δt", "Q = m·g·h", "Q = ½·m·v²", "Q = p·V"},
         4, 0},
        {"Jaká je přibližně měrná tepelná kapacita vody?",
         {"4180 J/(kg·K)", "9,81 J/(kg·K)", "101 J/(kg·K)", "3·10⁸ J/(kg·K)"},
         4, 0},
        {"Který přenos tepla funguje i ve vakuu?",
         {"Vedení", "Proudění", "Záření", "Difuzi částic"},
         4, 2},
    };
    static const char *hints[] = {
        "100 °C je 373,15 K. Rozdíl 1 °C je stejný jako rozdíl 1 K.",
        "Δt je změna teploty. Větší hmotnost i větší c znamenají více tepla.",
        "Voda má velkou měrnou tepelnou kapacitu, proto se ohřívá pomalu.",
        "Vedení a proudění potřebují látku. Záření ne.",
    };

    return fyz_mcq_page(1, 1, "fyz2unit2", "fyz2_ex2_title", "fyz2_quiz2_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz2_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Kalorimetr", "Směšovací rovnice",
            "Tip: teplejší těleso teplo odevzdá, chladnější přijme.",
            {
                "V izolované soustavě platí Q odevzdané = Q přijaté.",
                "Při smíchání dvou dávek téže kapaliny je m₁·c·(t₁ − t) = m₂·c·(t − t₂).",
                "c se u stejné látky zkrátí.",
                "Kalorimetr sám také přijme část tepla. Přesný výpočet s ním počítá.",
                NULL,
            },
        },
        {
            "2 / 3   •   Skupenství", "Teplo bez změny teploty",
            "Tip: při tání a varu se teplota čisté látky nemění.",
            {
                "Skupenské teplo tání je Lt = m·lt.",
                "Skupenské teplo varu je Lv = m·lv.",
                "Měrné skupenské teplo tání ledu je asi 334 kJ/kg.",
                "Měrné skupenské teplo varu vody je asi 2,26 MJ/kg.",
                NULL,
            },
        },
        {
            "3 / 3   •   1. věta", "Zákon zachování",
            "Tip: perpetuum mobile prvního druhu odporuje první větě.",
            {
                "První termodynamická věta: ΔU = W + Q.",
                "Znaménka závisí na dohodě, škola často bere teplo přijaté a práci vykonanou na soustavě.",
                "Energie nevzniká ani nezaniká, jen se přeměňuje.",
                "Stroj nemůže trvale konat práci bez přísunu energie.",
                NULL,
            },
        },
    };

    return fyz_unit_page(1, &fyz2_lessons[2], "fyz2_unit3", "fyz2_unit3_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz2_unit3_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co platí v izolované soustavě při výměně tepla?",
         {"Teplo odevzdané je větší než přijaté",
          "Teplo odevzdané se rovná teplu přijatému",
          "Teplota obou těles zůstane původní",
          "Hmotnost se zdvojnásobí"},
         4, 1},
        {"Co se děje s teplotou čisté látky při tání?",
         {"Stoupá rovnoměrně", "Nemění se, dokud látka taje",
          "Klesá k absolutní nule", "Skáče o 100 K"},
         4, 1},
        {"Jak se počítá skupenské teplo tání?",
         {"Lt = m·c·Δt", "Lt = m·lt", "Lt = p·V", "Lt = ½·m·v²"},
         4, 1},
        {"Co říká první termodynamická věta?",
         {"Teplo se nedá nikam předat",
          "Změna vnitřní energie souvisí s teplem a prací",
          "Teplota je vždy 273 K",
          "Účinnost tepelného stroje je 100 %"},
         4, 1},
    };
    static const char *hints[] = {
        "Kalorimetrická rovnice z toho přímo vychází.",
        "Dodávané teplo při tání rozrušuje vazby, nezvedá teplotu.",
        "lt je měrné skupenské teplo tání. U ledu asi 334 kJ/kg.",
        "Druhá věta omezuje, kolik tepla lze proměnit v práci.",
    };

    return fyz_mcq_page(1, 2, "fyz2unit3", "fyz2_ex3_title", "fyz2_quiz3_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz2_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Model", "Ideální plyn",
            "Tip: stavová rovnice je p·V = n·R·T.",
            {
                "Ideální plyn má zanedbatelný vlastní objem částic a srážky bez ztráty kinetické energie.",
                "Stavová rovnice je p·V = n·R·T.",
                "R je molární plynová konstanta, asi 8,31 J/(mol·K).",
                "T v této rovnici musí být v kelvinech.",
                NULL,
            },
        },
        {
            "2 / 3   •   Děje", "Izochorický, izobarický, izotermický",
            "Tip: izotermický děj má stálou teplotu, proto p·V je konstantní.",
            {
                "Izochorický děj má stálý objem. Platí p / T = konst.",
                "Izobarický děj má stálý tlak. Platí V / T = konst.",
                "Izotermický děj má stálou teplotu. Platí p·V = konst.",
                "Adiabatický děj nevyměňuje teplo s okolím.",
                NULL,
            },
        },
        {
            "3 / 3   •   Tlak plynu", "Odkud se bere",
            "Tip: vyšší teplota znamená rychlejší částice a větší tlak.",
            {
                "Tlak plynu vzniká nárazy částic na stěnu.",
                "Střední kinetická energie částice je úměrná termodynamické teplotě.",
                "Normální podmínky jsou 0 °C a normální atmosférický tlak.",
                "Reálný plyn se ideálnímu blíží při nízkém tlaku a vyšší teplotě.",
                NULL,
            },
        },
    };

    return fyz_unit_page(1, &fyz2_lessons[3], "fyz2_unit4", "fyz2_unit4_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz2_unit4_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak zní stavová rovnice ideálního plynu?",
         {"p·V = n·R·T", "p / V = T", "Q = m·c·Δt", "F = m·a"},
         4, 0},
        {"Co je stálé při izotermickém ději?",
         {"Objem", "Tlak", "Teplota", "Hmotnost částice"},
         4, 2},
        {"Co platí při izochorickém ději?",
         {"V / T = konst.", "p / T = konst.", "p·V = konst.", "p·T = V"},
         4, 1},
        {"V jakých jednotkách musí být T ve stavové rovnici?",
         {"Ve stupních Celsia", "V kelvinech", "Ve wattech", "V pascalech"},
         4, 1},
    };
    static const char *hints[] = {
        "n je látkové množství, R ≈ 8,31 J/(mol·K).",
        "Boylův-Mariottův zákon: p·V = konst. při stálé teplotě.",
        "Izochorický znamená stálý objem. Zahřátí zvedne tlak.",
        "Dosazení Celsiovy teploty do p·V = n·R·T je chyba.",
    };

    return fyz_mcq_page(1, 3, "fyz2unit4", "fyz2_ex4_title", "fyz2_quiz4_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz2_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Deformace", "Hookův zákon",
            "Tip: v pružné oblasti je prodloužení úměrné síle.",
            {
                "Deformace je změna tvaru nebo objemu.",
                "Pružná deformace zmizí, až síla přestane působit.",
                "Tvárná deformace zůstane.",
                "Hookův zákon: F = k·Δl. k je tuhost.",
                NULL,
            },
        },
        {
            "2 / 3   •   Krystal", "Pevné látky",
            "Tip: amorfní látka nemá pravidelnou mřížku v celém objemu.",
            {
                "Krystal má částice v pravidelné mřížce.",
                "Amorfní látka, třeba sklo, pravidelnou mřížku na velkou vzdálenost nemá.",
                "Teplotní roztažnost zvětšuje rozměry při zahřátí.",
                "Dilatace mostů a kolejí počítá s mezerami.",
                NULL,
            },
        },
        {
            "3 / 3   •   Povrch", "Kapaliny",
            "Tip: kapka je kulatá, protože povrchová vrstva se stahuje.",
            {
                "Povrchová vrstva kapaliny se chová jako tenká blána.",
                "Povrchové napětí stahuje povrch na co nejmenší obsah.",
                "Kapilární elevace zvedá smáčivou kapalinu v úzké trubici.",
                "Kapilární deprese stahuje nesmáčivou kapalinu, třeba rtuť ve skle.",
                NULL,
            },
        },
    };

    return fyz_unit_page(1, &fyz2_lessons[4], "fyz2_unit5", "fyz2_unit5_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz2_unit5_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co říká Hookův zákon v pružné oblasti?",
         {"Síla je úměrná prodloužení",
          "Síla je úměrná druhé mocnině prodloužení vždy",
          "Deformace je vždy trvalá",
          "Tlak nezávisí na síle"},
         4, 0},
        {"Čím se krystal liší od amorfní látky?",
         {"Nemá částice",
          "Má pravidelnou mřížku",
          "Nemá teplotní roztažnost",
          "Je vždy plyn"},
         4, 1},
        {"Proč má kapka kulový tvar?",
         {"Protože povrchová vrstva stahuje povrch",
          "Protože v kapalině není tlak",
          "Protože kapka je krystal",
          "Protože Hookův zákon platí jen pro koule"},
         4, 0},
        {"Co je kapilární elevace?",
         {"Pokles nesmáčivé kapaliny",
          "Vzestup smáčivé kapaliny v úzké trubici",
          "Var při nízkém tlaku",
          "Trvalá deformace"},
         4, 1},
    };
    static const char *hints[] = {
        "Za mezí pružnosti už závislost není přímá a těleso se nevrátí.",
        "Sklo je typická amorfní látka. Sůl je krystal.",
        "Koule má při daném objemu nejmenší povrch.",
        "Deprese je pokles, třeba rtuti. Elevace je vzestup, třeba vody.",
    };

    return fyz_mcq_page(1, 4, "fyz2unit5", "fyz2_ex5_title", "fyz2_quiz5_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz2_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Tání a tuhnutí", "Pevná látka a kapalina",
            "Tip: teplota tání a teplota tuhnutí téže látky jsou stejné.",
            {
                "Tání mění pevnou látku na kapalinu při teplotě tání.",
                "Tuhnutí je opačný děj a u čisté látky probíhá při stejné teplotě.",
                "Příměsi teplotu tání obvykle snižují.",
                "Led potřebuje skupenské teplo tání, proto směs ledu a vody drží 0 °C.",
                NULL,
            },
        },
        {
            "2 / 3   •   Vypařování", "Var a pára",
            "Tip: var nastane, když se tlak syté páry rovná vnějšímu tlaku.",
            {
                "Vypařování probíhá z povrchu při každé teplotě.",
                "Var probíhá v celém objemu při teplotě varu.",
                "Sytá pára je v rovnováze se svou kapalinou.",
                "Nižší vnější tlak znamená nižší teplotu varu.",
                NULL,
            },
        },
        {
            "3 / 3   •   Vlhkost", "Pára ve vzduchu",
            "Tip: relativní vlhkost 100 % znamená, že pára je sytá.",
            {
                "Absolutní vlhkost udává hmotnost vodní páry v objemu vzduchu.",
                "Relativní vlhkost je podíl tlaku páry a tlaku syté páry při dané teplotě.",
                "Rosný bod je teplota, při které se pára ve vzduchu stane sytou.",
                "Kondenzace je vznik kapaliny z páry.",
                NULL,
            },
        },
    };

    return fyz_unit_page(1, &fyz2_lessons[5], "fyz2_unit6", "fyz2_unit6_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz2_unit6_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Při jaké teplotě tuhne čistá voda za normálního tlaku?",
         {"Při 100 °C", "Při 0 °C", "Při −273,15 °C", "Při teplotě varu rtuti"},
         4, 1},
        {"Čím se var liší od vypařování?",
         {"Var probíhá jen na povrchu a při každé teplotě",
          "Var probíhá v celém objemu při teplotě varu",
          "Var nepotřebuje skupenské teplo",
          "Vypařování nastane jen při 100 °C"},
         4, 1},
        {"Kdy nastane var?",
         {"Když je tlak syté páry roven vnějšímu tlaku",
          "Když je relativní vlhkost nulová",
          "Když klesne teplota k rosnému bodu",
          "Když je látka amorfní"},
         4, 0},
        {"Co je relativní vlhkost 100 %?",
         {"Ve vzduchu není žádná pára",
          "Pára ve vzduchu je sytá",
          "Teplota je 100 °C",
          "Tlak je 100 Pa"},
         4, 1},
    };
    static const char *hints[] = {
        "Tání a tuhnutí čisté vody jsou při 0 °C. Var je při 100 °C.",
        "Vypařování je povrchové a děje se i pod teplotou varu.",
        "Na horách je nižší tlak, proto voda vře při nižší teplotě.",
        "Rosný bod je teplota, při níž relativní vlhkost dosáhne 100 %.",
    };

    return fyz_mcq_page(1, 5, "fyz2unit6", "fyz2_ex6_title", "fyz2_quiz6_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz2_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Oscilátor", "Perioda a frekvence",
            "Tip: frekvence je převrácená hodnota periody.",
            {
                "Kmitání je pohyb, který se po periodě opakuje.",
                "Perioda T je doba jednoho kmitu. Frekvence f = 1 / T.",
                "Jednotka frekvence je hertz, Hz.",
                "Harmonické kmitání popisuje sinus nebo kosinus.",
                NULL,
            },
        },
        {
            "2 / 3   •   Dva oscilátory", "Pružina a kyvadlo",
            "Tip: perioda kyvadla nezávisí na hmotnosti závaží.",
            {
                "Pružinový oscilátor má T = 2π·√(m / k).",
                "Matematické kyvadlo má pro malé výchylky T = 2π·√(l / g).",
                "Delší kyvadlo kmitá pomaleji.",
                "Větší g znamená kratší periodu kyvadla.",
                NULL,
            },
        },
        {
            "3 / 3   •   Rezonance", "Vlastní a nucené kmitání",
            "Tip: rezonance nastane, když budicí frekvence souhlasí s vlastní.",
            {
                "Vlastní frekvence patří volnému oscilátoru.",
                "Nucené kmitání udržuje vnější síla.",
                "Při rezonanci amplituda silně vzroste.",
                "Tlumení energii kmitů odebírá a amplitudu zmenšuje.",
                NULL,
            },
        },
    };

    return fyz_unit_page(1, &fyz2_lessons[6], "fyz2_unit7", "fyz2_unit7_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz2_unit7_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak spolu souvisí frekvence a perioda?",
         {"f = T", "f = 1 / T", "f = T²", "f = 2π·T"},
         4, 1},
        {"Na čem závisí perioda matematického kyvadla?",
         {"Na hmotnosti závaží a na barvě",
          "Na délce a na tíhovém zrychlení",
          "Jen na amplitudě při velkých úhlech jako jediném údaji",
          "Na tlaku vzduchu jako na hlavní veličině"},
         4, 1},
        {"Jak se změní perioda, když kyvadlo prodloužíme?",
         {"Zkrátí se", "Prodlouží se", "Nezmění se", "Stane se nulovou"},
         4, 1},
        {"Kdy nastane rezonance?",
         {"Když je budicí frekvence rovna vlastní frekvenci",
          "Když je tlumení nekonečné",
          "Když oscilátor stojí",
          "Když je perioda rovna vlnové délce"},
         4, 0},
    };
    static const char *hints[] = {
        "10 Hz znamená 10 kmitů za sekundu, perioda je 0,1 s.",
        "Vzorec T = 2π·√(l / g) hmotnost neobsahuje.",
        "Pod odmocninou je délka. Delší l znamená větší T.",
        "Rezonance může stavbu nebo most rozkmitat nebezpečně.",
    };

    return fyz_mcq_page(1, 6, "fyz2unit7", "fyz2_ex7_title", "fyz2_quiz7_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz2_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Vlna", "Vlnová délka",
            "Tip: v = λ·f platí pro každou vlnu.",
            {
                "Vlnění přenáší energii, ne látku jako celek.",
                "Vlnová délka λ je vzdálenost dvou sousedních bodů ve stejné fázi.",
                "Rychlost vlny je v = λ·f.",
                "Postupné vlnění se šíří prostředím. Stojaté vzniká skládáním.",
                NULL,
            },
        },
        {
            "2 / 3   •   Druhy", "Příčné a podélné",
            "Tip: zvuk ve vzduchu je podélné vlnění.",
            {
                "U příčného vlnění kmitají částice kolmo ke směru šíření.",
                "U podélného vlnění kmitají ve směru šíření.",
                "Na hladině vody je vlnění příčné, zvuk v plynu podélný.",
                "Odraz, lom a ohyb patří k vlnění obecně.",
                NULL,
            },
        },
        {
            "3 / 3   •   Zvuk", "Výška, hlasitost, barva",
            "Tip: zdravé ucho slyší zhruba od 20 Hz do 20 kHz.",
            {
                "Zvuk je mechanické vlnění, které vnímáme sluchem.",
                "Výška tónu odpovídá frekvenci. Hlasitost souvisí s amplitudou.",
                "Infrazvuk je pod 20 Hz, ultrazvuk nad 20 kHz.",
                "Ve vzduchu při pokojové teplotě je rychlost zvuku asi 340 m/s.",
                NULL,
            },
        },
    };

    return fyz_unit_page(1, &fyz2_lessons[7], "fyz2_unit8", "fyz2_unit8_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz2_unit8_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak se počítá rychlost vlny?",
         {"v = λ / f", "v = λ·f", "v = f / λ", "v = T·λ²"},
         4, 1},
        {"Jaké vlnění je zvuk ve vzduchu?",
         {"Příčné elektromagnetické", "Podélné mechanické",
          "Stojaté jen na hladině", "Příčné jako světlo"},
         4, 1},
        {"Které frekvence člověk obvykle slyší?",
         {"Od 20 Hz do 20 kHz", "Jen nad 20 kHz",
          "Jen pod 20 Hz", "Od 340 Hz do 340 kHz"},
         4, 0},
        {"Jak rychle se šíří zvuk ve vzduchu při pokojové teplotě?",
         {"Asi 340 m/s", "Asi 3·10⁸ m/s", "Asi 9,81 m/s", "Asi 1500 km/s"},
         4, 0},
    };
    static const char *hints[] = {
        "Vyšší frekvence při stejné rychlosti znamená kratší vlnovou délku.",
        "Světlo je příčné elektromagnetické. Zvuk potřebuje látkové prostředí.",
        "Pod 20 Hz je infrazvuk, nad 20 kHz ultrazvuk.",
        "Ve vodě je zvuk rychlejší, zhruba 1500 m/s. Světlo má 3·10⁸ m/s.",
    };

    return fyz_mcq_page(1, 7, "fyz2unit8", "fyz2_ex8_title", "fyz2_quiz8_head",
                        qs, hints, 4);
}

void add_fyz2_pages(GtkStack *stack) {
    typedef GtkWidget *(*FyzBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        FyzBuilder build_unit;
        FyzBuilder build_ex;
    } pages[] = {
        {"fyz2unit1", "fyz2ex1", build_fyz2_unit1_page, build_fyz2_unit1_exercise_page},
        {"fyz2unit2", "fyz2ex2", build_fyz2_unit2_page, build_fyz2_unit2_exercise_page},
        {"fyz2unit3", "fyz2ex3", build_fyz2_unit3_page, build_fyz2_unit3_exercise_page},
        {"fyz2unit4", "fyz2ex4", build_fyz2_unit4_page, build_fyz2_unit4_exercise_page},
        {"fyz2unit5", "fyz2ex5", build_fyz2_unit5_page, build_fyz2_unit5_exercise_page},
        {"fyz2unit6", "fyz2ex6", build_fyz2_unit6_page, build_fyz2_unit6_exercise_page},
        {"fyz2unit7", "fyz2ex7", build_fyz2_unit7_page, build_fyz2_unit7_exercise_page},
        {"fyz2unit8", "fyz2ex8", build_fyz2_unit8_page, build_fyz2_unit8_exercise_page},
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
