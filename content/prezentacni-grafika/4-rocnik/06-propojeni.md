---
id: pg-4-propojeni
nazev: Propojení programů
popis: Logo z Illustratoru v After Effects a úvod v Premiere bez mezivýstupu.
nazev_cviceni: Kvíz: propojení
nadpis_kvizu: Vyberte, co se aktualizuje
---

# Výklad

## Jeden zdroj
*Odkaz*

- Logo má jeden zdroj, logo.ai. Photoshop, After Effects i InDesign ať na něj odkazují.
- Když značku přebarvíte v AI a uložíte, odkazy ji mohou načíst znovu. Kopírování JPG to neumí.
- JPG 80 px vložené do čtyř programů jsou čtyři různé značky. Jedna se opraví, tři zůstanou staré.
- Pojmenujte zdroj jednou. logo-final-final2.ai je cesta, jak odevzdat špatnou verzi.

> Tip: V After Effects se odkaz obnoví v panelu projektu. V InDesignu v panelu Odkazy.

## Dynamic Link
*AE do Premiere*

- Dynamic Link vloží kompozici After Effects na časovou osu Premiere bez mezivýstupu mp4.
- Když v úvodu posunete text, Premiere po uložení aep ukáže novou verzi.
- Oba programy mají být stejná generace. Starší Premiere nový aep často neotevře.
- Soubory aep a prproj mějte ve stejné složce. Odkaz jinak spadne při přesunu jen jednoho z nich.

> Tip: V Premiere: Soubor > Adobe Dynamic Link > Importovat kompozici After Effects. Na osu přijde jako záběr.

## Co nepropojovat
*Kopie*

- Zvuk do Illustratoru nepatří. Video do InDesignu taky ne, kromě náhledu, který tu neděláme.
- Plakát v PSD zůstává tisk. Do videa z něj berte jen logo nebo krátký náhled, ne celou spadávku se značkami.
- Ořezové značky plakátu ve videu jsou chyba. Ty má jen tiskové PDF.
- Když Dynamic Link nejde, exportujte úvod jako mp4 v ProRes nebo aspoň v kvalitním H.264 a ten vložte. Je to kopie, už se sama neopraví.

> Tip: ProRes je mezisoubor, velký a kvalitní. H.264 je menší a už jednou stlačený. Do dalšího exportu je horší.

## Zadání: úvod na ose
*Cvičení*

- V Premiere na začátek sekvence vložte kompozici uvod z After Effects přes Dynamic Link.
- Délka ať sedí: 8 sekund úvodu, pak pokračuje sestřih.
- Změňte v AI jednu barvu loga, uložte, a obnovte odkaz až do videa. Když se barva nezmění, máte někde JPG.
- Uložte prproj i aep. Mp4 celého filmu exportujte znovu až na konci, ne po každém kroku.

> Tip: Když se úvod na ose tváří jako offline, chybí aep nebo je jiná verze programu. Neexportujte přes to černou.

# Kvíz

## Kde má žít značka, aby šla opravit všude?
- [x] V jednom logo.ai
- Ve čtyřech různých JPG
- Jen vypálená ve videu
- V titulku Premiere jako jediném místě
> Jeden zdroj. Ostatní programy na něj odkazují.

## Co dělá Dynamic Link?
- Převede plakát do CMYK
- [x] Vloží kompozici After Effects do Premiere bez mezivýstupu
- Smaže vrstvy v PSD
- Nahradí InDesign
> Úprava v aep se po uložení ukáže na ose. Mp4 mezi tím není potřeba.

## Co do videa z plakátu nepatří?
- Logo
- [x] Ořezové značky
- Název akce
- Barva značky
> Značky jsou pro tiskárnu. Ve snímku videa vypadají jako omylem vyfocený tiskový arch.

## Kdy je mp4 z After Effects horší než Dynamic Link?
- [x] Když chcete, aby se oprava textu sama propsala do Premiere
- Když video má 25 sn/s
- Když je úvod dlouhý 8 sekund
- Mp4 je vždycky zdroj
> Mp4 je kopie. Další oprava textu už do něj sama nevleze, musíte exportovat znovu.
