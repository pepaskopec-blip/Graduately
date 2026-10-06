#include "graduately.h"

/* Year 3: electricity and magnetism. Charge, current, circuits,
 * matter in electric field, magnetic field, induction, alternating
 * current and electromagnetic waves. */

NetLesson fyz3_lessons[FYZ_N] = {
    { .n_slides = 3, .unit_page = "fyz3unit1", .ex_page = "fyz3ex1" },
    { .n_slides = 3, .unit_page = "fyz3unit2", .ex_page = "fyz3ex2" },
    { .n_slides = 3, .unit_page = "fyz3unit3", .ex_page = "fyz3ex3" },
    { .n_slides = 3, .unit_page = "fyz3unit4", .ex_page = "fyz3ex4" },
    { .n_slides = 3, .unit_page = "fyz3unit5", .ex_page = "fyz3ex5" },
    { .n_slides = 3, .unit_page = "fyz3unit6", .ex_page = "fyz3ex6" },
    { .n_slides = 3, .unit_page = "fyz3unit7", .ex_page = "fyz3ex7" },
    { .n_slides = 3, .unit_page = "fyz3unit8", .ex_page = "fyz3ex8" },
};

static GtkWidget *build_fyz3_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Náboj", "Dva druhy",
            "Tip: souhlasné náboje se odpuzují, nesouhlasné přitahují.",
            {
                "Elektrický náboj je kladný nebo záporný.",
                "Elementární náboj je e ≈ 1,6·10⁻¹⁹ C.",
                "Náboj se zachovává a je celistvým násobkem e.",
                "Třením se náboje rozdělují, nevznikají z ničeho.",
                NULL,
            },
        },
        {
            "2 / 3   •   Coulomb", "Síla mezi náboji",
            "Tip: síla klesá s druhou mocninou vzdálenosti.",
            {
                "Coulombův zákon: Fe = k·|Q₁·Q₂| / r².",
                "k je asi 9·10⁹ N·m²/C².",
                "Vodič má volné náboje, izolant je nemá.",
                "Elektrické pole je kolem každého náboje.",
                NULL,
            },
        },
        {
            "3 / 3   •   Pole", "Intenzita a napětí",
            "Tip: napětí je práce na jednotkový náboj.",
            {
                "Intenzita E = F / q ukazuje sílu na kladný jednotkový náboj.",
                "Homogenní pole má všude stejnou intenzitu, třeba mezi deskami.",
                "Napětí U = W / Q. Jednotka je volt.",
                "Ekvipotenciální hladina je množina bodů se stejným potenciálem.",
                NULL,
            },
        },
    };

    return fyz_unit_page(2, &fyz3_lessons[0], "fyz3_unit1", "fyz3_unit1_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz3_unit1_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak na sebe působí souhlasné náboje?",
         {"Přitahují se", "Odpuzují se", "Nepůsobí", "Jen při stejné hmotnosti"},
         4, 1},
        {"Jak velký je elementární náboj?",
         {"Asi 1,6·10⁻¹⁹ C", "Asi 9·10⁹ C", "1 C přesně jako proton", "Asi 6,67·10⁻¹¹ C"},
         4, 0},
        {"Jak závisí Coulombova síla na vzdálenosti?",
         {"Přímo úměrně", "Nepřímo úměrně druhé mocnině vzdálenosti",
          "Nezávisí na ní", "Přímo úměrně druhé mocnině"},
         4, 1},
        {"Co je elektrické napětí?",
         {"Síla na jednotkovou hmotnost",
          "Práce na jednotkový náboj",
          "Náboj za čas",
          "Odpor krát plocha"},
         4, 1},
    };
    static const char *hints[] = {
        "Nesouhlasné náboje se přitahují. Neutrální těleso má vyrovnané náboje.",
        "Proton má +e, elektron −e. Coulomb je velmi velký náboj.",
        "Stejný tvar má i gravitační zákon, jen konstanta je jiná.",
        "1 V = 1 J/C. Intenzita se měří v N/C nebo V/m.",
    };

    return fyz_mcq_page(2, 0, "fyz3unit1", "fyz3_ex1_title", "fyz3_quiz1_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz3_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Proud", "Náboj v pohybu",
            "Tip: dohodnutý směr proudu je směr pohybu kladného náboje.",
            {
                "Elektrický proud je usměrněný pohyb nabitých částic.",
                "Proud I = Q / t. Jednotka je ampér.",
                "V kovech proud nesou volné elektrony.",
                "Dohodnutý směr proudu je opačný než směr pohybu elektronů.",
                NULL,
            },
        },
        {
            "2 / 3   •   Ohm", "Odpor",
            "Tip: větší délka vodiče znamená větší odpor, větší průřez menší.",
            {
                "Ohmův zákon pro část obvodu: U = R·I.",
                "Odpor R závisí na materiálu, délce a průřezu.",
                "R = ρ·l / S. ρ je měrný elektrický odpor.",
                "S rostoucí teplotou odpor kovů obvykle roste.",
                NULL,
            },
        },
        {
            "3 / 3   •   Zdroj", "Svorkové napětí",
            "Tip: vnitřní odpor zdroje snižuje napětí při odběru proudu.",
            {
                "Zdroj má elektromotorické napětí a vnitřní odpor.",
                "Svorkové napětí je menší než elektromotorické, když obvodem teče proud.",
                "Zkrat je spojení s velmi malým odporem. Proud je nebezpečně velký.",
                "Ampérmetr se řadí sériově, voltmetr paralelně.",
                NULL,
            },
        },
    };

    return fyz_unit_page(2, &fyz3_lessons[1], "fyz3_unit2", "fyz3_unit2_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz3_unit2_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak se počítá proud?",
         {"I = Q·t", "I = Q / t", "I = U·R", "I = R / U"},
         4, 1},
        {"Jak zní Ohmův zákon pro část obvodu?",
         {"U = R / I", "U = R·I", "U = I / R", "R = U·I"},
         4, 1},
        {"Kdy má kovový vodič větší odpor?",
         {"Když je kratší a tlustší",
          "Když je delší a tenčí",
          "Když má větší průřez při stejné délce",
          "Když je z látky s nulovým měrným odporem"},
         4, 1},
        {"Jak se zapojuje ampérmetr?",
         {"Paralelně ke spotřebiči", "Sériově do obvodu",
          "Jen místo voltmetru paralelně", "Mimo obvod"},
         4, 1},
    };
    static const char *hints[] = {
        "Ampér je coulomb za sekundu.",
        "Odpor je U/I. Větší odpor při stejném napětí znamená menší proud.",
        "R = ρ·l / S. Měrný odpor ρ je vlastnost látky.",
        "Voltmetr má mít velký odpor a měří napětí vedle spotřebiče.",
    };

    return fyz_mcq_page(2, 1, "fyz3unit2", "fyz3_ex2_title", "fyz3_quiz2_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz3_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Spojení", "Sériově a paralelně",
            "Tip: sériově se odpory sčítají, proud je všude stejný.",
            {
                "Při sériovém spojení je proud stejný a napětí se sčítají.",
                "Výsledný odpor sériově je R = R₁ + R₂ + …",
                "Při paralelním spojení je napětí stejné a proudy se sčítají.",
                "Pro dva paralelní odpory je 1/R = 1/R₁ + 1/R₂.",
                NULL,
            },
        },
        {
            "2 / 3   •   Kirchhoff", "Uzly a smyčky",
            "Tip: v uzlu je součet přitékajících proudů roven součtu odtékajících.",
            {
                "První Kirchhoffův zákon je o proudu v uzlu.",
                "Druhý Kirchhoffův zákon: součet napětí ve smyčce je nulový.",
                "Zdroj ve smyčce dodává napětí, spotřebiče ho ubírají.",
                "Zákony stačí na jednoduché obvody se dvěma smyčkami.",
                NULL,
            },
        },
        {
            "3 / 3   •   Výkon", "Práce proudu",
            "Tip: příkon je P = U·I.",
            {
                "Práce elektrického proudu je W = U·I·t.",
                "Příkon je P = U·I = R·I² = U² / R.",
                "Kilowatthodina je jednotka energie, ne výkonu.",
                "Jouleovo teplo ve vodiči je Q = R·I²·t.",
                NULL,
            },
        },
    };

    return fyz_unit_page(2, &fyz3_lessons[2], "fyz3_unit3", "fyz3_unit3_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz3_unit3_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jaký je výsledný odpor dvou sériově spojených rezistorů?",
         {"R = R₁ + R₂", "1/R = 1/R₁ + 1/R₂", "R = R₁·R₂", "R = R₁ / R₂"},
         4, 0},
        {"Co je stejné na paralelně spojených spotřebičích?",
         {"Proud každým z nich", "Napětí",
          "Odpor každého z nich", "Jouleovo teplo bez ohledu na odpor"},
         4, 1},
        {"Co říká první Kirchhoffův zákon?",
         {"Součet napětí ve smyčce je nula",
          "Součet proudů v uzlu je nula",
          "Odpor roste s teplotou",
          "Výkon je U děleno I"},
         4, 1},
        {"Jak se počítá elektrická práce?",
         {"W = F·s", "W = U·I·t", "W = R / I", "W = Q·c·Δt"},
         4, 1},
    };
    static const char *hints[] = {
        "Paralelní spojení výsledný odpor zmenší. Dva stejné paralelně dají polovinu.",
        "Větší odpor při stejném napětí bere menší proud.",
        "Druhý zákon se týká napětí ve smyčce.",
        "1 kWh = 3,6·10⁶ J. Příkon ve wattech je U·I.",
    };

    return fyz_mcq_page(2, 2, "fyz3unit3", "fyz3_ex3_title", "fyz3_quiz3_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz3_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Polovodič", "Vlastní a příměsová vodivost",
            "Tip: dioda vede proud hlavně v propustném směru.",
            {
                "Polovodič má vodivost mezi vodičem a izolantem.",
                "S teplotou jeho odpor obvykle klesá, na rozdíl od kovu.",
                "Typ N má příměs s volnými elektrony, typ P s dírami.",
                "Přechod PN je základ diody. V závěrném směru téměř nevede.",
                NULL,
            },
        },
        {
            "2 / 3   •   Kapaliny", "Elektrolyt",
            "Tip: v elektrolytu nesou proud ionty, ne volné elektrony jako v kovu.",
            {
                "Elektrolyt je roztok nebo tavenina, která vede proud ionty.",
                "Kationty putují ke katodě, anionty k anodě.",
                "Elektrolýza vylučuje látku na elektrodách.",
                "Hmotnost vyloučené látky je úměrná náboji, který prošel.",
                NULL,
            },
        },
        {
            "3 / 3   •   Plyny", "Výboj",
            "Tip: plyn za běžných podmínek nevede, dokud se neionizuje.",
            {
                "Samostatný výboj udržuje ionizaci sám, nesamostatný potřebuje vnější ionizátor.",
                "Jiskra, oblouk a doutnavý výboj jsou druhy výboje.",
                "Blesk je jiskrový výboj v atmosféře.",
                "Výbojka a zářivka využívají záření výboje v plynu.",
                NULL,
            },
        },
    };

    return fyz_unit_page(2, &fyz3_lessons[3], "fyz3_unit4", "fyz3_unit4_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz3_unit4_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak se s teplotou obvykle mění odpor polovodiče?",
         {"Roste jako u kovu", "Klesá", "Nemění se", "Skokem se stane nulovým vždy"},
         4, 1},
        {"Co je polovodič typu N?",
         {"Má převahu děr", "Má převahu volných elektronů z příměsi",
          "Je izolant bez náboje", "Vede jen ionty"},
         4, 1},
        {"Čím je nesen proud v elektrolytu?",
         {"Jen volnými elektrony jako v mědi", "Ionty",
          "Fotony", "Neutrony"},
         4, 1},
        {"Kdy plyn začne výrazně vést proud?",
         {"Až když je ionizovaný", "Vždy stejně jako kov",
          "Jen pod absolutní nulou", "Jen když je to vakuum bez částic"},
         4, 0},
    };
    static const char *hints[] = {
        "Termistor s klesajícím odporem při zahřátí je typické užití.",
        "Typ P má díry. Dioda je jeden přechod PN.",
        "Katoda přitahuje kationty. Faradayův zákon váže hmotnost a náboj.",
        "Nesamostatný výboj zanikne, když zmizí vnější ionizátor.",
    };

    return fyz_mcq_page(2, 3, "fyz3unit4", "fyz3_ex4_title", "fyz3_quiz4_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz3_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Magnet", "Pole magnetu",
            "Tip: souhlasné póly se odpuzují.",
            {
                "Souhlasné magnetické póly se odpuzují, nesouhlasné přitahují.",
                "Severní pól střelky ukazuje k jižnímu magnetickému pólu Země.",
                "Magnetické indukční čáry jsou uzavřené.",
                "Zemské pole odklání část nabitých částic z vesmíru.",
                NULL,
            },
        },
        {
            "2 / 3   •   Proud", "Magnetické pole vodiče",
            "Tip: cívka s proudem se chová jako magnet.",
            {
                "Kolem přímého vodiče s proudem jsou indukční čáry kružnice.",
                "Cívka, solenoid, má uvnitř téměř homogenní pole.",
                "Na vodič s proudem v magnetickém poli působí síla F = B·I·l·sin α.",
                "B je magnetická indukce. Jednotka je tesla, T.",
                NULL,
            },
        },
        {
            "3 / 3   •   Užití", "Motor a měřidlo",
            "Tip: elektromotor mění elektrickou energii na pohybovou.",
            {
                "Stejnosměrný motor využívá sílu na závit s proudem v magnetickém poli.",
                "Komutátor mění směr proudu v závitu, aby se točil dál.",
                "Částice s nábojem v magnetickém poli zatáčí, pokud má složku rychlosti kolmou k poli.",
                "Magnetický záznam a elektromagnet jsou běžné užití pole.",
                NULL,
            },
        },
    };

    return fyz_unit_page(2, &fyz3_lessons[4], "fyz3_unit5", "fyz3_unit5_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz3_unit5_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jaký tvar mají magnetické indukční čáry?",
         {"Začínají na severním pólu a končí v nekonečnu jako přímky",
          "Jsou uzavřené",
          "Jsou vždy rovnoběžné s vodičem",
          "Existují jen ve vakuu"},
         4, 1},
        {"Jak se počítá síla na přímý vodič s proudem v magnetickém poli?",
         {"F = B·I·l·sin α", "F = k·Q₁·Q₂ / r²", "F = m·g", "F = U / R"},
         4, 0},
        {"Jaká je jednotka magnetické indukce?",
         {"Volt", "Tesla", "Henry jen jako indukce B", "Ampér"},
         4, 1},
        {"K čemu je v stejnosměrném motoru komutátor?",
         {"Aby zvýšil odpor na nekonečno",
          "Aby měnil směr proudu v závitu a motor se točil dál",
          "Aby zrušil magnetické pole",
          "Aby měřil napětí"},
         4, 1},
    };
    static const char *hints[] = {
        "Elektrické siločáry začínají a končí na nábojích. Magnetické jsou uzavřené.",
        "Když je vodič rovnoběžný s polem, sin α je 0 a síla je nulová.",
        "Tesla je N/(A·m). Henry je jednotka indukčnosti.",
        "Bez komutátoru by se závit po půl otáčce zastavil.",
    };

    return fyz_mcq_page(2, 4, "fyz3unit5", "fyz3_ex5_title", "fyz3_quiz5_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz3_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Faraday", "Indukované napětí",
            "Tip: napětí vzniká, jen když se magnetický tok mění.",
            {
                "Elektromagnetická indukce vzniká při změně magnetického toku plochou smyčky.",
                "Magnetický tok je Φ = B·S·cos α.",
                "Indukované napětí je tím větší, čím rychleji se tok mění.",
                "Stálé pole a klidná smyčka napětí neindukují.",
                NULL,
            },
        },
        {
            "2 / 3   •   Lenz", "Směr proudu",
            "Tip: indukovaný proud působí proti změně, která ho vyvolala.",
            {
                "Lenzův zákon určuje směr indukovaného proudu.",
                "Proud vytváří pole, které brání změně toku.",
                "Proto se magnet padající měděnou trubkou brzdí.",
                "Vlastní indukce brání změně proudu v cívce.",
                NULL,
            },
        },
        {
            "3 / 3   •   Transformátor", "Změna napětí",
            "Tip: poměr napětí je poměr počtů závitů.",
            {
                "Transformátor má dvě cívky na společném jádře.",
                "U₁ / U₂ = N₁ / N₂.",
                "Zvyšovací transformátor má na sekundáru víc závitů.",
                "U ideálního transformátoru je příkon roven výkonu, proud se mění opačně než napětí.",
                NULL,
            },
        },
    };

    return fyz_unit_page(2, &fyz3_lessons[5], "fyz3_unit6", "fyz3_unit6_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz3_unit6_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Kdy se ve smyčce indukuje napětí?",
         {"Když je magnetický tok stálý",
          "Když se magnetický tok plochou smyčky mění",
          "Když smyčkou neteče žádný náboj a tok je nulový trvale",
          "Jen při stejnosměrném proudu v klidném poli"},
         4, 1},
        {"Co říká Lenzův zákon?",
         {"Indukovaný proud podporuje změnu toku",
          "Indukovaný proud působí proti změně, která ho vyvolala",
          "Napětí je vždy rovno proudu",
          "Tok je B děleno S"},
         4, 1},
        {"Jaký je poměr napětí na ideálním transformátoru?",
         {"U₁ / U₂ = N₂ / N₁", "U₁ / U₂ = N₁ / N₂",
          "U₁ / U₂ = I₁ / I₂ vždy při stejném počtu závitů jen",
          "Napětí nezávisí na závitech"},
         4, 1},
        {"Co platí pro ideální transformátor?",
         {"Příkon je roven výkonu",
          "Sekundární proud je vždy nulový",
          "Funguje i se stejnosměrným stálým proudem",
          "Napětí na obou cívkách je stejné bez ohledu na závity"},
         4, 0},
    };
    static const char *hints[] = {
        "Pohyb magnetu u cívky, nebo změna proudu v primáru, tok mění.",
        "Zákon zachování energie: indukce nemůže změnu sama zesilovat donekonečna.",
        "Více závitů na sekundáru znamená vyšší sekundární napětí.",
        "Stejnosměrný stálý proud tok nemění, proto se netransformuje.",
    };

    return fyz_mcq_page(2, 5, "fyz3unit6", "fyz3_ex6_title", "fyz3_quiz6_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz3_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Střídání", "Perioda a frekvence",
            "Tip: síť v Česku má frekvenci 50 Hz.",
            {
                "Střídavý proud periodicky mění velikost i směr.",
                "Harmonické napětí popisuje sinus.",
                "Frekvence sítě je 50 Hz, perioda je 0,02 s.",
                "Okamžitá hodnota se stále mění. Měří se efektivní hodnota.",
                NULL,
            },
        },
        {
            "2 / 3   •   Efektivní", "Co ukáže voltmetr",
            "Tip: efektivní hodnota je maximální dělená odmocninou ze dvou.",
            {
                "Efektivní proud vydá na rezistoru stejný výkon jako stejný stejnosměrný proud.",
                "Pro sinusový průběh je Uef = Umax / √2.",
                "Síťové napětí 230 V je efektivní hodnota.",
                "Amplituda je tedy asi 325 V.",
                NULL,
            },
        },
        {
            "3 / 3   •   Obvod", "Odpor, cívka, kondenzátor",
            "Tip: na rezistoru jsou napětí a proud ve fázi.",
            {
                "Rezistor proud a napětí neposouvá.",
                "Cívka zpožďuje proud za napětím, kondenzátor ho předbíhá.",
                "Indukčnost a kapacita závisejí na frekvenci.",
                "Výkon na rezistoru je P = Uef·Ief.",
                NULL,
            },
        },
    };

    return fyz_unit_page(2, &fyz3_lessons[6], "fyz3_unit7", "fyz3_unit7_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz3_unit7_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jaká je frekvence elektrické sítě v Česku?",
         {"50 Hz", "230 Hz", "1 Hz", "60 kHz"},
         4, 0},
        {"Co znamená síťové napětí 230 V?",
         {"Maximální hodnotu", "Efektivní hodnotu",
          "Napětí za jednu periodu jako součet", "Vnitřní odpor"},
         4, 1},
        {"Jak spolu souvisí efektivní a maximální hodnota u sinusového napětí?",
         {"Uef = Umax·√2", "Uef = Umax / √2", "Uef = Umax", "Uef = Umax / 2"},
         4, 1},
        {"Jak je to s fází na rezistoru?",
         {"Proud se zpožďuje o čtvrt periodu",
          "Napětí a proud jsou ve fázi",
          "Proud předbíhá o půl periodu",
          "Fáze závisí jen na barvě rezistoru"},
         4, 1},
    };
    static const char *hints[] = {
        "Perioda 50 Hz je 20 ms. V USA je síť 60 Hz.",
        "Maximální napětí sítě je 230·√2, zhruba 325 V.",
        "√2 je asi 1,414. Efektivní hodnota je menší než amplituda.",
        "Fázový posun dělá cívka a kondenzátor, ne samotný rezistor.",
    };

    return fyz_mcq_page(2, 6, "fyz3unit7", "fyz3_ex7_title", "fyz3_quiz7_head",
                        qs, hints, 4);
}

