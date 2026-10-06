#include "graduately.h"

/* Year 1: mechanics. Quantities, kinematics, dynamics, energy,
 * gravitation, rigid bodies, fluids. */

NetLesson fyz_lessons[FYZ_N] = {
    { .n_slides = 3, .unit_page = "fyzunit1", .ex_page = "fyzex1" },
    { .n_slides = 3, .unit_page = "fyzunit2", .ex_page = "fyzex2" },
    { .n_slides = 3, .unit_page = "fyzunit3", .ex_page = "fyzex3" },
    { .n_slides = 3, .unit_page = "fyzunit4", .ex_page = "fyzex4" },
    { .n_slides = 3, .unit_page = "fyzunit5", .ex_page = "fyzex5" },
    { .n_slides = 3, .unit_page = "fyzunit6", .ex_page = "fyzex6" },
    { .n_slides = 3, .unit_page = "fyzunit7", .ex_page = "fyzex7" },
    { .n_slides = 3, .unit_page = "fyzunit8", .ex_page = "fyzex8" },
};

static GtkWidget *build_fyz_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Měření", "Veličina a jednotka",
            "Tip: základní jednotka hmotnosti je kilogram, ne gram.",
            {
                "Fyzikální veličina má číselnou hodnotu a jednotku.",
                "Soustava SI má sedm základních jednotek: m, kg, s, A, K, mol a cd.",
                "Odvozené jednotky vznikají ze základních, třeba newton nebo joule.",
                "Předpona kilo znamená tisíc, mili znamená tisícinu.",
                NULL,
            },
        },
        {
            "2 / 3   •   Druhy", "Skalár a vektor",
            "Tip: rychlost jako vektor má velikost i směr.",
            {
                "Skalár má jen velikost. Patří sem hmotnost, čas nebo teplota.",
                "Vektor má velikost i směr. Patří sem síla, rychlost a zrychlení.",
                "Vektory se skládají podle rovnoběžníku.",
                "Opačný vektor má stejnou velikost a opačný směr.",
                NULL,
            },
        },
        {
            "3 / 3   •   Přesnost", "Chyba měření",
            "Tip: relativní chyba se často udává v procentech.",
            {
                "Každé měření má chybu. Opakování měření ji zmenšuje.",
                "Absolutní chyba je rozdíl naměřené a správné hodnoty.",
                "Relativní chyba je podíl absolutní chyby a správné hodnoty.",
                "Výsledek se zaokrouhluje podle přesnosti měřidla.",
                NULL,
            },
        },
    };

    return fyz_unit_page(0, &fyz_lessons[0], "fyz_unit1", "fyz_unit1_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz_unit1_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Která jednotka je základní jednotka SI pro hmotnost?",
         {"Gram", "Kilogram", "Newton", "Tuna"},
         4, 1},
        {"Co má vektor navíc proti skaláru?",
         {"Jen jednotku", "Směr", "Jen číselnou hodnotu", "Předponu kilo"},
         4, 1},
        {"Co znamená předpona mili?",
         {"Tisíc", "Milion", "Tisícinu", "Setinu"},
         4, 2},
        {"Co je relativní chyba?",
         {"Rozdíl naměřené a správné hodnoty",
          "Podíl absolutní chyby a správné hodnoty",
          "Součet všech měření",
          "Nejmenší dílek měřidla"},
         4, 1},
    };
    static const char *hints[] = {
        "Gram je tisícina kilogramu. Newton je jednotka síly.",
        "Hmotnost je skalár. Rychlost a síla jsou vektory.",
        "Kilo je 1000, centi je 0,01, mili je 0,001.",
        "Absolutní chyba je rozdíl. Relativní je tento rozdíl dělený správnou hodnotou.",
    };

    return fyz_mcq_page(0, 0, "fyzunit1", "fyz_ex1_title", "fyz_quiz1_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Pohyb", "Dráha a rychlost",
            "Tip: při rovnoměrném pohybu je rychlost stálá.",
            {
                "Trajektorie je čára, po které se bod pohybuje. Dráha je její délka.",
                "Průměrná rychlost je dráha dělená časem.",
                "Rovnoměrný přímočarý pohyb má stálou rychlost a přímou trajektorii.",
                "Dráha rovnoměrného pohybu je s = v·t.",
                NULL,
            },
        },
        {
            "2 / 3   •   Změna rychlosti", "Zrychlení",
            "Tip: jednotka zrychlení je metr za sekundu na druhou.",
            {
                "Zrychlení popisuje, jak rychle se mění rychlost.",
                "Rovnoměrně zrychlený pohyb má stálé zrychlení.",
                "Rychlost je v = v₀ + a·t.",
                "Dráha z klidu je s = ½·a·t².",
                NULL,
            },
        },
        {
            "3 / 3   •   Pád", "Volný pád",
            "Tip: tíhové zrychlení u Země je přibližně 9,81 m/s².",
            {
                "Volný pád je pohyb jen pod vlivem tíhového zrychlení.",
                "Tíhové zrychlení g je u povrchu Země asi 9,81 m/s².",
                "Odpor vzduchu volný pád zpomaluje. Ve vakuu padají tělesa stejně.",
                "Graf rychlosti na čase je u rovnoměrně zrychleného pohybu přímka.",
                NULL,
            },
        },
    };

    return fyz_unit_page(0, &fyz_lessons[1], "fyz_unit2", "fyz_unit2_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz_unit2_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak se počítá dráha rovnoměrného pohybu?",
         {"s = v / t", "s = v·t", "s = a·t", "s = v·a"},
         4, 1},
        {"Jaká je jednotka zrychlení?",
         {"m/s", "m/s²", "N", "J"},
         4, 1},
        {"Čemu se rovná rychlost rovnoměrně zrychleného pohybu?",
         {"v = v₀ + a·t", "v = s·t", "v = a / t", "v = g·m"},
         4, 0},
        {"Jak velké je tíhové zrychlení u povrchu Země?",
         {"Asi 1 m/s²", "Asi 9,81 m/s²", "Asi 100 m/s²", "Přesně rychlost světla"},
         4, 1},
    };
    static const char *hints[] = {
        "Rychlost je dráha za čas, proto dráha je rychlost krát čas.",
        "Zrychlení je změna rychlosti za čas, tedy metr za sekundu za sekundu.",
        "Počáteční rychlost se k přírůstku a·t přičítá.",
        "Škola používá g ≈ 9,81 m/s², často i přibližně 10 m/s².",
    };

    return fyz_mcq_page(0, 1, "fyzunit2", "fyz_ex2_title", "fyz_quiz2_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   První zákon", "Setrvačnost",
            "Tip: zákon platí v inerciální soustavě.",
            {
                "Těleso zůstává v klidu nebo v rovnoměrném přímočarém pohybu, dokud na ně nepůsobí výsledná síla.",
                "Setrvačnost je snaha tělesa udržet svůj pohybový stav.",
                "Hmotnost je míra setrvačnosti.",
                "Inerciální soustava se vůči jiné inerciální nepohybuje se zrychlením.",
                NULL,
            },
        },
        {
            "2 / 3   •   Druhý zákon", "Síla a zrychlení",
            "Tip: newton je síla, která hmotnosti 1 kg udělí zrychlení 1 m/s².",
            {
                "Výsledná síla je F = m·a.",
                "Větší síla při stejné hmotnosti znamená větší zrychlení.",
                "Větší hmotnost při stejné síle znamená menší zrychlení.",
                "Jednotka síly je newton, N.",
                NULL,
            },
        },
        {
            "3 / 3   •   Třetí zákon", "Akce a reakce",
            "Tip: obě síly působí na dvě různá tělesa.",
            {
                "Síly, kterými na sebe dvě tělesa působí, jsou stejně velké a opačně orientované.",
                "Akce a reakce vznikají i zanikají současně.",
                "Navzájem se neruší, protože každá působí na jiné těleso.",
                "Hybnost je p = m·v. V izolované soustavě se hybnost zachovává.",
                NULL,
            },
        },
    };

    return fyz_unit_page(0, &fyz_lessons[2], "fyz_unit3", "fyz_unit3_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz_unit3_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Co říká první Newtonův zákon?",
         {"F = m·a",
          "Těleso nemění pohybový stav, dokud na ně nepůsobí výsledná síla",
          "Akce a reakce se sčítají na jednom tělese",
          "Hybnost se vždy ztrácí"},
         4, 1},
        {"Jak zní druhý Newtonův zákon?",
         {"F = m / a", "F = m·a", "F = m·v", "F = a / m"},
         4, 1},
        {"Na co působí akce a reakce?",
         {"Na jedno a totéž těleso a ruší se",
          "Na dvě různá tělesa",
          "Jen na těleso v klidu",
          "Jen ve směru tíhy"},
         4, 1},
        {"Co je hybnost?",
         {"m·a", "m·v", "F·t jen jako jednotka", "m·g·h"},
         4, 1},
    };
    static const char *hints[] = {
        "První zákon je zákon setrvačnosti. Druhý je F = m·a.",
        "Newton je kg·m/s². Síla a zrychlení mají stejný směr.",
        "Reakce není síla, která by na stejném tělese rušila akci.",
        "Hybnost je vektor. Izolovaná soustava ji zachovává.",
    };

    return fyz_mcq_page(0, 2, "fyzunit3", "fyz_ex3_title", "fyz_quiz3_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Tíha", "Tíhová a třecí síla",
            "Tip: tíhová síla je G = m·g.",
            {
                "Tíhová síla působí svisle dolů a je G = m·g.",
                "Třecí síla působí proti pohybu nebo proti snaze o pohyb.",
                "Smykové tření závisí na normálové síle a na součiniteli tření.",
                "Valivé tření je obvykle menší než smykové.",
                NULL,
            },
        },
        {
            "2 / 3   •   Tlak", "Síla na plochu",
            "Tip: pascal je newton na metr čtvereční.",
            {
                "Tlak je p = F / S, kde S je obsah plochy.",
                "Stejná síla na menší plochu znamená větší tlak.",
                "Jednotka tlaku je pascal, Pa.",
                "Ostrý hrot má malou plochu, proto velký tlak.",
                NULL,
            },
        },
        {
            "3 / 3   •   Skládání", "Výslednice sil",
            "Tip: dvě stejně velké opačné síly dají nulu.",
            {
                "Síly stejného směru se sčítají.",
                "Síly opačného směru se odečítají.",
                "Kolmé síly se skládají podle Pythagorovy věty.",
                "Rovnováha nastane, když je výslednice nulová.",
                NULL,
            },
        },
    };

    return fyz_unit_page(0, &fyz_lessons[3], "fyz_unit4", "fyz_unit4_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz_unit4_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak se počítá tíhová síla?",
         {"G = m / g", "G = m·g", "G = m·v", "G = F / S"},
         4, 1},
        {"Na čem závisí smykové tření?",
         {"Jen na barvě tělesa",
          "Na normálové síle a součiniteli tření",
          "Jen na objemu",
          "Na rychlosti světla"},
         4, 1},
        {"Jak se počítá tlak?",
         {"p = F·S", "p = F / S", "p = m·a", "p = S / F"},
         4, 1},
        {"Jaká je jednotka tlaku?",
         {"Newton", "Pascal", "Joule", "Watt"},
         4, 1},
    };
    static const char *hints[] = {
        "g je tíhové zrychlení. Tíha má směr svisle dolů.",
        "Ft = f·Fn. Drsnější styk má větší součinitel tření.",
        "Menší plocha při stejné síle zvětší tlak.",
        "1 Pa = 1 N/m². Atmosférický tlak je asi 101 kPa.",
    };

    return fyz_mcq_page(0, 3, "fyzunit4", "fyz_ex4_title", "fyz_quiz4_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Práce", "Síla po dráze",
            "Tip: síla kolmá k posunutí práci nekoná.",
            {
                "Mechanická práce je W = F·s·cos α.",
                "α je úhel mezi silou a posunutím.",
                "Když je síla kolmá k posunutí, práce je nulová.",
                "Jednotka práce je joule, J.",
                NULL,
            },
        },
        {
            "2 / 3   •   Energie", "Pohybová a polohová",
            "Tip: polohová energie roste s výškou.",
            {
                "Kinetická energie je Ek = ½·m·v².",
                "Polohová energie v tíhovém poli je Ep = m·g·h.",
                "Výšku h počítáme od zvolené hladiny.",
                "Bez tření se součet kinetické a polohové energie zachovává.",
                NULL,
            },
        },
        {
            "3 / 3   •   Výkon", "Práce za čas",
            "Tip: watt je joule za sekundu.",
            {
                "Výkon je P = W / t.",
                "Jednotka výkonu je watt, W.",
                "Účinnost je podíl užitečné a dodané energie.",
                "Účinnost je vždy menší než 1, protože část energie se ztrácí.",
                NULL,
            },
        },
    };

    return fyz_unit_page(0, &fyz_lessons[4], "fyz_unit5", "fyz_unit5_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz_unit5_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdy je mechanická práce nulová?",
         {"Když je síla rovnoběžná s posunutím",
          "Když je síla kolmá k posunutí",
          "Když je hmotnost velká",
          "Když je výkon 1 W"},
         4, 1},
        {"Jak se počítá kinetická energie?",
         {"Ek = m·g·h", "Ek = ½·m·v²", "Ek = F / s", "Ek = m·v"},
         4, 1},
        {"Jak se počítá polohová energie v tíhovém poli?",
         {"Ep = ½·m·v²", "Ep = m·g·h", "Ep = F·a", "Ep = p·V"},
         4, 1},
        {"Co je výkon?",
         {"Práce dělená časem", "Síla dělená plochou",
          "Hmotnost krát zrychlení", "Hybnost za dráhu"},
         4, 0},
    };
    static const char *hints[] = {
        "cos 90° je 0, proto kolmá síla práci nekoná.",
        "Dvojnásobná rychlost znamená čtyřnásobnou kinetickou energii.",
        "Hladinu nulové polohové energie si volíme.",
        "Watt je J/s. Účinnost je podíl energií, ne jejich součet.",
    };

    return fyz_mcq_page(0, 4, "fyzunit5", "fyz_ex5_title", "fyz_quiz5_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Zákon", "Newtonova gravitace",
            "Tip: gravitační síla klesá s druhou mocninou vzdálenosti.",
            {
                "Dvě tělesa se přitahují silou Fg = κ·m₁·m₂ / r².",
                "κ je gravitační konstanta, asi 6,67·10⁻¹¹ N·m²/kg².",
                "r je vzdálenost středů těles.",
                "Gravitace působí na dálku a je vždy přitažlivá.",
                NULL,
            },
        },
        {
            "2 / 3   •   U Země", "Tíhové zrychlení",
            "Tip: první kosmická rychlost je asi 7,9 km/s.",
            {
                "U povrchu Země je tíhové zrychlení přibližně 9,81 m/s².",
                "Tíhová síla je výslednice gravitační síly a setrvačné síly od otáčení Země.",
                "První kosmická rychlost je rychlost kruhové dráhy těsně u povrchu, asi 7,9 km/s.",
                "Druhá kosmická rychlost, asi 11,2 km/s, stačí k opuštění Země.",
                NULL,
            },
        },
        {
            "3 / 3   •   Pohyby", "Keplerovy zákony",
            "Tip: planety obíhají po elipsách, Slunce je v ohnisku.",
            {
                "Planety se pohybují po elipsách a Slunce je v jednom ohnisku.",
                "Průvodič planety opíše za stejný čas stejnou plochu.",
                "Poměr krychle velké poloosy a čtverce oběžné doby je pro planety stejný.",
                "Družice padá kolem Země, proto zůstává na oběžné dráze.",
                NULL,
            },
        },
    };

    return fyz_unit_page(0, &fyz_lessons[5], "fyz_unit6", "fyz_unit6_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz_unit6_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak závisí gravitační síla na vzdálenosti?",
         {"Je přímo úměrná vzdálenosti",
          "Je nepřímo úměrná druhé mocnině vzdálenosti",
          "Na vzdálenosti nezávisí",
          "Je úměrná třetí mocnině vzdálenosti"},
         4, 1},
        {"Jaká je přibližně gravitační konstanta κ?",
         {"9,81", "6,67·10⁻¹¹ N·m²/kg²", "3·10⁸ m/s", "101 kPa"},
         4, 1},
        {"Co je první kosmická rychlost?",
         {"Rychlost světla",
          "Rychlost kruhové dráhy těsně u Země, asi 7,9 km/s",
          "Rychlost zvuku",
          "Rychlost volného pádu z 1 m"},
         4, 1},
        {"Jaký tvar mají dráhy planet podle Keplera?",
         {"Kružnice se Sluncem ve středu vždy",
          "Elipsy a Slunce je v ohnisku",
          "Přímky",
          "Paraboly se Zemí v ohnisku"},
         4, 1},
    };
    static const char *hints[] = {
        "Dvojnásobná vzdálenost znamená čtvrtinovou sílu.",
        "κ je velmi malé číslo. g = 9,81 m/s² je zrychlení, ne konstanta κ.",
        "Druhá kosmická rychlost je asi 11,2 km/s.",
        "První Keplerův zákon mluví o elipse a ohnisku.",
    };

    return fyz_mcq_page(0, 5, "fyzunit6", "fyz_ex6_title", "fyz_quiz6_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Otáčení", "Moment síly",
            "Tip: moment je větší, když síla zabírá dál od osy.",
            {
                "Tuhé těleso nemění působením sil svůj tvar.",
                "Moment síly je M = F·d, kde d je rameno síly.",
                "Rameno je kolmá vzdálenost osy od přímky síly.",
                "Jednotka momentu je newton metr, N·m.",
                NULL,
            },
        },
        {
            "2 / 3   •   Páka", "Rovnováha",
            "Tip: na páce v rovnováze jsou momenty na obou stranách stejné.",
            {
                "Těleso je v rovnováze, když je výslednice sil i výsledný moment nulový.",
                "Na dvojzvratné páce platí F₁·d₁ = F₂·d₂.",
                "Páka, kladka a nakloněná rovina jsou jednoduché stroje.",
                "Jednoduchý stroj práci neušetří, jen změní velikost síly nebo dráhu.",
                NULL,
            },
        },
        {
            "3 / 3   •   Těžiště", "Stabilita",
            "Tip: těleso je stabilní, když se po malém vychýlení vrací.",
            {
                "Těžiště je působiště tíhové síly.",
                "U stejnorodého pravidelného tělesa je těžiště ve středu.",
                "Stabilní poloha se po vychýlení vrací, labilní se převrací.",
                "Nízké těžiště a široká základna zvětšují stabilitu.",
                NULL,
            },
        },
    };

    return fyz_unit_page(0, &fyz_lessons[6], "fyz_unit7", "fyz_unit7_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz_unit7_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak se počítá moment síly?",
         {"M = F / d", "M = F·d", "M = m·g·h", "M = p·V"},
         4, 1},
        {"Kdy je páka v rovnováze?",
         {"Když jsou síly vždy stejně velké bez ohledu na rameno",
          "Když se momenty na obou stranách rovnají",
          "Když je jedno rameno nulové",
          "Když páka koná práci zdarma"},
         4, 1},
        {"Co je těžiště?",
         {"Místo největšího tlaku kapaliny",
          "Působiště tíhové síly",
          "Ohnisko elipsy",
          "Střed elektrického náboje"},
         4, 1},
        {"Která poloha je stabilní?",
         {"Po malém vychýlení se těleso vrací",
          "Po malém vychýlení se těleso převrátí",
          "Těleso zůstane v nové poloze vždy",
          "Těžiště je nad bodem dotyku co nejvýš a základna je bod"},
         4, 0},
    };
    static const char *hints[] = {
        "Delší rameno znamená větší moment při stejné síle.",
        "F₁·d₁ = F₂·d₂. Menší silou na delším rameni zvedneme větší náklad.",
        "Těleso se chová, jako by v těžišti působila celá tíha.",
        "Labilní poloha se po vychýlení vzdaluje. Volná zůstane, kde je.",
    };

    return fyz_mcq_page(0, 6, "fyzunit7", "fyz_ex7_title", "fyz_quiz7_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Tlak", "Hydrostatický tlak",
            "Tip: tlak v kapalině roste s hloubkou.",
            {
                "Tlak v kapalině způsobený tíhou je p = h·ρ·g.",
                "h je hloubka, ρ je hustota kapaliny.",
                "V téže hloubce je tlak ve všech směrech stejný.",
                "Pascalův zákon: tlak v uzavřené kapalině se šíří všemi směry stejně.",
                NULL,
            },
        },
        {
            "2 / 3   •   Vztlak", "Archimédův zákon",
            "Tip: těleso plave, když se jeho průměrná hustota rovná hustotě kapaliny nebo je menší.",
            {
                "Na těleso v tekutině působí vztlaková síla svisle vzhůru.",
                "Vztlak se rovná tíze tekutiny tělesem vytlačené.",
                "Těleso se potápí, když je jeho hustota větší než hustota tekutiny.",
                "Těleso se vznáší, když jsou hustoty stejné.",
                NULL,
            },
        },
        {
            "3 / 3   •   Plyny", "Atmosféra a proudění",
            "Tip: normální atmosférický tlak je asi 101,3 kPa.",
            {
                "Atmosférický tlak u hladiny moře je přibližně 101,3 kPa.",
                "S výškou atmosférický tlak klesá.",
                "V užší části trubice proudí kapalina rychleji a tlak tam klesá.",
                "Na tom stojí vztlak křídla i rozprašovač.",
                NULL,
            },
        },
    };

    return fyz_unit_page(0, &fyz_lessons[7], "fyz_unit8", "fyz_unit8_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz_unit8_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak se počítá hydrostatický tlak?",
         {"p = h / ρ", "p = h·ρ·g", "p = F·S", "p = m·v"},
         4, 1},
        {"Co říká Archimédův zákon?",
         {"Vztlak se rovná tíze vytlačené tekutiny",
          "Tlak nezávisí na hloubce",
          "Kapaliny se nedají stlačit vůbec a nemají vztlak",
          "Vztlak působí dolů"},
         4, 0},
        {"Kdy se těleso v kapalině potápí?",
         {"Když má menší hustotu než kapalina",
          "Když má větší hustotu než kapalina",
          "Když má stejnou hustotu",
          "Když je hladina v klidu"},
         4, 1},
        {"Jak velký je přibližně normální atmosférický tlak?",
         {"1 Pa", "101 kPa", "9,81 Pa", "6,67·10⁻¹¹ Pa"},
         4, 1},
    };
    static const char *hints[] = {
        "Hustší kapalina a větší hloubka znamenají větší tlak.",
        "Vytlačená tekutina má objem ponořené části tělesa.",
        "Vznášení je při stejné hustotě. Plování je při menší průměrné hustotě.",
        "1013 hPa je totéž co asi 101,3 kPa.",
    };

    return fyz_mcq_page(0, 7, "fyzunit8", "fyz_ex8_title", "fyz_quiz8_head",
                        qs, hints, 4);
}

void add_fyz_pages(GtkStack *stack) {
    typedef GtkWidget *(*FyzBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        FyzBuilder build_unit;
        FyzBuilder build_ex;
    } pages[] = {
        {"fyzunit1", "fyzex1", build_fyz_unit1_page, build_fyz_unit1_exercise_page},
        {"fyzunit2", "fyzex2", build_fyz_unit2_page, build_fyz_unit2_exercise_page},
        {"fyzunit3", "fyzex3", build_fyz_unit3_page, build_fyz_unit3_exercise_page},
        {"fyzunit4", "fyzex4", build_fyz_unit4_page, build_fyz_unit4_exercise_page},
        {"fyzunit5", "fyzex5", build_fyz_unit5_page, build_fyz_unit5_exercise_page},
        {"fyzunit6", "fyzex6", build_fyz_unit6_page, build_fyz_unit6_exercise_page},
        {"fyzunit7", "fyzex7", build_fyz_unit7_page, build_fyz_unit7_exercise_page},
        {"fyzunit8", "fyzex8", build_fyz_unit8_page, build_fyz_unit8_exercise_page},
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
