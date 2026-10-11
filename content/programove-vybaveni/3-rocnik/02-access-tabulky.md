---
id: sw-3-access-tabulky
nazev: Access: tabulky a vztah
popis: Automatické číslo, cizí klíč a vztah 1:N bez kaskády.
nazev_cviceni: Kvíz: Access
nadpis_kvizu: Vyberte typ klíče a kaskádu
---

# Výklad

## Soubor
*accdb*

- Databáze Accessu je jeden soubor accdb. Jmenuje se evidence.accdb.
- Tabulku založíte v návrhovém zobrazení, ne jen v datovém listu. V návrhu je vidět typ.
- Tabulka tridy má pole id a nazev. Tabulka zaci má id, jmeno, prijmeni a trida_id.
- Název pole je bez mezery a bez diakritiky: trida_id. Popisek pro lidi se dá nastavit zvlášť.

> Tip: Datový list je na zapisování řádků. Typ pole se mění v návrhu. Když už v tabulce data jsou, změna typu může selhat.

## Klíče
*Automatické číslo*

- id je typ Automatické číslo a je to primární klíč. Access ho doplňuje sám: 1, 2, 3.
- trida_id je typ Číslo, dlouhé celé číslo. Není to automatické číslo. Píšete ho vy, nebo ho vybere vztah.
- nazev, jmeno a prijmeni jsou Krátký text. Délka 50 stačí.
- Dva primární klíče v jedné tabulce školního cvičení nejsou. Stačí id.

> Tip: Když trida_id necháte jako text, vztah na číselné id třídy Access odmítne. Typy obou konců musí sedět.

## Vztah 1:N
*Bez kaskády*

- Jedna třída má víc žáků. Žák má jednu třídu. To je vztah 1:N.
- Ve vztazích přetáhnete tridy.id na zaci.trida_id.
- Zaškrtněte vynucení referenční integrity. Žák se nesmí odkázat na třídu, která neexistuje.
- Kaskádové odstranění nechte vypnuté. Smazání třídy by jinak smazalo i žáky.

> Tip: Na straně tridy je jednička, na straně zaci je znak nekonečna. Když je to obráceně, táhli jste vztah špatným směrem.

## Zadání: evidence.accdb
*Cvičení*

- Založte evidence.accdb s tabulkami tridy a zaci podle návrhu.
- Vložte třídy 1.A a 1.B. Vložte Annu Novákovou s trida_id třídy 1.A a ještě dva žáky.
- Vztah 1:N s referenční integritou, bez kaskádového odstranění.
- Zkuste uložit žáka s trida_id 99. Access ho má odmítnout. Pak to číslo vraťte na platnou třídu.

> Tip: Když jde uložit trida_id 99, integrita není zapnutá. Otevřete vztahy a vztah vytvořte znovu.

# Kvíz

## Jaký typ má id žáka?
- Krátký text, který píšete ručně
- [x] Automatické číslo a primární klíč
- Ano/ne
- Dlouhý text s celým jménem třídy
> Automatické číslo žák nedoplňuje. Access ho dá samo a v tabulce se neopakuje.

## Jaký typ má trida_id?
- Automatické číslo
- [x] Číslo, dlouhé celé číslo
- Datum
- Ano/ne
> Je to cizí klíč na id třídy, které je taky číslo. Automatické číslo by si vymýšlelo vlastní hodnoty.

## Co dělá referenční integrita?
- Smaže žáky při smazání třídy
- [x] Nedovolí žáka s třídou, která neexistuje
- Schová tabulku
- Převede accdb na xlsx
> Odkaz musí vést na skutečný řádek. Třída 99 bez řádku v tridy projít nemá.

## Proč nechat kaskádové odstranění vypnuté?
- Access ho neumí
- [x] Smazání třídy by smazalo i její žáky
- Jinak nejde uložit accdb
- Kvůli Excelu
> Třída a žáci jsou různá data. Smazat třídu omylem a přijít o žáky je špatná pojistka pro školní cvičení.
