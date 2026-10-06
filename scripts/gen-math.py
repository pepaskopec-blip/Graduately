#!/usr/bin/env python3
"""Generate the mathematics lessons.

Numeric answers are built from fractions.Fraction, then checked by
src/math_check.c. Slide text is Czech, like the other subjects.
"""
from __future__ import annotations

import subprocess
import sys
from fractions import Fraction
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "src"


def show(value) -> str:
    fr = Fraction(value)
    if fr.denominator == 1:
        return str(fr.numerator)
    return f"{fr.numerator}/{fr.denominator}"


class Prob:
    def __init__(self, prompt: str, spec: str, hint: str, draw: str | None = None):
        self.prompt = prompt
        self.spec = spec
        self.hint = hint
        self.draw = draw


def N(prompt: str, value, how: str) -> Prob:
    fr = Fraction(value)
    return Prob(prompt, "num:" + show(fr), f"{how} Výsledek je {show(fr)}.")


def PI(prompt: str, coeff, how: str) -> Prob:
    fr = Fraction(coeff)
    if fr == 1:
        spec = "num:pi"
        shown = "π"
    elif fr == -1:
        spec = "num:-pi"
        shown = "-π"
    elif fr.denominator == 1:
        spec = f"num:{fr.numerator}*pi"
        shown = f"{fr.numerator}π"
    else:
        spec = f"num:{show(fr)}*pi"
        shown = f"({show(fr)})π"
    return Prob(prompt, spec, f"{how} Výsledek je {shown}.")


def SQRT(prompt: str, spec: str, how: str) -> Prob:
    if not spec.startswith("num:"):
        raise ValueError(spec)
    return Prob(prompt, spec, how)


def PAIR(prompt: str, a, b, how: str) -> Prob:
    return Prob(prompt, f"pair:{show(a)};{show(b)}",
                f"{how} Napište {show(a)};{show(b)}.")


def SET(prompt: str, values, how: str) -> Prob:
    body = ";".join(show(v) for v in values)
    return Prob(prompt, "set:" + body, f"{how} Napište {body}.")


def DRAW(prompt: str, spec: str, how: str, sample: str) -> Prob:
    return Prob(prompt, spec, how, draw=sample)


def c_escape(text: str) -> str:
    return text.replace("\\", "\\\\").replace('"', '\\"')


def c_string(text: str) -> str:
    if "\n" in text:
        raise ValueError(text)
    return '"' + c_escape(text) + '"'


def lesson(title_cs, title_en, sub_cs, sub_en, slides, problems):
    return {
        "title_cs": title_cs,
        "title_en": title_en,
        "sub_cs": sub_cs,
        "sub_en": sub_en,
        "slides": slides,
        "problems": problems,
    }


def S(kicker, title, tip, lines):
    if len(lines) < 3 or len(lines) > 7:
        raise ValueError(title)
    return (kicker, title, tip, lines)


def year0():
    return [
        lesson(
            "Pořadí operací", "Order of operations",
            "Závorky, násobení a dělení, sčítání a odčítání.",
            "Brackets, then multiplication and division, then addition and subtraction.",
            [
                S("1 / 3   •   Pravidlo", "Co se počítá dřív",
                  "Tip: násobení a dělení jsou stejně silné a jdou zleva doprava.",
                  [
                      "Nejprve se vyřeší závorky, zevnitř ven.",
                      "Pak jsou mocniny a odmocniny.",
                      "Násobení a dělení mají přednost před sčítáním a odčítáním.",
                      "Sčítání a odčítání se také provádějí zleva doprava.",
                  ]),
                S("2 / 3   •   Bez závorek", "Násobení má přednost",
                  "Tip: 2 + 3 · 4 není 20.",
                  [
                      "Ve výrazu 2 + 3 · 4 se nejdřív násobí: 3 · 4 = 12.",
                      "Teprve potom se přičtou dvě: 2 + 12 = 14.",
                      "Ve výrazu 20 − 6 / 2 se nejdřív dělí: 6 / 2 = 3.",
                      "Pak 20 − 3 = 17.",
                  ]),
                S("3 / 3   •   Závorky", "Závorka změní pořadí",
                  "Tip: závorka se chová jako jeden počet.",
                  [
                      "(8 − 3) · 2 začíná v závorce: 5 · 2 = 10.",
                      "3 · (4 + 1) je 3 · 5 = 15.",
                      "Bez závorky by 3 · 4 + 1 vyšlo 13.",
                      "Závorku proto pište vždy, když má sčítání předběhnout násobení.",
                  ]),
            ],
            [
                N("Spočítejte 2 + 3 · 4.", 2 + 3 * 4,
                  "Násobení má přednost: 3 · 4 = 12 a 2 + 12 = 14."),
                N("Spočítejte (8 − 3) · 2.", (8 - 3) * 2,
                  "Nejdřív závorka: 8 − 3 = 5, potom 5 · 2 = 10."),
                N("Spočítejte 20 − 6 / 2.", 20 - Fraction(6, 2),
                  "Nejdřív dělení: 6 / 2 = 3, potom 20 − 3 = 17."),
                N("Spočítejte 3 · (4 + 1).", 3 * (4 + 1),
                  "Nejdřív závorka: 4 + 1 = 5, potom 3 · 5 = 15."),
            ],
        ),
        lesson(
            "Zlomky", "Fractions",
            "Sčítání, odčítání, násobení a dělení zlomků.",
            "Adding, subtracting, multiplying and dividing fractions.",
            [
                S("1 / 3   •   Součet", "Společný jmenovatel",
                  "Tip: zlomek v základním tvaru má nesoudělný čitatel a jmenovatel.",
                  [
                      "Sčítat a odčítat lze zlomky se stejným jmenovatelem.",
                      "Jinak se jmenovatelé rozšíří na nejmenší společný násobek.",
                      "1/2 + 1/3 má společný jmenovatel 6: 3/6 + 2/6 = 5/6.",
                      "3/4 − 1/4 = 2/4 = 1/2.",
                  ]),
                S("2 / 3   •   Součin", "Čitatel s čitatelem",
                  "Tip: před násobením jde často krátit křížem.",
                  [
                      "Zlomky se násobí čitatel čitatelem a jmenovatel jmenovatelem.",
                      "2/3 · 3/5 = 6/15 = 2/5.",
                      "Krátit se smí jen čitatel se jmenovatelem, ne dvě horní čísla mezi sebou.",
                      "Smíšené číslo se před počtem převede na zlomek.",
                  ]),
                S("3 / 3   •   Podíl", "Dělit znamená násobit převráceným",
                  "Tip: převrácený zlomek k a/b je b/a.",
                  [
                      "Dělení zlomkem je násobení jeho převrácenou hodnotou.",
                      "(3/4) : (1/2) = 3/4 · 2/1 = 6/4 = 3/2.",
                      "Výsledek se uvede v základním tvaru.",
                      "Nulou se dělit nesmí, proto jmenovatel nesmí vyjít 0.",
                  ]),
            ],
            [
                N("Spočítejte 1/2 + 1/3. Zlomek v základním tvaru.",
                  Fraction(1, 2) + Fraction(1, 3),
                  "Společný jmenovatel je 6: 3/6 + 2/6 = 5/6."),
                N("Spočítejte 3/4 − 1/4.",
                  Fraction(3, 4) - Fraction(1, 4),
                  "Jmenovatel už je stejný: 2/4 = 1/2."),
                N("Spočítejte 2/3 · 3/5.",
                  Fraction(2, 3) * Fraction(3, 5),
                  "6/15 se zkrátí třemi na 2/5."),
                N("Spočítejte (3/4) : (1/2).",
                  Fraction(3, 4) / Fraction(1, 2),
                  "Násobí se převráceným zlomkem: 3/4 · 2/1 = 3/2."),
            ],
        ),
        lesson(
            "Desetinná čísla", "Decimals",
            "Sčítání, násobení a dělení desetinných čísel.",
            "Adding, multiplying and dividing decimals.",
            [
                S("1 / 3   •   Zápis", "Čárka i tečka",
                  "Tip: 0,5 je totéž co 1/2.",
                  [
                      "Desetinná čárka odděluje celou část od desetin, setin a tisícin.",
                      "0,1 je jedna desetina, 0,01 je jedna setina.",
                      "Při sčítání se čárky píšou pod sebe.",
                      "2,5 + 1,5 = 4.",
                  ]),
                S("2 / 3   •   Násobení", "Čárka se posouvá",
                  "Tip: násobení deseti posune čárku o jedno místo doprava.",
                  [
                      "Počet desetinných míst součinu je součet počtů míst činitelů.",
                      "0,2 · 5 = 1, protože dvě desetiny pětkrát jsou jedna celá.",
                      "1,25 − 0,5 = 0,75, což je 3/4.",
                      "Výsledek lze psát desetinně i zlomkem.",
                  ]),
                S("3 / 3   •   Dělení", "Dělit desetinným číslem",
                  "Tip: dělitel se nejdřív upraví na celé číslo.",
                  [
                      "3,6 : 0,3 se rozšíří deseti na 36 : 3 = 12.",
                      "Čárka se v děliteli i děleném posune o stejný počet míst.",
                      "Dělení deseti posune čárku o jedno místo doleva.",
                      "Nulou se ani u desetinných čísel dělit nesmí.",
                  ]),
            ],
            [
                N("Spočítejte 2,5 + 1,5.", Fraction("2.5") + Fraction("1.5"),
                  "Pět desetin a pět desetin je jedna celá, celkem 4."),
                N("Spočítejte 0,2 · 5.", Fraction("0.2") * 5,
                  "Dvě desetiny pětkrát jsou 10 desetin, tedy 1."),
                N("Spočítejte 3,6 : 0,3.", Fraction("3.6") / Fraction("0.3"),
                  "Posun čárky dá 36 : 3 = 12."),
                N("Spočítejte 1,25 − 0,5.", Fraction("1.25") - Fraction("0.5"),
                  "1,25 − 0,50 = 0,75, tedy 3/4."),
            ],
        ),
        lesson(
            "Procenta", "Percentages",
            "Procento ze základu, počet procent a cena po slevě.",
            "A percentage of a base, the rate, and a price after a discount.",
            [
                S("1 / 3   •   Základ", "Jedno procento je setina",
                  "Tip: pište jen číslo, znak procenta ne.",
                  [
                      "1 % ze základu je jedna setina základu.",
                      "p % ze základu z je (p/100) · z.",
                      "20 % ze 150 je 0,2 · 150 = 30.",
                      "Základ, procentová část a počet procent jsou tři veličiny.",
                  ]),
                S("2 / 3   •   Kolik procent", "Část dělená základem",
                  "Tip: výsledek 0,25 znamená 25 %.",
                  [
                      "Počet procent je (část / základ) · 100.",
                      "15 ze 60 je 15/60 = 1/4, tedy 25 %.",
                      "Do odpovědi se píše 25, ne 0,25 a ne 25 %.",
                      "Zlomek se na procenta násobí stem.",
                  ]),
                S("3 / 3   •   Sleva a základ", "Dopočítání celku",
                  "Tip: sleva 10 % nechá z ceny 90 %.",
                  [
                      "Po slevě 10 % zbývá 90 % původní ceny.",
                      "400 − 10 % je 0,9 · 400 = 360.",
                      "Když 20 % základu je 18, je 1 % rovno 18/20.",
                      "Celý základ je pak 100 takových dílů: 90.",
                  ]),
            ],
            [
                N("Kolik je 20 % ze 150? Napište jen číslo.",
                  Fraction(20, 100) * 150,
                  "20 % je 0,2 a 0,2 · 150 = 30."),
                N("15 je kolik procent z 60? Napište jen číslo, bez znaku %.",
                  Fraction(15, 60) * 100,
                  "15/60 = 1/4 = 25 %."),
                N("Cena 400 se sníží o 10 %. Jaká bude nová cena?",
                  400 * Fraction(90, 100),
                  "Zůstane 90 %: 0,9 · 400 = 360."),
                N("20 % základu je 18. Jak velký je základ?",
                  18 / Fraction(20, 100),
                  "1 % je 18/20 = 0,9 a 100 % je 90."),
            ],
        ),
        lesson(
            "Poměr a měřítko", "Ratio and scale",
            "Dělení v poměru, úměra a měřítko mapy.",
            "Sharing in a ratio, proportion and the scale of a map.",
            [
                S("1 / 3   •   Poměr", "Díly celku",
                  "Tip: poměr 2 : 3 má dohromady 5 dílů.",
                  [
                      "Poměr a : b říká, na kolik stejných dílů se celek rozdělí.",
                      "Počet dílů je a + b.",
                      "První část je a/(a + b) z celku.",
                      "Z 20 v poměru 2 : 3 připadá na první část 8.",
                  ]),
                S("2 / 3   •   Úměra", "Součin vnějších a vnitřních členů",
                  "Tip: 3 : 5 = x : 20 znamená 3/5 = x/20.",
                  [
                      "V úměře a : b = c : d platí a · d = b · c.",
                      "Z 3/5 = x/20 vyjde x = 3/5 · 20 = 12.",
                      "Stejný poměr lze rozšiřovat i krátit.",
                      "Neznámý člen se dopočítá jedním násobením a jedním dělením.",
                  ]),
                S("3 / 3   •   Měřítko", "Mapa a skutečnost",
                  "Tip: 1 : 1000 znamená, že 1 cm na mapě je 10 m ve skutečnosti.",
                  [
                      "Měřítko 1 : k zmenšuje: skutečnost je k-krát větší než plán.",
                      "5 cm na mapě 1 : 1000 je 5000 cm, tedy 50 m.",
                      "Měřítko 5 : 1 je zvětšení, obraz je pětkrát větší.",
                      "4 cm v měřítku 5 : 1 odpovídá obrazu 20 cm.",
                  ]),
            ],
            [
                N("Číslo 20 rozdělte v poměru 2 : 3. Jak velká je první část?",
                  20 * Fraction(2, 5),
                  "Dílů je 5 a první část jsou 2 z nich: 2/5 · 20 = 8."),
                N("Na mapě 1 : 1000 jsou 5 cm. Kolik metrů je to ve skutečnosti?",
                  Fraction(5 * 1000, 100),
                  "5 cm · 1000 = 5000 cm = 50 m."),
                N("Platí 3 : 5 = x : 20. Kolik je x?",
                  Fraction(3, 5) * 20,
                  "x = 3/5 · 20 = 12."),
                N("Úsečka 4 cm se zobrazí v měřítku 5 : 1. Jak dlouhý je obraz v centimetrech?",
                  4 * 5,
                  "Měřítko 5 : 1 zvětšuje pětkrát: 4 · 5 = 20."),
            ],
        ),
        lesson(
            "Záporná čísla", "Negative numbers",
            "Sčítání, násobení a absolutní hodnota.",
            "Addition, multiplication and absolute value.",
            [
                S("1 / 3   •   Číselná osa", "Vpravo roste, vlevo klesá",
                  "Tip: přičíst kladné číslo znamená posun doprava.",
                  [
                      "Kladná čísla leží vpravo od nuly, záporná vlevo.",
                      "−3 + 8 je posun z −3 o 8 kroků doprava, tedy na 5.",
                      "Odčítání záporného čísla je totéž co přičítání kladného.",
                      "6 − (−2) = 6 + 2 = 8.",
                  ]),
                S("2 / 3   •   Násobení", "Znaménka",
                  "Tip: dvě záporná znaménka dají kladný součin.",
                  [
                      "Součin kladného a záporného čísla je záporný.",
                      "Součin dvou záporných čísel je kladný.",
                      "(−4) · (−5) = 20.",
                      "Stejné pravidlo platí pro dělení.",
                  ]),
                S("3 / 3   •   Absolutní hodnota", "Vzdálenost od nuly",
                  "Tip: absolutní hodnota nikdy není záporná.",
                  [
                      "|a| je vzdálenost čísla a od nuly na ose.",
                      "|−7| = 7 a |7| = 7.",
                      "|0| = 0.",
                      "Absolutní hodnota smaže znaménko, ale číslo jinak nezmění.",
                  ]),
            ],
            [
                N("Spočítejte −3 + 8.", -3 + 8,
                  "Z −3 se jde o 8 doprava, na 5."),
                N("Spočítejte (−4) · (−5).", (-4) * (-5),
                  "Součin dvou záporných čísel je kladný: 20."),
                N("Spočítejte |−7|.", 7,
                  "Vzdálenost −7 od nuly je 7."),
                N("Spočítejte 6 − (−2).", 6 - (-2),
                  "Odčítat −2 znamená přičíst 2: 6 + 2 = 8."),
            ],
        ),
        lesson(
            "Lineární rovnice", "Linear equations",
            "Ekvivalentní úpravy a jednoduché rovnice.",
            "Equivalent steps and simple equations.",
            [
                S("1 / 3   •   Rovnováha", "Stejně na obě strany",
                  "Tip: co uděláte vlevo, udělejte i vpravo.",
                  [
                      "Rovnice říká, že levá strana se rovná pravé.",
                      "K oběma stranám lze přičíst stejné číslo.",
                      "Obě strany lze vynásobit stejným nenulovým číslem.",
                      "Cílem je osamostatnit neznámou.",
                  ]),
                S("2 / 3   •   Kroky", "Nejdřív číslo, pak koeficient",
                  "Tip: 2x − 4 = 10 se nejdřív zbaví čtyřky.",
                  [
                      "x + 5 = 12 se odečtením pěti změní na x = 7.",
                      "3x = 15 se vydělí třemi: x = 5.",
                      "2x − 4 = 10 dá po přičtení čtyř 2x = 14, tedy x = 7.",
                      "Zkouška dosadí výsledek do původní rovnice.",
                  ]),
                S("3 / 3   •   Dělení neznámé", "Zlomek s x",
                  "Tip: x/2 = 6 se násobí dvěma.",
                  [
                      "x/2 je totéž co (1/2) · x.",
                      "Násobení obou stran dvěma dá x = 12.",
                      "Násobit nulou se nesmí, rovnice by ztratila význam.",
                      "U lineární rovnice s nenulovým koeficientem je řešení jedno.",
                  ]),
            ],
            [
                N("Vyřešte x + 5 = 12. Napište x.", 12 - 5,
                  "Od obou stran se odečte 5."),
                N("Vyřešte 3x = 15. Napište x.", Fraction(15, 3),
                  "Obě strany se vydělí 3."),
                N("Vyřešte 2x − 4 = 10. Napište x.", Fraction(10 + 4, 2),
                  "2x = 14, takže x = 7."),
                N("Vyřešte x/2 = 6. Napište x.", 6 * 2,
                  "Obě strany se násobí 2."),
            ],
        ),
        lesson(
            "Obvod, obsah a čtverec", "Perimeter, area and a square",
            "Obdélník, trojúhelník a doplnění čtverce na mřížce.",
            "A rectangle, a triangle, and completing a square on the grid.",
            [
                S("1 / 3   •   Obdélník", "Obvod a obsah",
                  "Tip: obvod je součet všech stran, obsah je součin dvou sousedních.",
                  [
                      "Obdélník a × b má obvod 2 · (a + b).",
                      "Obdélník 5 × 3 má obvod 2 · 8 = 16.",
                      "Obsah je a · b. Obdélník 6 × 4 má obsah 24.",
                      "Jednotky obvodu jsou délkové, jednotky obsahu čtvereční.",
                  ]),
                S("2 / 3   •   Trojúhelník", "Základna a výška",
                  "Tip: výška je kolmá na základnu, ne délka šikmé strany.",
                  [
                      "Součet vnitřních úhlů trojúhelníku je 180°.",
                      "Třetí úhel je 180° minus součet dvou známých.",
                      "Úhly 60° a 50° doplňuje 70°.",
                      "Obsah je (základna · výška) / 2. Pro 8 a 5 je to 20.",
                  ]),
                S("3 / 3   •   Čtverec", "Doplnění na mřížce",
                  "Tip: osa y míří nahoru. Klepnutím na bod ho odeberete.",
                  [
                      "Čtverec má čtyři shodné strany a čtyři pravé úhly.",
                      "Druhé dva vrcholy vzniknou posunutím strany o stejnou délku kolmo.",
                      "U vodorovné strany leží zbývající vrcholy přímo nad ní, nebo pod ní.",
                      "Na mřížce jsou platné jen oba vrcholy téže strany.",
                  ]),
            ],
            [
                N("Obdélník má strany 5 a 3. Jaký je jeho obvod?",
                  2 * (5 + 3),
                  "2 · (5 + 3) = 16."),
                N("Obdélník má strany 6 a 4. Jaký je jeho obsah?",
                  6 * 4,
                  "Obsah je 6 · 4 = 24."),
                N("Trojúhelník má základnu 8 a výšku 5. Jaký je jeho obsah?",
                  Fraction(8 * 5, 2),
                  "Obsah je (8 · 5) / 2 = 20."),
                N("Dva úhly trojúhelníku jsou 60° a 50°. Jak velký je třetí úhel ve stupních?",
                  180 - 60 - 50,
                  "Součet je 180°, takže třetí úhel je 70°."),
                DRAW("Na mřížce je strana čtverce od A = [2; 3] k B = [5; 3]. Doplňte zbývající dva vrcholy.",
                     "square:2,3;5,3",
                     "Strana má délku 3. Druhé dva vrcholy jsou [2; 6] a [5; 6], nebo [2; 0] a [5; 0].",
                     "5,6;2,6"),
            ],
        ),
    ]


