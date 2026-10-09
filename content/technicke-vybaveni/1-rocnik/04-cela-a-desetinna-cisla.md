---
id: hardware-1-cela-a-desetinna-cisla
puvodni: hw.4
nazev: Celá a desetinná čísla
popis: 2 snímky o integer a real a pak kvíz.
nazev_cviceni: Kvíz: integer a real
nadpis_kvizu: Kvíz k celým a desetinným číslům
---

# Výklad

## Proměnná typu integer (celá čísla)
*Integer*

- Celá čísla se ukládají binárně v pevné řádové čárce.
- Mají pevný počet bitů – standardně 16 nebo 32 (tedy 2 nebo 4 byty).
- Při 16bitovém formátu je 2¹⁶ = 65 536 různých hodnot; u nezáporných čísel interval 0 až 2¹⁶ − 1, tedy 0–65 535.
- Obecně u n bitů je M = 2ⁿ různých hodnot (u 16 bitů je nejvyšší nezáporné číslo 65 535).
- Ostatní čísla se do tohoto intervalu převádějí transformací (mapováním na přirozená čísla z daného rozsahu).

> Tip: n bitů → 2ⁿ různých hodnot (např. 0 až 2ⁿ − 1)

## Proměnná typu real (desetinná čísla)
*Real*

- Desetinná čísla se ukládají v pohyblivé řádové čárce ve tvaru mantisa a exponent.
- Mantisa je normalizovaná tak, že první platná číslice je hned za desetinnou čárkou.
- Posun řádové čárky vyrovná odpovídající změna exponentu.
- Standardně zabírá 4 nebo 8 bytů v paměti.
- Více bytů se používá jen tehdy, když potřebujeme vyšší přesnost výpočtů.

> Tip: tvar = mantisa × základ^exponent


# Kvíz

## Jak se ukládají celá čísla (integer)?
- Jen jako text
- [x] Binárně v pevné řádové čárce s pevným počtem bitů
- Jen jako obrázek
- Jen v pohyblivé řádové čárce bez exponentu

## Kolik různých hodnot má 16bitový nezáporný integer?
- 256
- 1024
- [x] 65 536 (0 až 65 535)
- 4

## Jak se ukládají desetinná čísla (real)?
- Jen jako celá čísla bez tečky
- [x] V pohyblivé řádové čárce jako mantisa a exponent
- Jen jako 1 bit
- Jen jako název souboru

## Co znamená normalizace mantisy?
- Že se číslo smaže
- [x] Že první platná číslice je hned za desetinnou čárkou a exponent se upraví
- Že se použije jen 1 bit
- Že se číslo uloží jako text
