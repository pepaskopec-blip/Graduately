#include "graduately.h"

NetLesson mat2_lessons[MATH_N] = {
    { .n_slides = 3, .unit_page = "mat2unit1", .ex_page = "mat2ex1" },
    { .n_slides = 3, .unit_page = "mat2unit2", .ex_page = "mat2ex2" },
    { .n_slides = 3, .unit_page = "mat2unit3", .ex_page = "mat2ex3" },
    { .n_slides = 3, .unit_page = "mat2unit4", .ex_page = "mat2ex4" },
    { .n_slides = 3, .unit_page = "mat2unit5", .ex_page = "mat2ex5" },
    { .n_slides = 3, .unit_page = "mat2unit6", .ex_page = "mat2ex6" },
    { .n_slides = 3, .unit_page = "mat2unit7", .ex_page = "mat2ex7" },
    { .n_slides = 3, .unit_page = "mat2unit8", .ex_page = "mat2ex8" },
};

static GtkWidget *build_mat2_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Předpis", "y = ax + b",
            "Tip: a je směrnice, b je úsek na ose y.",
            {
                "Lineární funkce má předpis f(x) = ax + b.",
                "Grafem je přímka.",
                "b je hodnota f(0), tedy průsečík s osou y.",
                "Pro f(x) = 2x − 3 je průsečík s osou y číslo −3.",
                NULL,
            },
        },
        {
            "2 / 3   •   Směrnice", "O kolik y vzroste",
            "Tip: směrnice je podíl změny y a změny x.",
            {
                "Mezi body [x₁; y₁] a [x₂; y₂] je a = (y₂ − y₁)/(x₂ − x₁).",
                "Body [1; 2] a [3; 8] dávají směrnici (8 − 2)/(3 − 1) = 3.",
                "Kladná směrnice znamená růst, záporná pokles.",
                "Nulová směrnice je vodorovná přímka.",
                NULL,
            },
        },
        {
            "3 / 3   •   Hodnota a kořen", "Dosazení a rovnice",
            "Tip: nulový bod je x, pro které je f(x) = 0.",
            {
                "f(4) u funkce 2x − 3 je 8 − 3 = 5.",
                "Rovnice 2x − 6 = 0 má řešení x = 3.",
                "To je průsečík přímky y = 2x − 6 s osou x.",
                "Lineární funkce s a ≠ 0 má právě jeden nulový bod.",
                NULL,
            },
        },
    };

    return math_unit_page(2, &mat2_lessons[0], "mat2_unit1", "mat2_unit1_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat2_unit1_exercise_page(void) {
    static const MathItem qs[] = {
        {"Funkce f(x) = 2x − 3. Kolik je f(4)?", "num:5", "2 · 4 − 3 = 5. Výsledek je 5."},
        {"Jaký je průsečík funkce f(x) = 2x − 3 s osou y?", "num:-3", "f(0) = −3. Výsledek je -3."},
        {"Jaká je směrnice přímky body [1; 2] a [3; 8]?", "num:3", "(8 − 2)/(3 − 1) = 3. Výsledek je 3."},
        {"Vyřešte 2x − 6 = 0. Napište x.", "num:3", "2x = 6, takže x = 3. Výsledek je 3."},
    };

    return math_practice_page(2, 0, "mat2unit1",
                              "mat2_ex1_title", "mat2_quiz1_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat2_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Dvojice", "Jedno x a jedno y",
            "Tip: výsledek pište x;y, nejdřív x.",
            {
                "Soustava dvou lineárních rovnic hledá dvojici [x; y], která vyhovuje oběma.",
                "Řešením je průsečík dvou přímek.",
                "Sčítací metoda sečte rovnice tak, aby jedna neznámá zmizela.",
                "Dosazovací metoda vyjádří jednu neznámou a dosadí ji do druhé rovnice.",
                NULL,
            },
        },
        {
            "2 / 3   •   Sčítání", "Opačné koeficienty",
            "Tip: x + y = 10 a x − y = 2 se sčítají na 2x = 12.",
            {
                "Součet dá 2x = 12, tedy x = 6.",
                "Z x + y = 10 je y = 4.",
                "Zkouška ve druhé rovnici: 6 − 4 = 2.",
                "Odečtení rovnic se použije, když má neznámá v obou stejný koeficient.",
                NULL,
            },
        },
        {
            "3 / 3   •   Odečtení", "Stejný koeficient",
            "Tip: rovnice lze před odečtením násobit.",
            {
                "2x + y = 8 a x + y = 5 se odečtou na x = 3.",
                "Pak y = 2.",
                "3x + y = 10 a x + y = 6 dají 2x = 4, tedy x = 2 a y = 4.",
                "x + y = 7 a x − y = 1 dají x = 4 a y = 3.",
                NULL,
            },
        },
    };

    return math_unit_page(2, &mat2_lessons[1], "mat2_unit2", "mat2_unit2_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat2_unit2_exercise_page(void) {
    static const MathItem qs[] = {
        {"Vyřešte soustavu x + y = 10 a x − y = 2. Napište x a y oddělené středníkem.", "pair:6;4", "Sečtení dá 2x = 12, x = 6 a y = 4. Napište 6;4."},
        {"Vyřešte soustavu 2x + y = 8 a x + y = 5. Napište x a y oddělené středníkem.", "pair:3;2", "Odečtení dá x = 3 a y = 2. Napište 3;2."},
        {"Vyřešte soustavu x + y = 7 a x − y = 1. Napište x a y oddělené středníkem.", "pair:4;3", "Sečtení dá 2x = 8, x = 4 a y = 3. Napište 4;3."},
        {"Vyřešte soustavu 3x + y = 10 a x + y = 6. Napište x a y oddělené středníkem.", "pair:2;4", "Odečtení dá 2x = 4, x = 2 a y = 4. Napište 2;4."},
    };

    return math_practice_page(2, 1, "mat2unit2",
                              "mat2_ex2_title", "mat2_quiz2_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat2_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Tvar", "ax² + bx + c = 0",
            "Tip: kořeny pište oddělené středníkem, pořadí nehraje roli.",
            {
                "Kvadratická rovnice má a ≠ 0.",
                "Kořeny rovnice x² − 5x + 6 = 0 jsou 2 a 3, protože (x − 2)(x − 3) = 0.",
                "x² − 4 = 0 má kořeny −2 a 2.",
                "Součin je nula, právě když je nula aspoň jeden činitel.",
                NULL,
            },
        },
        {
            "2 / 3   •   Diskriminant", "D = b² − 4ac",
            "Tip: D > 0 znamená dva různé reálné kořeny.",
            {
                "Pro ax² + bx + c = 0 je D = b² − 4ac.",
                "U x² − 2x − 3 je a = 1, b = −2, c = −3 a D = 4 + 12 = 16.",
                "D = 0 znamená jeden dvojnásobný kořen.",
                "D < 0 znamená, že v reálných číslech řešení není.",
                NULL,
            },
        },
        {
            "3 / 3   •   Vzorec", "Kořen je (−b ± √D) / (2a)",
            "Tip: dvojnásobný kořen x² + 6x + 9 = 0 je −3.",
            {
                "x = (−b ± √D) / (2a).",
                "x² + 6x + 9 = (x + 3)², takže kořen −3 je dvojnásobný.",
                "U dvojnásobného kořene stačí napsat číslo jednou.",
                "Zkouška dosadí kořen zpět do rovnice.",
                NULL,
            },
        },
    };

    return math_unit_page(2, &mat2_lessons[2], "mat2_unit3", "mat2_unit3_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat2_unit3_exercise_page(void) {
    static const MathItem qs[] = {
        {"Najděte kořeny rovnice x² − 5x + 6 = 0. Oddělte je středníkem.", "set:2;3", "(x − 2)(x − 3) = 0. Napište 2;3."},
        {"Najděte kořeny rovnice x² − 4 = 0. Oddělte je středníkem.", "set:-2;2", "x² = 4, takže x = −2 nebo x = 2. Napište -2;2."},
        {"Jaký je diskriminant rovnice x² − 2x − 3 = 0?", "num:16", "D = (−2)² − 4 · 1 · (−3) = 4 + 12 = 16. Výsledek je 16."},
        {"Jaký je dvojnásobný kořen rovnice x² + 6x + 9 = 0?", "num:-3", "(x + 3)² = 0, kořen je −3. Výsledek je -3."},
    };

    return math_practice_page(2, 2, "mat2unit3",
                              "mat2_ex3_title", "mat2_quiz3_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat2_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Obor", "Jmenovatel nesmí být nula",
            "Tip: podmínku pište dřív, než krátíte.",
            {
                "Lomený výraz je podíl dvou mnohočlenů.",
                "Není definován tam, kde je jmenovatel nula.",
                "(x + 2)/(x − 2) nemá smysl pro x = 2.",
                "Po krácení se vyloučené hodnoty do oboru nevracejí.",
                NULL,
            },
        },
        {
            "2 / 3   •   Krácení", "Rozdíl čtverců",
            "Tip: x² − 1 = (x − 1)(x + 1).",
            {
                "(x² − 1)/(x − 1) = x + 1 pro x ≠ 1.",
                "Při x = 4 je hodnota 5.",
                "(x² − 4)/(x + 2) = x − 2 pro x ≠ −2.",
                "Při x = 5 je hodnota 3.",
                NULL,
            },
        },
        {
            "3 / 3   •   Násobení", "Čitatel s čitatelem",
            "Tip: x ve jmenovateli se zkrátí jen pro x ≠ 0.",
            {
                "Zlomky se násobí stejně jako číselné.",
                "(2x/4) · (6/x) = 12x/(4x) = 3 pro x ≠ 0.",
                "6x/(3x) = 2 pro x ≠ 0.",
                "Než se zkrátí písmeno, ověří se, že není nula.",
                NULL,
            },
        },
    };

    return math_unit_page(2, &mat2_lessons[3], "mat2_unit4", "mat2_unit4_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat2_unit4_exercise_page(void) {
    static const MathItem qs[] = {
        {"Do výrazu (x² − 1)/(x − 1) dosažte x = 4.", "num:5", "Pro x ≠ 1 je výraz x + 1, tedy 5. Výsledek je 5."},
        {"Pro které x nemá výraz (x + 2)/(x − 2) smysl?", "num:2", "Jmenovatel x − 2 je nula právě při x = 2. Výsledek je 2."},
        {"Upravte (2x/4) · (6/x) pro x ≠ 0.", "num:3", "12x/(4x) = 3. Výsledek je 3."},
        {"Do výrazu (x² − 4)/(x + 2) dosažte x = 5.", "num:3", "Pro x ≠ −2 je výraz x − 2, tedy 3. Výsledek je 3."},
    };

    return math_practice_page(2, 3, "mat2unit4",
                              "mat2_ex4_title", "mat2_quiz4_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat2_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Odmocnina jako exponent", "a^(1/n)",
            "Tip: a^(1/2) je druhá odmocnina z a.",
            {
                "Pro a > 0 je a^(1/n) n-tá odmocnina z a.",
                "16^(1/2) = 4.",
                "a^(m/n) je n-tá odmocnina z aᵐ, nebo m-tá mocnina odmocniny.",
                "8^(2/3) = (8^(1/3))² = 2² = 4.",
                NULL,
            },
        },
        {
            "2 / 3   •   Stejný základ", "Exponenty se sčítají",
            "Tip: 3² · 3³ = 3⁵.",
            {
                "aᵐ · aⁿ = aᵐ⁺ⁿ.",
                "3² · 3³ = 3⁵ = 243.",
                "(aᵐ)ⁿ = aᵐ·ⁿ.",
                "Základ musí být při sčítání exponentů stejný.",
                NULL,
            },
        },
        {
            "3 / 3   •   Záporný exponent", "Převrácená hodnota",
            "Tip: a⁻ⁿ = 1/aⁿ pro a ≠ 0.",
            {
                "10⁻² = 1/100.",
                "10⁻¹ = 0,1 a 10⁰ = 1.",
                "Záporný exponent nepřevrací znaménko základu, ale celou mocninu.",
                "(−2)⁻² = 1/4, protože (−2)² = 4.",
                NULL,
            },
        },
    };

    return math_unit_page(2, &mat2_lessons[4], "mat2_unit5", "mat2_unit5_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat2_unit5_exercise_page(void) {
    static const MathItem qs[] = {
        {"Spočítejte 16^(1/2).", "num:4", "Druhá odmocnina z 16 je 4. Výsledek je 4."},
        {"Spočítejte 8^(2/3).", "num:4", "Třetí odmocnina z 8 je 2 a 2² = 4. Výsledek je 4."},
        {"Spočítejte 3² · 3³.", "num:243", "3⁵ = 243. Výsledek je 243."},
        {"Spočítejte 10⁻².", "num:1/100", "10⁻² = 1/10² = 1/100. Výsledek je 1/100."},
    };

    return math_practice_page(2, 4, "mat2unit5",
                              "mat2_ex5_title", "mat2_quiz5_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat2_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Poměry", "Odvěsny a přepona",
            "Tip: sinus je protilehlá odvěsna ku přeponě.",
            {
                "V pravoúhlém trojúhelníku je sin α = protilehlá / přepona.",
                "cos α = přilehlá / přepona.",
                "tan α = protilehlá / přilehlá.",
                "Přepona je strana proti pravému úhlu.",
                NULL,
            },
        },
        {
            "2 / 3   •   Tabulkové úhly", "30°, 45° a 60°",
            "Tip: sinus 30° je 1/2.",
            {
                "sin 30° = 1/2 a cos 60° = 1/2.",
                "sin 60° = cos 30° = √3/2.",
                "sin 45° = cos 45° = √2/2.",
                "tan 45° = 1.",
                NULL,
            },
        },
        {
            "3 / 3   •   Dopočet strany", "Přepona a úhel",
            "Tip: proti úhlu 30° je polovina přepony.",
            {
                "Známe-li přeponu a úhel, protilehlá odvěsna je přepona · sin α.",
                "Proti 30° při přeponě 10 je odvěsna 5.",
                "Přilehlá odvěsna by byla 10 · cos 30°.",
                "Tangens se hodí, když jsou známé dvě odvěsny a ne přepona.",
                NULL,
            },
        },
    };

    return math_unit_page(2, &mat2_lessons[5], "mat2_unit6", "mat2_unit6_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat2_unit6_exercise_page(void) {
    static const MathItem qs[] = {
        {"Kolik je sin 30°?", "num:1/2", "Sinus 30° je 1/2. Výsledek je 1/2."},
        {"Kolik je cos 60°?", "num:1/2", "Kosinus 60° je 1/2. Výsledek je 1/2."},
        {"Kolik je tan 45°?", "num:1", "Protilehlá a přilehlá odvěsna jsou u 45° stejně dlouhé. Výsledek je 1."},
        {"Přepona pravoúhlého trojúhelníku je 10 a jeden úhel je 30°. Jak dlouhá je odvěsna proti tomuto úhlu?", "num:5", "10 · sin 30° = 10 · 1/2 = 5. Výsledek je 5."},
    };

    return math_practice_page(2, 5, "mat2unit6",
                              "mat2_ex6_title", "mat2_quiz6_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat2_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Pythagoras", "a² + b² = c²",
            "Tip: 8² + 15² = 64 + 225 = 289 = 17².",
            {
                "V pravoúhlém trojúhelníku se součet čtverců nad odvěsnami rovná čtverci nad přeponou.",
                "Trojúhelník 8, 15, 17 je pravoúhlý a přepona je 17.",
                "Čtverec nad přeponou trojúhelníku 3, 4, 5 má obsah 25.",
                "Věta platí jen v pravoúhlém trojúhelníku.",
                NULL,
            },
        },
        {
            "2 / 3   •   Eukleides", "Úsek přepony",
            "Tip: a² = c · a_c, kde a_c je úsek přepony přilehlý k odvěsně a.",
            {
                "Odvěsna je geometrický průměr přepony a svého úseku.",
                "a² = c · a_c a b² = c · b_c.",
                "Úseky a_c a b_c dají dohromady přeponu c.",
                "U odvěsen 6 a 8 je přepona 10 a úsek u odvěsny 6 je 36/10 = 18/5.",
                NULL,
            },
        },
        {
            "3 / 3   •   Výška", "v² = a_c · b_c",
            "Tip: také platí a · b = c · v.",
            {
                "Výška k přeponě je geometrický průměr obou úseků.",
                "Z a · b = c · v plyne v = ab/c.",
                "Pro odvěsny 6 a 8 a přeponu 10 je v = 48/10 = 24/5.",
                "Obsah trojúhelníku je pak (c · v) / 2, stejně jako (a · b) / 2.",
                NULL,
            },
        },
    };

    return math_unit_page(2, &mat2_lessons[6], "mat2_unit7", "mat2_unit7_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat2_unit7_exercise_page(void) {
    static const MathItem qs[] = {
        {"Pravoúhlý trojúhelník má odvěsny 8 a 15. Jak dlouhá je přepona?", "num:17", "64 + 225 = 289 = 17². Výsledek je 17."},
        {"Jaký obsah má čtverec sestrojený nad přeponou pravoúhlého trojúhelníku se stranami 3, 4 a 5?", "num:25", "Přepona je 5 a 5² = 25. Výsledek je 25."},
        {"Odvěsny jsou 6 a 8, přepona je 10. Jak dlouhý je úsek přepony přilehlý k odvěsně 6?", "num:18/5", "a² = c · a_c, takže a_c = 36/10 = 18/5. Výsledek je 18/5."},
        {"Odvěsny jsou 6 a 8, přepona je 10. Jak dlouhá je výška k přeponě?", "num:24/5", "v = ab/c = 48/10 = 24/5. Výsledek je 24/5."},
    };

    return math_practice_page(2, 6, "mat2unit7",
                              "mat2_ex7_title", "mat2_quiz7_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat2_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Úhly", "Obvodový je polovina středového",
            "Tip: oba úhly musí být nad stejným obloukem.",
            {
                "Středový úhel má vrchol ve středu kružnice.",
                "Obvodový úhel má vrchol na kružnici.",
                "Nad stejným obloukem je obvodový úhel polovinou středového.",
                "Středový úhel 80° patří k obvodovému úhlu 40°.",
                NULL,
            },
        },
        {
            "2 / 3   •   Oblouk", "Část obvodu",
            "Tip: délka oblouku je (α/360°) · 2πr.",
            {
                "Celý obvod je 2πr.",
                "Oblouk 60° je šestina obvodu.",
                "Při r = 6 je jeho délka 2π.",
                "Výsledek nechte v násobcích π.",
                NULL,
            },
        },
        {
            "3 / 3   •   Thales", "Pravý úhel nad průměrem",
            "Tip: vrchol pravého úhlu leží na kružnici s průměrem AB.",
            {
                "Thaletova věta: úhel nad průměrem je pravý.",
                "Je-li AB průměr, je úhel ACB pravý pro každé C na kružnici různé od A a B.",
                "Na mřížce má průměr od [2; 4] do [8; 4] střed [5; 4] a poloměr 3.",
                "Vyhovují body [5; 7] a [5; 1].",
                NULL,
            },
        },
    };

    return math_unit_page(2, &mat2_lessons[7], "mat2_unit8", "mat2_unit8_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat2_unit8_exercise_page(void) {
    static const MathItem qs[] = {
        {"Středový úhel je 80°. Jak velký je obvodový úhel nad stejným obloukem ve stupních?", "num:40", "Obvodový úhel je polovina středového. Výsledek je 40."},
        {"Kružnice má poloměr 6. Jak dlouhý je oblouk 60°? Nechte π ve výsledku.", "num:2*pi", "(60/360) · 2π · 6 = 2π. Výsledek je 2π."},
        {"Obvodový úhel je 35°. Jak velký je středový úhel nad stejným obloukem ve stupních?", "num:70", "Středový úhel je dvojnásobek obvodového. Výsledek je 70."},
        {"AB je průměr od [2; 4] do [8; 4]. Zakreslete bod C na kružnici, ve kterém je úhel ACB pravý.", "right:2,4;8,4", "Thaletova kružnice má střed [5; 4] a poloměr 3. Na mřížce vyhovuje [5; 7] nebo [5; 1]."},
    };

    return math_practice_page(2, 7, "mat2unit8",
                              "mat2_ex8_title", "mat2_quiz8_head",
                              qs, G_N_ELEMENTS(qs));
}

void add_mat2_pages(GtkStack *stack) {
    typedef GtkWidget *(*MathBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        MathBuilder build_unit;
        MathBuilder build_ex;
    } pages[] = {
        {"mat2unit1", "mat2ex1", build_mat2_unit1_page, build_mat2_unit1_exercise_page},
        {"mat2unit2", "mat2ex2", build_mat2_unit2_page, build_mat2_unit2_exercise_page},
        {"mat2unit3", "mat2ex3", build_mat2_unit3_page, build_mat2_unit3_exercise_page},
        {"mat2unit4", "mat2ex4", build_mat2_unit4_page, build_mat2_unit4_exercise_page},
        {"mat2unit5", "mat2ex5", build_mat2_unit5_page, build_mat2_unit5_exercise_page},
        {"mat2unit6", "mat2ex6", build_mat2_unit6_page, build_mat2_unit6_exercise_page},
        {"mat2unit7", "mat2ex7", build_mat2_unit7_page, build_mat2_unit7_exercise_page},
        {"mat2unit8", "mat2ex8", build_mat2_unit8_page, build_mat2_unit8_exercise_page},
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