def year1():
    return [
        lesson(
            "Dělitelnost a prvočísla", "Divisibility and primes",
            "Největší společný dělitel, násobek a rozklad.",
            "Highest common factor, lowest common multiple and factorisation.",
            [
                S("1 / 3   •   Dělitel", "Čísla, která dělí beze zbytku",
                  "Tip: prvočíslo má právě dva dělitele, jedničku a sebe.",
                  [
                      "Dělitel čísla a je celé číslo, kterým a dělí beze zbytku.",
                      "Číslo 12 má dělitele 1, 2, 3, 4, 6 a 12, tedy šest dělitelů.",
                      "Prvočíslo nelze rozložit na součin dvou menších celých čísel větších než 1.",
                      "Jednička prvočíslo není.",
                  ]),
                S("2 / 3   •   NSD a NSN", "Společný dělitel a násobek",
                  "Tip: NSD · NSN dvou čísel je jejich součin.",
                  [
                      "Největší společný dělitel je největší číslo, které dělí obě čísla.",
                      "NSD(24, 36) = 12.",
                      "Nejmenší společný násobek je nejmenší kladné číslo, které je násobkem obou.",
                      "NSN(6, 8) = 24.",
                  ]),
                S("3 / 3   •   Rozklad", "Součin mocnin prvočísel",
                  "Tip: společné prvočinitele berte v nejmenší mocnině pro NSD.",
                  [
                      "Každé celé číslo větší než 1 má jediný rozklad na prvočísla.",
                      "72 = 2³ · 3².",
                      "V NSD se bere nižší exponent, v NSN vyšší.",
                      "Rozklad usnadní krácení zlomků i hledání společného jmenovatele.",
                  ]),
            ],
            [
                N("Jaký je největší společný dělitel čísel 24 a 36?", 12,
                  "Společní dělitelé jsou 1, 2, 3, 4, 6 a 12."),
                N("Jaký je nejmenší společný násobek čísel 6 a 8?", 24,
                  "Násobky 6 jsou 6, 12, 18, 24, … a 24 je první z nich dělitelné osmi."),
                N("Spočítejte 2³ · 3².", (2 ** 3) * (3 ** 2),
                  "8 · 9 = 72."),
                N("Kolik kladných dělitelů má číslo 12?", 6,
                  "Jsou to 1, 2, 3, 4, 6 a 12."),
            ],
        ),
        lesson(
            "Mocniny a odmocniny", "Powers and roots",
            "Mocniny se stejným základem a druhá odmocnina.",
            "Powers with the same base and the square root.",
            [
                S("1 / 3   •   Mocnina", "Opakované násobení",
                  "Tip: aⁿ je a násobené sebou n-krát.",
                  [
                      "2⁵ = 2 · 2 · 2 · 2 · 2 = 32.",
                      "10³ = 1000, tři nuly za jedničkou.",
                      "Každé číslo na první je ono samo a a⁰ = 1 pro a ≠ 0.",
                      "Záporný exponent znamená převrácenou hodnotu mocniny.",
                  ]),
                S("2 / 3   •   Pravidla", "Stejný základ",
                  "Tip: (aᵐ)ⁿ = aᵐ·ⁿ.",
                  [
                      "aᵐ · aⁿ = aᵐ⁺ⁿ.",
                      "aᵐ : aⁿ = aᵐ⁻ⁿ pro a ≠ 0.",
                      "(a · b)ⁿ = aⁿ · bⁿ.",
                      "(2³)² = 2⁶ = 64.",
                  ]),
                S("3 / 3   •   Odmocnina", "Druhá odmocnina",
                  "Tip: √a je nezáporné číslo, jehož druhá mocnina je a.",
                  [
                      "√81 = 9, protože 9² = 81.",
                      "√0 = 0 a √1 = 1.",
                      "Ze záporného čísla se v reálných číslech druhá odmocnina nedělá.",
                      "√(a²) = |a|, ne vždy a.",
                  ]),
            ],
            [
                N("Spočítejte 2⁵.", 2 ** 5, "Pět dvojkových činitelů je 32."),
                N("Spočítejte 10³.", 10 ** 3, "10 · 10 · 10 = 1000."),
                N("Spočítejte √81.", 9, "9 · 9 = 81."),
                N("Spočítejte (2³)².", (2 ** 3) ** 2, "(8)² = 64, nebo 2⁶ = 64."),
            ],
        ),
        lesson(
            "Algebraické výrazy", "Algebraic expressions",
            "Dosazení, druhá mocnina součtu a rozklad rozdílu čtverců.",
            "Substitution, the square of a sum and a difference of squares.",
            [
                S("1 / 3   •   Dosazení", "Písmeno je číslo",
                  "Tip: nejdřív závorky, potom mocniny a násobení.",
                  [
                      "Do výrazu se za proměnnou dosadí dané číslo.",
                      "2(x + 3) při x = 4 je 2 · 7 = 14.",
                      "3x − 2x + 5 se nejdřív sloučí na x + 5.",
                      "Při x = 2 je x + 5 = 7.",
                  ]),
                S("2 / 3   •   Vzorce", "Druhá mocnina dvojčlenu",
                  "Tip: (a + b)² není a² + b².",
                  [
                      "(a + b)² = a² + 2ab + b².",
                      "(a − b)² = a² − 2ab + b².",
                      "a² − b² = (a − b)(a + b).",
                      "Pro a = 3 a b = 1 je (a + b)² = 16.",
                  ]),
                S("3 / 3   •   Zkrácení", "Rozdíl čtverců",
                  "Tip: krátit lze jen pro x, která nejsou kořenem jmenovatele.",
                  [
                      "x² − 9 = (x − 3)(x + 3).",
                      "(x² − 9)/(x − 3) = x + 3 pro x ≠ 3.",
                      "Při x = 5 je hodnota 8.",
                      "Dosazení x = 3 by dělilo nulou, výraz tam není definován.",
                  ]),
            ],
            [
                N("Do výrazu 2(x + 3) dosažte x = 4.", 2 * (4 + 3),
                  "2 · (4 + 3) = 14."),
                N("Do výrazu (a + b)² dosažte a = 3 a b = 1.", (3 + 1) ** 2,
                  "4² = 16. Rozepsáno 9 + 6 + 1 = 16."),
                N("Do výrazu 3x − 2x + 5 dosažte x = 2.", 3 * 2 - 2 * 2 + 5,
                  "Výraz je x + 5, při x = 2 je 7."),
                N("Do výrazu (x² − 9)/(x − 3) dosažte x = 5.",
                  Fraction(5 ** 2 - 9, 5 - 3),
                  "Pro x ≠ 3 je výraz roven x + 3, tedy 8."),
            ],
        ),
        lesson(
            "Lineární rovnice s úpravami", "Linear equations",
            "Rovnice s koeficientem, závorkou a zlomkem.",
            "Equations with a coefficient, a bracket and a fraction.",
            [
                S("1 / 3   •   Převod", "Neznámé na jednu stranu",
                  "Tip: člen s x se převádí s opačným znaménkem.",
                  [
                      "2x + 1 = 9 se odečtením jedné změní na 2x = 8.",
                      "x = 4.",
                      "Členy s x patří obvykle vlevo, čísla vpravo.",
                      "Po každé úpravě zůstává rovnost zachovaná.",
                  ]),
                S("2 / 3   •   Obě strany", "Stejný člen vlevo i vpravo",
                  "Tip: 5x − 3 = 2x + 9 se zbaví 2x odečtením.",
                  [
                      "5x − 2x = 9 + 3.",
                      "3x = 12 a x = 4.",
                      "Zkouška: levá strana 5 · 4 − 3 = 17, pravá 8 + 9 = 17.",
                      "Když koeficient u x vyjde 0, rovnice buď nemá řešení, nebo jich má nekonečně mnoho.",
                  ]),
                S("3 / 3   •   Závorka a zlomek", "Nejdřív roznásobit",
                  "Tip: x/2 + 1 = 5 se nejdřív zbaví jedničky.",
                  [
                      "x/2 + 1 = 5 dá x/2 = 4, tedy x = 8.",
                      "3(x − 2) = 12 se vydělí třemi: x − 2 = 4.",
                      "Nebo se závorka roznásobí: 3x − 6 = 12.",
                      "Obě cesty dají x = 6.",
                  ]),
            ],
            [
                N("Vyřešte 2x + 1 = 9. Napište x.", Fraction(9 - 1, 2),
                  "2x = 8, takže x = 4."),
                N("Vyřešte 5x − 3 = 2x + 9. Napište x.", Fraction(9 + 3, 5 - 2),
                  "3x = 12, takže x = 4."),
                N("Vyřešte x/2 + 1 = 5. Napište x.", (5 - 1) * 2,
                  "x/2 = 4, takže x = 8."),
                N("Vyřešte 3(x − 2) = 12. Napište x.", 12 / 3 + 2,
                  "x − 2 = 4, takže x = 6."),
            ],
        ),
        lesson(
            "Slovní úlohy", "Word problems",
            "Dráha, společná práce, směs a sleva.",
            "Distance, joint work, a mixture and a discount.",
            [
                S("1 / 3   •   Dráha", "Rychlost krát čas",
                  "Tip: jednotky času a rychlosti musí patřit k sobě.",
                  [
                      "Při stálé rychlosti je dráha s = v · t.",
                      "60 km/h po dobu 2,5 h urazí 150 km.",
                      "2,5 h je dvě a půl hodiny, ne dvě hodiny a padesát minut.",
                      "Z dráhy a času se rychlost dopočítá jako s/t.",
                  ]),
                S("2 / 3   •   Práce a směs", "Sčítají se výkony, ne časy",
                  "Tip: kdo práci udělá za 3 hodiny, udělá za hodinu třetinu.",
                  [
                      "První pracovník udělá za hodinu 1/6 práce, druhý 1/3.",
                      "Společně udělají 1/6 + 2/6 = 1/2 práce za hodinu, tedy celou za 2 hodiny.",
                      "U směsi se sčítá množství čisté látky.",
                      "2 kg dvacetiprocentní a 3 kg desetiprocentní směsi obsahují 0,4 + 0,3 = 0,7 kg, tedy 14 %.",
                  ]),
                S("3 / 3   •   Sleva", "Nová cena je část základu",
                  "Tip: po slevě 20 % zbývá 80 % původní ceny.",
                  [
                      "Když je nová cena 160 a sleva byla 20 %, je 160 rovno 80 % původní ceny.",
                      "Původní cena je 160 / 0,8 = 200.",
                      "Sleva v korunách je pak 40.",
                      "Nepleťte si slevu 20 % s tím, že se k nové ceně přičte 20 %.",
                  ]),
            ],
            [
                N("Auto jede 2,5 hodiny rychlostí 60 km/h. Kolik kilometrů ujede?",
                  Fraction("2.5") * 60,
                  "s = v · t = 60 · 2,5 = 150."),
                N("První pracovník práci udělá za 6 hodin, druhý za 3 hodiny. Za kolik hodin ji udělají společně?",
                  1 / (Fraction(1, 6) + Fraction(1, 3)),
                  "Za hodinu spolu udělají 1/2 práce, celou tedy za 2 hodiny."),
                N("Smíchají se 2 kg směsi s 20 % látky a 3 kg směsi s 10 % látky. Kolik procent látky má směs? Napište jen číslo.",
                  (2 * 20 + 3 * 10) / 5,
                  "Čisté látky je 0,7 kg z 5 kg, tedy 14 %."),
                N("Po slevě 20 % stojí zboží 160. Jaká byla původní cena?",
                  160 / Fraction(80, 100),
                  "160 je 80 % původní ceny, základ je 200."),
            ],
        ),
        lesson(
            "Planimetrie", "Plane figures",
            "Úhly trojúhelníku, vnější úhel a pravidelný mnohoúhelník.",
            "Angles of a triangle, the exterior angle and a regular polygon.",
            [
                S("1 / 3   •   Trojúhelník", "Součet 180°",
                  "Tip: rovnoramenný trojúhelník má dvě ramena a dva úhly u základny shodné.",
                  [
                      "Součet vnitřních úhlů každého trojúhelníku je 180°.",
                      "V rovnoramenném trojúhelníku jsou úhly při základně shodné.",
                      "Když jsou úhly při základně 70°, je úhel proti základně 40°.",
                      "Rovnostranný trojúhelník má všechny strany shodné a každý úhel 60°.",
                  ]),
                S("2 / 3   •   Vnější úhel", "Součet dvou vzdálených vnitřních",
                  "Tip: vnější úhel a sousední vnitřní úhel dávají 180°.",
                  [
                      "Vnější úhel vznikne prodloužením jedné strany.",
                      "Je roven součtu dvou vnitřních úhlů, které s ním nesousedí.",
                      "Vnitřní úhly 40° a 60° dávají vnější úhel 100° u zbývajícího vrcholu.",
                      "Součet vnějšího úhlu a přilehlého vnitřního je 180°.",
                  ]),
                S("3 / 3   •   Mnohoúhelník", "Pravidelný šestiúhelník",
                  "Tip: součet vnitřních úhlů n-úhelníku je (n − 2) · 180°.",
                  [
                      "Trojúhelník má součet 180°, čtyřúhelník 360°.",
                      "Obecný vzorec je (n − 2) · 180°.",
                      "Pravidelný mnohoúhelník má všechny strany i úhly shodné.",
                      "Pravidelný šestiúhelník má vnitřní úhel 720° / 6 = 120°.",
                  ]),
            ],
            [
                N("Vnější úhel trojúhelníku je součet vnitřních úhlů 40° a 60°. Jak je velký ve stupních?",
                  40 + 60,
                  "Vnější úhel se rovná součtu dvou nesousedních vnitřních úhlů: 100°."),
                N("Rovnoramenný trojúhelník má úhly při základně 70°. Jak velký je úhel proti základně?",
                  180 - 70 - 70,
                  "180° − 140° = 40°."),
                N("Rovnostranný trojúhelník má stranu 6. Jaký je jeho obvod?",
                  3 * 6,
                  "Tři shodné strany: 3 · 6 = 18."),
                N("Jak velký je jeden vnitřní úhel pravidelného šestiúhelníku ve stupních?",
                  (6 - 2) * 180 / 6,
                  "Součet je (6 − 2) · 180° = 720° a jeden úhel je 120°."),
            ],
        ),
        lesson(
            "Obvody a obsahy", "Perimeter and area",
            "Kružnice, lichoběžník a Pythagorova věta.",
            "The circle, the trapezium and Pythagoras.",
            [
                S("1 / 3   •   Kružnice", "Obvod a obsah",
                  "Tip: u výsledku s π nechte π ve výrazu a pište ho jako pi.",
                  [
                      "Kružnice s poloměrem r má obvod 2πr.",
                      "Obsah kruhu je πr².",
                      "Pro r = 7 je obvod 14π.",
                      "Pro r = 3 je obsah 9π.",
                  ]),
                S("2 / 3   •   Lichoběžník", "Dvě základny a výška",
                  "Tip: obsah je průměr základen krát výška.",
                  [
                      "Lichoběžník má právě jednu dvojici rovnoběžných stran, základny a a c.",
                      "Obsah je (a + c) · v / 2.",
                      "Základny 4 a 6 a výška 3 dávají obsah 15.",
                      "Výška je kolmá vzdálenost základen.",
                  ]),
                S("3 / 3   •   Pythagoras", "Pravoúhlý trojúhelník",
                  "Tip: přepona je nejdelší strana a leží proti pravému úhlu.",
                  [
                      "V pravoúhlém trojúhelníku je a² + b² = c².",
                      "c je přepona, a a b jsou odvěsny.",
                      "Trojúhelník 5, 12, 13 je pravoúhlý, protože 25 + 144 = 169.",
                      "Přepona je 13.",
                  ]),
            ],
            [
                PI("Jaký je obvod kružnice s poloměrem 7? Nechte výsledek v násobcích π.",
                   2 * 7, "Obvod je 2πr = 14π."),
                PI("Jaký je obsah kruhu s poloměrem 3? Nechte výsledek v násobcích π.",
                   3 ** 2, "Obsah je πr² = 9π."),
                N("Lichoběžník má základny 4 a 6 a výšku 3. Jaký je jeho obsah?",
                  Fraction(4 + 6, 2) * 3,
                  "(4 + 6) · 3 / 2 = 15."),
                N("Pravoúhlý trojúhelník má odvěsny 5 a 12. Jak dlouhá je přepona?",
                  13, "5² + 12² = 25 + 144 = 169 = 13²."),
            ],
        ),
        lesson(
            "Konstrukce trojúhelníku", "Constructing a triangle",
            "Vzdálenost bodů a trojúhelník daný třemi stranami.",
            "The distance between points and a triangle given by three sides.",
            [
                S("1 / 3   •   Body", "Vzdálenost na mřížce",
                  "Tip: u svislé nebo vodorovné úsečky stačí rozdíl souřadnic.",
                  [
                      "Bod v rovině má souřadnice [x; y].",
                      "Vzdálenost [x₁; y₁] a [x₂; y₂] je √((x₂ − x₁)² + (y₂ − y₁)²).",
                      "A = [1; 1] a B = [5; 1] mají vzdálenost 4.",
                      "Osa y na naší mřížce míří nahoru.",
                  ]),
                S("2 / 3   •   SSS", "Trojúhelník ze tří stran",
                  "Tip: třetí vrchol je průsečík dvou kružnic.",
                  [
                      "Konstrukce SSS použije tři strany.",
                      "Z bodu A se rýsuje kružnice o poloměru |AC|, z bodu B o poloměru |BC|.",
                      "Kružnice se protnou ve dvou bodech, pokud trojúhelník existuje.",
                      "Trojúhelníková nerovnost: součet dvou stran je větší než třetí.",
                  ]),
                S("3 / 3   •   Příklad", "Strany 4, 3 a 5",
                  "Tip: 3² + 4² = 5², pravý úhel je u bodu A.",
                  [
                      "A = [1; 1], B = [5; 1], takže |AB| = 4.",
                      "Hledáme C tak, aby |AC| = 3 a |BC| = 5.",
                      "Na mřížce od 0 do 10 a od 0 do 8 leží jen C = [1; 4].",
                      "Druhý průsečík [1; −2] je pod mřížkou.",
                  ]),
            ],
            [
                N("Jaká je vzdálenost bodů A = [1; 1] a B = [5; 1]?",
                  4, "Body mají stejné y, vzdálenost je 5 − 1 = 4."),
                N("Jaký je obsah pravoúhlého trojúhelníku s odvěsnami 3 a 4?",
                  Fraction(3 * 4, 2),
                  "Obsah je (3 · 4) / 2 = 6."),
                N("Jaký je obvod trojúhelníku se stranami 3, 4 a 5?",
                  3 + 4 + 5, "3 + 4 + 5 = 12."),
                DRAW("A = [1; 1] a B = [5; 1] jsou dané. Zakreslete C, pro které je |AC| = 3 a |BC| = 5.",
                     "points:1,4;fix:1,1;fix:5,1",
                     "Kružnice se na mřížce protnou v [1; 4]. Bod [1; −2] už na ní není.",
                     "1,4"),
            ],
        ),
    ]


