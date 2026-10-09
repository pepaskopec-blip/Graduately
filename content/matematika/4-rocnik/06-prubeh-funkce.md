---
id: matematika-4-prubeh-funkce
puvodni: mat4.6
nazev: Průběh funkce
popis: Stacionární body, lokální extrémy a nulová derivace.
nazev_cviceni: Příklady
nadpis_kvizu: Průběh funkce
---

# Výklad

## Derivace je nula
*Stacionární bod*

- V lokálním extrému diferencovatelné funkce je derivace nulová.
- U f(x) = x³ − 3x je f'(x) = 3x² − 3.
- 3(x² − 1) = 0 dává x = −1 a x = 1.
- To jsou jediní kandidáti na lokální extrém.

> Tip: f'(x) = 0 je vodorovná tečna.

## Znaménko druhé derivace
*Maximum a minimum*

- V x = −1 je f''(−1) = −6 < 0, takže jde o lokální maximum.
- f(−1) = −1 + 3 = 2.
- V x = 1 je f''(1) = 6 > 0, takže jde o lokální minimum.
- f(1) = 1 − 3 = −2.

> Tip: f''(x) = 6x. Záporná druhá derivace znamená lokální maximum.

## Lineární derivace
*Jiný příklad*

- Kde je derivace nula, má graf vodorovnou tečnu.
- Rovnice 2x − 4 = 0 má řešení x = 2.
- Pro x < 2 je derivace 2x − 4 záporná, funkce klesá.
- Pro x > 2 je derivace kladná, funkce roste.

> Tip: g'(x) = 2x − 4 je nula při x = 2.


# Příklady

## Funkce f(x) = x³ − 3x. Ve kterém x má lokální maximum?
= num:-1
> f'(x) = 3x² − 3 = 0 pro x = ±1 a v −1 je maximum. Výsledek je -1.

## Funkce f(x) = x³ − 3x. Jaká je hodnota lokálního maxima?
= num:2
> f(−1) = −1 + 3 = 2. Výsledek je 2.

## Funkce f(x) = x³ − 3x. Ve kterém x má lokální minimum?
= num:1
> Druhá derivace 6x je v x = 1 kladná. Výsledek je 1.

## Derivace funkce je g'(x) = 2x − 4. Pro které x je derivace nulová?
= num:2
> 2x − 4 = 0 dává x = 2. Výsledek je 2.
