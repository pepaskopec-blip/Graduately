---
id: matematika-1-konstrukce-trojuhelniku
puvodni: mat.8
nazev: Konstrukce trojúhelníku
popis: Vzdálenost bodů a trojúhelník daný třemi stranami.
nazev_cviceni: Příklady
nadpis_kvizu: Konstrukce trojúhelníku
---

# Výklad

## Vzdálenost na mřížce
*Body*

- Bod v rovině má souřadnice [x; y].
- Vzdálenost [x₁; y₁] a [x₂; y₂] je √((x₂ − x₁)² + (y₂ − y₁)²).
- A = [1; 1] a B = [5; 1] mají vzdálenost 4.
- Osa y na naší mřížce míří nahoru.

> Tip: u svislé nebo vodorovné úsečky stačí rozdíl souřadnic.

## Trojúhelník ze tří stran
*SSS*

- Konstrukce SSS použije tři strany.
- Z bodu A se rýsuje kružnice o poloměru |AC|, z bodu B o poloměru |BC|.
- Kružnice se protnou ve dvou bodech, pokud trojúhelník existuje.
- Trojúhelníková nerovnost: součet dvou stran je větší než třetí.

> Tip: třetí vrchol je průsečík dvou kružnic.

## Strany 4, 3 a 5
*Příklad*

- A = [1; 1], B = [5; 1], takže |AB| = 4.
- Hledáme C tak, aby |AC| = 3 a |BC| = 5.
- Na mřížce od 0 do 10 a od 0 do 8 leží jen C = [1; 4].
- Druhý průsečík [1; −2] je pod mřížkou.

> Tip: 3² + 4² = 5², pravý úhel je u bodu A.


# Příklady

## Jaká je vzdálenost bodů A = [1; 1] a B = [5; 1]?
= num:4
> Body mají stejné y, vzdálenost je 5 − 1 = 4. Výsledek je 4.

## Jaký je obsah pravoúhlého trojúhelníku s odvěsnami 3 a 4?
= num:6
> Obsah je (3 · 4) / 2 = 6. Výsledek je 6.

## Jaký je obvod trojúhelníku se stranami 3, 4 a 5?
= num:12
> 3 + 4 + 5 = 12. Výsledek je 12.

## A = [1; 1] a B = [5; 1] jsou dané. Zakreslete C, pro které je |AC| = 3 a |BC| = 5.
= points:1,4;fix:1,1;fix:5,1
> Kružnice se na mřížce protnou v [1; 4]. Bod [1; −2] už na ní není.