def year2():
    return [
        lesson(
            "Lineární funkce", "Linear functions",
            "Předpis, průsečík s osou y, směrnice a nulový bod.",
            "The formula, the y-intercept, the slope and a root.",
            [
                S("1 / 3   •   Předpis", "y = ax + b",
                  "Tip: a je směrnice, b je úsek na ose y.",
                  [
                      "Lineární funkce má předpis f(x) = ax + b.",
                      "Grafem je přímka.",
                      "b je hodnota f(0), tedy průsečík s osou y.",
                      "Pro f(x) = 2x − 3 je průsečík s osou y číslo −3.",
                  ]),
                S("2 / 3   •   Směrnice", "O kolik y vzroste",
                  "Tip: směrnice je podíl změny y a změny x.",
                  [
                      "Mezi body [x₁; y₁] a [x₂; y₂] je a = (y₂ − y₁)/(x₂ − x₁).",
                      "Body [1; 2] a [3; 8] dávají směrnici (8 − 2)/(3 − 1) = 3.",
                      "Kladná směrnice znamená růst, záporná pokles.",
                      "Nulová směrnice je vodorovná přímka.",
                  ]),
                S("3 / 3   •   Hodnota a kořen", "Dosazení a rovnice",
                  "Tip: nulový bod je x, pro které je f(x) = 0.",
                  [
                      "f(4) u funkce 2x − 3 je 8 − 3 = 5.",
                      "Rovnice 2x − 6 = 0 má řešení x = 3.",
                      "To je průsečík přímky y = 2x − 6 s osou x.",
                      "Lineární funkce s a ≠ 0 má právě jeden nulový bod.",
                  ]),
            ],
            [
                N("Funkce f(x) = 2x − 3. Kolik je f(4)?", 2 * 4 - 3,
                  "2 · 4 − 3 = 5."),
                N("Jaký je průsečík funkce f(x) = 2x − 3 s osou y?", -3,
                  "f(0) = −3."),
                N("Jaká je směrnice přímky body [1; 2] a [3; 8]?",
                  Fraction(8 - 2, 3 - 1),
                  "(8 − 2)/(3 − 1) = 3."),
                N("Vyřešte 2x − 6 = 0. Napište x.", Fraction(6, 2),
                  "2x = 6, takže x = 3."),
            ],
        ),
        lesson(
            "Soustavy rovnic", "Systems of equations",
            "Dvě lineární rovnice o dvou neznámých.",
            "Two linear equations in two unknowns.",
            [
                S("1 / 3   •   Dvojice", "Jedno x a jedno y",
                  "Tip: výsledek pište x;y, nejdřív x.",
                  [
                      "Soustava dvou lineárních rovnic hledá dvojici [x; y], která vyhovuje oběma.",
                      "Řešením je průsečík dvou přímek.",
                      "Sčítací metoda sečte rovnice tak, aby jedna neznámá zmizela.",
                      "Dosazovací metoda vyjádří jednu neznámou a dosadí ji do druhé rovnice.",
                  ]),
                S("2 / 3   •   Sčítání", "Opačné koeficienty",
                  "Tip: x + y = 10 a x − y = 2 se sčítají na 2x = 12.",
                  [
                      "Součet dá 2x = 12, tedy x = 6.",
                      "Z x + y = 10 je y = 4.",
                      "Zkouška ve druhé rovnici: 6 − 4 = 2.",
                      "Odečtení rovnic se použije, když má neznámá v obou stejný koeficient.",
                  ]),
                S("3 / 3   •   Odečtení", "Stejný koeficient",
                  "Tip: rovnice lze před odečtením násobit.",
                  [
                      "2x + y = 8 a x + y = 5 se odečtou na x = 3.",
                      "Pak y = 2.",
                      "3x + y = 10 a x + y = 6 dají 2x = 4, tedy x = 2 a y = 4.",
                      "x + y = 7 a x − y = 1 dají x = 4 a y = 3.",
                  ]),
            ],
            [
                PAIR("Vyřešte soustavu x + y = 10 a x − y = 2. Napište x a y oddělené středníkem.",
                     6, 4, "Sečtení dá 2x = 12, x = 6 a y = 4."),
                PAIR("Vyřešte soustavu 2x + y = 8 a x + y = 5. Napište x a y oddělené středníkem.",
                     3, 2, "Odečtení dá x = 3 a y = 2."),
                PAIR("Vyřešte soustavu x + y = 7 a x − y = 1. Napište x a y oddělené středníkem.",
                     4, 3, "Sečtení dá 2x = 8, x = 4 a y = 3."),
                PAIR("Vyřešte soustavu 3x + y = 10 a x + y = 6. Napište x a y oddělené středníkem.",
                     2, 4, "Odečtení dá 2x = 4, x = 2 a y = 4."),
            ],
        ),
        lesson(
            "Kvadratické rovnice", "Quadratic equations",
            "Rozklad, diskriminant a dvojnásobný kořen.",
            "Factoring, the discriminant and a double root.",
            [
                S("1 / 3   •   Tvar", "ax² + bx + c = 0",
                  "Tip: kořeny pište oddělené středníkem, pořadí nehraje roli.",
                  [
                      "Kvadratická rovnice má a ≠ 0.",
                      "Kořeny rovnice x² − 5x + 6 = 0 jsou 2 a 3, protože (x − 2)(x − 3) = 0.",
                      "x² − 4 = 0 má kořeny −2 a 2.",
                      "Součin je nula, právě když je nula aspoň jeden činitel.",
                  ]),
                S("2 / 3   •   Diskriminant", "D = b² − 4ac",
                  "Tip: D > 0 znamená dva různé reálné kořeny.",
                  [
                      "Pro ax² + bx + c = 0 je D = b² − 4ac.",
                      "U x² − 2x − 3 je a = 1, b = −2, c = −3 a D = 4 + 12 = 16.",
                      "D = 0 znamená jeden dvojnásobný kořen.",
                      "D < 0 znamená, že v reálných číslech řešení není.",
                  ]),
                S("3 / 3   •   Vzorec", "Kořen je (−b ± √D) / (2a)",
                  "Tip: dvojnásobný kořen x² + 6x + 9 = 0 je −3.",
                  [
                      "x = (−b ± √D) / (2a).",
                      "x² + 6x + 9 = (x + 3)², takže kořen −3 je dvojnásobný.",
                      "U dvojnásobného kořene stačí napsat číslo jednou.",
                      "Zkouška dosadí kořen zpět do rovnice.",
                  ]),
            ],
            [
                SET("Najděte kořeny rovnice x² − 5x + 6 = 0. Oddělte je středníkem.",
                    [2, 3], "(x − 2)(x − 3) = 0."),
                SET("Najděte kořeny rovnice x² − 4 = 0. Oddělte je středníkem.",
                    [-2, 2], "x² = 4, takže x = −2 nebo x = 2."),
                N("Jaký je diskriminant rovnice x² − 2x − 3 = 0?",
                  (-2) ** 2 - 4 * 1 * (-3),
                  "D = (−2)² − 4 · 1 · (−3) = 4 + 12 = 16."),
                N("Jaký je dvojnásobný kořen rovnice x² + 6x + 9 = 0?",
                  -3, "(x + 3)² = 0, kořen je −3."),
            ],
        ),
        lesson(
            "Lomené výrazy", "Rational expressions",
            "Krácení, definiční obor a násobení zlomků s proměnnou.",
            "Cancelling, the domain and multiplying algebraic fractions.",
            [
                S("1 / 3   •   Obor", "Jmenovatel nesmí být nula",
                  "Tip: podmínku pište dřív, než krátíte.",
                  [
                      "Lomený výraz je podíl dvou mnohočlenů.",
                      "Není definován tam, kde je jmenovatel nula.",
                      "(x + 2)/(x − 2) nemá smysl pro x = 2.",
                      "Po krácení se vyloučené hodnoty do oboru nevracejí.",
                  ]),
                S("2 / 3   •   Krácení", "Rozdíl čtverců",
                  "Tip: x² − 1 = (x − 1)(x + 1).",
                  [
                      "(x² − 1)/(x − 1) = x + 1 pro x ≠ 1.",
                      "Při x = 4 je hodnota 5.",
                      "(x² − 4)/(x + 2) = x − 2 pro x ≠ −2.",
                      "Při x = 5 je hodnota 3.",
                  ]),
                S("3 / 3   •   Násobení", "Čitatel s čitatelem",
                  "Tip: x ve jmenovateli se zkrátí jen pro x ≠ 0.",
                  [
                      "Zlomky se násobí stejně jako číselné.",
                      "(2x/4) · (6/x) = 12x/(4x) = 3 pro x ≠ 0.",
                      "6x/(3x) = 2 pro x ≠ 0.",
                      "Než se zkrátí písmeno, ověří se, že není nula.",
                  ]),
            ],
            [
                N("Do výrazu (x² − 1)/(x − 1) dosažte x = 4.",
                  Fraction(4 ** 2 - 1, 4 - 1),
                  "Pro x ≠ 1 je výraz x + 1, tedy 5."),
                N("Pro které x nemá výraz (x + 2)/(x − 2) smysl?",
                  2, "Jmenovatel x − 2 je nula právě při x = 2."),
                N("Upravte (2x/4) · (6/x) pro x ≠ 0.",
                  3, "12x/(4x) = 3."),
                N("Do výrazu (x² − 4)/(x + 2) dosažte x = 5.",
                  Fraction(25 - 4, 5 + 2),
                  "Pro x ≠ −2 je výraz x − 2, tedy 3."),
            ],
        ),
        lesson(
            "Mocniny s racionálním exponentem", "Rational exponents",
            "Odmocniny, součin mocnin a záporný exponent.",
            "Roots, a product of powers and a negative exponent.",
            [
                S("1 / 3   •   Odmocnina jako exponent", "a^(1/n)",
                  "Tip: a^(1/2) je druhá odmocnina z a.",
                  [
                      "Pro a > 0 je a^(1/n) n-tá odmocnina z a.",
                      "16^(1/2) = 4.",
                      "a^(m/n) je n-tá odmocnina z aᵐ, nebo m-tá mocnina odmocniny.",
                      "8^(2/3) = (8^(1/3))² = 2² = 4.",
                  ]),
                S("2 / 3   •   Stejný základ", "Exponenty se sčítají",
                  "Tip: 3² · 3³ = 3⁵.",
                  [
                      "aᵐ · aⁿ = aᵐ⁺ⁿ.",
                      "3² · 3³ = 3⁵ = 243.",
                      "(aᵐ)ⁿ = aᵐ·ⁿ.",
                      "Základ musí být při sčítání exponentů stejný.",
                  ]),
                S("3 / 3   •   Záporný exponent", "Převrácená hodnota",
                  "Tip: a⁻ⁿ = 1/aⁿ pro a ≠ 0.",
                  [
                      "10⁻² = 1/100.",
                      "10⁻¹ = 0,1 a 10⁰ = 1.",
                      "Záporný exponent nepřevrací znaménko základu, ale celou mocninu.",
                      "(−2)⁻² = 1/4, protože (−2)² = 4.",
                  ]),
            ],
            [
                N("Spočítejte 16^(1/2).", 4, "Druhá odmocnina z 16 je 4."),
                N("Spočítejte 8^(2/3).", 4, "Třetí odmocnina z 8 je 2 a 2² = 4."),
                N("Spočítejte 3² · 3³.", 3 ** 5, "3⁵ = 243."),
                N("Spočítejte 10⁻².", Fraction(1, 100), "10⁻² = 1/10² = 1/100."),
            ],
        ),
        lesson(
            "Goniometrie pravoúhlého trojúhelníku", "Right-triangle trigonometry",
            "Sinus, kosinus a tangens ostrého úhlu.",
            "Sine, cosine and tangent of an acute angle.",
            [
                S("1 / 3   •   Poměry", "Odvěsny a přepona",
                  "Tip: sinus je protilehlá odvěsna ku přeponě.",
                  [
                      "V pravoúhlém trojúhelníku je sin α = protilehlá / přepona.",
                      "cos α = přilehlá / přepona.",
                      "tan α = protilehlá / přilehlá.",
                      "Přepona je strana proti pravému úhlu.",
                  ]),
                S("2 / 3   •   Tabulkové úhly", "30°, 45° a 60°",
                  "Tip: sinus 30° je 1/2.",
                  [
                      "sin 30° = 1/2 a cos 60° = 1/2.",
                      "sin 60° = cos 30° = √3/2.",
                      "sin 45° = cos 45° = √2/2.",
                      "tan 45° = 1.",
                  ]),
                S("3 / 3   •   Dopočet strany", "Přepona a úhel",
                  "Tip: proti úhlu 30° je polovina přepony.",
                  [
                      "Známe-li přeponu a úhel, protilehlá odvěsna je přepona · sin α.",
                      "Proti 30° při přeponě 10 je odvěsna 5.",
                      "Přilehlá odvěsna by byla 10 · cos 30°.",
                      "Tangens se hodí, když jsou známé dvě odvěsny a ne přepona.",
                  ]),
            ],
            [
                N("Kolik je sin 30°?", Fraction(1, 2),
                  "Sinus 30° je 1/2."),
                N("Kolik je cos 60°?", Fraction(1, 2),
                  "Kosinus 60° je 1/2."),
                N("Kolik je tan 45°?", 1,
                  "Protilehlá a přilehlá odvěsna jsou u 45° stejně dlouhé."),
                N("Přepona pravoúhlého trojúhelníku je 10 a jeden úhel je 30°. Jak dlouhá je odvěsna proti tomuto úhlu?",
                  10 * Fraction(1, 2),
                  "10 · sin 30° = 10 · 1/2 = 5."),
            ],
        ),
        lesson(
            "Pythagorova a Eukleidovy věty", "Pythagoras and Euclid",
            "Přepona, obsah čtverce nad ní a úseky přepony.",
            "The hypotenuse, the square on it, and the projections on it.",
            [
                S("1 / 3   •   Pythagoras", "a² + b² = c²",
                  "Tip: 8² + 15² = 64 + 225 = 289 = 17².",
                  [
                      "V pravoúhlém trojúhelníku se součet čtverců nad odvěsnami rovná čtverci nad přeponou.",
                      "Trojúhelník 8, 15, 17 je pravoúhlý a přepona je 17.",
                      "Čtverec nad přeponou trojúhelníku 3, 4, 5 má obsah 25.",
                      "Věta platí jen v pravoúhlém trojúhelníku.",
                  ]),
                S("2 / 3   •   Eukleides", "Úsek přepony",
                  "Tip: a² = c · a_c, kde a_c je úsek přepony přilehlý k odvěsně a.",
                  [
                      "Odvěsna je geometrický průměr přepony a svého úseku.",
                      "a² = c · a_c a b² = c · b_c.",
                      "Úseky a_c a b_c dají dohromady přeponu c.",
                      "U odvěsen 6 a 8 je přepona 10 a úsek u odvěsny 6 je 36/10 = 18/5.",
                  ]),
                S("3 / 3   •   Výška", "v² = a_c · b_c",
                  "Tip: také platí a · b = c · v.",
                  [
                      "Výška k přeponě je geometrický průměr obou úseků.",
                      "Z a · b = c · v plyne v = ab/c.",
                      "Pro odvěsny 6 a 8 a přeponu 10 je v = 48/10 = 24/5.",
                      "Obsah trojúhelníku je pak (c · v) / 2, stejně jako (a · b) / 2.",
                  ]),
            ],
            [
                N("Pravoúhlý trojúhelník má odvěsny 8 a 15. Jak dlouhá je přepona?",
                  17, "64 + 225 = 289 = 17²."),
                N("Jaký obsah má čtverec sestrojený nad přeponou pravoúhlého trojúhelníku se stranami 3, 4 a 5?",
                  25, "Přepona je 5 a 5² = 25."),
                N("Odvěsny jsou 6 a 8, přepona je 10. Jak dlouhý je úsek přepony přilehlý k odvěsně 6?",
                  Fraction(36, 10),
                  "a² = c · a_c, takže a_c = 36/10 = 18/5."),
                N("Odvěsny jsou 6 a 8, přepona je 10. Jak dlouhá je výška k přeponě?",
                  Fraction(6 * 8, 10),
                  "v = ab/c = 48/10 = 24/5."),
            ],
        ),
        lesson(
            "Kružnice a Thaletova věta", "Circles and Thales",
            "Obvodový a středový úhel, délka oblouku a pravý úhel nad průměrem.",
            "An inscribed and a central angle, arc length, and the right angle in a semicircle.",
            [
                S("1 / 3   •   Úhly", "Obvodový je polovina středového",
                  "Tip: oba úhly musí být nad stejným obloukem.",
                  [
                      "Středový úhel má vrchol ve středu kružnice.",
                      "Obvodový úhel má vrchol na kružnici.",
                      "Nad stejným obloukem je obvodový úhel polovinou středového.",
                      "Středový úhel 80° patří k obvodovému úhlu 40°.",
                  ]),
                S("2 / 3   •   Oblouk", "Část obvodu",
                  "Tip: délka oblouku je (α/360°) · 2πr.",
                  [
                      "Celý obvod je 2πr.",
                      "Oblouk 60° je šestina obvodu.",
                      "Při r = 6 je jeho délka 2π.",
                      "Výsledek nechte v násobcích π.",
                  ]),
                S("3 / 3   •   Thales", "Pravý úhel nad průměrem",
                  "Tip: vrchol pravého úhlu leží na kružnici s průměrem AB.",
                  [
                      "Thaletova věta: úhel nad průměrem je pravý.",
                      "Je-li AB průměr, je úhel ACB pravý pro každé C na kružnici různé od A a B.",
                      "Na mřížce má průměr od [2; 4] do [8; 4] střed [5; 4] a poloměr 3.",
                      "Vyhovují body [5; 7] a [5; 1].",
                  ]),
            ],
            [
                N("Středový úhel je 80°. Jak velký je obvodový úhel nad stejným obloukem ve stupních?",
                  40, "Obvodový úhel je polovina středového."),
                PI("Kružnice má poloměr 6. Jak dlouhý je oblouk 60°? Nechte π ve výsledku.",
                   2, "(60/360) · 2π · 6 = 2π."),
                N("Obvodový úhel je 35°. Jak velký je středový úhel nad stejným obloukem ve stupních?",
                  70, "Středový úhel je dvojnásobek obvodového."),
                DRAW("AB je průměr od [2; 4] do [8; 4]. Zakreslete bod C na kružnici, ve kterém je úhel ACB pravý.",
                     "right:2,4;8,4",
                     "Thaletova kružnice má střed [5; 4] a poloměr 3. Na mřížce vyhovuje [5; 7] nebo [5; 1].",
                     "5,7"),
            ],
        ),
    ]


