---
id: tg-4-rotace
nazev: Rotace profilu
popis: Součást souměrná podle osy vznikne otočením řezu.
nazev_cviceni: Kvíz: rotace
nadpis_kvizu: Vyberte profil a osu
---

# Výklad

## Kdy rotovat
*Místo skládání válců*

- `_REVOLVE` otočí uzavřený profil kolem osy o zadaný úhel.
- 360 stupňů udělá plné rotační těleso. 180 udělá polovinu.
- Hodí se na hřídel, pouzdro, přírubu a každou součást souměrnou podle osy.
- Profil je polovina řezu. Osa rotace je osa součásti a profil ji smí jen lemovat, ne křížit.

> Tip: pouzdro z třetího ročníku je jeden profil stěny otočený o 360 stupňů, ne válec minus válec. Oba postupy jsou správně, rotace je u čistě rotačního dílu kratší.

## Profil
*Polovina*

- Nakreslete jen polovinu řezu, vpravo od osy. Uzavřete ho.
- U pouzdra Ø60 / Ø36 dlouhého 80 je profil obdélník 12 × 80. Dvanáct je tloušťka stěny.
- Osa je svislá hrana díry. Profil se osy dotýká jen vnější? Ne: vnitřní hrana profilu je o poloměr díry od osy.
- Vnitřní hrana leží 18 mm od osy, vnější 30 mm. Šířka profilu je 12 mm.

> Tip: 36 / 2 = 18 a 60 / 2 = 30. Profil je pás mezi těmito poloměry.

## Osa a úhel
*360*

- Osu vyberte dva body, nebo jednu úsečku nakreslenou na hladině OSA.
- Úhel 360. Menší úhel nechá výseč a na školní hřídeli bývá omyl.
- Profil nesmí osu křížit. Křížení AutoCAD odmítne, nebo udělá těleso, které se protíná samo.
- Po rotaci profil zmrazte. V řezu ho zase použijete jako kontrolu.

> Tip: když vznikne plný válec bez díry, profil začínal na ose. Vnitřní hrana musí být 18 mm od osy, ne 0.

## Zadání: pouzdro rotací
*Cvičení*

- Osa je úsečka z 0,0 do 0,80.
- Profil: obdélník s rohy 18,0 a 30,80.
- `_REVOLVE` o 360 stupňů kolem této osy.
- `_MASSPROP` objem mezikruží: π × (30² − 18²) × 80. Má vyjít zhruba 144 764 mm³.

> Tip: 900 − 324 = 576, krát π je asi 1 810, krát 80 je asi 144 800. Řád tisíců, ne desítek.


# Kvíz

## Jaký úhel udělá plné rotační těleso?
- 90
- 180
- [x] 360
- 45
> Polovina otáčky je výseč. Uzavřené pouzdro potřebuje celých 360 stupňů.

## Kde leží vnitřní hrana profilu pouzdra Ø36?
- Na ose
- [x] 18 mm od osy
- 36 mm od osy
- 60 mm od osy
> Poloměr díry je polovina průměru. Profil díru ohraničuje, nesmí začínat na ose, jinak díra zmizí.

## Co se stane, když profil osu kříží?
- Vznikne správné pouzdro
- [x] AutoCAD rotaci odmítne, nebo těleso protne samo sebe
- Profil se jen posune
- Vznikne šrafa
> Osa smí být hranou profilu, ne smí jím procházet skrz.

## Proč rotovat pouzdro místo skládání dvou válců?
- Jinak to nejde
- [x] U rotačního dílu stačí jeden profil
- Rotace umí i hranaté osazení bez profilu
- `_REVOLVE` funguje i v LT
> Oba postupy jsou správně. Rotace je kratší, když je součást souměrná podle osy. LT ji neumí.
