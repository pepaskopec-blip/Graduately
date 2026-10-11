---
id: tg-4-prostor
nazev: Modelový prostor ve 3D
popis: Osy, uživatelský souřadný systém a obíhání kolem modelu.
nazev_cviceni: Kvíz: prostor
nadpis_kvizu: Najděte osu a verzi AutoCADu
---

# Výklad

## Tři osy
*X, Y, Z*

- K ose X a Y přibude Z. Na obrazovce při startu míří Z k vám.
- Pravidlo pravé ruky: prsty od X k Y, palec ukazuje Z.
- `_BOX` a další tělesa potřebují plný AutoCAD. Verze LT tělesa nevytvoří.
- 2D výkres z dřívějška zůstává. 3D model je druhý soubor, nebo stejný model v jiném pohledu.

> Tip: když příkaz `_BOX` AutoCAD nezná, máte LT. Tělesa v ní nejdou.

## Pohled
*Orbit*

- ViewCube v rohu nebo `_ORBIT` otáčí pohled. Model se neotáčí, otáčí se kamera.
- Přední, horní a levý pohled odpovídají nárysu, půdorysu a bokorysu.
- `_UCS` posune a natočí souřadnice. Nová tělesa vznikají vůči aktuálnímu UCS.
- `_UCS` a volba World vrátí původní osy. Ztracený UCS je nejčastější zmatek ve 3D.

> Tip: než vysunete profil, zkontrolujte ikonu os. Z má trčet z plochy, kterou vysouváte.

## Drát a stínování
*Ať něco vidíte*

- Drátový model ukáže všechny hrany, i zadní. Na začátku se v něm líp trefuje.
- Stínovaný pohled schová zadní hrany. Přepíná se `_VSCURRENT` nebo nabídkou pohledu.
- Zoom kolečkem pořád platí. Výřez na papír přijde až v lekci o výkresu z modelu.
- Jednotky zůstávají milimetry. Těleso 60 × 40 × 30 je v tomtéž souboru jako 2D profil.

> Tip: ztracený model najdete `_ZOOM` Extents. Často leží daleko od počátku, protože UCS byl posunutý.

## Zadání: tři osy a kvádr
*Cvičení*

- V novém souboru v milimetrech nakreslete `_BOX` se základnou 60 × 40 a výškou 30, roh v 0,0,0.
- Přepněte na přední, horní a levý pohled. V každém má být vidět jiná dvojice rozměrů.
- `_UCS` World a `_DIST` mezi dvěma rohy podél X má dát 60.
- Uložte jako kvadr-60x40x30.dwg. LT na tento soubor nestačí.

> Tip: přední pohled ukáže 60 × 30, horní 60 × 40, levý 40 × 30.


# Kvíz

## Který AutoCAD umí tělesa `_BOX`?
- Každá verze včetně LT
- [x] Plný AutoCAD, ne LT
- Jen verze na Mac bez příkazů
- Žádný, tělesa jsou v jiném programu
> LT je 2D. Příkaz `_BOX` v ní není.

## Co dělá `_ORBIT`?
- Otočí těleso v souřadnicích
- [x] Otočí pohled kamery
- Nastaví měřítko tisku
- Smaže osu Z
> Model zůstane. Změní se, odkud se na něj díváte.

## Co vrátí souřadnice do původního směru?
- `_ZOOM` Extents
- [x] `_UCS` a volba World
- Vypnutí hladiny 0
- `_PLOT`
> Posunutý UCS nechá nová tělesa „ležet na boku“. World je výchozí osy výkresu.

## Co ukáže přední pohled na kvádr 60 × 40 × 30, když 30 je výška?
- 60 × 40
- [x] 60 × 30
- 40 × 30
- Jen osu Z
> Přední pohled je šířka a výška. Hloubka 40 je v půdorysu a v bokorysu.
