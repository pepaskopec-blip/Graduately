---
id: pg-2-chytre-objekty
nazev: Chytré objekty a filtry
popis: Rozostření a doostření, které jde změnit.
nazev_cviceni: Kvíz: filtry
nadpis_kvizu: Doplňte hodnoty filtru
---

# Výklad

## Chytrý objekt
*Obal*

- Vrstva > Chytré objekty > Převést na chytrý objekt zabalí vrstvu do obalu.
- Filtr na chytrém objektu zůstane jako položka pod vrstvou. Dvojklik ho otevře znovu.
- Filtr na obyčejné vrstvě pixely přepíše. Ve školním plakátu ho neberte.
- Logo z Illustratoru vkládejte rovnou jako chytrý objekt, ať jde zvětšit.

> Tip: V rohu miniatury chytrého objektu je malá ikona. Když tam není, filtr se zapíše natvrdo.

## Rozostření pozadí
*Hloubka*

- Gaussovské rozostření oddělí pozadí od předmětu. Na pozadí stačí 8 px.
- Předmět rozostřením nechte být. Buď je na vrstvě nad ním, nebo má masku.
- Silnější rozostření, třeba 30 px, už pozadí smaže. Berte ho jen jako záměr.
- Rozostření celého obrazu včetně tváře je chyba, ne hloubka ostrosti.

> Tip: Nejdřív výřez maskou z minulé lekce, potom rozostření jen vrstvy pozadí.

## Doostření
*Unsharp*

- Filtr Doostření (Unsharp Mask) zvedne kontrast na hraně. Není to zvětšení pixelů.
- Školní hodnoty na fotku: množství 80 %, poloměr 1 px, práh 2.
- Vyšší množství dělá světlý a tmavý obrys kolem větví a vlasů.
- Doostřujte až nakonec a jen jednou. Doostřené doostření je vidět.

> Tip: Náhled filtru přepínejte, ať vidíte rozdíl. Na 100 %, ne na zmenšeném plátně.

## Zadání: hloubka a hrana
*Cvičení*

- Pozadí převeďte na chytrý objekt a dejte mu Gaussovské rozostření 8 px.
- Předmět nechte ostrý. Maska nebo samostatná vrstva ho chrání.
- Na předmět dejte Doostření 80 %, poloměr 1 px, práh 2, taky jako filtr chytrého objektu.
- Uložte filtry.psd. Oba filtry musí jít otevřít dvojklikem a hodnoty změnit.

> Tip: Když dvojklik filtr neotevře, vrstva nebyla chytrý objekt. Vezměte krok z historie a převeďte ji znovu.

# Kvíz

## Jaké rozostření má školní pozadí?
- [x] 8 px Gaussovské
- 80 px na celou fotku včetně tváře
- 0 px
- Rozostření se dělá v Premiere
> 8 px pozadí oddělí a ještě ho nechá poznat. Celý obraz včetně předmětu se nerozostřuje.

## Jaké hodnoty má školní Doostření?
- [x] množství 80 %, poloměr 1 px, práh 2
- množství 300 %, poloměr 20 px
- jen krytí 70 %
- 300 ppi
> Malý poloměr drží hranu. Velký poloměr kreslí kolem předmětů svatozář.

## Proč filtr patří na chytrý objekt?
- Jinak nejde uložit PSD
- [x] Hodnoty jdou potom změnit a pixely se nepřepíšou
- Chytrý objekt je vždy CMYK
- Kvůli 25 sn/s
> Pod vrstvou zůstane řádek filtru. Dvojklik ho otevře se stejnými čísly.

## Co Doostření ve skutečnosti dělá?
- Dopočítá nové pixely jako při zvětšení
- [x] Zvedne kontrast na hraně
- Převede fotku na křivky
- Přidá masku
> Unsharp Mask není zvětšení. Měkkou fotku z malého souboru nezachrání, jen zdůrazní hranu.
