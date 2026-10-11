---
id: tg-4-boolean
nazev: Sjednocení, rozdíl a průnik
popis: Tělesa se sčítají a odečítají jako množiny.
nazev_cviceni: Kvíz: boolean
nadpis_kvizu: Vyberte pořadí těles
---

# Výklad

## Sjednocení
*Jeden kus*

- `_UNION` slepí tělesa, která se dotýkají nebo pronikají, v jeden objekt.
- Osazení na hřídeli jsou dva válce. Po sjednocení je to jedna součást.
- Tělesa, která se nedotýkají, sjednocení také spojí do jedné skupiny, ale výrobek to není.
- Než sjednotíte, zkontrolujte přesah. Mezera 0,01 mm nechá dva kusy.

> Tip: `_INTERFERE` ukáže, jestli se dvě tělesa opravdu pronikají, nebo se jen zdají dotýkat.

## Rozdíl
*Díra*

- `_SUBTRACT` nejdřív vyberete těleso, které má zůstat, potvrdíte, a teprve pak těleso, které se odečte.
- Obrácené pořadí smaže součást a nechá díru jako váleček. To je nejčastější chyba.
- Díra Ø10 je válec Ø10 delší, než je tloušťka stěny, ať rozdíl prořízne celou stěnu.
- Válec díry po rozdílu zmizí. Nemusíte ho mazat ručně.

> Tip: válec díry nechte o milimetr přečuhovat z obou stran. Jinak na stěně zůstane blanka.

## Průnik
*Společný objem*

- `_INTERSECT` nechá jen objem, který mají tělesa společný. Zbytek zmizí.
- Hodí se, když potřebujete tvar daný dvěma podmínkami, třeba válec seříznutý kvádrem.
- Na běžnou díru průnik není. Díra je rozdíl.
- Po průniku zůstanou obě původní tělesa pryč. Když je budete potřebovat, předem je zkopírujte.

> Tip: sjednocení přidá, rozdíl ubere, průnik nechá jen překryv. U díry je to vždy rozdíl.

## Zadání: destička s dírou
*Cvičení*

- Destička `_BOX` 120 × 70 × 10.
- Válec Ø16, střed v 60,35, výška od -1 do 11, ať prochází skrz.
- `_SUBTRACT`: nejdřív destička, potom válec.
- `_MASSPROP` objem: 84 000 minus objem válce v tloušťce 10. Válec Ø16 a výška 10 má objem π × 8² × 10 ≈ 2 011. Výsledek je asi 81 989 mm³.

> Tip: když zmizela destička a zbyl váleček, pořadí rozdílu bylo obrácené. Vraťte krok zpět.


# Kvíz

## Jakým pořadím vznikne díra?
- Nejdřív válec, potom destička
- [x] Nejdřív destička, potom válec
- `_UNION`
- `_INTERSECT`
> První výběr zůstává. Druhý se odečte. Obrácené pořadí nechá jen váleček.

## Proč má válec díry přečnívat z destičky?
- Aby šel okótovat
- [x] Aby ve stěně nezůstala blanka
- Přečnívat nesmí, rozdíl by selhal
- Aby vznikl závit
> Válec zarovnaný přesně s plochou někdy nechá tenkou slupku. Milimetr přesahu ji odstraní.

## Který příkaz slepí dva dotýkající se válce hřídele?
- `_SUBTRACT`
- [x] `_UNION`
- `_INTERSECT`
- `_REVOLVE`
> Osazení je jeden kus. Sjednocení z dvou válců udělá jedno těleso.

## Co nechá `_INTERSECT`?
- Součet objemů
- [x] Jen společný překryv
- Díru
- Původní tělesa vedle výsledku
> Zbytek obou těles zmizí. Před průnikem si je zkopírujte, pokud je ještě budete potřebovat.