static GtkWidget *build_fyz3_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Vznik", "Elektromagnetická vlna",
            "Tip: ve vakuu se vlna šíří rychlostí světla.",
            {
                "Kmitající náboj vyzařuje elektromagnetickou vlnu.",
                "Elektrická a magnetická složka jsou navzájem kolmé a kolmé ke směru šíření.",
                "Ve vakuu je rychlost c = 3·10⁸ m/s.",
                "Vlna přenáší energii a nepotřebuje látkové prostředí.",
                NULL,
            },
        },
        {
            "2 / 3   •   Spektrum", "Od rádia po záření gama",
            "Tip: viditelné světlo je jen úzká část spektra.",
            {
                "Spektrum tvoří rádiové vlny, mikrovlny, infračervené záření, světlo, ultrafialové, rentgenové a gama.",
                "Větší frekvence znamená kratší vlnovou délku, protože c = λ·f.",
                "Viditelné světlo je zhruba 400 až 750 nm.",
                "Za fialovou je ultrafialové záření, před červenou infračervené.",
                NULL,
            },
        },
        {
            "3 / 3   •   Užití", "Spoje a ohřev",
            "Tip: mikrovlny rozkmitávají molekuly vody.",
            {
                "Rádiové vlny přenášejí rozhlas, televizi i mobilní signál.",
                "Mikrovlny se používají v radarové technice a v troubě.",
                "Rentgenové záření prochází měkkými tkáněmi víc než kostmi.",
                "Všechny tyto vlny jsou totéž elektromagnetické vlnění, liší se frekvencí.",
                NULL,
            },
        },
    };

    return fyz_unit_page(2, &fyz3_lessons[7], "fyz3_unit8", "fyz3_unit8_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_fyz3_unit8_exercise_page(void) {
    static const ChoiceQ qs[] = {
        {"Jak rychle se šíří elektromagnetická vlna ve vakuu?",
         {"Asi 340 m/s", "Asi 3·10⁸ m/s", "Asi 9,81 m/s", "Závisí na hlasitosti"},
         4, 1},
        {"Co platí pro vlnovou délku a frekvenci ve vakuu?",
         {"c = λ / f", "c = λ·f", "c = f / λ", "λ nezávisí na f"},
         4, 1},
        {"Kde ve spektru leží viditelné světlo?",
         {"Mezi rádiovými vlnami a mikrovlnami",
          "Mezi infračerveným a ultrafialovým zářením",
          "Za zářením gama",
          "Jen jako zvuk"},
         4, 1},
        {"Čím se od sebe liší rádiová vlna a rentgenové záření?",
         {"Rentgen není elektromagnetická vlna",
          "Mají jinou frekvenci",
          "Rentgen potřebuje vzduch, rádio vakuum",
          "Rádiová vlna má vždy vyšší frekvenci"},
         4, 1},
    };
    static const char *hints[] = {
        "340 m/s je zvuk. Světlo je skoro milionkrát rychlejší.",
        "Stejný vztah jako u mechanické vlny, rychlost je c.",
        "Fialová má kratší vlnovou délku než červená.",
        "Gama a rentgen mají vysokou frekvenci a krátkou vlnovou délku.",
    };

    return fyz_mcq_page(2, 7, "fyz3unit8", "fyz3_ex8_title", "fyz3_quiz8_head",
                        qs, hints, 4);
}

void add_fyz3_pages(GtkStack *stack) {
    typedef GtkWidget *(*FyzBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        FyzBuilder build_unit;
        FyzBuilder build_ex;
    } pages[] = {
        {"fyz3unit1", "fyz3ex1", build_fyz3_unit1_page, build_fyz3_unit1_exercise_page},
        {"fyz3unit2", "fyz3ex2", build_fyz3_unit2_page, build_fyz3_unit2_exercise_page},
        {"fyz3unit3", "fyz3ex3", build_fyz3_unit3_page, build_fyz3_unit3_exercise_page},
        {"fyz3unit4", "fyz3ex4", build_fyz3_unit4_page, build_fyz3_unit4_exercise_page},
        {"fyz3unit5", "fyz3ex5", build_fyz3_unit5_page, build_fyz3_unit5_exercise_page},
        {"fyz3unit6", "fyz3ex6", build_fyz3_unit6_page, build_fyz3_unit6_exercise_page},
        {"fyz3unit7", "fyz3ex7", build_fyz3_unit7_page, build_fyz3_unit7_exercise_page},
        {"fyz3unit8", "fyz3ex8", build_fyz3_unit8_page, build_fyz3_unit8_exercise_page},
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
