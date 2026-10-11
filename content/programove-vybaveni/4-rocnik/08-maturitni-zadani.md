---
id: sw-4-maturita
nazev: Maturitní zadání
popis: Složky, dopis, známky, evidence, web, sestava a účet s zásadou hesla.
nazev_cviceni: Kvíz: celé zadání
nadpis_kvizu: Projdi odevzdání
---

# Výklad

## Systém
*Složky a účet*

- slozky.bat založí Skola\1-rocnik\word, excel a system v Dokumentech toho účtu, který ho spustí.
- V bat není del a není v něm heslo. Je v něm @echo off, mkdir a pause.
- Účet zaci je běžný uživatel ve skupině Users, není v Administrators.
- Minimální délka hesla je 8 znaků. Zámek po nečinnosti je 300 sekund, pokud edice Windows zásady má.

> Tip: Do textového souboru patří příkazy a hodnoty. Skutečné heslo ne.

## Office
*Dopis a známky*

- zadost.docx je jedna strana, datum 12. 6. 2026, řádek Věc, k tomu PDF.
- znamky.xlsx počítá stav vzorcem KDYŽ. Známka 5 je neprospěl, 1 až 4 prospěl.
- Pod známkami je průměr, ne součet.
- prodej.xlsx a prodej.pbix: tabulka načtená jednou, průřez třídy, PDF bez výběru, ať jsou vidět obě třídy.

> Tip: pbix bez xlsx zdroje učitel neobnoví. Xlsx vedle pbix nechte.

## Data a web
*Evidence a Školní den*

- Access nebo MySQL: tabulky tridy a zaci, vztah 1:N, Anna Nováková ve třídě 1.A.
- Dotaz ukáže název 1.A, ne jen číslo. UPDATE a DELETE, pokud jsou v SQL, mají WHERE.
- WordPress: název Školní den, stránka O škole, příspěvek s datem, účet autora. Správce je jeden.
- V textech o webu a účtech není heslo.

> Tip: Stačí jeden databázový nástroj, ale celý. Access s integritou, nebo SQL s JOINem.

## Složka odevzdání
*Pořádek*

- Všechno leží pod Skola, ne na ploše a ne v Stažených.
- Přípony jsou vidět. Názvy nemají final2.
- Na obhajobu otevřete nejdřív bat a složky, pak Excel a Power BI, pak databázi a nakonec web.
- U každé části umíte říct, kdo smí heslo, kdo smí instalovat a proč pětka není prospěl.

> Tip: Když se zeptají, proč zaci není správce, odpověď je: denní účet nemá právo měnit systém.

# Kvíz

## Co slozky.bat nesmí obsahovat?
- mkdir
- @echo off
- [x] del přes celý disk nebo cizí složky
- pause
> Cvičení složky zakládá. Mazání do něj nepatří a neznámé mazání už vůbec ne.

## Jaký je stav známky 5?
- prospěl
- [x] neprospěl
- průměr
- 1.A
> Prochází 1 až 4. Pětka je neprospěl. Rozhoduje vzorec KDYŽ, ne rukou psaný text.

## Jakou roli má účet, který ve WordPressu jen píše novinky?
- Správce
- [x] Autor
- Jediný sdílený účet učitele
- Administrators ve Windows
> Autor zveřejní vlastní příspěvek. Doplňky a uživatele spravuje jeden správce.

## Jaké jsou dvě hodnoty zásad?
- Heslo 0 znaků a zámek 8 sekund
- [x] Heslo aspoň 8 znaků a zámek po 300 sekundách
- Heslo v souboru bat a zámek vypnutý
- 300 znaků hesla a 8 sekund
> Osm znaků je délka hesla. 300 sekund je pět minut nečinnosti.
