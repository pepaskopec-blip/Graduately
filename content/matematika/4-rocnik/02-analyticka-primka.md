---
id: matematika-4-analyticka-primka
puvodni: mat4.2
nazev: Analytická přímka
popis: Směrnice, úsek, parametrické vyjádření a vzdálenost od přímky.
nazev_cviceni: Příklady
nadpis_kvizu: Analytická přímka
---

# Výklad

## y = ax + b
*Směrnice*

- Body [1; 2] a [4; 8] určují směrnici (8 − 2)/(4 − 1) = 2.
- Přímka procházející bodem [0; 1] se směrnicí 2 má rovnici y = 2x + 1.
- Úsek na ose y je 1.
- Svislá přímka směrnici tohoto tvaru nemá.

> Tip: a = (y₂ − y₁)/(x₂ − x₁).

## Bod a směrový vektor
*Parametr*

- Parametrické vyjádření je x = x₀ + t · u₁, y = y₀ + t · u₂.
- Přímka (1; 3) + t(2; 0) má při t = 2 bod [5; 3].
- Každé t dá právě jeden bod přímky.
- Směrový vektor (2; 0) je vodorovný.

> Tip: X = A + t · u.

## Bod od přímky
*Vzdálenost*

- Přímka 3x + 4y − 10 = 0 má a = 3, b = 4 a c = −10.
- Vzdálenost počátku je |−10| / √(9 + 16) = 10/5 = 2.
- Čitatel je absolutní hodnota, vzdálenost není záporná.
- Čtvrtý vrchol rovnoběžníku k bodům A, B a C je B + C − A.

> Tip: pro ax + by + c = 0 je vzdálenost |ax₀ + by₀ + c| / √(a² + b²).


# Příklady

## Jaká je směrnice přímky body [1; 2] a [4; 8]?
= num:2
> (8 − 2)/(4 − 1) = 2. Výsledek je 2.

## Přímka má rovnici y = 2x + 1. Jaký je její úsek na ose y?
= num:1
> Úsek je absolutní člen, tedy 1. Výsledek je 1.

## Bod přímky je (1; 3) + t(2; 0). Kam přímka dojde pro t = 2? Napište x a y středníkem.
= pair:5;3
> (1; 3) + 2 · (2; 0) = (5; 3). Napište 5;3.

## Jaká je vzdálenost bodu [0; 0] od přímky 3x + 4y − 10 = 0?
= num:2
> |−10| / √(9 + 16) = 10/5 = 2. Výsledek je 2.

## A = [1; 1], B = [4; 1] a C = [2; 3]. Zakreslete čtvrtý vrchol rovnoběžníku, ve kterém jsou AB a AC sousední strany.
= para:1,1;4,1;2,3
> D = B + C − A = [4 + 2 − 1; 1 + 3 − 1] = [5; 3].