def year3():
    return [
        lesson(
            "Kvadratická funkce", "The quadratic function",
            "Vrchol, kořeny a hodnota paraboly.",
            "The vertex, the roots and a value of a parabola.",
            [
                S("1 / 3   •   Parabola", "f(x) = ax² + bx + c",
                  "Tip: pro a > 0 má parabola minimum, pro a < 0 maximum.",
                  [
                      "Grafem kvadratické funkce je parabola.",
                      "Průsečík s osou y je bod [0; c].",
                      "U f(x) = x² − 4x + 3 je f(0) = 3.",
                      "Osa paraboly je svislá.",
                  ]),
                S("2 / 3   •   Vrchol", "x = −b/(2a)",
                  "Tip: souřadnice vrcholu pište x;y.",
                  [
                      "x-ová souřadnice vrcholu je −b/(2a).",
                      "U x² − 4x + 3 je to −(−4)/2 = 2.",
                      "f(2) = 4 − 8 + 3 = −1.",
                      "Vrchol je tedy [2; −1].",
                  ]),
                S("3 / 3   •   Kořeny", "Průsečíky s osou x",
                  "Tip: kořeny jsou řešení f(x) = 0.",
                  [
                      "x² − 4x + 3 = (x − 1)(x − 3).",
                      "Kořeny jsou 1 a 3.",
                      "Leží symetricky kolem osy paraboly.",
                      "f(4) = 16 − 16 + 3 = 3, stejně jako f(0).",
                  ]),
            ],
            [
                PAIR("Najděte vrchol funkce f(x) = x² − 4x + 3. Napište x a y oddělené středníkem.",
                     2, -1, "x = 4/2 = 2 a f(2) = −1."),
                SET("Najděte kořeny funkce f(x) = x² − 4x + 3. Oddělte je středníkem.",
                    [1, 3], "(x − 1)(x − 3) = 0."),
                N("Kolik je f(0) u funkce f(x) = x² − 4x + 3?", 3,
                  "f(0) = 3, to je absolutní člen."),
                N("Kolik je f(4) u funkce f(x) = x² − 4x + 3?", 16 - 16 + 3,
                  "16 − 16 + 3 = 3."),
            ],
        ),
        lesson(
            "Exponenciální funkce a logaritmus", "Exponentials and logarithms",
            "Mocnina se základem a logaritmus jako její opak.",
            "A power and the logarithm as its inverse.",
            [
                S("1 / 3   •   Definice", "Logaritmus je exponent",
                  "Tip: log_b(x) = y právě tehdy, když bʸ = x.",
                  [
                      "Základ logaritmu je kladný a různý od 1.",
                      "Argument logaritmu je kladný.",
                      "log₁₀(1000) = 3, protože 10³ = 1000.",
                      "log₂(32) = 5, protože 2⁵ = 32.",
                  ]),
                S("2 / 3   •   Rovnice", "Stejný základ",
                  "Tip: 2ˣ = 16 se přepíše jako 2ˣ = 2⁴.",
                  [
                      "Exponentialní rovnice se stejným základem porovná exponenty.",
                      "2ˣ = 16 má řešení x = 4.",
                      "log₃(81) = 4, protože 3⁴ = 81.",
                      "log_b(b) = 1 a log_b(1) = 0.",
                  ]),
                S("3 / 3   •   Pravidla", "Součin a podíl",
                  "Tip: logaritmus součinu je součet logaritmů.",
                  [
                      "log_b(xy) = log_b(x) + log_b(y).",
                      "log_b(x/y) = log_b(x) − log_b(y).",
                      "log_b(xⁿ) = n · log_b(x).",
                      "Těmito pravidly se argument zjednodušuje, ne základ.",
                  ]),
            ],
            [
                N("Spočítejte logaritmus o základu 10 z 1000.", 3,
                  "10³ = 1000, takže logaritmus je 3."),
                N("Spočítejte logaritmus o základu 2 z 32.", 5,
                  "2⁵ = 32."),
                N("Vyřešte 2ˣ = 16. Napište x.", 4,
                  "16 = 2⁴, takže x = 4."),
                N("Spočítejte logaritmus o základu 3 z 81.", 4,
                  "3⁴ = 81."),
            ],
        ),
        lesson(
            "Goniometrické funkce", "Trigonometric functions",
            "Sinus a kosinus na jednotkové kružnici.",
            "Sine and cosine on the unit circle.",
            [
                S("1 / 3   •   Jednotková kružnice", "Úhel od kladné poloosy x",
                  "Tip: sinus je y-ová souřadnice, kosinus x-ová.",
                  [
                      "Bod na jednotkové kružnici pod úhlem α má souřadnice [cos α; sin α].",
                      "cos 0° = 1 a sin 0° = 0.",
                      "sin 90° = 1 a cos 90° = 0.",
                      "cos 180° = −1 a sin 180° = 0.",
                  ]),
                S("2 / 3   •   45°", "Polovina pravého úhlu",
                  "Tip: sin 45° = cos 45° = √2/2.",
                  [
                      "Úhel 45° půlí čtvrtkružnici.",
                      "Souřadnice bodu splňují x = y a x² + y² = 1.",
                      "Kladné řešení je √2/2.",
                      "Odpověď lze psát jako sqrt(2)/2 nebo 1/sqrt(2).",
                  ]),
                S("3 / 3   •   Znaménka", "Čtyři kvadranty",
                  "Tip: v prvním kvadrantu jsou sinus i kosinus kladné.",
                  [
                      "Ve druhém kvadrantu je sinus kladný a kosinus záporný.",
                      "Ve třetím jsou oba záporné.",
                      "Ve čtvrtém je kosinus kladný a sinus záporný.",
                      "Tangens je sinus dělený kosinem a není definován při cos α = 0.",
                  ]),
            ],
            [
                N("Kolik je sin 90°?", 1, "Bod [0; 1] má y-ovou souřadnici 1."),
                N("Kolik je cos 180°?", -1, "Bod [−1; 0] má x-ovou souřadnici −1."),
                SQRT("Kolik je sin 45°? Můžete psát sqrt(2)/2.",
                     "num:sqrt(2)/2",
                     "sin 45° = √2/2. Stejnou hodnotu má 1/√2."),
                N("Kolik je cos 0°?", 1, "Úhel 0° je bod [1; 0]."),
            ],
        ),
        lesson(
            "Sinová a kosinová věta", "The sine and cosine rules",
            "Obecný trojúhelník, poloměr kružnice a obsah.",
            "A general triangle, the circumradius and the area.",
            [
                S("1 / 3   •   Sinová věta", "a/sin α = 2R",
                  "Tip: poměr strany a sinu protilehlého úhlu je pro všechny strany stejný.",
                  [
                      "a/sin α = b/sin β = c/sin γ = 2R.",
                      "R je poloměr kružnice opsané.",
                      "Když a = 10 a α = 30°, je 10 / (1/2) = 20 = 2R.",
                      "Poloměr R je 10.",
                  ]),
                S("2 / 3   •   Kosinová věta", "c² = a² + b² − 2ab cos γ",
                  "Tip: pro pravý úhel je cos 90° = 0 a věta přejde v Pythagora.",
                  [
                      "Kosinová věta dopočítá stranu proti známému úhlu.",
                      "Jsou-li dvě strany 2 a sevřený úhel 60°, je třetí strana také 2.",
                      "a² = 4 + 4 − 2 · 2 · 2 · cos 60° = 8 − 8 · 1/2 = 4.",
                      "Rovnostranný trojúhelník má všechny úhly 60°.",
                  ]),
                S("3 / 3   •   Obsah", "Dvě strany a sevřený úhel",
                  "Tip: S = (1/2) · b · c · sin α.",
                  [
                      "Obsah je polovina součinu dvou stran a sinu sevřeného úhlu.",
                      "(1/2) · 4 · 6 · sin 30° = 12 · 1/2 = 6.",
                      "Sinová věta také dopočítá další stranu: b = a · sin β / sin α.",
                      "Pro a = 8, α = 30° a β = 45° je b = 8√2.",
                  ]),
            ],
            [
                N("Strana a = 10 leží proti úhlu 30°. Jaký je poloměr kružnice opsané?",
                  10, "a/sin 30° = 20 = 2R, takže R = 10."),
                N("Dvě strany trojúhelníku mají délku 2 a svírají úhel 60°. Jak dlouhá je třetí strana?",
                  2, "a² = 4 + 4 − 8 · 1/2 = 4, takže a = 2."),
                N("Dvě strany mají délky 4 a 6 a svírají úhel 30°. Jaký je obsah trojúhelníku?",
                  Fraction(1, 2) * 4 * 6 * Fraction(1, 2),
                  "(1/2) · 4 · 6 · 1/2 = 6."),
                SQRT("Strana a = 8 je proti 30° a úhel β je 45°. Jak dlouhá je strana b? Sin 45° je √2/2.",
                     "num:8*sqrt(2)",
                     "b = 8 · sin 45° / sin 30° = 8 · (√2/2) / (1/2) = 8√2."),
            ],
        ),
        lesson(
            "Posloupnosti a řady", "Sequences and series",
            "Aritmetická a geometrická posloupnost a nekonečná řada.",
            "An arithmetic and a geometric sequence, and an infinite series.",
            [
                S("1 / 3   •   Aritmetická", "Stálý rozdíl",
                  "Tip: aₙ = a₁ + (n − 1)d.",
                  [
                      "Aritmetická posloupnost přičítá stále stejný rozdíl d.",
                      "Pro a₁ = 3 a d = 2 je a₅ = 3 + 4 · 2 = 11.",
                      "Součet prvních n členů je sₙ = n/2 · (2a₁ + (n − 1)d).",
                      "s₅ = 5/2 · (6 + 8) = 35.",
                  ]),
                S("2 / 3   •   Geometrická", "Stálý kvocient",
                  "Tip: aₙ = a₁ · qⁿ⁻¹.",
                  [
                      "Geometrická posloupnost násobí stále stejným kvocientem q.",
                      "Pro a₁ = 2 a q = 3 je a₄ = 2 · 3³ = 54.",
                      "q = 1 znamená, že se členy nemění.",
                      "q = 0 je po prvním členu samá nula, pokud je první člen definován.",
                  ]),
                S("3 / 3   •   Nekonečná řada", "Součet pro |q| < 1",
                  "Tip: s = a₁ / (1 − q).",
                  [
                      "Nekonečná geometrická řada má součet jen pro |q| < 1.",
                      "Pro a₁ = 6 a q = 1/2 je s = 6 / (1/2) = 12.",
                      "Pro |q| ≥ 1 se součet členů neblíží ke konečnému číslu, kromě případu q = 1 a a₁ = 0.",
                      "Řada 6 + 3 + 3/2 + … se tedy blíží ke 12.",
                  ]),
            ],
            [
                N("Aritmetická posloupnost má a₁ = 3 a d = 2. Kolik je a₅?",
                  3 + 4 * 2, "a₅ = 3 + 4 · 2 = 11."),
                N("Aritmetická posloupnost má a₁ = 3 a d = 2. Kolik je součet prvních pěti členů?",
                  Fraction(5, 2) * (2 * 3 + 4 * 2),
                  "s₅ = 5/2 · (6 + 8) = 35."),
                N("Geometrická posloupnost má a₁ = 2 a q = 3. Kolik je a₄?",
                  2 * (3 ** 3), "a₄ = 2 · 27 = 54."),
                N("Nekonečná geometrická řada má a₁ = 6 a q = 1/2. Jaký je její součet?",
                  6 / Fraction(1, 2), "s = 6 / (1/2) = 12."),
            ],
        ),
        lesson(
            "Kombinatorika", "Combinatorics",
            "Permutace, variace a kombinace.",
            "Permutations, variations and combinations.",
            [
                S("1 / 3   •   Permutace", "Pořadí všech prvků",
                  "Tip: n! = 1 · 2 · … · n a 0! = 1.",
                  [
                      "Permutace je uspořádání všech n různých prvků.",
                      "Počet je n!.",
                      "4! = 24 a 3! = 6.",
                      "Na prvním místě je n možností, na dalším n − 1, a tak dál.",
                  ]),
                S("2 / 3   •   Variace", "Pořadí vybraných prvků",
                  "Tip: variace bez opakování V(n, k) = n! / (n − k)!.",
                  [
                      "Variace vybírá k prvků z n a záleží na pořadí.",
                      "V(5, 2) = 5 · 4 = 20.",
                      "Opakování by dovolilo vybrat stejný prvek víckrát.",
                      "Permutace je variace, která vybírá všech n prvků.",
                  ]),
                S("3 / 3   •   Kombinace", "Pořadí nehraje roli",
                  "Tip: C(n, k) = n! / (k! · (n − k)!).",
                  [
                      "Kombinace vybírá k prvků a dvě vybrané skupiny se stejným obsahem jsou jedna.",
                      "C(5, 2) = 10.",
                      "C(n, k) = C(n, n − k).",
                      "Počet variací je k! krát větší než počet kombinací.",
                  ]),
            ],
            [
                N("Kolik je permutací čtyř různých prvků?", 24,
                  "4! = 24."),
                N("Kolik je kombinací 2 prvků z 5?", 10,
                  "C(5, 2) = (5 · 4) / 2 = 10."),
                N("Kolik je variací 2 prvků z 5 bez opakování?", 20,
                  "V(5, 2) = 5 · 4 = 20."),
                N("Kolik je 3!?", 6, "3 · 2 · 1 = 6."),
            ],
        ),
        lesson(
            "Pravděpodobnost", "Probability",
            "Klasická pravděpodobnost, kostka, mince a karty.",
            "Classical probability with a die, a coin and cards.",
            [
                S("1 / 3   •   Definice", "Příznivé ku všem",
                  "Tip: všechny výsledky musí být stejně možné.",
                  [
                      "Klasická pravděpodobnost je počet příznivých výsledků dělený počtem všech.",
                      "Hodnota leží od 0 do 1.",
                      "Jev jistý má pravděpodobnost 1, jev nemožný 0.",
                      "Na kostce je sudé číslo ve třech případech ze šesti, tedy 1/2.",
                  ]),
                S("2 / 3   •   Nezávislost", "Součin pravděpodobností",
                  "Tip: dvě mince mají čtyři stejně možné dvojice výsledků.",
                  [
                      "U dvou nezávislých mincí jsou možnosti PP, PO, OP a OO.",
                      "Dva panny jsou jedna možnost ze čtyř, tedy 1/4.",
                      "U dvou kostek je 36 stejně možných dvojic.",
                      "Součet 7 nastane u šesti dvojic: 1+6, 2+5, 3+4, 4+3, 5+2 a 6+1.",
                  ]),
                S("3 / 3   •   Karty", "Čtyři esa v balíčku 32 karet",
                  "Tip: mariášový balíček má 32 karet.",
                  [
                      "Pravděpodobnost esa je 4/32 = 1/8.",
                      "Tažení jedné karty považuje každou kartu za stejně možnou.",
                      "Doplňkový jev má pravděpodobnost 1 − p.",
                      "Výsledek se krátí do základního tvaru.",
                  ]),
            ],
            [
                N("Jaká je pravděpodobnost sudého čísla při hodu hrací kostkou?",
                  Fraction(3, 6),
                  "Sudá jsou 2, 4 a 6, tedy 3 ze 6, čili 1/2."),
                N("Jaká je pravděpodobnost, že na dvou mincích padnou dvě panny?",
                  Fraction(1, 4),
                  "Ze čtyř dvojic je příznivá jedna."),
                N("V balíčku 32 karet jsou 4 esa. Jaká je pravděpodobnost, že tažená karta je eso?",
                  Fraction(4, 32),
                  "4/32 = 1/8."),
                N("Jaká je pravděpodobnost, že součet na dvou hracích kostkách je 7?",
                  Fraction(6, 36),
                  "Příznivých dvojic je 6 z 36, tedy 1/6."),
            ],
        ),
        lesson(
            "Stereometrie", "Solid geometry",
            "Krychle, válec, koule a jehlan.",
            "The cube, the cylinder, the sphere and a pyramid.",
            [
                S("1 / 3   •   Krychle", "Objem a povrch",
                  "Tip: krychle o hraně a má objem a³ a povrch 6a².",
                  [
                      "Objem je počet jednotkových krychlí, které se do tělesa vejdou.",
                      "Krychle o hraně 3 má objem 27.",
                      "Povrch je součet obsahů všech stěn: 6 · 9 = 54.",
                      "Jednotky objemu jsou krychlové, jednotky povrchu čtvereční.",
                  ]),
                S("2 / 3   •   Válec a koule", "Vzorce s π",
                  "Tip: výsledek nechte v násobcích π.",
                  [
                      "Objem válce je πr²v.",
                      "Pro r = 2 a v = 5 je objem 20π.",
                      "Objem koule je (4/3)πr³.",
                      "Pro r = 3 je objem (4/3)π · 27 = 36π.",
                  ]),
                S("3 / 3   •   Jehlan", "Třetina hranolu",
                  "Tip: objem jehlanu je (1/3) · obsah podstavy · výška.",
                  [
                      "Jehlan má stejný objem jako třetina hranolu se stejnou podstavou a výškou.",
                      "Podstava o obsahu 12 a výška 4 dávají objem 16.",
                      "Výška je kolmá vzdálenost vrcholu od roviny podstavy.",
                      "Stejný vztah třetiny platí i pro kužel vůči válci.",
                  ]),
            ],
            [
                N("Krychle má hranu 3. Jaký je její objem?", 3 ** 3,
                  "3³ = 27."),
                N("Krychle má hranu 3. Jaký je její povrch?", 6 * (3 ** 2),
                  "Šest stěn o obsahu 9 dává 54."),
                PI("Válec má poloměr 2 a výšku 5. Jaký je jeho objem? Nechte π ve výsledku.",
                   (2 ** 2) * 5, "πr²v = π · 4 · 5 = 20π."),
                PI("Koule má poloměr 3. Jaký je její objem? Nechte π ve výsledku.",
                   Fraction(4, 3) * (3 ** 3),
                   "(4/3)π · 27 = 36π."),
            ],
        ),
    ]


