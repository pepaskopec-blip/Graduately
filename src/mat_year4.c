#include "graduately.h"

NetLesson mat4_lessons[MATH_N] = {
    { .n_slides = 3, .unit_page = "mat4unit1", .ex_page = "mat4ex1" },
    { .n_slides = 3, .unit_page = "mat4unit2", .ex_page = "mat4ex2" },
    { .n_slides = 3, .unit_page = "mat4unit3", .ex_page = "mat4ex3" },
    { .n_slides = 3, .unit_page = "mat4unit4", .ex_page = "mat4ex4" },
    { .n_slides = 3, .unit_page = "mat4unit5", .ex_page = "mat4ex5" },
    { .n_slides = 3, .unit_page = "mat4unit6", .ex_page = "mat4ex6" },
    { .n_slides = 3, .unit_page = "mat4unit7", .ex_page = "mat4ex7" },
    { .n_slides = 3, .unit_page = "mat4unit8", .ex_page = "mat4ex8" },
};

static GtkWidget *build_mat4_unit1_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Souřadnice", "Velikost a součet",
            "Tip: velikost vektoru (a; b) je √(a² + b²).",
            {
                "Vektor má velikost a směr.",
                "|(3; 4)| = 5, protože 9 + 16 = 25.",
                "Vektory se sčítají po souřadnicích.",
                "(1; 2) + (3; −1) = (4; 1).",
                NULL,
            },
        },
        {
            "2 / 3   •   Skalární součin", "Číslo, ne vektor",
            "Tip: (a; b) · (c; d) = ac + bd.",
            {
                "Skalární součin dvou vektorů je číslo.",
                "(1; 2) · (3; 4) = 3 + 8 = 11.",
                "Nulový skalární součin znamená kolmé vektory.",
                "Velikost lze psát jako odmocninu ze skalárního součinu vektoru se sebou.",
                NULL,
            },
        },
        {
            "3 / 3   •   Vzdálenost", "Velikost rozdílu",
            "Tip: na mřížce osa y míří nahoru.",
            {
                "Vzdálenost bodů je velikost vektoru mezi nimi.",
                "Od [1; 1] do [4; 5] je vektor (3; 4) a vzdálenost 5.",
                "Koncový bod vektoru (3; 2) umístěného do [1; 1] je [4; 3].",
                "Pořadí souřadnic je x a potom y.",
                NULL,
            },
        },
    };

    return math_unit_page(4, &mat4_lessons[0], "mat4_unit1", "mat4_unit1_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat4_unit1_exercise_page(void) {
    static const MathItem qs[] = {
        {"Jaká je velikost vektoru (3; 4)?", "num:5", "√(9 + 16) = √25 = 5. Výsledek je 5."},
        {"Sečtěte vektory (1; 2) a (3; −1). Napište souřadnice oddělené středníkem.", "pair:4;1", "(1 + 3; 2 + (−1)) = (4; 1). Napište 4;1."},
        {"Spočítejte skalární součin vektorů (1; 2) a (3; 4).", "num:11", "1 · 3 + 2 · 4 = 11. Výsledek je 11."},
        {"Jaká je vzdálenost bodů [1; 1] a [4; 5]?", "num:5", "Rozdíl je (3; 4) a jeho velikost je 5. Výsledek je 5."},
        {"Z bodu A = [1; 1] naneste vektor (3; 2). Zakreslete koncový bod.", "points:4,3;fix:1,1", "Ke každé souřadnici se přičte složka vektoru: [1 + 3; 1 + 2] = [4; 3]."},
    };

    return math_practice_page(4, 0, "mat4unit1",
                              "mat4_ex1_title", "mat4_quiz1_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat4_unit2_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Směrnice", "y = ax + b",
            "Tip: a = (y₂ − y₁)/(x₂ − x₁).",
            {
                "Body [1; 2] a [4; 8] určují směrnici (8 − 2)/(4 − 1) = 2.",
                "Přímka procházející bodem [0; 1] se směrnicí 2 má rovnici y = 2x + 1.",
                "Úsek na ose y je 1.",
                "Svislá přímka směrnici tohoto tvaru nemá.",
                NULL,
            },
        },
        {
            "2 / 3   •   Parametr", "Bod a směrový vektor",
            "Tip: X = A + t · u.",
            {
                "Parametrické vyjádření je x = x₀ + t · u₁, y = y₀ + t · u₂.",
                "Přímka (1; 3) + t(2; 0) má při t = 2 bod [5; 3].",
                "Každé t dá právě jeden bod přímky.",
                "Směrový vektor (2; 0) je vodorovný.",
                NULL,
            },
        },
        {
            "3 / 3   •   Vzdálenost", "Bod od přímky",
            "Tip: pro ax + by + c = 0 je vzdálenost |ax₀ + by₀ + c| / √(a² + b²).",
            {
                "Přímka 3x + 4y − 10 = 0 má a = 3, b = 4 a c = −10.",
                "Vzdálenost počátku je |−10| / √(9 + 16) = 10/5 = 2.",
                "Čitatel je absolutní hodnota, vzdálenost není záporná.",
                "Čtvrtý vrchol rovnoběžníku k bodům A, B a C je B + C − A.",
                NULL,
            },
        },
    };

    return math_unit_page(4, &mat4_lessons[1], "mat4_unit2", "mat4_unit2_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat4_unit2_exercise_page(void) {
    static const MathItem qs[] = {
        {"Jaká je směrnice přímky body [1; 2] a [4; 8]?", "num:2", "(8 − 2)/(4 − 1) = 2. Výsledek je 2."},
        {"Přímka má rovnici y = 2x + 1. Jaký je její úsek na ose y?", "num:1", "Úsek je absolutní člen, tedy 1. Výsledek je 1."},
        {"Bod přímky je (1; 3) + t(2; 0). Kam přímka dojde pro t = 2? Napište x a y středníkem.", "pair:5;3", "(1; 3) + 2 · (2; 0) = (5; 3). Napište 5;3."},
        {"Jaká je vzdálenost bodu [0; 0] od přímky 3x + 4y − 10 = 0?", "num:2", "|−10| / √(9 + 16) = 10/5 = 2. Výsledek je 2."},
        {"A = [1; 1], B = [4; 1] a C = [2; 3]. Zakreslete čtvrtý vrchol rovnoběžníku, ve kterém jsou AB a AC sousední strany.", "para:1,1;4,1;2,3", "D = B + C − A = [4 + 2 − 1; 1 + 3 − 1] = [5; 3]."},
    };

    return math_practice_page(4, 1, "mat4unit2",
                              "mat4_ex2_title", "mat4_quiz2_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat4_unit3_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Kružnice", "Střed a poloměr",
            "Tip: (x − m)² + (y − n)² = r² má střed [m; n].",
            {
                "Kružnice je množina bodů stejně vzdálených od středu.",
                "(x − 1)² + (y + 2)² = 9 má střed [1; −2].",
                "y + 2 je totéž co y − (−2).",
                "Poloměr je √9 = 3, ne 9.",
                NULL,
            },
        },
        {
            "2 / 3   •   Elipsa", "Dvě poloosy",
            "Tip: ve tvaru x²/a² + y²/b² = 1 je a poloosa na ose x.",
            {
                "Elipsa v osové poloze se středem v počátku má rovnici x²/a² + y²/b² = 1.",
                "U x²/25 + y²/9 = 1 je a = 5 a b = 3.",
                "Delší poloosa je hlavní, kratší vedlejší.",
                "Vrcholy na ose x jsou [±a; 0], na ose y [0; ±b].",
                NULL,
            },
        },
        {
            "3 / 3   •   Excentricita", "Vzdálenost ohniska od středu",
            "Tip: pro a > b je e = √(a² − b²).",
            {
                "Lineární výstřednost e splňuje e² = a² − b², když je a hlavní poloosa.",
                "√(25 − 9) = √16 = 4.",
                "Ohniska elipsy jsou [±e; 0].",
                "Součet vzdáleností bodu elipsy od obou ohnisek je 2a.",
                NULL,
            },
        },
    };

    return math_unit_page(4, &mat4_lessons[2], "mat4_unit3", "mat4_unit3_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat4_unit3_exercise_page(void) {
    static const MathItem qs[] = {
        {"Kružnice má rovnici (x − 1)² + (y + 2)² = 9. Napište souřadnice středu jako x;y.", "pair:1;-2", "Střed je [1; −2]. Napište 1;-2."},
        {"Jaký je poloměr kružnice (x − 1)² + (y + 2)² = 9?", "num:3", "Pravá strana je r², takže r = 3. Výsledek je 3."},
        {"Elipsa má rovnici x²/25 + y²/9 = 1. Jak dlouhá je poloosa a na ose x?", "num:5", "a² = 25, takže a = 5. Výsledek je 5."},
        {"Elipsa má rovnici x²/25 + y²/9 = 1. Jak dlouhá je poloosa b na ose y?", "num:3", "b² = 9, takže b = 3. Výsledek je 3."},
        {"Elipsa má a = 5 a b = 3. Jaká je její lineární výstřednost e?", "num:4", "e = √(25 − 9) = √16 = 4. Výsledek je 4."},
    };

    return math_practice_page(4, 2, "mat4unit3",
                              "mat4_ex3_title", "mat4_quiz3_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat4_unit4_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Tvar", "a + bi",
            "Tip: i² = −1. Reálnou a imaginární část pište oddělené středníkem.",
            {
                "Komplexní číslo a + bi má reálnou část a a imaginární část b.",
                "i je imaginární jednotka a i² = −1.",
                "Čísla se sčítají po složkách.",
                "(2 + 3i) + (1 − i) = 3 + 2i.",
                NULL,
            },
        },
        {
            "2 / 3   •   Násobení", "Jako dvojčlen",
            "Tip: (1 + i)(1 − i) je rozdíl čtverců.",
            {
                "(a + bi)(c + di) = ac + adi + bci + bdi².",
                "bdi² = −bd, proto se imaginární jednotka ve výsledku nemusí objevit.",
                "(1 + i)(1 − i) = 1 − (i)² = 1 − (−1) = 2.",
                "Komplexní čísla lze násobit i v goniometrickém tvaru sčítáním argumentů.",
                NULL,
            },
        },
        {
            "3 / 3   •   Absolutní hodnota", "Vzdálenost od nuly",
            "Tip: |a + bi| = √(a² + b²).",
            {
                "Absolutní hodnota je vzdálenost obrazu čísla od počátku Gaussovy roviny.",
                "|3 + 4i| = 5.",
                "|z| je vždy reálné nezáporné číslo.",
                "i² = −1, i³ = −i a i⁴ = 1.",
                NULL,
            },
        },
    };

    return math_unit_page(4, &mat4_lessons[3], "mat4_unit4", "mat4_unit4_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat4_unit4_exercise_page(void) {
    static const MathItem qs[] = {
        {"Sečtěte 2 + 3i a 1 − i. Napište reálnou a imaginární část středníkem.", "pair:3;2", "(2 + 1) + (3 − 1)i = 3 + 2i. Napište 3;2."},
        {"Spočítejte (1 + i)(1 − i).", "num:2", "1 − i² = 1 − (−1) = 2. Výsledek je 2."},
        {"Jaká je absolutní hodnota čísla 3 + 4i?", "num:5", "√(9 + 16) = 5. Výsledek je 5."},
        {"Kolik je i²?", "num:-1", "Imaginární jednotka splňuje i² = −1. Výsledek je -1."},
    };

    return math_practice_page(4, 3, "mat4unit4",
                              "mat4_ex4_title", "mat4_quiz4_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat4_unit5_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Mocnina", "(xⁿ)' = n xⁿ⁻¹",
            "Tip: derivace je směrnice tečny.",
            {
                "Derivace funkce v bodě je směrnice tečny grafu v tom bodě.",
                "(x³)' = 3x². V bodě x = 2 je 3 · 4 = 12.",
                "(5x²)' = 10x. V bodě x = 1 je 10.",
                "Derivace konstanty je 0 a konstanta se vytýká.",
                NULL,
            },
        },
        {
            "2 / 3   •   Tečna", "Směrnice v bodě",
            "Tip: u f(x) = x² je f'(x) = 2x.",
            {
                "Tečna ke grafu y = x² v bodě x = 1 má směrnici 2.",
                "Rovnice tečny používá bod [1; 1] a tuto směrnici.",
                "Kladná derivace znamená, že funkce v bodě roste.",
                "Záporná derivace znamená, že funkce klesá.",
                NULL,
            },
        },
        {
            "3 / 3   •   Sinus", "Derivace sinu je kosinus",
            "Tip: (sin x)' = cos x a cos 0 = 1.",
            {
                "Derivace funkce sin x je cos x.",
                "Derivace funkce cos x je −sin x.",
                "V bodě x = 0 je derivace sinu rovna cos 0 = 1.",
                "Úhel v tomto vzorci je v radiánech, ale nula je nula v obou jednotkách.",
                NULL,
            },
        },
    };

    return math_unit_page(4, &mat4_lessons[4], "mat4_unit5", "mat4_unit5_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat4_unit5_exercise_page(void) {
    static const MathItem qs[] = {
        {"Jaká je derivace funkce x³ v bodě x = 2?", "num:12", "(x³)' = 3x² a 3 · 4 = 12. Výsledek je 12."},
        {"Jaká je derivace funkce 5x² v bodě x = 1?", "num:10", "(5x²)' = 10x a v jedničce je 10. Výsledek je 10."},
        {"Jakou směrnici má tečna ke grafu y = x² v bodě x = 1?", "num:2", "Derivace 2x je v bodě 1 rovna 2. Výsledek je 2."},
        {"Jaká je derivace funkce sin x v bodě x = 0? Derivace sinu je kosinus.", "num:1", "(sin x)' = cos x a cos 0 = 1. Výsledek je 1."},
    };

    return math_practice_page(4, 4, "mat4unit5",
                              "mat4_ex5_title", "mat4_quiz5_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat4_unit6_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Stacionární bod", "Derivace je nula",
            "Tip: f'(x) = 0 je vodorovná tečna.",
            {
                "V lokálním extrému diferencovatelné funkce je derivace nulová.",
                "U f(x) = x³ − 3x je f'(x) = 3x² − 3.",
                "3(x² − 1) = 0 dává x = −1 a x = 1.",
                "To jsou jediní kandidáti na lokální extrém.",
                NULL,
            },
        },
        {
            "2 / 3   •   Maximum a minimum", "Znaménko druhé derivace",
            "Tip: f''(x) = 6x. Záporná druhá derivace znamená lokální maximum.",
            {
                "V x = −1 je f''(−1) = −6 < 0, takže jde o lokální maximum.",
                "f(−1) = −1 + 3 = 2.",
                "V x = 1 je f''(1) = 6 > 0, takže jde o lokální minimum.",
                "f(1) = 1 − 3 = −2.",
                NULL,
            },
        },
        {
            "3 / 3   •   Jiný příklad", "Lineární derivace",
            "Tip: g'(x) = 2x − 4 je nula při x = 2.",
            {
                "Kde je derivace nula, má graf vodorovnou tečnu.",
                "Rovnice 2x − 4 = 0 má řešení x = 2.",
                "Pro x < 2 je derivace 2x − 4 záporná, funkce klesá.",
                "Pro x > 2 je derivace kladná, funkce roste.",
                NULL,
            },
        },
    };

    return math_unit_page(4, &mat4_lessons[5], "mat4_unit6", "mat4_unit6_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat4_unit6_exercise_page(void) {
    static const MathItem qs[] = {
        {"Funkce f(x) = x³ − 3x. Ve kterém x má lokální maximum?", "num:-1", "f'(x) = 3x² − 3 = 0 pro x = ±1 a v −1 je maximum. Výsledek je -1."},
        {"Funkce f(x) = x³ − 3x. Jaká je hodnota lokálního maxima?", "num:2", "f(−1) = −1 + 3 = 2. Výsledek je 2."},
        {"Funkce f(x) = x³ − 3x. Ve kterém x má lokální minimum?", "num:1", "Druhá derivace 6x je v x = 1 kladná. Výsledek je 1."},
        {"Derivace funkce je g'(x) = 2x − 4. Pro které x je derivace nulová?", "num:2", "2x − 4 = 0 dává x = 2. Výsledek je 2."},
    };

    return math_practice_page(4, 5, "mat4unit6",
                              "mat4_ex6_title", "mat4_quiz6_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat4_unit7_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Primitivní funkce", "Opačný postup k derivaci",
            "Tip: primitivní funkce k xⁿ je xⁿ⁺¹/(n + 1) pro n ≠ −1.",
            {
                "Neurčitý integrál je množina primitivních funkcí.",
                "Liší se o konstantu, protože derivace konstanty je nula.",
                "Primitivní funkce k 2x je x².",
                "Primitivní funkce k x² je x³/3.",
                NULL,
            },
        },
        {
            "2 / 3   •   Určitý integrál", "Newtonův vzorec",
            "Tip: ∫ od a do b z f je F(b) − F(a).",
            {
                "Určitý integrál nezáporné funkce je obsah plochy pod grafem.",
                "∫ od 0 do 3 z 2x dx = [x²] od 0 do 3 = 9.",
                "∫ od 0 do 2 z x² dx = [x³/3] od 0 do 2 = 8/3.",
                "Dolní mez 0 u těchto mocnin často vynuluje první člen.",
                NULL,
            },
        },
        {
            "3 / 3   •   Konstanta", "Násobek lze vytknout",
            "Tip: integrál z c · f je c krát integrál z f.",
            {
                "∫ od 0 do 1 z 3x² dx = [x³] od 0 do 1 = 1.",
                "∫ od 0 do 2 z 6x dx = [3x²] od 0 do 2 = 12.",
                "Záporná funkce dává záporný integrál, nejde vždy o obsah beze znaménka.",
                "Součet funkcí se integruje člen po členu.",
                NULL,
            },
        },
    };

    return math_unit_page(4, &mat4_lessons[6], "mat4_unit7", "mat4_unit7_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat4_unit7_exercise_page(void) {
    static const MathItem qs[] = {
        {"Spočítejte integrál od 0 do 3 z funkce 2x.", "num:9", "Primitivní funkce je x² a 9 − 0 = 9. Výsledek je 9."},
        {"Spočítejte integrál od 0 do 2 z funkce x².", "num:8/3", "Primitivní funkce je x³/3 a 8/3 − 0 = 8/3. Výsledek je 8/3."},
        {"Spočítejte integrál od 0 do 1 z funkce 3x².", "num:1", "Primitivní funkce je x³ a 1 − 0 = 1. Výsledek je 1."},
        {"Spočítejte integrál od 0 do 2 z funkce 6x.", "num:12", "Primitivní funkce je 3x² a 12 − 0 = 12. Výsledek je 12."},
    };

    return math_practice_page(4, 6, "mat4unit7",
                              "mat4_ex7_title", "mat4_quiz7_head",
                              qs, G_N_ELEMENTS(qs));
}

