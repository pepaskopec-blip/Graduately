---
id: tg-3-tisk
nazev: Tisk a PDF
popis: Papír, oblast tisku a černobílé pero.
nazev_cviceni: Kvíz: tisk
nadpis_kvizu: Vyberte nastavení plotru
---

# Výklad

## Plotr
*Na papír nebo do PDF*

- `_PLOT` tiskne aktuální rozvržení. Oblast tisku nechte Layout, ne Extents.
- Tiskárna DWG To PDF uloží výkres, který jde odevzdat bez posunu měřítka.
- Formát papíru musí sedět s rozvržením. A3 v tisku a A4 v rozvržení ořízne razítko.
- Měřítko tisku z rozvržení je 1:1. Měřítko součásti už drží výřez.

> Tip: tisk 1:2 z rozvržení, které už má výřez 1:2, zmenší výkres podruhé. Na papíře bude 1:4.

## Pero
*Černá čára*

- Tabulka per monochrome.ctb tiskne vše černě a tloušťku bere z hladin.
- Barevný tisk ve škole nechte jen na náhled. Odevzdává se černobílý výkres.
- Tloušťka obrysu 0,5 mm a tenká čára 0,25 mm musí být v tabulce nebo na hladině vidět.
- Náhled tisku ukáže, jestli razítko neleze z papíru. Bez náhledu netiskněte.

> Tip: když je na PDF všechno vlásečnicové, tloušťky hladin se netisknou. V `_PLOT` je zapněte.

## Co do PDF nepatří
*Čistý výkres*

- Rámeček výřezu je vypnutý. Pomocné konstrukce taky.
- Kóty jsou čitelné a nejsou přes šrafu, pokud to šlo jinak.
- PDF pojmenujte jako výkres, třeba priruba-A3.pdf, ne Plot1.pdf.
- Vytiskněte i kontrolní kópii na papír, když učitel odevzdání na papíře chce.

> Tip: otevřete PDF a změřte rámeček A3. Má mít 420 × 297 mm. Jinak je špatně formát nebo měřítko tisku.

## Zadání: PDF příruby
*Cvičení*

- Rozvržení A3, výřez 1:1 nebo 1:2 podle toho, co se vejde i s kótami.
- `_PLOT`, tiskárna DWG To PDF, oblast Layout, měřítko 1:1, pero monochrome.ctb.
- Náhled zkontroluje, že razítko je celé a šrafa není černá plocha.
- Soubor uložte jako priruba-A3.pdf vedle dwg.

> Tip: hustá šrafa v náhledu znamená velké měřítko vzoru nebo malé. Vraťte se do `_HATCH` a rozteč zvětšete.


# Kvíz

## Jaké měřítko tisku se dává při tisku rozvržení?
- Stejné jako měřítko výřezu, znovu
- [x] 1:1
- Fit to paper
- 2:1, ať je výkres větší
> Měřítko součásti už má výřez. Tisk papíru je 1:1, jinak se měřítko zdvojí.

## K čemu je monochrome.ctb?
- Zvětší písmo
- [x] Tiskne černě a dodrží tloušťky
- Zamkne výřez
- Smaže hladinu OSA
> Barevné čáry na obrazovce jsou pro práci. Odevzdává se černý výkres s tlustým obrysem.

## Proč nenechat oblast tisku na Extents?
- Extents je rychlejší
- [x] Vzalo by i objekty mimo papír a rozhodilo by měřítko
- Extents netiskne kóty
- Extents vyžaduje 3D
> Layout tiskne papír tak, jak jste ho složili. Extents hledá vše nakreslené a měřítko si dopočítá samo.

## Co má mít rámeček A3 v hotovém PDF?
- 210 × 297 mm
- [x] 420 × 297 mm
- 100 × 70 mm
- Libovolný rozměr, rozhoduje obrazovka
> A3 je 420 × 297 mm. Když PDF vyjde menší, je špatně formát papíru nebo měřítko tisku.