def year4():
    return [
        lesson(
            "Vektory", "Vectors",
            "Velikost, součet, skalární součin a koncový bod.",
            "Magnitude, a sum, the dot product and an endpoint.",
            [
                S("1 / 3   •   Souřadnice", "Velikost a součet",
                  "Tip: velikost vektoru (a; b) je √(a² + b²).",
                  [
                      "Vektor má velikost a směr.",
                      "|(3; 4)| = 5, protože 9 + 16 = 25.",
                      "Vektory se sčítají po souřadnicích.",
                      "(1; 2) + (3; −1) = (4; 1).",
                  ]),
                S("2 / 3   •   Skalární součin", "Číslo, ne vektor",
                  "Tip: (a; b) · (c; d) = ac + bd.",
                  [
                      "Skalární součin dvou vektorů je číslo.",
                      "(1; 2) · (3; 4) = 3 + 8 = 11.",
                      "Nulový skalární součin znamená kolmé vektory.",
                      "Velikost lze psát jako odmocninu ze skalárního součinu vektoru se sebou.",
                  ]),
                S("3 / 3   •   Vzdálenost", "Velikost rozdílu",
                  "Tip: na mřížce osa y míří nahoru.",
                  [
                      "Vzdálenost bodů je velikost vektoru mezi nimi.",
                      "Od [1; 1] do [4; 5] je vektor (3; 4) a vzdálenost 5.",
                      "Koncový bod vektoru (3; 2) umístěného do [1; 1] je [4; 3].",
                      "Pořadí souřadnic je x a potom y.",
                  ]),
            ],
            [
                N("Jaká je velikost vektoru (3; 4)?", 5,
                  "√(9 + 16) = √25 = 5."),
                PAIR("Sečtěte vektory (1; 2) a (3; −1). Napište souřadnice oddělené středníkem.",
                     4, 1, "(1 + 3; 2 + (−1)) = (4; 1)."),
                N("Spočítejte skalární součin vektorů (1; 2) a (3; 4).",
                  1 * 3 + 2 * 4, "1 · 3 + 2 · 4 = 11."),
                N("Jaká je vzdálenost bodů [1; 1] a [4; 5]?", 5,
                  "Rozdíl je (3; 4) a jeho velikost je 5."),
                DRAW("Z bodu A = [1; 1] naneste vektor (3; 2). Zakreslete koncový bod.",
                     "points:4,3;fix:1,1",
                     "Ke každé souřadnici se přičte složka vektoru: [1 + 3; 1 + 2] = [4; 3].",
                     "4,3"),
            ],
        ),
        lesson(
            "Analytická přímka", "The line in the plane",
            "Směrnice, úsek, parametrické vyjádření a vzdálenost od přímky.",
            "Slope, intercept, a parametric equation and the distance from a line.",
            [
                S("1 / 3   •   Směrnice", "y = ax + b",
                  "Tip: a = (y₂ − y₁)/(x₂ − x₁).",
                  [
                      "Body [1; 2] a [4; 8] určují směrnici (8 − 2)/(4 − 1) = 2.",
                      "Přímka procházející bodem [0; 1] se směrnicí 2 má rovnici y = 2x + 1.",
                      "Úsek na ose y je 1.",
                      "Svislá přímka směrnici tohoto tvaru nemá.",
                  ]),
                S("2 / 3   •   Parametr", "Bod a směrový vektor",
                  "Tip: X = A + t · u.",
                  [
                      "Parametrické vyjádření je x = x₀ + t · u₁, y = y₀ + t · u₂.",
                      "Přímka (1; 3) + t(2; 0) má při t = 2 bod [5; 3].",
                      "Každé t dá právě jeden bod přímky.",
                      "Směrový vektor (2; 0) je vodorovný.",
                  ]),
                S("3 / 3   •   Vzdálenost", "Bod od přímky",
                  "Tip: pro ax + by + c = 0 je vzdálenost |ax₀ + by₀ + c| / √(a² + b²).",
                  [
                      "Přímka 3x + 4y − 10 = 0 má a = 3, b = 4 a c = −10.",
                      "Vzdálenost počátku je |−10| / √(9 + 16) = 10/5 = 2.",
                      "Čitatel je absolutní hodnota, vzdálenost není záporná.",
                      "Čtvrtý vrchol rovnoběžníku k bodům A, B a C je B + C − A.",
                  ]),
            ],
            [
                N("Jaká je směrnice přímky body [1; 2] a [4; 8]?",
                  Fraction(8 - 2, 4 - 1),
                  "(8 − 2)/(4 − 1) = 2."),
                N("Přímka má rovnici y = 2x + 1. Jaký je její úsek na ose y?",
                  1, "Úsek je absolutní člen, tedy 1."),
                PAIR("Bod přímky je (1; 3) + t(2; 0). Kam přímka dojde pro t = 2? Napište x a y středníkem.",
                     5, 3, "(1; 3) + 2 · (2; 0) = (5; 3)."),
                N("Jaká je vzdálenost bodu [0; 0] od přímky 3x + 4y − 10 = 0?",
                  2, "|−10| / √(9 + 16) = 10/5 = 2."),
                DRAW("A = [1; 1], B = [4; 1] a C = [2; 3]. Zakreslete čtvrtý vrchol rovnoběžníku, ve kterém jsou AB a AC sousední strany.",
                     "para:1,1;4,1;2,3",
                     "D = B + C − A = [4 + 2 − 1; 1 + 3 − 1] = [5; 3].",
                     "5,3"),
            ],
        ),
        lesson(
            "Kuželosečky", "Conic sections",
            "Kružnice a elipsa v osové poloze.",
            "A circle and an ellipse aligned with the axes.",
            [
                S("1 / 3   •   Kružnice", "Střed a poloměr",
                  "Tip: (x − m)² + (y − n)² = r² má střed [m; n].",
                  [
                      "Kružnice je množina bodů stejně vzdálených od středu.",
                      "(x − 1)² + (y + 2)² = 9 má střed [1; −2].",
                      "y + 2 je totéž co y − (−2).",
                      "Poloměr je √9 = 3, ne 9.",
                  ]),
                S("2 / 3   •   Elipsa", "Dvě poloosy",
                  "Tip: ve tvaru x²/a² + y²/b² = 1 je a poloosa na ose x.",
                  [
                      "Elipsa v osové poloze se středem v počátku má rovnici x²/a² + y²/b² = 1.",
                      "U x²/25 + y²/9 = 1 je a = 5 a b = 3.",
                      "Delší poloosa je hlavní, kratší vedlejší.",
                      "Vrcholy na ose x jsou [±a; 0], na ose y [0; ±b].",
                  ]),
                S("3 / 3   •   Excentricita", "Vzdálenost ohniska od středu",
                  "Tip: pro a > b je e = √(a² − b²).",
                  [
                      "Lineární výstřednost e splňuje e² = a² − b², když je a hlavní poloosa.",
                      "√(25 − 9) = √16 = 4.",
                      "Ohniska elipsy jsou [±e; 0].",
                      "Součet vzdáleností bodu elipsy od obou ohnisek je 2a.",
                  ]),
            ],
            [
                PAIR("Kružnice má rovnici (x − 1)² + (y + 2)² = 9. Napište souřadnice středu jako x;y.",
                     1, -2, "Střed je [1; −2]."),
                N("Jaký je poloměr kružnice (x − 1)² + (y + 2)² = 9?",
                  3, "Pravá strana je r², takže r = 3."),
                N("Elipsa má rovnici x²/25 + y²/9 = 1. Jak dlouhá je poloosa a na ose x?",
                  5, "a² = 25, takže a = 5."),
                N("Elipsa má rovnici x²/25 + y²/9 = 1. Jak dlouhá je poloosa b na ose y?",
                  3, "b² = 9, takže b = 3."),
                N("Elipsa má a = 5 a b = 3. Jaká je její lineární výstřednost e?",
                  4, "e = √(25 − 9) = √16 = 4."),
            ],
        ),
        lesson(
            "Komplexní čísla", "Complex numbers",
            "Součet, součin, absolutní hodnota a mocnina i.",
            "A sum, a product, the modulus and a power of i.",
            [
                S("1 / 3   •   Tvar", "a + bi",
                  "Tip: i² = −1. Reálnou a imaginární část pište oddělené středníkem.",
                  [
                      "Komplexní číslo a + bi má reálnou část a a imaginární část b.",
                      "i je imaginární jednotka a i² = −1.",
                      "Čísla se sčítají po složkách.",
                      "(2 + 3i) + (1 − i) = 3 + 2i.",
                  ]),
                S("2 / 3   •   Násobení", "Jako dvojčlen",
                  "Tip: (1 + i)(1 − i) je rozdíl čtverců.",
                  [
                      "(a + bi)(c + di) = ac + adi + bci + bdi².",
                      "bdi² = −bd, proto se imaginární jednotka ve výsledku nemusí objevit.",
                      "(1 + i)(1 − i) = 1 − (i)² = 1 − (−1) = 2.",
                      "Komplexní čísla lze násobit i v goniometrickém tvaru sčítáním argumentů.",
                  ]),
                S("3 / 3   •   Absolutní hodnota", "Vzdálenost od nuly",
                  "Tip: |a + bi| = √(a² + b²).",
                  [
                      "Absolutní hodnota je vzdálenost obrazu čísla od počátku Gaussovy roviny.",
                      "|3 + 4i| = 5.",
                      "|z| je vždy reálné nezáporné číslo.",
                      "i² = −1, i³ = −i a i⁴ = 1.",
                  ]),
            ],
            [
                PAIR("Sečtěte 2 + 3i a 1 − i. Napište reálnou a imaginární část středníkem.",
                     3, 2, "(2 + 1) + (3 − 1)i = 3 + 2i."),
                N("Spočítejte (1 + i)(1 − i).", 2,
                  "1 − i² = 1 − (−1) = 2."),
                N("Jaká je absolutní hodnota čísla 3 + 4i?", 5,
                  "√(9 + 16) = 5."),
                N("Kolik je i²?", -1, "Imaginární jednotka splňuje i² = −1."),
            ],
        ),
        lesson(
            "Derivace", "The derivative",
            "Derivace mocniny, tečna a derivace sinu.",
            "The derivative of a power, a tangent and the derivative of sine.",
            [
                S("1 / 3   •   Mocnina", "(xⁿ)' = n xⁿ⁻¹",
                  "Tip: derivace je směrnice tečny.",
                  [
                      "Derivace funkce v bodě je směrnice tečny grafu v tom bodě.",
                      "(x³)' = 3x². V bodě x = 2 je 3 · 4 = 12.",
                      "(5x²)' = 10x. V bodě x = 1 je 10.",
                      "Derivace konstanty je 0 a konstanta se vytýká.",
                  ]),
                S("2 / 3   •   Tečna", "Směrnice v bodě",
                  "Tip: u f(x) = x² je f'(x) = 2x.",
                  [
                      "Tečna ke grafu y = x² v bodě x = 1 má směrnici 2.",
                      "Rovnice tečny používá bod [1; 1] a tuto směrnici.",
                      "Kladná derivace znamená, že funkce v bodě roste.",
                      "Záporná derivace znamená, že funkce klesá.",
                  ]),
                S("3 / 3   •   Sinus", "Derivace sinu je kosinus",
                  "Tip: (sin x)' = cos x a cos 0 = 1.",
                  [
                      "Derivace funkce sin x je cos x.",
                      "Derivace funkce cos x je −sin x.",
                      "V bodě x = 0 je derivace sinu rovna cos 0 = 1.",
                      "Úhel v tomto vzorci je v radiánech, ale nula je nula v obou jednotkách.",
                  ]),
            ],
            [
                N("Jaká je derivace funkce x³ v bodě x = 2?", 3 * (2 ** 2),
                  "(x³)' = 3x² a 3 · 4 = 12."),
                N("Jaká je derivace funkce 5x² v bodě x = 1?", 10 * 1,
                  "(5x²)' = 10x a v jedničce je 10."),
                N("Jakou směrnici má tečna ke grafu y = x² v bodě x = 1?", 2,
                  "Derivace 2x je v bodě 1 rovna 2."),
                N("Jaká je derivace funkce sin x v bodě x = 0? Derivace sinu je kosinus.",
                  1, "(sin x)' = cos x a cos 0 = 1."),
            ],
        ),
        lesson(
            "Průběh funkce", "The shape of a function",
            "Stacionární body, lokální extrémy a nulová derivace.",
            "Stationary points, local extrema and a zero derivative.",
            [
                S("1 / 3   •   Stacionární bod", "Derivace je nula",
                  "Tip: f'(x) = 0 je vodorovná tečna.",
                  [
                      "V lokálním extrému diferencovatelné funkce je derivace nulová.",
                      "U f(x) = x³ − 3x je f'(x) = 3x² − 3.",
                      "3(x² − 1) = 0 dává x = −1 a x = 1.",
                      "To jsou jediní kandidáti na lokální extrém.",
                  ]),
                S("2 / 3   •   Maximum a minimum", "Znaménko druhé derivace",
                  "Tip: f''(x) = 6x. Záporná druhá derivace znamená lokální maximum.",
                  [
                      "V x = −1 je f''(−1) = −6 < 0, takže jde o lokální maximum.",
                      "f(−1) = −1 + 3 = 2.",
                      "V x = 1 je f''(1) = 6 > 0, takže jde o lokální minimum.",
                      "f(1) = 1 − 3 = −2.",
                  ]),
                S("3 / 3   •   Jiný příklad", "Lineární derivace",
                  "Tip: g'(x) = 2x − 4 je nula při x = 2.",
                  [
                      "Kde je derivace nula, má graf vodorovnou tečnu.",
                      "Rovnice 2x − 4 = 0 má řešení x = 2.",
                      "Pro x < 2 je derivace 2x − 4 záporná, funkce klesá.",
                      "Pro x > 2 je derivace kladná, funkce roste.",
                  ]),
            ],
            [
                N("Funkce f(x) = x³ − 3x. Ve kterém x má lokální maximum?",
                  -1, "f'(x) = 3x² − 3 = 0 pro x = ±1 a v −1 je maximum."),
                N("Funkce f(x) = x³ − 3x. Jaká je hodnota lokálního maxima?",
                  (-1) ** 3 - 3 * (-1),
                  "f(−1) = −1 + 3 = 2."),
                N("Funkce f(x) = x³ − 3x. Ve kterém x má lokální minimum?",
                  1, "Druhá derivace 6x je v x = 1 kladná."),
                N("Derivace funkce je g'(x) = 2x − 4. Pro které x je derivace nulová?",
                  2, "2x − 4 = 0 dává x = 2."),
            ],
        ),
        lesson(
            "Integrál", "The integral",
            "Určitý integrál mocniny jako obsah pod grafem.",
            "The definite integral of a power as the area under the graph.",
            [
                S("1 / 3   •   Primitivní funkce", "Opačný postup k derivaci",
                  "Tip: primitivní funkce k xⁿ je xⁿ⁺¹/(n + 1) pro n ≠ −1.",
                  [
                      "Neurčitý integrál je množina primitivních funkcí.",
                      "Liší se o konstantu, protože derivace konstanty je nula.",
                      "Primitivní funkce k 2x je x².",
                      "Primitivní funkce k x² je x³/3.",
                  ]),
                S("2 / 3   •   Určitý integrál", "Newtonův vzorec",
                  "Tip: ∫ od a do b z f je F(b) − F(a).",
                  [
                      "Určitý integrál nezáporné funkce je obsah plochy pod grafem.",
                      "∫ od 0 do 3 z 2x dx = [x²] od 0 do 3 = 9.",
                      "∫ od 0 do 2 z x² dx = [x³/3] od 0 do 2 = 8/3.",
                      "Dolní mez 0 u těchto mocnin často vynuluje první člen.",
                  ]),
                S("3 / 3   •   Konstanta", "Násobek lze vytknout",
                  "Tip: integrál z c · f je c krát integrál z f.",
                  [
                      "∫ od 0 do 1 z 3x² dx = [x³] od 0 do 1 = 1.",
                      "∫ od 0 do 2 z 6x dx = [3x²] od 0 do 2 = 12.",
                      "Záporná funkce dává záporný integrál, nejde vždy o obsah beze znaménka.",
                      "Součet funkcí se integruje člen po členu.",
                  ]),
            ],
            [
                N("Spočítejte integrál od 0 do 3 z funkce 2x.",
                  3 ** 2 - 0, "Primitivní funkce je x² a 9 − 0 = 9."),
                N("Spočítejte integrál od 0 do 2 z funkce x².",
                  Fraction(2 ** 3, 3),
                  "Primitivní funkce je x³/3 a 8/3 − 0 = 8/3."),
                N("Spočítejte integrál od 0 do 1 z funkce 3x².",
                  1, "Primitivní funkce je x³ a 1 − 0 = 1."),
                N("Spočítejte integrál od 0 do 2 z funkce 6x.",
                  3 * (2 ** 2),
                  "Primitivní funkce je 3x² a 12 − 0 = 12."),
            ],
        ),
        lesson(
            "Maturitní mix", "Maturita mix",
            "Logaritmus, kombinace, vektor, zlomek a kvadratická rovnice.",
            "A logarithm, a combination, a vector, a fraction and a quadratic equation.",
            [
                S("1 / 3   •   Číslo", "Logaritmus a zlomek",
                  "Tip: log₂(8) = 3, protože 2³ = 8.",
                  [
                      "Logaritmus vrací exponent.",
                      "1/2 + 1/3 = 5/6.",
                      "Společný jmenovatel se hledá i v maturitním příkladu stejně jako na základní škole.",
                      "Výsledek se krátí.",
                  ]),
                S("2 / 3   •   Počet a vektor", "Kombinace a velikost",
                  "Tip: C(6, 2) = 15 a |(3; 4)| = 5.",
                  [
                      "C(6, 2) = (6 · 5) / 2 = 15.",
                      "Pořadí vybrané dvojice se nepočítá dvakrát.",
                      "Velikost vektoru (3; 4) je přepona trojúhelníku 3-4-5.",
                      "Skalární součin by u kolmých vektorů vyšel nula.",
                  ]),
                S("3 / 3   •   Rovnice", "x² − 1 = 0",
                  "Tip: rozdíl čtverců (x − 1)(x + 1).",
                  [
                      "x² − 1 = 0 má kořeny −1 a 1.",
                      "Oba se napíšou, oddělené středníkem.",
                      "Diskriminant je 0 − 4 · 1 · (−1) = 4.",
                      "Zkouška: 1 − 1 = 0 a 1 − 1 = 0.",
                  ]),
            ],
            [
                N("Spočítejte logaritmus o základu 2 z 8.", 3,
                  "2³ = 8."),
                N("Kolik je kombinací 2 prvků ze 6?", 15,
                  "C(6, 2) = 15."),
                N("Jaká je velikost vektoru (3; 4)?", 5,
                  "√(9 + 16) = 5."),
                N("Spočítejte 1/2 + 1/3.", Fraction(1, 2) + Fraction(1, 3),
                  "3/6 + 2/6 = 5/6."),
                SET("Najděte kořeny rovnice x² − 1 = 0. Oddělte je středníkem.",
                    [-1, 1], "(x − 1)(x + 1) = 0."),
            ],
        ),
    ]


