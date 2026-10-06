#include "graduately.h"

NetLesson mat0_lessons[MATH_N] = {
    { .n_slides = 3, .unit_page = "mat0unit1", .ex_page = "mat0ex1" },
    { .n_slides = 3, .unit_page = "mat0unit2", .ex_page = "mat0ex2" },
    { .n_slides = 3, .unit_page = "mat0unit3", .ex_page = "mat0ex3" },
    { .n_slides = 3, .unit_page = "mat0unit4", .ex_page = "mat0ex4" },
    { .n_slides = 3, .unit_page = "mat0unit5", .ex_page = "mat0ex5" },
    { .n_slides = 3, .unit_page = "mat0unit6", .ex_page = "mat0ex6" },
    { .n_slides = 3, .unit_page = "mat0unit7", .ex_page = "mat0ex7" },
    { .n_slides = 3, .unit_page = "mat0unit8", .ex_page = "mat0ex8" },
};

static GtkWidget *build_mat0_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Pravidlo", "Co se počítá dřív",
            "Tip: násobení a dělení jsou stejně silné a jdou zleva doprava.",
            {
                "Nejprve se vyřeší závorky, zevnitř ven.",
                "Pak jsou mocniny a odmocniny.",
                "Násobení a dělení mají přednost před sčítáním a odčítáním.",
                "Sčítání a odčítání se také provádějí zleva doprava.",
                NULL,
            },
        },
        {
            "2 / 3   •   Bez závorek", "Násobení má přednost",
            "Tip: 2 + 3 · 4 není 20.",
            {
                "Ve výrazu 2 + 3 · 4 se nejdřív násobí: 3 · 4 = 12.",
                "Teprve potom se přičtou dvě: 2 + 12 = 14.",
                "Ve výrazu 20 − 6 / 2 se nejdřív dělí: 6 / 2 = 3.",
                "Pak 20 − 3 = 17.",
                NULL,
            },
        },
        {
            "3 / 3   •   Závorky", "Závorka změní pořadí",
            "Tip: závorka se chová jako jeden počet.",
            {
                "(8 − 3) · 2 začíná v závorce: 5 · 2 = 10.",
                "3 · (4 + 1) je 3 · 5 = 15.",
                "Bez závorky by 3 · 4 + 1 vyšlo 13.",
                "Závorku proto pište vždy, když má sčítání předběhnout násobení.",
                NULL,
            },
        },
    };

    return math_unit_page(0, &mat0_lessons[0], "mat0_unit1", "mat0_unit1_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat0_unit1_exercise_page(void) {
    static const MathItem qs[] = {
        {"Spočítejte 2 + 3 · 4.", "num:14", "Násobení má přednost: 3 · 4 = 12 a 2 + 12 = 14. Výsledek je 14."},
        {"Spočítejte (8 − 3) · 2.", "num:10", "Nejdřív závorka: 8 − 3 = 5, potom 5 · 2 = 10. Výsledek je 10."},
        {"Spočítejte 20 − 6 / 2.", "num:17", "Nejdřív dělení: 6 / 2 = 3, potom 20 − 3 = 17. Výsledek je 17."},
        {"Spočítejte 3 · (4 + 1).", "num:15", "Nejdřív závorka: 4 + 1 = 5, potom 3 · 5 = 15. Výsledek je 15."},
    };

    return math_practice_page(0, 0, "mat0unit1",
                              "mat0_ex1_title", "mat0_quiz1_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat0_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Součet", "Společný jmenovatel",
            "Tip: zlomek v základním tvaru má nesoudělný čitatel a jmenovatel.",
            {
                "Sčítat a odčítat lze zlomky se stejným jmenovatelem.",
                "Jinak se jmenovatelé rozšíří na nejmenší společný násobek.",
                "1/2 + 1/3 má společný jmenovatel 6: 3/6 + 2/6 = 5/6.",
                "3/4 − 1/4 = 2/4 = 1/2.",
                NULL,
            },
        },
        {
            "2 / 3   •   Součin", "Čitatel s čitatelem",
            "Tip: před násobením jde často krátit křížem.",
            {
                "Zlomky se násobí čitatel čitatelem a jmenovatel jmenovatelem.",
                "2/3 · 3/5 = 6/15 = 2/5.",
                "Krátit se smí jen čitatel se jmenovatelem, ne dvě horní čísla mezi sebou.",
                "Smíšené číslo se před počtem převede na zlomek.",
                NULL,
            },
        },
        {
            "3 / 3   •   Podíl", "Dělit znamená násobit převráceným",
            "Tip: převrácený zlomek k a/b je b/a.",
            {
                "Dělení zlomkem je násobení jeho převrácenou hodnotou.",
                "(3/4) : (1/2) = 3/4 · 2/1 = 6/4 = 3/2.",
                "Výsledek se uvede v základním tvaru.",
                "Nulou se dělit nesmí, proto jmenovatel nesmí vyjít 0.",
                NULL,
            },
        },
    };

    return math_unit_page(0, &mat0_lessons[1], "mat0_unit2", "mat0_unit2_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat0_unit2_exercise_page(void) {
    static const MathItem qs[] = {
        {"Spočítejte 1/2 + 1/3. Zlomek v základním tvaru.", "num:5/6", "Společný jmenovatel je 6: 3/6 + 2/6 = 5/6. Výsledek je 5/6."},
        {"Spočítejte 3/4 − 1/4.", "num:1/2", "Jmenovatel už je stejný: 2/4 = 1/2. Výsledek je 1/2."},
        {"Spočítejte 2/3 · 3/5.", "num:2/5", "6/15 se zkrátí třemi na 2/5. Výsledek je 2/5."},
        {"Spočítejte (3/4) : (1/2).", "num:3/2", "Násobí se převráceným zlomkem: 3/4 · 2/1 = 3/2. Výsledek je 3/2."},
    };

    return math_practice_page(0, 1, "mat0unit2",
                              "mat0_ex2_title", "mat0_quiz2_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat0_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Zápis", "Čárka i tečka",
            "Tip: 0,5 je totéž co 1/2.",
            {
                "Desetinná čárka odděluje celou část od desetin, setin a tisícin.",
                "0,1 je jedna desetina, 0,01 je jedna setina.",
                "Při sčítání se čárky píšou pod sebe.",
                "2,5 + 1,5 = 4.",
                NULL,
            },
        },
        {
            "2 / 3   •   Násobení", "Čárka se posouvá",
            "Tip: násobení deseti posune čárku o jedno místo doprava.",
            {
                "Počet desetinných míst součinu je součet počtů míst činitelů.",
                "0,2 · 5 = 1, protože dvě desetiny pětkrát jsou jedna celá.",
                "1,25 − 0,5 = 0,75, což je 3/4.",
                "Výsledek lze psát desetinně i zlomkem.",
                NULL,
            },
        },
        {
            "3 / 3   •   Dělení", "Dělit desetinným číslem",
            "Tip: dělitel se nejdřív upraví na celé číslo.",
            {
                "3,6 : 0,3 se rozšíří deseti na 36 : 3 = 12.",
                "Čárka se v děliteli i děleném posune o stejný počet míst.",
                "Dělení deseti posune čárku o jedno místo doleva.",
                "Nulou se ani u desetinných čísel dělit nesmí.",
                NULL,
            },
        },
    };

    return math_unit_page(0, &mat0_lessons[2], "mat0_unit3", "mat0_unit3_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat0_unit3_exercise_page(void) {
    static const MathItem qs[] = {
        {"Spočítejte 2,5 + 1,5.", "num:4", "Pět desetin a pět desetin je jedna celá, celkem 4. Výsledek je 4."},
        {"Spočítejte 0,2 · 5.", "num:1", "Dvě desetiny pětkrát jsou 10 desetin, tedy 1. Výsledek je 1."},
        {"Spočítejte 3,6 : 0,3.", "num:12", "Posun čárky dá 36 : 3 = 12. Výsledek je 12."},
        {"Spočítejte 1,25 − 0,5.", "num:3/4", "1,25 − 0,50 = 0,75, tedy 3/4. Výsledek je 3/4."},
    };

    return math_practice_page(0, 2, "mat0unit3",
                              "mat0_ex3_title", "mat0_quiz3_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat0_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Základ", "Jedno procento je setina",
            "Tip: pište jen číslo, znak procenta ne.",
            {
                "1 % ze základu je jedna setina základu.",
                "p % ze základu z je (p/100) · z.",
                "20 % ze 150 je 0,2 · 150 = 30.",
                "Základ, procentová část a počet procent jsou tři veličiny.",
                NULL,
            },
        },
        {
            "2 / 3   •   Kolik procent", "Část dělená základem",
            "Tip: výsledek 0,25 znamená 25 %.",
            {
                "Počet procent je (část / základ) · 100.",
                "15 ze 60 je 15/60 = 1/4, tedy 25 %.",
                "Do odpovědi se píše 25, ne 0,25 a ne 25 %.",
                "Zlomek se na procenta násobí stem.",
                NULL,
            },
        },
        {
            "3 / 3   •   Sleva a základ", "Dopočítání celku",
            "Tip: sleva 10 % nechá z ceny 90 %.",
            {
                "Po slevě 10 % zbývá 90 % původní ceny.",
                "400 − 10 % je 0,9 · 400 = 360.",
                "Když 20 % základu je 18, je 1 % rovno 18/20.",
                "Celý základ je pak 100 takových dílů: 90.",
                NULL,
            },
        },
    };

    return math_unit_page(0, &mat0_lessons[3], "mat0_unit4", "mat0_unit4_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat0_unit4_exercise_page(void) {
    static const MathItem qs[] = {
        {"Kolik je 20 % ze 150? Napište jen číslo.", "num:30", "20 % je 0,2 a 0,2 · 150 = 30. Výsledek je 30."},
        {"15 je kolik procent z 60? Napište jen číslo, bez znaku %.", "num:25", "15/60 = 1/4 = 25 %. Výsledek je 25."},
        {"Cena 400 se sníží o 10 %. Jaká bude nová cena?", "num:360", "Zůstane 90 %: 0,9 · 400 = 360. Výsledek je 360."},
        {"20 % základu je 18. Jak velký je základ?", "num:90", "1 % je 18/20 = 0,9 a 100 % je 90. Výsledek je 90."},
    };

    return math_practice_page(0, 3, "mat0unit4",
                              "mat0_ex4_title", "mat0_quiz4_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat0_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Poměr", "Díly celku",
            "Tip: poměr 2 : 3 má dohromady 5 dílů.",
            {
                "Poměr a : b říká, na kolik stejných dílů se celek rozdělí.",
                "Počet dílů je a + b.",
                "První část je a/(a + b) z celku.",
                "Z 20 v poměru 2 : 3 připadá na první část 8.",
                NULL,
            },
        },
        {
            "2 / 3   •   Úměra", "Součin vnějších a vnitřních členů",
            "Tip: 3 : 5 = x : 20 znamená 3/5 = x/20.",
            {
                "V úměře a : b = c : d platí a · d = b · c.",
                "Z 3/5 = x/20 vyjde x = 3/5 · 20 = 12.",
                "Stejný poměr lze rozšiřovat i krátit.",
                "Neznámý člen se dopočítá jedním násobením a jedním dělením.",
                NULL,
            },
        },
        {
            "3 / 3   •   Měřítko", "Mapa a skutečnost",
            "Tip: 1 : 1000 znamená, že 1 cm na mapě je 10 m ve skutečnosti.",
            {
                "Měřítko 1 : k zmenšuje: skutečnost je k-krát větší než plán.",
                "5 cm na mapě 1 : 1000 je 5000 cm, tedy 50 m.",
                "Měřítko 5 : 1 je zvětšení, obraz je pětkrát větší.",
                "4 cm v měřítku 5 : 1 odpovídá obrazu 20 cm.",
                NULL,
            },
        },
    };

    return math_unit_page(0, &mat0_lessons[4], "mat0_unit5", "mat0_unit5_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat0_unit5_exercise_page(void) {
    static const MathItem qs[] = {
        {"Číslo 20 rozdělte v poměru 2 : 3. Jak velká je první část?", "num:8", "Dílů je 5 a první část jsou 2 z nich: 2/5 · 20 = 8. Výsledek je 8."},
        {"Na mapě 1 : 1000 jsou 5 cm. Kolik metrů je to ve skutečnosti?", "num:50", "5 cm · 1000 = 5000 cm = 50 m. Výsledek je 50."},
        {"Platí 3 : 5 = x : 20. Kolik je x?", "num:12", "x = 3/5 · 20 = 12. Výsledek je 12."},
        {"Úsečka 4 cm se zobrazí v měřítku 5 : 1. Jak dlouhý je obraz v centimetrech?", "num:20", "Měřítko 5 : 1 zvětšuje pětkrát: 4 · 5 = 20. Výsledek je 20."},
    };

    return math_practice_page(0, 4, "mat0unit5",
                              "mat0_ex5_title", "mat0_quiz5_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat0_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Číselná osa", "Vpravo roste, vlevo klesá",
            "Tip: přičíst kladné číslo znamená posun doprava.",
            {
                "Kladná čísla leží vpravo od nuly, záporná vlevo.",
                "−3 + 8 je posun z −3 o 8 kroků doprava, tedy na 5.",
                "Odčítání záporného čísla je totéž co přičítání kladného.",
                "6 − (−2) = 6 + 2 = 8.",
                NULL,
            },
        },
        {
            "2 / 3   •   Násobení", "Znaménka",
            "Tip: dvě záporná znaménka dají kladný součin.",
            {
                "Součin kladného a záporného čísla je záporný.",
                "Součin dvou záporných čísel je kladný.",
                "(−4) · (−5) = 20.",
                "Stejné pravidlo platí pro dělení.",
                NULL,
            },
        },
        {
            "3 / 3   •   Absolutní hodnota", "Vzdálenost od nuly",
            "Tip: absolutní hodnota nikdy není záporná.",
            {
                "|a| je vzdálenost čísla a od nuly na ose.",
                "|−7| = 7 a |7| = 7.",
                "|0| = 0.",
                "Absolutní hodnota smaže znaménko, ale číslo jinak nezmění.",
                NULL,
            },
        },
    };

    return math_unit_page(0, &mat0_lessons[5], "mat0_unit6", "mat0_unit6_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat0_unit6_exercise_page(void) {
    static const MathItem qs[] = {
        {"Spočítejte −3 + 8.", "num:5", "Z −3 se jde o 8 doprava, na 5. Výsledek je 5."},
        {"Spočítejte (−4) · (−5).", "num:20", "Součin dvou záporných čísel je kladný: 20. Výsledek je 20."},
        {"Spočítejte |−7|.", "num:7", "Vzdálenost −7 od nuly je 7. Výsledek je 7."},
        {"Spočítejte 6 − (−2).", "num:8", "Odčítat −2 znamená přičíst 2: 6 + 2 = 8. Výsledek je 8."},
    };

    return math_practice_page(0, 5, "mat0unit6",
                              "mat0_ex6_title", "mat0_quiz6_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat0_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Rovnováha", "Stejně na obě strany",
            "Tip: co uděláte vlevo, udělejte i vpravo.",
            {
                "Rovnice říká, že levá strana se rovná pravé.",
                "K oběma stranám lze přičíst stejné číslo.",
                "Obě strany lze vynásobit stejným nenulovým číslem.",
                "Cílem je osamostatnit neznámou.",
                NULL,
            },
        },
        {
            "2 / 3   •   Kroky", "Nejdřív číslo, pak koeficient",
            "Tip: 2x − 4 = 10 se nejdřív zbaví čtyřky.",
            {
                "x + 5 = 12 se odečtením pěti změní na x = 7.",
                "3x = 15 se vydělí třemi: x = 5.",
                "2x − 4 = 10 dá po přičtení čtyř 2x = 14, tedy x = 7.",
                "Zkouška dosadí výsledek do původní rovnice.",
                NULL,
            },
        },
        {
            "3 / 3   •   Dělení neznámé", "Zlomek s x",
            "Tip: x/2 = 6 se násobí dvěma.",
            {
                "x/2 je totéž co (1/2) · x.",
                "Násobení obou stran dvěma dá x = 12.",
                "Násobit nulou se nesmí, rovnice by ztratila význam.",
                "U lineární rovnice s nenulovým koeficientem je řešení jedno.",
                NULL,
            },
        },
    };

    return math_unit_page(0, &mat0_lessons[6], "mat0_unit7", "mat0_unit7_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat0_unit7_exercise_page(void) {
    static const MathItem qs[] = {
        {"Vyřešte x + 5 = 12. Napište x.", "num:7", "Od obou stran se odečte 5. Výsledek je 7."},
        {"Vyřešte 3x = 15. Napište x.", "num:5", "Obě strany se vydělí 3. Výsledek je 5."},
        {"Vyřešte 2x − 4 = 10. Napište x.", "num:7", "2x = 14, takže x = 7. Výsledek je 7."},
        {"Vyřešte x/2 = 6. Napište x.", "num:12", "Obě strany se násobí 2. Výsledek je 12."},
    };

    return math_practice_page(0, 6, "mat0unit7",
                              "mat0_ex7_title", "mat0_quiz7_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat0_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Obdélník", "Obvod a obsah",
            "Tip: obvod je součet všech stran, obsah je součin dvou sousedních.",
            {
                "Obdélník a × b má obvod 2 · (a + b).",
                "Obdélník 5 × 3 má obvod 2 · 8 = 16.",
                "Obsah je a · b. Obdélník 6 × 4 má obsah 24.",
                "Jednotky obvodu jsou délkové, jednotky obsahu čtvereční.",
                NULL,
            },
        },
        {
            "2 / 3   •   Trojúhelník", "Základna a výška",
            "Tip: výška je kolmá na základnu, ne délka šikmé strany.",
            {
                "Součet vnitřních úhlů trojúhelníku je 180°.",
                "Třetí úhel je 180° minus součet dvou známých.",
                "Úhly 60° a 50° doplňuje 70°.",
                "Obsah je (základna · výška) / 2. Pro 8 a 5 je to 20.",
                NULL,
            },
        },
        {
            "3 / 3   •   Čtverec", "Doplnění na mřížce",
            "Tip: osa y míří nahoru. Klepnutím na bod ho odeberete.",
            {
                "Čtverec má čtyři shodné strany a čtyři pravé úhly.",
                "Druhé dva vrcholy vzniknou posunutím strany o stejnou délku kolmo.",
                "U vodorovné strany leží zbývající vrcholy přímo nad ní, nebo pod ní.",
                "Na mřížce jsou platné jen oba vrcholy téže strany.",
                NULL,
            },
        },
    };

    return math_unit_page(0, &mat0_lessons[7], "mat0_unit8", "mat0_unit8_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat0_unit8_exercise_page(void) {
    static const MathItem qs[] = {
        {"Obdélník má strany 5 a 3. Jaký je jeho obvod?", "num:16", "2 · (5 + 3) = 16. Výsledek je 16."},
        {"Obdélník má strany 6 a 4. Jaký je jeho obsah?", "num:24", "Obsah je 6 · 4 = 24. Výsledek je 24."},
        {"Trojúhelník má základnu 8 a výšku 5. Jaký je jeho obsah?", "num:20", "Obsah je (8 · 5) / 2 = 20. Výsledek je 20."},
        {"Dva úhly trojúhelníku jsou 60° a 50°. Jak velký je třetí úhel ve stupních?", "num:70", "Součet je 180°, takže třetí úhel je 70°. Výsledek je 70."},
        {"Na mřížce je strana čtverce od A = [2; 3] k B = [5; 3]. Doplňte zbývající dva vrcholy.", "square:2,3;5,3", "Strana má délku 3. Druhé dva vrcholy jsou [2; 6] a [5; 6], nebo [2; 0] a [5; 0]."},
    };

    return math_practice_page(0, 7, "mat0unit8",
                              "mat0_ex8_title", "mat0_quiz8_head",
                              qs, G_N_ELEMENTS(qs));
}

void add_mat0_pages(GtkStack *stack) {
    typedef GtkWidget *(*MathBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        MathBuilder build_unit;
        MathBuilder build_ex;
    } pages[] = {
        {"mat0unit1", "mat0ex1", build_mat0_unit1_page, build_mat0_unit1_exercise_page},
        {"mat0unit2", "mat0ex2", build_mat0_unit2_page, build_mat0_unit2_exercise_page},
        {"mat0unit3", "mat0ex3", build_mat0_unit3_page, build_mat0_unit3_exercise_page},
        {"mat0unit4", "mat0ex4", build_mat0_unit4_page, build_mat0_unit4_exercise_page},
        {"mat0unit5", "mat0ex5", build_mat0_unit5_page, build_mat0_unit5_exercise_page},
        {"mat0unit6", "mat0ex6", build_mat0_unit6_page, build_mat0_unit6_exercise_page},
        {"mat0unit7", "mat0ex7", build_mat0_unit7_page, build_mat0_unit7_exercise_page},
        {"mat0unit8", "mat0ex8", build_mat0_unit8_page, build_mat0_unit8_exercise_page},
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