static GtkWidget *build_mat4_unit8_page(void) {
    static const NetSlide slides[] = {
        {
            "1 / 3   •   Číslo", "Logaritmus a zlomek",
            "Tip: log₂(8) = 3, protože 2³ = 8.",
            {
                "Logaritmus vrací exponent.",
                "1/2 + 1/3 = 5/6.",
                "Společný jmenovatel se hledá i v maturitním příkladu stejně jako na základní škole.",
                "Výsledek se krátí.",
                NULL,
            },
        },
        {
            "2 / 3   •   Počet a vektor", "Kombinace a velikost",
            "Tip: C(6, 2) = 15 a |(3; 4)| = 5.",
            {
                "C(6, 2) = (6 · 5) / 2 = 15.",
                "Pořadí vybrané dvojice se nepočítá dvakrát.",
                "Velikost vektoru (3; 4) je přepona trojúhelníku 3-4-5.",
                "Skalární součin by u kolmých vektorů vyšel nula.",
                NULL,
            },
        },
        {
            "3 / 3   •   Rovnice", "x² − 1 = 0",
            "Tip: rozdíl čtverců (x − 1)(x + 1).",
            {
                "x² − 1 = 0 má kořeny −1 a 1.",
                "Oba se napíšou, oddělené středníkem.",
                "Diskriminant je 0 − 4 · 1 · (−1) = 4.",
                "Zkouška: 1 − 1 = 0 a 1 − 1 = 0.",
                NULL,
            },
        },
    };

    return math_unit_page(4, &mat4_lessons[7], "mat4_unit8", "mat4_unit8_sub",
                         slides, G_N_ELEMENTS(slides));
}