YEARS = [
    {
        "prefix": "mat0",
        "year": 0,
        "array": "mat0_lessons",
        "add": "add_mat0_pages",
        "title_key": "mat0_year",
        "title_cs": "Opakování základní školy",
        "title_en": "Elementary-school review",
        "sub_key": "mat0_sub",
        "sub_cs": "Pořadí operací, zlomky, procenta, rovnice a rýsování.",
        "sub_en": "Order of operations, fractions, percentages, equations and a construction.",
        "lessons": year0(),
    },
    {
        "prefix": "mat",
        "year": 1,
        "array": "mat_lessons",
        "add": "add_mat_pages",
        "title_key": "mat_year1",
        "title_cs": "1. ročník",
        "title_en": "Year 1",
        "sub_key": "mat_sub",
        "sub_cs": "Dělitelnost, výrazy, rovnice, planimetrie a konstrukce.",
        "sub_en": "Divisibility, expressions, equations, plane figures and a construction.",
        "lessons": year1(),
    },
    {
        "prefix": "mat2",
        "year": 2,
        "array": "mat2_lessons",
        "add": "add_mat2_pages",
        "title_key": "mat_year2",
        "title_cs": "2. ročník",
        "title_en": "Year 2",
        "sub_key": "mat2_sub",
        "sub_cs": "Funkce, soustavy, kvadratické rovnice, goniometrie a kružnice.",
        "sub_en": "Functions, systems, quadratic equations, trigonometry and circles.",
        "lessons": year2(),
    },
    {
        "prefix": "mat3",
        "year": 3,
        "array": "mat3_lessons",
        "add": "add_mat3_pages",
        "title_key": "mat_year3",
        "title_cs": "3. ročník",
        "title_en": "Year 3",
        "sub_key": "mat3_sub",
        "sub_cs": "Logaritmus, posloupnosti, pravděpodobnost a stereometrie.",
        "sub_en": "Logarithms, sequences, probability and solid geometry.",
        "lessons": year3(),
    },
    {
        "prefix": "mat4",
        "year": 4,
        "array": "mat4_lessons",
        "add": "add_mat4_pages",
        "title_key": "mat_year4",
        "title_cs": "4. ročník",
        "title_en": "Year 4",
        "sub_key": "mat4_sub",
        "sub_cs": "Analytická geometrie, derivace, integrál a maturitní opakování.",
        "sub_en": "Coordinate geometry, derivatives, integrals and a maturita review.",
        "lessons": year4(),
    },
]


