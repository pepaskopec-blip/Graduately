---
id: tg-2-orez-ekvidistanta
nazev: Ořez, prodloužení a ekvidistanta
popis: Úprava čar, které přesahují nebo mají stálý odstup.
nazev_cviceni: Kvíz: ořez a odstup
nadpis_kvizu: Vyberte příkaz a vzdálenost
---

# Výklad

## Ořez a prodloužení
*Na hranu*

- `_TRIM` uřízne kus objektu po hraně, kterou vyberete jako řeznou.
- V novějším AutoCADu stačí Enter bez výběru hran a řeže se o všechno, co čára protíná.
- `_EXTEND` dělá opak: natáhne čáru až k vybrané hraně.
- Maže se ta část, na kterou kliknete. Klik na špatnou stranu uřízne to, co mělo zůstat.

> Tip: když ořez „nebere“, čáry se ve skutečnosti neprotínají. Přibližte roh a napojte ho úchopem.

## Ekvidistanta
*Stálý odstup*

- `_OFFSET` vytvoří rovnoběžnou křivku v zadané vzdálenosti.
- Hodí se na tloušťku stěny, rámeček i osazení. Zadáte vzdálenost a stranu.
- U kružnice vznikne soustředná kružnice. Průměr se změní o dvojnásobek odstupu.
- Ekvidistanta obdélníku udělá menší nebo větší obdélník se stejnými rohy, ne se zaoblením.

> Tip: stěna tlustá 8 mm je ekvidistanta obrysu o 8 mm dovnitř. U kružnice Ø40 tak vznikne Ø24.

## Co neřeže
*Otevřený roh*

- Ořez potřebuje průsečík. Dvě čáry, které se jen míjejí, se neoříznou.
- Nejdřív je prodlužte nebo je nakreslete znovu s úchopem, teprve pak řežte.
- Ekvidistanta otevřené křivky zůstane otevřená. Rohy se samy nedovřou.
- Po ořezu zkontrolujte, že nezůstaly pahýly kratší než milimetr. Ty `_ERASE` smaže.

> Tip: pahýl po ořezu je pořád objekt. Na tisku udělá tečku, která do výkresu nepatří.

## Zadání: rámeček stěny
*Cvičení*

- Vnější obdélník 100 × 60. Ekvidistantou o 8 mm dovnitř udělejte vnitřní obrys.
- Vnitřní obdélník má tedy 84 × 44.
- Čáry, které po dalších úpravách přesáhnou vnější roh, ořízněte.
- Oba obrysy nechte na hladině OBRYS. Kóty zatím nepřidávejte.

> Tip: 100 minus dvakrát 8 je 84. 60 minus 16 je 44. Když vnitřek vyjde jinak, odstup šel ven, ne dovnitř.


# Kvíz

## Který příkaz uřízne přesahující kus čáry?
- `_OFFSET`
- [x] `_TRIM`
- `_EXTEND`
- `_MOVE`
> Ořez bere kus za řeznou hranou. Prodloužení čáru naopak natahuje.

## Ekvidistanta kružnice Ø40 o 8 mm dovnitř má průměr?
- 32
- [x] 24
- 48
- 8
> Poloměr klesne o 8, průměr o 16. 40 minus 16 je 24.

## Proč ořez na dvou čarách nic neudělá?
- Orto je zapnuté
- [x] Čáry se neprotínají
- Chybí razítko
- Objekt je kružnice
> Bez průsečíku není kde řezat. Nejdřív čáry napojte, potom řežte.

## Vnitřní obrys obdélníku 100 × 60 po ekvidistantě 8 mm dovnitř je?
- 100 × 60
- 92 × 52
- [x] 84 × 44
- 108 × 68
> Z šířky i výšky se ubere 8 mm z každé strany, tedy 16 mm. 84 × 44.
