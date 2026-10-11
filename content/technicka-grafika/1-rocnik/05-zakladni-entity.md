---
id: tg-1-zakladni-entity
nazev: Úsečka, kružnice, oblouk a obdélník
popis: Čtyři příkazy, kterými začíná každá součást.
nazev_cviceni: Kvíz: základní entity
nadpis_kvizu: Vyberte správný příkaz
---

# Výklad

## Úsečka a obdélník
*Hrany*

- `_LINE` chce první a další bod. Končí se Enterem, zavření obrysu volbou Close.
- `_RECTANG` chce dva protilehlé rohy. Z bodu 0,0 a `@90,40` vznikne obdélník 90 × 40.
- Obdélník je jedna uzavřená křivka. Úsečky jsou samostatné objekty.
- Uzavřená křivka se později líp šrafuje a vysouvá do 3D.

> Tip: když budete tvar vysouvat, kreslete ho jako `_RECTANG` nebo uzavřenou `_PLINE`, ne jako čtyři volné úsečky.

## Kružnice
*Díry*

- `_CIRCLE` se nejčastěji zadá středem a poloměrem.
- Volba D přepne na průměr. U díry Ø12 je průměr 12 a poloměr 6.
- Střed díry se kótuje, ne její okraj. Okraj je důsledek průměru.
- Víc stejných děr se nekreslí znovu: první se zkopíruje nebo rozloží do pole.

> Tip: v zadání Ø znamená průměr. Do příkazu s poloměrem dáte polovinu.

## Oblouk
*Zaoblení z příkazu*

- `_ARC` umí oblouk třemi body nebo středem, počátkem a úhlem.
- Zaoblení rohu ale nedělejte obloukem od ruky. Na to je později `_FILLET`.
- Oblouk má směr. Špatné pořadí bodů ho vyklenu na druhou stranu.
- U drážky se oblouk často nahradí `_PLINE` s volbou Arc uprostřed úseček.

> Tip: tři body oblouku zadávejte v pořadí, v jakém oblouk plyne. Prostřední bod je na oblouku, ne střed kružnice.

## Zadání: destička s dírou
*Cvičení*

- `_RECTANG` z 0,0 do `@90,50`.
- Střed destičky je 45,25. Tam `_CIRCLE`, volba D, průměr 16.
- Osu svislou i vodorovnou nechte přesáhnout obrys o 3 mm. Patří na hladinu OSA.
- Uložte jako desticka-90x50.dwg.

> Tip: polovina z 90 je 45 a polovina z 50 je 25. Střed neměřte od oka.


# Kvíz

## Čím nakreslíte obdélník 90 × 40 jedním objektem?
- Čtyřmi úsečkami `_LINE`
- [x] `_RECTANG` a druhým rohem `@90,40`
- `_CIRCLE` s poloměrem 90
- `_ARC` o úhlu 90
> `_RECTANG` chce dva rohy. Relativní `@90,40` je šířka a výška od prvního rohu.

## Zadání říká díra Ø12. Jaký poloměr má kružnice?
- 12
- [x] 6
- 24
- 3
> Průměr je 12, poloměr je polovina. V `_CIRCLE` buď poloměr 6, nebo volba D a průměr 12.

## Kde leží střed destičky 90 × 50, která začíná v 0,0?
- 90,50
- [x] 45,25
- 0,0
- 25,45
> Střed je v polovině šířky a polovině výšky.

## Kdy dát přednost uzavřené křivce před čtyřmi úsečkami?
- Nikdy, úsečka je přesnější
- [x] Když se obrys bude šrafovat nebo vysouvat
- Jen u razítka
- Když je tvar otevřený
> Šrafa i vysunutí potřebují uzavřenou oblast. Čtyři úsečky, které se netrefí, oblast neutvoří.
