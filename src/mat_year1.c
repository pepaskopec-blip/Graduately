#include "graduately.h"

NetLesson mat_lessons[MATH_N] = {
    { .n_slides = 3, .unit_page = "matunit1", .ex_page = "matex1" },
    { .n_slides = 3, .unit_page = "matunit2", .ex_page = "matex2" },
    { .n_slides = 3, .unit_page = "matunit3", .ex_page = "matex3" },
    { .n_slides = 3, .unit_page = "matunit4", .ex_page = "matex4" },
    { .n_slides = 3, .unit_page = "matunit5", .ex_page = "matex5" },
    { .n_slides = 3, .unit_page = "matunit6", .ex_page = "matex6" },
    { .n_slides = 3, .unit_page = "matunit7", .ex_page = "matex7" },
    { .n_slides = 3, .unit_page = "matunit8", .ex_page = "matex8" },
};

static GtkWidget *build_mat_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Dělitel", "Čísla, která dělí beze zbytku",
            "Tip: prvočíslo má právě dva dělitele, jedničku a sebe.",
            {
                "Dělitel čísla a je celé číslo, kterým a dělí beze zbytku.",
                "Číslo 12 má dělitele 1, 2, 3, 4, 6 a 12, tedy šest dělitelů.",
                "Prvočíslo nelze rozložit na součin dvou menších celých čísel větších než 1.",
                "Jednička prvočíslo není.",
                NULL,
            },
        },
        {
            "2 / 3   •   NSD a NSN", "Společný dělitel a násobek",
            "Tip: NSD · NSN dvou čísel je jejich součin.",
            {
                "Největší společný dělitel je největší číslo, které dělí obě čísla.",
                "NSD(24, 36) = 12.",
                "Nejmenší společný násobek je nejmenší kladné číslo, které je násobkem obou.",
                "NSN(6, 8) = 24.",
                NULL,
            },
        },
        {
            "3 / 3   •   Rozklad", "Součin mocnin prvočísel",
            "Tip: společné prvočinitele berte v nejmenší mocnině pro NSD.",
            {
                "Každé celé číslo větší než 1 má jediný rozklad na prvočísla.",
                "72 = 2³ · 3².",
                "V NSD se bere nižší exponent, v NSN vyšší.",
                "Rozklad usnadní krácení zlomků i hledání společného jmenovatele.",
                NULL,
            },
        },
    };

    return math_unit_page(1, &mat_lessons[0], "mat_unit1", "mat_unit1_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat_unit1_exercise_page(void) {
    static const MathItem qs[] = {
        {"Jaký je největší společný dělitel čísel 24 a 36?", "num:12", "Společní dělitelé jsou 1, 2, 3, 4, 6 a 12. Výsledek je 12."},
        {"Jaký je nejmenší společný násobek čísel 6 a 8?", "num:24", "Násobky 6 jsou 6, 12, 18, 24, … a 24 je první z nich dělitelné osmi. Výsledek je 24."},
        {"Spočítejte 2³ · 3².", "num:72", "8 · 9 = 72. Výsledek je 72."},
        {"Kolik kladných dělitelů má číslo 12?", "num:6", "Jsou to 1, 2, 3, 4, 6 a 12. Výsledek je 6."},
    };

    return math_practice_page(1, 0, "matunit1",
                              "mat_ex1_title", "mat_quiz1_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Mocnina", "Opakované násobení",
            "Tip: aⁿ je a násobené sebou n-krát.",
            {
                "2⁵ = 2 · 2 · 2 · 2 · 2 = 32.",
                "10³ = 1000, tři nuly za jedničkou.",
                "Každé číslo na první je ono samo a a⁰ = 1 pro a ≠ 0.",
                "Záporný exponent znamená převrácenou hodnotu mocniny.",
                NULL,
            },
        },
        {
            "2 / 3   •   Pravidla", "Stejný základ",
            "Tip: (aᵐ)ⁿ = aᵐ·ⁿ.",
            {
                "aᵐ · aⁿ = aᵐ⁺ⁿ.",
                "aᵐ : aⁿ = aᵐ⁻ⁿ pro a ≠ 0.",
                "(a · b)ⁿ = aⁿ · bⁿ.",
                "(2³)² = 2⁶ = 64.",
                NULL,
            },
        },
        {
            "3 / 3   •   Odmocnina", "Druhá odmocnina",
            "Tip: √a je nezáporné číslo, jehož druhá mocnina je a.",
            {
                "√81 = 9, protože 9² = 81.",
                "√0 = 0 a √1 = 1.",
                "Ze záporného čísla se v reálných číslech druhá odmocnina nedělá.",
                "√(a²) = |a|, ne vždy a.",
                NULL,
            },
        },
    };

    return math_unit_page(1, &mat_lessons[1], "mat_unit2", "mat_unit2_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat_unit2_exercise_page(void) {
    static const MathItem qs[] = {
        {"Spočítejte 2⁵.", "num:32", "Pět dvojkových činitelů je 32. Výsledek je 32."},
        {"Spočítejte 10³.", "num:1000", "10 · 10 · 10 = 1000. Výsledek je 1000."},
        {"Spočítejte √81.", "num:9", "9 · 9 = 81. Výsledek je 9."},
        {"Spočítejte (2³)².", "num:64", "(8)² = 64, nebo 2⁶ = 64. Výsledek je 64."},
    };

    return math_practice_page(1, 1, "matunit2",
                              "mat_ex2_title", "mat_quiz2_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Dosazení", "Písmeno je číslo",
            "Tip: nejdřív závorky, potom mocniny a násobení.",
            {
                "Do výrazu se za proměnnou dosadí dané číslo.",
                "2(x + 3) při x = 4 je 2 · 7 = 14.",
                "3x − 2x + 5 se nejdřív sloučí na x + 5.",
                "Při x = 2 je x + 5 = 7.",
                NULL,
            },
        },
        {
            "2 / 3   •   Vzorce", "Druhá mocnina dvojčlenu",
            "Tip: (a + b)² není a² + b².",
            {
                "(a + b)² = a² + 2ab + b².",
                "(a − b)² = a² − 2ab + b².",
                "a² − b² = (a − b)(a + b).",
                "Pro a = 3 a b = 1 je (a + b)² = 16.",
                NULL,
            },
        },
        {
            "3 / 3   •   Zkrácení", "Rozdíl čtverců",
            "Tip: krátit lze jen pro x, která nejsou kořenem jmenovatele.",
            {
                "x² − 9 = (x − 3)(x + 3).",
                "(x² − 9)/(x − 3) = x + 3 pro x ≠ 3.",
                "Při x = 5 je hodnota 8.",
                "Dosazení x = 3 by dělilo nulou, výraz tam není definován.",
                NULL,
            },
        },
    };

    return math_unit_page(1, &mat_lessons[2], "mat_unit3", "mat_unit3_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat_unit3_exercise_page(void) {
    static const MathItem qs[] = {
        {"Do výrazu 2(x + 3) dosažte x = 4.", "num:14", "2 · (4 + 3) = 14. Výsledek je 14."},
        {"Do výrazu (a + b)² dosažte a = 3 a b = 1.", "num:16", "4² = 16. Rozepsáno 9 + 6 + 1 = 16. Výsledek je 16."},
        {"Do výrazu 3x − 2x + 5 dosažte x = 2.", "num:7", "Výraz je x + 5, při x = 2 je 7. Výsledek je 7."},
        {"Do výrazu (x² − 9)/(x − 3) dosažte x = 5.", "num:8", "Pro x ≠ 3 je výraz roven x + 3, tedy 8. Výsledek je 8."},
    };

    return math_practice_page(1, 2, "matunit3",
                              "mat_ex3_title", "mat_quiz3_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Převod", "Neznámé na jednu stranu",
            "Tip: člen s x se převádí s opačným znaménkem.",
            {
                "2x + 1 = 9 se odečtením jedné změní na 2x = 8.",
                "x = 4.",
                "Členy s x patří obvykle vlevo, čísla vpravo.",
                "Po každé úpravě zůstává rovnost zachovaná.",
                NULL,
            },
        },
        {
            "2 / 3   •   Obě strany", "Stejný člen vlevo i vpravo",
            "Tip: 5x − 3 = 2x + 9 se zbaví 2x odečtením.",
            {
                "5x − 2x = 9 + 3.",
                "3x = 12 a x = 4.",
                "Zkouška: levá strana 5 · 4 − 3 = 17, pravá 8 + 9 = 17.",
                "Když koeficient u x vyjde 0, rovnice buď nemá řešení, nebo jich má nekonečně mnoho.",
                NULL,
            },
        },
        {
            "3 / 3   •   Závorka a zlomek", "Nejdřív roznásobit",
            "Tip: x/2 + 1 = 5 se nejdřív zbaví jedničky.",
            {
                "x/2 + 1 = 5 dá x/2 = 4, tedy x = 8.",
                "3(x − 2) = 12 se vydělí třemi: x − 2 = 4.",
                "Nebo se závorka roznásobí: 3x − 6 = 12.",
                "Obě cesty dají x = 6.",
                NULL,
            },
        },
    };

    return math_unit_page(1, &mat_lessons[3], "mat_unit4", "mat_unit4_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat_unit4_exercise_page(void) {
    static const MathItem qs[] = {
        {"Vyřešte 2x + 1 = 9. Napište x.", "num:4", "2x = 8, takže x = 4. Výsledek je 4."},
        {"Vyřešte 5x − 3 = 2x + 9. Napište x.", "num:4", "3x = 12, takže x = 4. Výsledek je 4."},
        {"Vyřešte x/2 + 1 = 5. Napište x.", "num:8", "x/2 = 4, takže x = 8. Výsledek je 8."},
        {"Vyřešte 3(x − 2) = 12. Napište x.", "num:6", "x − 2 = 4, takže x = 6. Výsledek je 6."},
    };

    return math_practice_page(1, 3, "matunit4",
                              "mat_ex4_title", "mat_quiz4_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Dráha", "Rychlost krát čas",
            "Tip: jednotky času a rychlosti musí patřit k sobě.",
            {
                "Při stálé rychlosti je dráha s = v · t.",
                "60 km/h po dobu 2,5 h urazí 150 km.",
                "2,5 h je dvě a půl hodiny, ne dvě hodiny a padesát minut.",
                "Z dráhy a času se rychlost dopočítá jako s/t.",
                NULL,
            },
        },
        {
            "2 / 3   •   Práce a směs", "Sčítají se výkony, ne časy",
            "Tip: kdo práci udělá za 3 hodiny, udělá za hodinu třetinu.",
            {
                "První pracovník udělá za hodinu 1/6 práce, druhý 1/3.",
                "Společně udělají 1/6 + 2/6 = 1/2 práce za hodinu, tedy celou za 2 hodiny.",
                "U směsi se sčítá množství čisté látky.",
                "2 kg dvacetiprocentní a 3 kg desetiprocentní směsi obsahují 0,4 + 0,3 = 0,7 kg, tedy 14 %.",
                NULL,
            },
        },
        {
            "3 / 3   •   Sleva", "Nová cena je část základu",
            "Tip: po slevě 20 % zbývá 80 % původní ceny.",
            {
                "Když je nová cena 160 a sleva byla 20 %, je 160 rovno 80 % původní ceny.",
                "Původní cena je 160 / 0,8 = 200.",
                "Sleva v korunách je pak 40.",
                "Nepleťte si slevu 20 % s tím, že se k nové ceně přičte 20 %.",
                NULL,
            },
        },
    };

    return math_unit_page(1, &mat_lessons[4], "mat_unit5", "mat_unit5_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat_unit5_exercise_page(void) {
    static const MathItem qs[] = {
        {"Auto jede 2,5 hodiny rychlostí 60 km/h. Kolik kilometrů ujede?", "num:150", "s = v · t = 60 · 2,5 = 150. Výsledek je 150."},
        {"První pracovník práci udělá za 6 hodin, druhý za 3 hodiny. Za kolik hodin ji udělají společně?", "num:2", "Za hodinu spolu udělají 1/2 práce, celou tedy za 2 hodiny. Výsledek je 2."},
        {"Smíchají se 2 kg směsi s 20 % látky a 3 kg směsi s 10 % látky. Kolik procent látky má směs? Napište jen číslo.", "num:14", "Čisté látky je 0,7 kg z 5 kg, tedy 14 %. Výsledek je 14."},
        {"Po slevě 20 % stojí zboží 160. Jaká byla původní cena?", "num:200", "160 je 80 % původní ceny, základ je 200. Výsledek je 200."},
    };

    return math_practice_page(1, 4, "matunit5",
                              "mat_ex5_title", "mat_quiz5_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Trojúhelník", "Součet 180°",
            "Tip: rovnoramenný trojúhelník má dvě ramena a dva úhly u základny shodné.",
            {
                "Součet vnitřních úhlů každého trojúhelníku je 180°.",
                "V rovnoramenném trojúhelníku jsou úhly při základně shodné.",
                "Když jsou úhly při základně 70°, je úhel proti základně 40°.",
                "Rovnostranný trojúhelník má všechny strany shodné a každý úhel 60°.",
                NULL,
            },
        },
        {
            "2 / 3   •   Vnější úhel", "Součet dvou vzdálených vnitřních",
            "Tip: vnější úhel a sousední vnitřní úhel dávají 180°.",
            {
                "Vnější úhel vznikne prodloužením jedné strany.",
                "Je roven součtu dvou vnitřních úhlů, které s ním nesousedí.",
                "Vnitřní úhly 40° a 60° dávají vnější úhel 100° u zbývajícího vrcholu.",
                "Součet vnějšího úhlu a přilehlého vnitřního je 180°.",
                NULL,
            },
        },
        {
            "3 / 3   •   Mnohoúhelník", "Pravidelný šestiúhelník",
            "Tip: součet vnitřních úhlů n-úhelníku je (n − 2) · 180°.",
            {
                "Trojúhelník má součet 180°, čtyřúhelník 360°.",
                "Obecný vzorec je (n − 2) · 180°.",
                "Pravidelný mnohoúhelník má všechny strany i úhly shodné.",
                "Pravidelný šestiúhelník má vnitřní úhel 720° / 6 = 120°.",
                NULL,
            },
        },
    };

    return math_unit_page(1, &mat_lessons[5], "mat_unit6", "mat_unit6_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat_unit6_exercise_page(void) {
    static const MathItem qs[] = {
        {"Vnější úhel trojúhelníku je součet vnitřních úhlů 40° a 60°. Jak je velký ve stupních?", "num:100", "Vnější úhel se rovná součtu dvou nesousedních vnitřních úhlů: 100°. Výsledek je 100."},
        {"Rovnoramenný trojúhelník má úhly při základně 70°. Jak velký je úhel proti základně?", "num:40", "180° − 140° = 40°. Výsledek je 40."},
        {"Rovnostranný trojúhelník má stranu 6. Jaký je jeho obvod?", "num:18", "Tři shodné strany: 3 · 6 = 18. Výsledek je 18."},
        {"Jak velký je jeden vnitřní úhel pravidelného šestiúhelníku ve stupních?", "num:120", "Součet je (6 − 2) · 180° = 720° a jeden úhel je 120°. Výsledek je 120."},
    };

    return math_practice_page(1, 5, "matunit6",
                              "mat_ex6_title", "mat_quiz6_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Kružnice", "Obvod a obsah",
            "Tip: u výsledku s π nechte π ve výrazu a pište ho jako pi.",
            {
                "Kružnice s poloměrem r má obvod 2πr.",
                "Obsah kruhu je πr².",
                "Pro r = 7 je obvod 14π.",
                "Pro r = 3 je obsah 9π.",
                NULL,
            },
        },
        {
            "2 / 3   •   Lichoběžník", "Dvě základny a výška",
            "Tip: obsah je průměr základen krát výška.",
            {
                "Lichoběžník má právě jednu dvojici rovnoběžných stran, základny a a c.",
                "Obsah je (a + c) · v / 2.",
                "Základny 4 a 6 a výška 3 dávají obsah 15.",
                "Výška je kolmá vzdálenost základen.",
                NULL,
            },
        },
        {
            "3 / 3   •   Pythagoras", "Pravoúhlý trojúhelník",
            "Tip: přepona je nejdelší strana a leží proti pravému úhlu.",
            {
                "V pravoúhlém trojúhelníku je a² + b² = c².",
                "c je přepona, a a b jsou odvěsny.",
                "Trojúhelník 5, 12, 13 je pravoúhlý, protože 25 + 144 = 169.",
                "Přepona je 13.",
                NULL,
            },
        },
    };

    return math_unit_page(1, &mat_lessons[6], "mat_unit7", "mat_unit7_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat_unit7_exercise_page(void) {
    static const MathItem qs[] = {
        {"Jaký je obvod kružnice s poloměrem 7? Nechte výsledek v násobcích π.", "num:14*pi", "Obvod je 2πr = 14π. Výsledek je 14π."},
        {"Jaký je obsah kruhu s poloměrem 3? Nechte výsledek v násobcích π.", "num:9*pi", "Obsah je πr² = 9π. Výsledek je 9π."},
        {"Lichoběžník má základny 4 a 6 a výšku 3. Jaký je jeho obsah?", "num:15", "(4 + 6) · 3 / 2 = 15. Výsledek je 15."},
        {"Pravoúhlý trojúhelník má odvěsny 5 a 12. Jak dlouhá je přepona?", "num:13", "5² + 12² = 25 + 144 = 169 = 13². Výsledek je 13."},
    };

    return math_practice_page(1, 6, "matunit7",
                              "mat_ex7_title", "mat_quiz7_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Body", "Vzdálenost na mřížce",
            "Tip: u svislé nebo vodorovné úsečky stačí rozdíl souřadnic.",
            {
                "Bod v rovině má souřadnice [x; y].",
                "Vzdálenost [x₁; y₁] a [x₂; y₂] je √((x₂ − x₁)² + (y₂ − y₁)²).",
                "A = [1; 1] a B = [5; 1] mají vzdálenost 4.",
                "Osa y na naší mřížce míří nahoru.",
                NULL,
            },
        },
        {
            "2 / 3   •   SSS", "Trojúhelník ze tří stran",
            "Tip: třetí vrchol je průsečík dvou kružnic.",
            {
                "Konstrukce SSS použije tři strany.",
                "Z bodu A se rýsuje kružnice o poloměru |AC|, z bodu B o poloměru |BC|.",
                "Kružnice se protnou ve dvou bodech, pokud trojúhelník existuje.",
                "Trojúhelníková nerovnost: součet dvou stran je větší než třetí.",
                NULL,
            },
        },
        {
            "3 / 3   •   Příklad", "Strany 4, 3 a 5",
            "Tip: 3² + 4² = 5², pravý úhel je u bodu A.",
            {
                "A = [1; 1], B = [5; 1], takže |AB| = 4.",
                "Hledáme C tak, aby |AC| = 3 a |BC| = 5.",
                "Na mřížce od 0 do 10 a od 0 do 8 leží jen C = [1; 4].",
                "Druhý průsečík [1; −2] je pod mřížkou.",
                NULL,
            },
        },
    };

    return math_unit_page(1, &mat_lessons[7], "mat_unit8", "mat_unit8_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat_unit8_exercise_page(void) {
    static const MathItem qs[] = {
        {"Jaká je vzdálenost bodů A = [1; 1] a B = [5; 1]?", "num:4", "Body mají stejné y, vzdálenost je 5 − 1 = 4. Výsledek je 4."},
        {"Jaký je obsah pravoúhlého trojúhelníku s odvěsnami 3 a 4?", "num:6", "Obsah je (3 · 4) / 2 = 6. Výsledek je 6."},
        {"Jaký je obvod trojúhelníku se stranami 3, 4 a 5?", "num:12", "3 + 4 + 5 = 12. Výsledek je 12."},
        {"A = [1; 1] a B = [5; 1] jsou dané. Zakreslete C, pro které je |AC| = 3 a |BC| = 5.", "points:1,4;fix:1,1;fix:5,1", "Kružnice se na mřížce protnou v [1; 4]. Bod [1; −2] už na ní není."},
    };

    return math_practice_page(1, 7, "matunit8",
                              "mat_ex8_title", "mat_quiz8_head",
                              qs, G_N_ELEMENTS(qs));
}

void add_mat_pages(GtkStack *stack) {
    typedef GtkWidget *(*MathBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        MathBuilder build_unit;
        MathBuilder build_ex;
    } pages[] = {
        {"matunit1", "matex1", build_mat_unit1_page, build_mat_unit1_exercise_page},
        {"matunit2", "matex2", build_mat_unit2_page, build_mat_unit2_exercise_page},
        {"matunit3", "matex3", build_mat_unit3_page, build_mat_unit3_exercise_page},
        {"matunit4", "matex4", build_mat_unit4_page, build_mat_unit4_exercise_page},
        {"matunit5", "matex5", build_mat_unit5_page, build_mat_unit5_exercise_page},
        {"matunit6", "matex6", build_mat_unit6_page, build_mat_unit6_exercise_page},
        {"matunit7", "matex7", build_mat_unit7_page, build_mat_unit7_exercise_page},
        {"matunit8", "matex8", build_mat_unit8_page, build_mat_unit8_exercise_page},
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
