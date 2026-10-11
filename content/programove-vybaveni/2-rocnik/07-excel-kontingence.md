---
id: sw-2-excel-kontingence
nazev: Excel: tabulka, filtr a kontingenční tabulka
popis: Seznam jako tabulka, filtr a součet podle třídy.
nazev_cviceni: Kvíz: kontingenční tabulka
nadpis_kvizu: Řekněte, co patří do řádků a co do hodnot
---

# Výklad

## Tabulka
*Ctrl+T*

- Seznam se záhlavím se označí a Ctrl+T z něj udělá tabulku. Filtr v záhlaví vznikne sám.
- Záhlaví je jeden řádek: Polozka, Trida, Castka. Dva řádky nadpisů Ctrl+T rozbije.
- Hned pod tabulku se nepíše další text. Souhrn je v řádku souhrnů, nebo na jiném listu.
- Řazení je šipka v záhlaví. Seřadit jen půlku sloupce mimo tabulku rozhodí řádky.

> Tip: Tabulka dostane na kartě Návrh název, třeba Prodej. Pak se na ni dá odkazovat jménem.

## Filtr
*Jen 1.A*

- Filtr Trida = 1.A ostatní řádky schová. Nesmaže je.
- Zrušení filtru je Vybrat vše, nebo Vymazat filtr na kartě Data.
- Řádek souhrnů tabulky sečte jen to, co filtr nechá vidět.
- Obyčejná SUMA přes stejné buňky sečte i řádky, které filtr schoval. Na filtrovaný součet to není.

> Tip: Když po filtru zmizí skoro všechno, podívejte se, jestli nemáte mezeru v názvu třídy. 1.A a 1.A s mezerou navíc jsou dvě hodnoty.

## Kontingenční tabulka
*Součet podle třídy*

- Kontingenční tabulka je na kartě Vložení. Zdroj je tabulka i se záhlavím.
- Do řádků přijde Trida. Do hodnot Součet z Castka. Sloupce nechte prázdné, dokud není druhý rozměr.
- Známky se v kontingenci nesčítají. U známek dejte průměr, u peněz součet.
- Kontingence patří na nový list. Když ji položíte přes zdroj, zdroj přepíšete.

> Tip: Po změně dat dejte Obnovit. Kontingenční tabulka se sama nehne.

## Zadání: prodej po třídách
*Cvičení*

- List Prodej: aspoň 12 řádků, sloupce Polozka, Trida, Castka. Třídy jen 1.A a 1.B.
- Ctrl+T a název tabulky Prodej. Filtr ať umí ukázat jen 1.A a zase všechno.
- Kontingenční tabulka na novém listu: řádky Trida, hodnoty součet Castka.
- Uložte prodej.xlsx do složky Skola\1-rocnik\excel. Ruční součty vedle kontingence nepište.

> Tip: Když kontingence ukáže Počet místo Součet, v hodnotách počítá buňky. Přepněte souhrn na Součet.

# Kvíz

## Co udělá Ctrl+T na seznamu se záhlavím?
- Smaže ho
- [x] Udělá z něj tabulku s filtrem
- Převede ho na PDF
- Uzamkne list heslem
> Tabulka má filtr v záhlaví a jde pojmenovat. Obyčejná oblast to nemá.

## Co sečte jen řádky, které filtr nechá vidět?
- Obyčejná SUMA přes celou oblast
- [x] Řádek souhrnů tabulky
- Nadpis grafu
- Ručně opsaný součet
> SUMA schované řádky filtru pořád počítá. Řádek souhrnů tabulky bere jen viditelné.

## Kam v kontingenční tabulce patří Trida a kam Castka?
- Trida do hodnot jako součet, Castka do řádků
- [x] Trida do řádků, součet Castka do hodnot
- Obojí do filtrů a nikam jinam
- Trida do názvu souboru
> Řádky jsou třídy. Hodnoty jsou součet peněz. Součet názvů tříd nedává smysl.

## Kdy se u známek v kontingenci použije průměr?
- Vždy se známky sčítají
- [x] Když má vyjít průměrná známka, ne součet známek
- Průměr Excel neumí
- Jen u textu
> Součet známek nic neříká. Peníze se sčítají, známky se průměrují.
