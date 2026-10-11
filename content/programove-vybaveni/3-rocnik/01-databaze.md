---
id: sw-3-databaze
nazev: Databáze
popis: Tabulka, záznam, klíč. Kdy už nestačí Excel.
nazev_cviceni: Kvíz: databáze
nadpis_kvizu: Poznejte klíč a důvod databáze
---

# Výklad

## Excel nestačí
*Vztahy*

- Excel je výborný na jeden seznam. Jakmile se třída opakuje na každém řádku žáka, začne se název třídy rozcházet.
- Databáze drží věci v samostatných tabulkách a spojuje je klíčem.
- Tabulka tridy má název třídy jednou. Tabulka zaci se na ni jen odkáže.
- Access i MySQL umí totéž v principu. Access je soubor accdb, MySQL je server a příkazy SQL.

> Tip: Když opravujete 1.A na 1. A v padesáti řádcích Excelu, má to být tabulka tříd, ne opis.

## Řádek a sloupec
*Záznam a pole*

- Řádek je záznam, třeba jeden žák. Sloupec je pole, třeba prijmeni.
- Každá hodnota je atomická. Jméno a příjmení nepatří do jedné buňky.
- Primární klíč je hodnota, která se v tabulce neopakuje. U žáka je to id, ne příjmení.
- Cizí klíč je pole, které ukazuje na primární klíč jiné tabulky. trida_id v tabulce zaci ukazuje na id v tridy.

> Tip: Dva Nováci se smějí jmenovat stejně. Jejich id je různé. Proto příjmení klíč není.

## Typy
*Číslo, text, datum*

- id je číslo. Jméno je krátký text. Datum narození je datum, ne text 12.6.
- Ano/ne je samostatný typ, ne text ano.
- Do číselného pole nepatří jednotka. Počet žáků je 28, ne 28 žáků.
- Prázdné pole a nula nejsou totéž. Nula je známá hodnota, prázdno je nevíme.

> Tip: Text v číselném poli se nepodaří uložit, nebo se sečte špatně. Typ zvolte při založení sloupce.

## Zadání: návrh na papír
*Cvičení*

- Navrhněte dvě tabulky: tridy (id, nazev) a zaci (id, jmeno, prijmeni, trida_id).
- U každého pole napište typ. id v obou tabulkách je primární klíč.
- Napište dva řádky tříd, 1.A a 1.B, a tři žáky, z toho Annu Novákovou ve třídě 1.A.
- Soubor navrh.txt uložte k databázím. Access ani MySQL ještě neotevírejte, dokud návrh nesedí.

> Tip: Šipka vede od trida_id k id tabulky tridy. Opačně by žák byl klíčem třídy, což není.

# Kvíz

## Co je primární klíč žáka?
- Příjmení
- [x] id, které se neopakuje
- Název třídy
- První sloupec, ať je v něm cokoli
> Příjmení se opakuje. id jednoho žáka ne.

## Co je trida_id v tabulce zaci?
- Primární klíč třídy v téže tabulce
- [x] Cizí klíč na tabulku tridy
- Text 1.A opsaný do každého řádku
- Heslo
> Cizí klíč drží jen číslo třídy. Název třídy žije v tabulce tridy jednou.

## Kdy už je Excel na seznam žáků těsný?
- Jakmile má víc než jeden sloupec
- [x] Když se údaj, třeba název třídy, opakuje a musí zůstat stejný
- Excel nestačí nikdy
- Jakmile je soubor xlsx
> Opakovaný název se rozjede. Databáze ho má v jedné tabulce a ostatní se na ni odkážou.

## Čím se liší nula a prázdné pole?
- Ničím
- [x] Nula je hodnota, prázdné pole znamená, že hodnota chybí
- Prázdné pole je vždy nula
- Nula je text
> Průměr nulu počítá. Prázdnou buňku často ne. Typ i význam se liší.
