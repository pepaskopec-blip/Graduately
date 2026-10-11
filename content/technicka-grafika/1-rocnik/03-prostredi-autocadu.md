---
id: tg-1-prostredi
nazev: Prostředí AutoCADu
popis: Model, příkazový řádek, jednotky a meze.
nazev_cviceni: Kvíz: prostředí
nadpis_kvizu: Ověřte si první nastavení výkresu
---

# Výklad

## Kam se kreslí
*Model*

- Součást se kreslí v modelu. Papír, rámeček a výřezy patří až do rozvržení.
- Příkazový řádek je dole. Čte se pořád: AutoCAD se ptá na další bod.
- Stisk Enter nebo mezerník zopakuje poslední příkaz.
- Esc příkaz zruší. Neukládejte rozpracovaný příkaz klávesou Esc uprostřed bodu.

> Tip: když AutoCAD „nic nedělá“, podívejte se, na co se příkazový řádek ptá.

## Příkaz, který funguje vždy
*Podtržítko*

- Český AutoCAD má přeložené příkazy. Úsečka se jmenuje ÚSEČKA.
- Mezinárodní jméno se píše s podtržítkem: `_LINE`. Funguje v každé jazykové verzi.
- Na pásu karet je stejný příkaz pod ikonou. Ikona i text dělají totéž.
- V těchto lekcích je u každého kroku mezinárodní příkaz. Naučte se ho psát.

> Tip: `_LINE` a ÚSEČKA spustí stejné kreslení. Podtržítko se hodí, když čeština na klávesnici zlobí.

## Jednotky a meze
*Milimetry*

- `_UNITS` nastavte na desetinné milimetry. Úhly nechte ve stupních.
- Přesnost 0 stačí na celé milimetry, 0,0 když budete potřebovat desetinu.
- `_LIMITS` určí levý dolní a pravý horní roh kreslicí plochy, třeba 0,0 a 420,297.
- `_ZOOM` a volba All ukáže celé meze. Kolečko myši přibližuje ke kurzoru.

> Tip: nový výkres začněte jednotkami. Přepočet z palců na milimetry uprostřed práce nic nespraví.

## Zadání: prázdný výkres A3
*Cvičení*

- Založte soubor v milimetrech a nastavte meze 0,0 až 420,297.
- Příkazem `_RECTANG` nakreslete obrys papíru z bodu 0,0 do bodu 420,297.
- Přibližte vše příkazem `_ZOOM` a volbou Extents.
- Uložte soubor jako A3-prazdny.dwg.

> Tip: Extents přiblíží to, co je nakreslené. All přiblíží meze, i když jsou prázdné.


# Kvíz

## Kde se kreslí tvar součásti?
- V rozvržení, přes razítko
- [x] V modelu
- Jen na pásu karet
- V příkazu PLOT
> Model je prostor součásti. Rozvržení je papír, na který se model později vloží výřezem.

## Co udělá příkaz `_LINE` v českém AutoCADu?
- Nastaví jednotky
- [x] Spustí kreslení úsečky
- Smaže poslední čáru
- Otevře hladiny
> Podtržítko volá mezinárodní příkaz. `_LINE` je úsečka, ať je AutoCAD v jakémkoli jazyce.

## Jakou jednotku má školní výkres?
- Palec
- [x] Milimetr
- Metr
- Pixel
> `_UNITS` nastavte na desetinné milimetry dřív, než zadáte první rozměr.

## Co ukáže `_ZOOM` s volbou Extents?
- Celé nastavené meze, i prázdné
- [x] Všechno, co je nakreslené
- Jen razítko
- Poslední příkaz
> Extents orámuje nakreslené objekty. All ukáže meze výkresu.
