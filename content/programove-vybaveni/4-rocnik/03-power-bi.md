---
id: sw-4-power-bi
nazev: Power BI: data z Excelu
popis: Jeden sešit, jedna tabulka, graf podle třídy. Stejná data se nenačítají dvakrát.
nazev_cviceni: Kvíz: Power BI
nadpis_kvizu: Vyberte zdroj a vizuál
---

# Výklad

## Soubor
*pbix*

- Power BI Desktop ukládá práci do pbix. Není to xlsx a není to obrázek grafu.
- Na odevzdání vedle pbix patří PDF stránky sestavy, ať ji otevře i člověk bez Power BI.
- Zdroj je prodej.xlsx, tabulka Prodej se sloupci Polozka, Trida, Castka.
- Načtení je Načíst data, Excel, a vybere se tabulka Prodej, ne celý sešit naslepo dvakrát.

> Tip: Když v modelu vidíte Prodej a Prodej (2), tabulka se načetla dvakrát. Jednu odeberte, než postavíte graf.

## Model
*Jedna řádka, jedna skutečnost*

- Každý řádek je jeden prodej: položka, třída, částka. Součet v tomtéž sloupci navíc řádek souhrnu být nemá.
- Řádek souhrnů z Excelu do Power BI nepatří. Součet si spočítá vizuál.
- Trida je text 1.A a 1.B. Castka je číslo bez Kč v buňce.
- Když Excel opravíte, v Power BI dejte Obnovit. Sám se o uložení xlsx nedozví.

> Tip: Sloupec, který Power BI bere jako text, se nesčítá. Často je v něm mezera nebo Kč. V Excelu ho opravte a obnovte.

## Vizuál
*Sloupec a karta*

- Sloupcový graf: osa Trida, hodnota součet Castka. Porovnání dvou tříd.
- Karta ukáže jedno číslo, celkový součet Castka.
- Koláč pro dvě třídy je zbytečný. Pro dvanáct měsíců byl špatně už v Excelu.
- Průřez, anglicky slicer, na pole Trida nechte na příští lekci. Teď ať graf ukáže obě třídy.

> Tip: Když graf ukáže počet řádků místo součtu korun, v hodnotách je Počet. Přepněte souhrn na Součet.

## Zadání: první pbix
*Cvičení*

- prodej.pbix načte tabulku Prodej jednou.
- Na stránce jsou sloupcový graf podle třídy a karta s celkovou částkou.
- Obnovte data potom, co v Excelu změníte jednu částku. Karta se musí pohnout.
- Uložte pbix do složky Skola\1-rocnik\excel a vedle něj prodej.pdf.

> Tip: PDF je snímek sestavy. pbix je práce, ve které jde vizuál opravit. Odevzdávají se oba.

# Kvíz

## Jaký soubor je pracovní v Power BI?
- xlsx jako jediný
- [x] pbix
- pdf jako jediný
- accdb
> pbix drží model a vizuály. Excel je zdroj. PDF je náhled na odevzdání.

## Jak se mají načíst data prodeje?
- [x] Tabulka Prodej jednou
- Tentýž list dvakrát
- Jako obrázek grafu z Excelu
- Přepisem do Wordu
> Jedna tabulka v modelu. Druhá kopie téhož zdroje mate součty.

## Jaký vizuál porovná částku tříd 1.A a 1.B?
- Koláč s dvanácti díly
- [x] Sloupcový graf, osa Trida, součet Castka
- Karta bez popisku a bez čísla
- Průřez místo grafu jako jediný prvek
> Dvě kategorie a jejich součet jsou sloupce. Karta je jen jedno číslo celkem.

## Co uděláte v Power BI potom, co změníte číslo v Excelu?
- Nic, pbix se mění sám v tu vteřinu
- [x] Obnovit
- Smazat pbix a začít ve Wordu
- Převést Castka na text
> Power BI si data při načtení vezme. Další změna Excelu se projeví až po obnovení.