def emit_year(info) -> str:
    prefix = info["prefix"]
    lines = [
        '#include "graduately.h"',
        "",
        f"NetLesson {info['array']}[MATH_N] = {{",
    ]
    for i in range(1, 9):
        lines.append(
            f'    {{ .n_slides = 3, .unit_page = "{prefix}unit{i}", .ex_page = "{prefix}ex{i}" }},'
        )
    lines.append("};")
    lines.append("")
    for i, lesson in enumerate(info["lessons"], start=1):
        lines.append(f"static GtkWidget *build_{prefix}_unit{i}_page(void) {{")
        lines.append("    static const NetSlide slides[] = {")
        for kicker, title, tip, body in lesson["slides"]:
            lines.append("        {")
            lines.append(f"            {c_string(kicker)}, {c_string(title)},")
            lines.append(f"            {c_string(tip)},")
            lines.append("            {")
            for row in body:
                lines.append(f"                {c_string(row)},")
            lines.append("                NULL,")
            lines.append("            },")
            lines.append("        },")
        lines.append("    };")
        lines.append("")
        lines.append(
            f'    return math_unit_page({info["year"]}, &{info["array"]}[{i - 1}], '
            f'"{prefix}_unit{i}", "{prefix}_unit{i}_sub",'
        )
        lines.append("                         slides, G_N_ELEMENTS(slides));")
        lines.append("}")
        lines.append("")
        lines.append(f"static GtkWidget *build_{prefix}_unit{i}_exercise_page(void) {{")
        lines.append("    static const MathItem qs[] = {")
        for prob in lesson["problems"]:
            lines.append(
                f"        {{{c_string(prob.prompt)}, {c_string(prob.spec)}, {c_string(prob.hint)}}},"
            )
        lines.append("    };")
        lines.append("")
        lines.append(
            f'    return math_practice_page({info["year"]}, {i - 1}, "{prefix}unit{i}",'
        )
        lines.append(
            f'                              "{prefix}_ex{i}_title", "{prefix}_quiz{i}_head",'
        )
        lines.append("                              qs, G_N_ELEMENTS(qs));")
        lines.append("}")
        lines.append("")
    lines.append(f"void {info['add']}(GtkStack *stack) {{")
    lines.append("    typedef GtkWidget *(*MathBuilder)(void);")
    lines.append("    static const struct {")
    lines.append("        const char *unit_name;")
    lines.append("        const char *ex_name;")
    lines.append("        MathBuilder build_unit;")
    lines.append("        MathBuilder build_ex;")
    lines.append("    } pages[] = {")
    for i in range(1, 9):
        lines.append(
            f'        {{"{prefix}unit{i}", "{prefix}ex{i}", '
            f"build_{prefix}_unit{i}_page, build_{prefix}_unit{i}_exercise_page}},"
        )
    lines.append("    };")
    lines.append("")
    lines.append("    for (guint i = 0; i < G_N_ELEMENTS(pages); i++) {")
    lines.append("        gtk_stack_add_named(stack, pages[i].build_unit(), pages[i].unit_name);")
    lines.append("        gtk_stack_add_named(stack, pages[i].build_ex(), pages[i].ex_name);")
    lines.append("    }")
    lines.append("}")
    lines.append("")
    return "\n".join(lines)


