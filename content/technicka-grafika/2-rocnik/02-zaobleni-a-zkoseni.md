---
id: tg-2-zaobleni-zkoseni
nazev: Zaoblení a zkosení
popis: Roh poloměrem nebo dvěma vzdálenostmi.
nazev_cviceni: Kvíz: rohy
nadpis_kvizu: Dopočítejte roh
---

# Výklad

## Zaoblení
*Poloměr*

- `_FILLET` spojí dvě úsečky obloukem. Volba R nastaví poloměr.
- Poloměr 8 znamená oblouk o poloměru 8 mm, ne průměr.
- R0 ostrý roh jen napojí, oblouk nevznikne. Hodí se na nedotažené čáry.
- Příkaz si poloměr pamatuje. Než zaoblíte jiný roh, R zkontrolujte.

> Tip: u drážky se zaobleným koncem je poloměr polovina šířky drážky. Drážka 16 mm má R8.

## Zkosení
*Dvě vzdálenosti*

- `_CHAMFER` srazí roh úsečkou. Volba D zadá dvě vzdálenosti od vrcholu.
- Stejné vzdálenosti 2 a 2 srazí roh pod 45 stupňů, pokud jsou hrany kolmé.
- Různé vzdálenosti, třeba 2 a 4, dají šikminu, která není 45 stupňů.
- Zkosení se kótuje délkou a úhlem, nebo oběma délkami. Ne poloměrem.

> Tip: na výkresu je zkosení 2 × 45°. V příkazu jsou to vzdálenosti 2 a 2, ne úhel napsaný místo D.

## Kdy který roh
*Dílec*

- Zaoblení bývá tam, kde se obrábí frézou nebo kde ostrá hrana vadí.
- Zkosení bývá na hraně díry a na konci hřídele, aby díl navedl protikus.
- Na jednom rohu nedávejte zaoblení i zkosení. Zadání říká jen jedno.
- Po zaoblení se změní délka rovné hrany. Kóta rovné části už není celá šířka.

> Tip: hrana dlouhá 40 mm se dvěma zaobleními R5 na koncích má rovný úsek 30 mm.

## Zadání: drážka a sražení
*Cvičení*

- Obdélník 80 × 30. Na obou kratších koncích `_FILLET` s R8, na obou dlouhých hranách.
- Vznikne oválná drážka: rovný střed je dlouhý 64 mm, šířka zůstane 30? Pozor, R8 na výšce 30 nejde, šířka drážky musí být 16.
- Překreslete tvar jako drážku 64 × 16 a konce zaoblete R8.
- Jeden vnější roh destičky 80 × 50 mimo drážku srazte `_CHAMFER` 2 a 2.

> Tip: poloměr zaoblení konce drážky se rovná polovině její šířky. 16 / 2 = 8.


# Kvíz

## Co nastaví `_FILLET` volbou R8?
- Zkosení 8 × 8
- [x] Oblouk o poloměru 8 mm
- Průměr 8
- Osazení o 8 mm
> R je poloměr oblouku. Průměr oblouku by byl 16.

## Jak v příkazu zadáte zkosení 2 × 45° na kolmém rohu?
- R2
- [x] Vzdálenosti 2 a 2
- Úhel 45 místo obou vzdáleností, bez délek
- Ekvidistantu 2
> Stejné vzdálenosti na kolmých hranách dají 45 stupňů. Do příkazu patří délky.

## Jaký poloměr má konec drážky široké 16 mm?
- 16
- [x] 8
- 4
- 32
> Konec je půlkruh. Jeho poloměr je polovina šířky.

## Co udělá zaoblení R0?
- Smaže roh
- [x] Napojí čáry ostrým rohem
- Udělá kružnici
- Nastaví měřítko
> Nulový poloměr je způsob, jak dvě minuté čáry spojit do ostrého rohu.
