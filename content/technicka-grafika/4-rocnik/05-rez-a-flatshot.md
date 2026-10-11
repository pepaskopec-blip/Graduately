---
id: tg-4-rez-modelu
nazev: Řez tělesem a plochý snímek
popis: Z modelu zpátky čáry pro výkres.
nazev_cviceni: Kvíz: řez modelu
nadpis_kvizu: Vyberte, co vznikne na papíře
---

# Výklad

## Řezná rovina
*Uvnitř tělesa*

- `_SECTIONPLANE` položí rovinu řezu. Živé zobrazení ukáže řez, aniž by těleso rozřízlo natrvalo.
- `_SLICE` těleso rozřízne doopravdy. Polovinu můžete nechat, nebo smazat.
- Na školní výkres řežte živě nebo si před `_SLICE` těleso zkopírujte. Původní model budete ještě potřebovat.
- Řez osou pouzdra má ukázat stěnu 12 mm a díru Ø36, stejně jako ruční řez ve třetím ročníku.

> Tip: rovina, která mine osu, ukáže jinou tloušťku stěny. U pouzdra veďte řez osou.

## Plochý snímek
*Čáry z pohledu*

- `_FLATSHOT` promítne aktuální pohled do čar v rovině XY.
- Viditelné hrany dá na jednu hladinu, skryté na druhou. Skryté pak přepněte na čárkované.
- Než příkaz spustíte, nastavte pohled: přední pro nárys, horní pro půdorys.
- Snímek je 2D. Dá se kótovat jako výkres z druháku. Model vedle něj nechte nedotčený.

> Tip: plochý snímek pořízený v šikmém pohledu není nárys. Nejdřív srovnejte pohled, potom foťte.

## Co snímek neudělá
*Kóty a šrafa*

- Snímek neumí šrafu řezu ani kóty. Ty doplníte vy, na hladiny SRAFA a KOTA.
- Neumí ani razítko. To patří do rozvržení.
- Čáry snímku bývají na nových hladinách. Přesuňte je na OBRYS a SKRYTE, ať platí školní tloušťky.
- Když model potom změníte, snímek se sám neopraví. Po úpravě modelu ho vytvořte znovu.

> Tip: výkresové pohledy z pozdější lekce se na rozdíl od snímku umí s modelem aktualizovat. Snímek je rychlá kontrola.

## Zadání: nárys z tělesa
*Cvičení*

- Otevřete pouzdro z rotace. Přední pohled, `_FLATSHOT`, viditelné hrany na OBRYS.
- Do řezu osou doplňte šrafu stěny ručně, nebo použijte živý řez a z něj čáry.
- Okótujte Ø60, Ø36 a délku 80. Čísla musí sedět s `_MASSPROP` a s poloměry profilu.
- Snímek nesmí zůstat šikmo. Nárys je v rovině, díra je obdélník 36 × 80 uvnitř obdélníku 60 × 80.

> Tip: v nárysu řezu jsou dvě vodorovné čáry díry. Jejich vzdálenost je 36.


# Kvíz

## Čím rozříznete těleso natrvalo?
- `_FLATSHOT`
- [x] `_SLICE`
- `_ORBIT`
- `_PLOT`
> Živá řezná rovina model nechá celý. `_SLICE` ho rozdělí. Předtím si nechte kopii.

## Co musí předcházet plochému snímku nárysu?
- Tisk
- [x] Srovnaný přední pohled
- Rozbití tělesa
- Šrafa
> Snímek promítne to, co právě vidíte. Šikmý orbit není nárys.

## Doplní `_FLATSHOT` kóty?
- Ano, z hladiny KOTA
- [x] Ne
- Jen průměry
- Jen když je měřítko 1:1
> Snímek jsou čáry. Kóty, šrafu a razítko dodáte vy.

## Jak daleko jsou v nárysu řezu pouzdra od sebe čáry díry Ø36?
- 18
- 60
- [x] 36
- 12
> V řezu je díra obdélník. Jeho výška je průměr, ne poloměr.