static GtkWidget *build_mat4_unit8_exercise_page(void) {
    static const MathItem qs[] = {
        {"Spočítejte logaritmus o základu 2 z 8.", "num:3", "2³ = 8. Výsledek je 3."},
        {"Kolik je kombinací 2 prvků ze 6?", "num:15", "C(6, 2) = 15. Výsledek je 15."},
        {"Jaká je velikost vektoru (3; 4)?", "num:5", "√(9 + 16) = 5. Výsledek je 5."},
        {"Spočítejte 1/2 + 1/3.", "num:5/6", "3/6 + 2/6 = 5/6. Výsledek je 5/6."},
        {"Najděte kořeny rovnice x² − 1 = 0. Oddělte je středníkem.", "set:-1;1", "(x − 1)(x + 1) = 0. Napište -1;1."},
    };

    return math_practice_page(4, 7, "mat4unit8",
                              "mat4_ex8_title", "mat4_quiz8_head",
                              qs, G_N_ELEMENTS(qs));
}

void add_mat4_pages(GtkStack *stack) {
    typedef GtkWidget *(*MathBuilder)(void);
    static const struct {
        const char *unit_name;
        const char *ex_name;
        MathBuilder build_unit;
        MathBuilder build_ex;
    } pages[] = {
        {"mat4unit1", "mat4ex1", build_mat4_unit1_page, build_mat4_unit1_exercise_page},
        {"mat4unit2", "mat4ex2", build_mat4_unit2_page, build_mat4_unit2_exercise_page},
        {"mat4unit3", "mat4ex3", build_mat4_unit3_page, build_mat4_unit3_exercise_page},
        {"mat4unit4", "mat4ex4", build_mat4_unit4_page, build_mat4_unit4_exercise_page},
        {"mat4unit5", "mat4ex5", build_mat4_unit5_page, build_mat4_unit5_exercise_page},
        {"mat4unit6", "mat4ex6", build_mat4_unit6_page, build_mat4_unit6_exercise_page},
        {"mat4unit7", "mat4ex7", build_mat4_unit7_page, build_mat4_unit7_exercise_page},
        {"mat4unit8", "mat4ex8", build_mat4_unit8_page, build_mat4_unit8_exercise_page},
    };

    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {
        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);
        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);
    }
}
