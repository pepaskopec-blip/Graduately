#include "graduately.h"

NetLesson mat3_lessons[MATH_N] = {
    { .n_slides = 3, .unit_page = "mat3unit1", .ex_page = "mat3ex1" },
    { .n_slides = 3, .unit_page = "mat3unit2", .ex_page = "mat3ex2" },
    { .n_slides = 3, .unit_page = "mat3unit3", .ex_page = "mat3ex3" },
    { .n_slides = 3, .unit_page = "mat3unit4", .ex_page = "mat3ex4" },
    { .n_slides = 3, .unit_page = "mat3unit5", .ex_page = "mat3ex5" },
    { .n_slides = 3, .unit_page = "mat3unit6", .ex_page = "mat3ex6" },
    { .n_slides = 3, .unit_page = "mat3unit7", .ex_page = "mat3ex7" },
    { .n_slides = 3, .unit_page = "mat3unit8", .ex_page = "mat3ex8" },
};

static GtkWidget *build_mat3_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Parabola", "f(x) = ax² + bx + c",
            "Tip: pro a > 0 má parabola minimum, pro a < 0 maximum.",
            {
                "Grafem kvadratické funkce je parabola.",
                "Průsečík s osou y je bod [0; c].",
                "U f(x) = x² − 4x + 3 je f(0) = 3.",
                "Osa paraboly je svislá.",
                NULL,
            },
        },
        {
            "2 / 3   •   Vrchol", "x = −b/(2a)",
            "Tip: souřadnice vrcholu pište x;y.",
            {
                "x-ová souřadnice vrcholu je −b/(2a).",
                "U x² − 4x + 3 je to −(−4)/2 = 2.",
                "f(2) = 4 − 8 + 3 = −1.",
                "Vrchol je tedy [2; −1].",
                NULL,
            },
        },
        {
            "3 / 3   •   Kořeny", "Průsečíky s osou x",
            "Tip: kořeny jsou řešení f(x) = 0.",
            {
                "x² − 4x + 3 = (x − 1)(x − 3).",
                "Kořeny jsou 1 a 3.",
                "Leží symetricky kolem osy paraboly.",
                "f(4) = 16 − 16 + 3 = 3, stejně jako f(0).",
                NULL,
            },
        },
    };

    return math_unit_page(3, &mat3_lessons[0], "mat3_unit1", "mat3_unit1_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat3_unit1_exercise_page(void) {
    static const MathItem qs[] = {
        {"Najděte vrchol funkce f(x) = x² − 4x + 3. Napište x a y oddělené středníkem.", "pair:2;-1", "x = 4/2 = 2 a f(2) = −1. Napište 2;-1."},
        {"Najděte kořeny funkce f(x) = x² − 4x + 3. Oddělte je středníkem.", "set:1;3", "(x − 1)(x − 3) = 0. Napište 1;3."},
        {"Kolik je f(0) u funkce f(x) = x² − 4x + 3?", "num:3", "f(0) = 3, to je absolutní člen. Výsledek je 3."},
        {"Kolik je f(4) u funkce f(x) = x² − 4x + 3?", "num:3", "16 − 16 + 3 = 3. Výsledek je 3."},
    };

    return math_practice_page(3, 0, "mat3unit1",
                              "mat3_ex1_title", "mat3_quiz1_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat3_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Definice", "Logaritmus je exponent",
            "Tip: log_b(x) = y právě tehdy, když bʸ = x.",
            {
                "Základ logaritmu je kladný a různý od 1.",
                "Argument logaritmu je kladný.",
                "log₁₀(1000) = 3, protože 10³ = 1000.",
                "log₂(32) = 5, protože 2⁵ = 32.",
                NULL,
            },
        },
        {
            "2 / 3   •   Rovnice", "Stejný základ",
            "Tip: 2ˣ = 16 se přepíše jako 2ˣ = 2⁴.",
            {
                "Exponentialní rovnice se stejným základem porovná exponenty.",
                "2ˣ = 16 má řešení x = 4.",
                "log₃(81) = 4, protože 3⁴ = 81.",
                "log_b(b) = 1 a log_b(1) = 0.",
                NULL,
            },
        },
        {
            "3 / 3   •   Pravidla", "Součin a podíl",
            "Tip: logaritmus součinu je součet logaritmů.",
            {
                "log_b(xy) = log_b(x) + log_b(y).",
                "log_b(x/y) = log_b(x) − log_b(y).",
                "log_b(xⁿ) = n · log_b(x).",
                "Těmito pravidly se argument zjednodušuje, ne základ.",
                NULL,
            },
        },
    };

    return math_unit_page(3, &mat3_lessons[1], "mat3_unit2", "mat3_unit2_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat3_unit2_exercise_page(void) {
    static const MathItem qs[] = {
        {"Spočítejte logaritmus o základu 10 z 1000.", "num:3", "10³ = 1000, takže logaritmus je 3. Výsledek je 3."},
        {"Spočítejte logaritmus o základu 2 z 32.", "num:5", "2⁵ = 32. Výsledek je 5."},
        {"Vyřešte 2ˣ = 16. Napište x.", "num:4", "16 = 2⁴, takže x = 4. Výsledek je 4."},
        {"Spočítejte logaritmus o základu 3 z 81.", "num:4", "3⁴ = 81. Výsledek je 4."},
    };

    return math_practice_page(3, 1, "mat3unit2",
                              "mat3_ex2_title", "mat3_quiz2_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat3_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Jednotková kružnice", "Úhel od kladné poloosy x",
            "Tip: sinus je y-ová souřadnice, kosinus x-ová.",
            {
                "Bod na jednotkové kružnici pod úhlem α má souřadnice [cos α; sin α].",
                "cos 0° = 1 a sin 0° = 0.",
                "sin 90° = 1 a cos 90° = 0.",
                "cos 180° = −1 a sin 180° = 0.",
                NULL,
            },
        },
        {
            "2 / 3   •   45°", "Polovina pravého úhlu",
            "Tip: sin 45° = cos 45° = √2/2.",
            {
                "Úhel 45° půlí čtvrtkružnici.",
                "Souřadnice bodu splňují x = y a x² + y² = 1.",
                "Kladné řešení je √2/2.",
                "Odpověď lze psát jako sqrt(2)/2 nebo 1/sqrt(2).",
                NULL,
            },
        },
        {
            "3 / 3   •   Znaménka", "Čtyři kvadranty",
            "Tip: v prvním kvadrantu jsou sinus i kosinus kladné.",
            {
                "Ve druhém kvadrantu je sinus kladný a kosinus záporný.",
                "Ve třetím jsou oba záporné.",
                "Ve čtvrtém je kosinus kladný a sinus záporný.",
                "Tangens je sinus dělený kosinem a není definován při cos α = 0.",
                NULL,
            },
        },
    };

    return math_unit_page(3, &mat3_lessons[2], "mat3_unit3", "mat3_unit3_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat3_unit3_exercise_page(void) {
    static const MathItem qs[] = {
        {"Kolik je sin 90°?", "num:1", "Bod [0; 1] má y-ovou souřadnici 1. Výsledek je 1."},
        {"Kolik je cos 180°?", "num:-1", "Bod [−1; 0] má x-ovou souřadnici −1. Výsledek je -1."},
        {"Kolik je sin 45°? Můžete psát sqrt(2)/2.", "num:sqrt(2)/2", "sin 45° = √2/2. Stejnou hodnotu má 1/√2."},
        {"Kolik je cos 0°?", "num:1", "Úhel 0° je bod [1; 0]. Výsledek je 1."},
    };

    return math_practice_page(3, 2, "mat3unit3",
                              "mat3_ex3_title", "mat3_quiz3_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat3_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Sinová věta", "a/sin α = 2R",
            "Tip: poměr strany a sinu protilehlého úhlu je pro všechny strany stejný.",
            {
                "a/sin α = b/sin β = c/sin γ = 2R.",
                "R je poloměr kružnice opsané.",
                "Když a = 10 a α = 30°, je 10 / (1/2) = 20 = 2R.",
                "Poloměr R je 10.",
                NULL,
            },
        },
        {
            "2 / 3   •   Kosinová věta", "c² = a² + b² − 2ab cos γ",
            "Tip: pro pravý úhel je cos 90° = 0 a věta přejde v Pythagora.",
            {
                "Kosinová věta dopočítá stranu proti známému úhlu.",
                "Jsou-li dvě strany 2 a sevřený úhel 60°, je třetí strana také 2.",
                "a² = 4 + 4 − 2 · 2 · 2 · cos 60° = 8 − 8 · 1/2 = 4.",
                "Rovnostranný trojúhelník má všechny úhly 60°.",
                NULL,
            },
        },
        {
            "3 / 3   •   Obsah", "Dvě strany a sevřený úhel",
            "Tip: S = (1/2) · b · c · sin α.",
            {
                "Obsah je polovina součinu dvou stran a sinu sevřeného úhlu.",
                "(1/2) · 4 · 6 · sin 30° = 12 · 1/2 = 6.",
                "Sinová věta také dopočítá další stranu: b = a · sin β / sin α.",
                "Pro a = 8, α = 30° a β = 45° je b = 8√2.",
                NULL,
            },
        },
    };

    return math_unit_page(3, &mat3_lessons[3], "mat3_unit4", "mat3_unit4_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat3_unit4_exercise_page(void) {
    static const MathItem qs[] = {
        {"Strana a = 10 leží proti úhlu 30°. Jaký je poloměr kružnice opsané?", "num:10", "a/sin 30° = 20 = 2R, takže R = 10. Výsledek je 10."},
        {"Dvě strany trojúhelníku mají délku 2 a svírají úhel 60°. Jak dlouhá je třetí strana?", "num:2", "a² = 4 + 4 − 8 · 1/2 = 4, takže a = 2. Výsledek je 2."},
        {"Dvě strany mají délky 4 a 6 a svírají úhel 30°. Jaký je obsah trojúhelníku?", "num:6", "(1/2) · 4 · 6 · 1/2 = 6. Výsledek je 6."},
        {"Strana a = 8 je proti 30° a úhel β je 45°. Jak dlouhá je strana b? Sin 45° je √2/2.", "num:8*sqrt(2)", "b = 8 · sin 45° / sin 30° = 8 · (√2/2) / (1/2) = 8√2."},
    };

    return math_practice_page(3, 3, "mat3unit4",
                              "mat3_ex4_title", "mat3_quiz4_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat3_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Aritmetická", "Stálý rozdíl",
            "Tip: aₙ = a₁ + (n − 1)d.",
            {
                "Aritmetická posloupnost přičítá stále stejný rozdíl d.",
                "Pro a₁ = 3 a d = 2 je a₅ = 3 + 4 · 2 = 11.",
                "Součet prvních n členů je sₙ = n/2 · (2a₁ + (n − 1)d).",
                "s₅ = 5/2 · (6 + 8) = 35.",
                NULL,
            },
        },
        {
            "2 / 3   •   Geometrická", "Stálý kvocient",
            "Tip: aₙ = a₁ · qⁿ⁻¹.",
            {
                "Geometrická posloupnost násobí stále stejným kvocientem q.",
                "Pro a₁ = 2 a q = 3 je a₄ = 2 · 3³ = 54.",
                "q = 1 znamená, že se členy nemění.",
                "q = 0 je po prvním členu samá nula, pokud je první člen definován.",
                NULL,
            },
        },
        {
            "3 / 3   •   Nekonečná řada", "Součet pro |q| < 1",
            "Tip: s = a₁ / (1 − q).",
            {
                "Nekonečná geometrická řada má součet jen pro |q| < 1.",
                "Pro a₁ = 6 a q = 1/2 je s = 6 / (1/2) = 12.",
                "Pro |q| ≥ 1 se součet členů neblíží ke konečnému číslu, kromě případu q = 1 a a₁ = 0.",
                "Řada 6 + 3 + 3/2 + … se tedy blíží ke 12.",
                NULL,
            },
        },
    };

    return math_unit_page(3, &mat3_lessons[4], "mat3_unit5", "mat3_unit5_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat3_unit5_exercise_page(void) {
    static const MathItem qs[] = {
        {"Aritmetická posloupnost má a₁ = 3 a d = 2. Kolik je a₅?", "num:11", "a₅ = 3 + 4 · 2 = 11. Výsledek je 11."},
        {"Aritmetická posloupnost má a₁ = 3 a d = 2. Kolik je součet prvních pěti členů?", "num:35", "s₅ = 5/2 · (6 + 8) = 35. Výsledek je 35."},
        {"Geometrická posloupnost má a₁ = 2 a q = 3. Kolik je a₄?", "num:54", "a₄ = 2 · 27 = 54. Výsledek je 54."},
        {"Nekonečná geometrická řada má a₁ = 6 a q = 1/2. Jaký je její součet?", "num:12", "s = 6 / (1/2) = 12. Výsledek je 12."},
    };

    return math_practice_page(3, 4, "mat3unit5",
                              "mat3_ex5_title", "mat3_quiz5_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat3_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Permutace", "Pořadí všech prvků",
            "Tip: n! = 1 · 2 · … · n a 0! = 1.",
            {
                "Permutace je uspořádání všech n různých prvků.",
                "Počet je n!.",
                "4! = 24 a 3! = 6.",
                "Na prvním místě je n možností, na dalším n − 1, a tak dál.",
                NULL,
            },
        },
        {
            "2 / 3   •   Variace", "Pořadí vybraných prvků",
            "Tip: variace bez opakování V(n, k) = n! / (n − k)!.",
            {
                "Variace vybírá k prvků z n a záleží na pořadí.",
                "V(5, 2) = 5 · 4 = 20.",
                "Opakování by dovolilo vybrat stejný prvek víckrát.",
                "Permutace je variace, která vybírá všech n prvků.",
                NULL,
            },
        },
        {
            "3 / 3   •   Kombinace", "Pořadí nehraje roli",
            "Tip: C(n, k) = n! / (k! · (n − k)!).",
            {
                "Kombinace vybírá k prvků a dvě vybrané skupiny se stejným obsahem jsou jedna.",
                "C(5, 2) = 10.",
                "C(n, k) = C(n, n − k).",
                "Počet variací je k! krát větší než počet kombinací.",
                NULL,
            },
        },
    };

    return math_unit_page(3, &mat3_lessons[5], "mat3_unit6", "mat3_unit6_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat3_unit6_exercise_page(void) {
    static const MathItem qs[] = {
        {"Kolik je permutací čtyř různých prvků?", "num:24", "4! = 24. Výsledek je 24."},
        {"Kolik je kombinací 2 prvků z 5?", "num:10", "C(5, 2) = (5 · 4) / 2 = 10. Výsledek je 10."},
        {"Kolik je variací 2 prvků z 5 bez opakování?", "num:20", "V(5, 2) = 5 · 4 = 20. Výsledek je 20."},
        {"Kolik je 3!?", "num:6", "3 · 2 · 1 = 6. Výsledek je 6."},
    };

    return math_practice_page(3, 5, "mat3unit6",
                              "mat3_ex6_title", "mat3_quiz6_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat3_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Definice", "Příznivé ku všem",
            "Tip: všechny výsledky musí být stejně možné.",
            {
                "Klasická pravděpodobnost je počet příznivých výsledků dělený počtem všech.",
                "Hodnota leží od 0 do 1.",
                "Jev jistý má pravděpodobnost 1, jev nemožný 0.",
                "Na kostce je sudé číslo ve třech případech ze šesti, tedy 1/2.",
                NULL,
            },
        },
        {
            "2 / 3   •   Nezávislost", "Součin pravděpodobností",
            "Tip: dvě mince mají čtyři stejně možné dvojice výsledků.",
            {
                "U dvou nezávislých mincí jsou možnosti PP, PO, OP a OO.",
                "Dva panny jsou jedna možnost ze čtyř, tedy 1/4.",
                "U dvou kostek je 36 stejně možných dvojic.",
                "Součet 7 nastane u šesti dvojic: 1+6, 2+5, 3+4, 4+3, 5+2 a 6+1.",
                NULL,
            },
        },
        {
            "3 / 3   •   Karty", "Čtyři esa v balíčku 32 karet",
            "Tip: mariášový balíček má 32 karet.",
            {
                "Pravděpodobnost esa je 4/32 = 1/8.",
                "Tažení jedné karty považuje každou kartu za stejně možnou.",
                "Doplňkový jev má pravděpodobnost 1 − p.",
                "Výsledek se krátí do základního tvaru.",
                NULL,
            },
        },
    };

    return math_unit_page(3, &mat3_lessons[6], "mat3_unit7", "mat3_unit7_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat3_unit7_exercise_page(void) {
    static const MathItem qs[] = {
        {"Jaká je pravděpodobnost sudého čísla při hodu hrací kostkou?", "num:1/2", "Sudá jsou 2, 4 a 6, tedy 3 ze 6, čili 1/2. Výsledek je 1/2."},
        {"Jaká je pravděpodobnost, že na dvou mincích padnou dvě panny?", "num:1/4", "Ze čtyř dvojic je příznivá jedna. Výsledek je 1/4."},
        {"V balíčku 32 karet jsou 4 esa. Jaká je pravděpodobnost, že tažená karta je eso?", "num:1/8", "4/32 = 1/8. Výsledek je 1/8."},
        {"Jaká je pravděpodobnost, že součet na dvou hracích kostkách je 7?", "num:1/6", "Příznivých dvojic je 6 z 36, tedy 1/6. Výsledek je 1/6."},
    };

    return math_practice_page(3, 6, "mat3unit7",
                              "mat3_ex7_title", "mat3_quiz7_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat3_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Krychle", "Objem a povrch",
            "Tip: krychle o hraně a má objem a³ a povrch 6a².",
            {
                "Objem je počet jednotkových krychlí, které se do tělesa vejdou.",
                "Krychle o hraně 3 má objem 27.",
                "Povrch je součet obsahů všech stěn: 6 · 9 = 54.",
                "Jednotky objemu jsou krychlové, jednotky povrchu čtvereční.",
                NULL,
            },
        },
        {
            "2 / 3   •   Válec a koule", "Vzorce s π",
            "Tip: výsledek nechte v násobcích π.",
            {
                "Objem válce je πr²v.",
                "Pro r = 2 a v = 5 je objem 20π.",
                "Objem koule je (4/3)πr³.",
                "Pro r = 3 je objem (4/3)π · 27 = 36π.",
                NULL,
            },
        },
        {
            "3 / 3   •   Jehlan", "Třetina hranolu",
            "Tip: objem jehlanu je (1/3) · obsah podstavy · výška.",
            {
                "Jehlan má stejný objem jako třetina hranolu se stejnou podstavou a výškou.",
                "Podstava o obsahu 12 a výška 4 dávají objem 16.",
                "Výška je kolmá vzdálenost vrcholu od roviny podstavy.",
                "Stejný vztah třetiny platí i pro kužel vůči válci.",
                NULL,
            },
        },
    };

    return math_unit_page(3, &mat3_lessons[7], "mat3_unit8", "mat3_unit8_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat3_unit8_exercise_page(void) {
    static const MathItem qs[] = {
        {"Krychle má hranu 3. Jaký je její objem?", "num:27", "3³ = 27. Výsledek je 27."},
        {"Krychle má hranu 3. Jaký je její povrch?", "num:54", "Šest stěn o obsahu 9 dává 54. Výsledek je 54."},
        {"Válec má poloměr 2 a výšku 5. Jaký je jeho objem? Nechte π ve výsledku.", "num:20*pi", "πr²v = π · 4 · 5 = 20π. Výsledek je 20π."},
        {"Koule má poloměr 3. Jaký je její objem? Nechte π ve výsledku.", "num:36*pi", "(4/3)π · 27 = 36π. Výsledek je 36π."},
    };

    return math_practice_page(3, 7, "mat3unit8",
                              "mat3_ex8_title", "mat3_quiz8_head",
                              qs, G_N_ELEMENTS(qs));
}

void add_mat3_pages(GtkStack *stack) {
    typedef GtkWidget *(*MathBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        MathBuilder build_unit;
        MathBuilder build_ex;
    } pages[] = {
        {"mat3unit1", "mat3ex1", build_mat3_unit1_page, build_mat3_unit1_exercise_page},
        {"mat3unit2", "mat3ex2", build_mat3_unit2_page, build_mat3_unit2_exercise_page},
        {"mat3unit3", "mat3ex3", build_mat3_unit3_page, build_mat3_unit3_exercise_page},
        {"mat3unit4", "mat3ex4", build_mat3_unit4_page, build_mat3_unit4_exercise_page},
        {"mat3unit5", "mat3ex5", build_mat3_unit5_page, build_mat3_unit5_exercise_page},
        {"mat3unit6", "mat3ex6", build_mat3_unit6_page, build_mat3_unit6_exercise_page},
        {"mat3unit7", "mat3ex7", build_mat3_unit7_page, build_mat3_unit7_exercise_page},
        {"mat3unit8", "mat3ex8", build_mat3_unit8_page, build_mat3_unit8_exercise_page},
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