def emit_i18n() -> str:
    rows = [
        ("mat_years_sub",
         "Opakování základní školy a čtyři ročníky gymnázia.",
         "Elementary review and four years of gymnasium mathematics."),
        ("math_quiz_intro",
         "Napište výsledek. Zlomky jako 3/4, desetinnou čárku i tečku, π jako pi, odmocninu jako sqrt(2) nebo √2. Více čísel oddělte středníkem.",
         "Write the result. Fractions as 3/4, a decimal comma or point, π as pi, a root as sqrt(2) or √2. Separate several numbers with a semicolon."),
        ("math_answer_ph", "výsledek", "result"),
        ("math_undo", "Zpět", "Undo"),
        ("math_clear", "Smazat body", "Clear points"),
        ("math_draw_hint", "Klepněte na mřížku. Osa y směřuje nahoru.",
         "Tap the grid. The y-axis points up."),
    ]
    for info in YEARS:
        rows.append((info["title_key"], info["title_cs"], info["title_en"]))
        rows.append((info["sub_key"], info["sub_cs"], info["sub_en"]))
        for i, lesson in enumerate(info["lessons"], start=1):
            p = info["prefix"]
            rows.append((f"{p}_unit{i}", lesson["title_cs"], lesson["title_en"]))
            rows.append((f"{p}_unit{i}_sub", lesson["sub_cs"], lesson["sub_en"]))
            rows.append((f"{p}_ex{i}_title", "Příklady", "Practice"))
            rows.append((f"{p}_quiz{i}_head", lesson["title_cs"], lesson["title_en"]))
    lines = []
    for key, cs, en in rows:
        lines.append(f"    {{{c_string(key)}, {c_string(cs)}, {c_string(en)}}},")
    return "\n".join(lines) + "\n"


def cases() -> list[str]:
    out = []
    for info in YEARS:
        for lesson in info["lessons"]:
            for prob in lesson["problems"]:
                if prob.draw:
                    out.append(f"draw {prob.spec}\t{prob.draw}\t1")
                else:
                    kind, _, body = prob.spec.partition(":")
                    user = body.split("|", 1)[0] if kind == "word" else body
                    out.append(f"{prob.spec}\t{user}\t1")
    out.append("draw square:2,3;5,3\t5,0;2,0\t1")
    out.append("draw right:2,4;8,4\t5,1\t1")
    return out


def main() -> int:
    for info in YEARS:
        if len(info["lessons"]) != 8:
            raise SystemExit(f"{info['prefix']} has {len(info['lessons'])} lessons")
        text = emit_year(info)
        name = {
            "mat0": "mat_year0.c",
            "mat": "mat_year1.c",
            "mat2": "mat_year2.c",
            "mat3": "mat_year3.c",
            "mat4": "mat_year4.c",
        }[info["prefix"]]
        (SRC / name).write_text(text, encoding="utf-8")
        print(f"wrote {name}")
    (SRC / "math_i18n.inc").write_text(emit_i18n(), encoding="utf-8")
    case_path = Path("/tmp/math_cases.txt")
    case_path.write_text("\n".join(cases()) + "\n", encoding="utf-8")
    binary = Path("/tmp/mathcheck")
    if binary.exists():
        result = subprocess.run([str(binary), str(case_path)], check=False)
        if result.returncode != 0:
            return result.returncode
        print("generated answers accepted")
    else:
        print("checker binary missing, cases written only")
    return 0


if __name__ == "__main__":
    sys.exit(main())
