---
id: tg-3-bloky
nazev: Bloky
popis: Opakovaný díl jako jeden objekt se základním bodem.
nazev_cviceni: Kvíz: bloky
nadpis_kvizu: Vložte blok správně
---

# Výklad

## K čemu blok je
*Jeden díl*

- Blok je pojmenovaná skupina objektů, která se vkládá jako jeden kus.
- Šroub, značka nebo razítko se nekreslí pokaždé znovu.
- Úprava definice bloku se propíše do všech vložení. Kopie by zůstaly staré.
- Blok patří do výkresu. `_WBLOCK` ho navíc uloží do vlastního souboru.

> Tip: čtyři stejné šrouby jsou čtyři vložení jednoho bloku, ne čtyři překreslené šrouby.

## Základní bod
*Kam se chytá*

- `_BLOCK` se zeptá na jméno, základní bod a objekty.
- Základní bod je místo, za které budete blok vkládat. U šroubu třeba průsečík osy a dosedací plochy.
- Špatný základní bod posune každé vložení o stejnou chybu.
- Objekty bloku nedávejte na hladinu 0, pokud nemají přebírat barvu místa vložení. Na škole nechte jejich vlastní hladiny.

> Tip: základní bod chytejte úchopem. Bod „někde u šroubu“ se při vložení mine o několik milimetrů.

## Vložení
*Místo, měřítko, úhel*

- `_INSERT` vybere jméno, bod vložení, měřítko a otočení.
- Měřítko 1 a úhel 0 vloží blok tak, jak byl nakreslen.
- Měřítko 2 ho zvětší. Na výkresu 1:1 se měřítko bloku nemění, mění se až měřítko výřezu.
- Rozbití bloku příkazem `_EXPLODE` z něj zase udělá obyčejné čáry. Dělejte to jen když máte měnit jediné vložení.

> Tip: rozbitý blok se už neaktualizuje s definicí. Rozbijte ho, jen když opravdu chcete výjimku.

## Zadání: šroub M8 jako blok
*Cvičení*

- V nárysu nakreslete zjednodušený šroub: dřík Ø8 dlouhý 20 a hlava široká 13 vysoká 5,5.
- Základní bod dejte na osu, do dosedací plochy pod hlavou.
- Blok pojmenujte SROUB-M8 a vložte ho dvakrát, po 40 mm.
- Změňte délku dříku v definici na 25. Obě vložení se musí prodloužit.

> Tip: dosedací plocha je rozhraní hlavy a dříku. Od ní se měří, jak hluboko šroub sedí.


# Kvíz

## Čím se liší blok od obyčejné kopie?
- Ničím
- [x] Úprava definice se propíše do všech vložení
- Blok nejde otočit
- Kopie zabere méně místa
> Kopie jsou nezávislé. Vložení bloku sdílejí jednu definici.

## Co je základní bod bloku?
- Střed razítka
- [x] Bod, za který se blok vkládá
- Vždy 0,0 výkresu
- Pravý horní roh objektů
> Špatný základní bod posune každé vložení. Chytejte ho úchopem na osu nebo na dosedací plochu.

## Kdy blok rozbít příkazem `_EXPLODE`?
- Vždy po vložení
- [x] Jen když jedno vložení má být výjimka
- Aby se zmenšil soubor
- Před každým tiskem
> Rozbitý blok se s definicí už neaktualizuje. Ostatní vložení nechte celé.

## Měřítko vložení 1 na výkresu 1:1 znamená?
- Blok se kreslí poloviční
- [x] Blok se vloží ve velikosti, v jaké byl nakreslen
- Blok se převrátí
- Měřítko razítka se změní na 1:2
> Měřítko výkresu řeší až výřez na papíře. Blok součásti nechte v měřítku 1.
