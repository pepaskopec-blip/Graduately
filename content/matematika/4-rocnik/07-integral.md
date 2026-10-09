---
id: matematika-4-integral
puvodni: mat4.7
nazev: Integrál
popis: Určitý integrál mocniny jako obsah pod grafem.
nazev_cviceni: Příklady
nadpis_kvizu: Integrál
---

# Výklad

## Opačný postup k derivaci
*Primitivní funkce*

- Neurčitý integrál je množina primitivních funkcí.
- Liší se o konstantu, protože derivace konstanty je nula.
- Primitivní funkce k 2x je x².
- Primitivní funkce k x² je x³/3.

> Tip: primitivní funkce k xⁿ je xⁿ⁺¹/(n + 1) pro n ≠ −1.

## Newtonův vzorec
*Určitý integrál*

- Určitý integrál nezáporné funkce je obsah plochy pod grafem.
- ∫ od 0 do 3 z 2x dx = [x²] od 0 do 3 = 9.
- ∫ od 0 do 2 z x² dx = [x³/3] od 0 do 2 = 8/3.
- Dolní mez 0 u těchto mocnin často vynuluje první člen.

> Tip: ∫ od a do b z f je F(b) − F(a).

## Násobek lze vytknout
*Konstanta*

- ∫ od 0 do 1 z 3x² dx = [x³] od 0 do 1 = 1.
- ∫ od 0 do 2 z 6x dx = [3x²] od 0 do 2 = 12.
- Záporná funkce dává záporný integrál, nejde vždy o obsah beze znaménka.
- Součet funkcí se integruje člen po členu.

> Tip: integrál z c · f je c krát integrál z f.


# Příklady

## Spočítejte integrál od 0 do 3 z funkce 2x.
= num:9
> Primitivní funkce je x² a 9 − 0 = 9. Výsledek je 9.

## Spočítejte integrál od 0 do 2 z funkce x².
= num:8/3
> Primitivní funkce je x³/3 a 8/3 − 0 = 8/3. Výsledek je 8/3.

## Spočítejte integrál od 0 do 1 z funkce 3x².
= num:1
> Primitivní funkce je x³ a 1 − 0 = 1. Výsledek je 1.

## Spočítejte integrál od 0 do 2 z funkce 6x.
= num:12
> Primitivní funkce je 3x² a 12 − 0 = 12. Výsledek je 12.
