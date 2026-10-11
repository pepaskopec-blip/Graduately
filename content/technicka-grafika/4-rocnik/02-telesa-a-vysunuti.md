---
id: tg-4-vysunuti
nazev: Tělesa a vysunutí
popis: Kvádr, válec a profil tažený o výšku.
nazev_cviceni: Kvíz: vysunutí
nadpis_kvizu: Vyberte profil a výšku
---

# Výklad

## Hotová tělesa
*Kvádr a válec*

- `_BOX` chce roh nebo střed základny, půdorys a výšku.
- `_CYLINDER` chce střed, poloměr nebo průměr a výšku.
- Výška se měří ve směru osy Z aktuálního UCS.
- Tato tělesa jsou plná. Nejsou to stěny z ploch.

> Tip: válec Ø40 a výška 25 má v příkazu buď poloměr 20, nebo volbu průměru 40.

## Vysunutí profilu
*Z 2D do výšky*

- `_EXTRUDE` vezme uzavřenou křivku nebo oblast a táhne ji o výšku.
- Profil musí být zavřený. Čtyři nenapojené úsečky vysunout nejdou.
- Kladná výška jde ve směru Z. Záporná na opačnou stranu.
- `_PRESSPULL` umí přitáhnout uzavřenou oblast, na kterou kliknete, bez předchozího výběru.

> Tip: obdélník 80 × 50 vysunutý o 12 je destička z druháku, teď jako těleso.

## Profil nejdřív ve 2D
*Plocha*

- Profil nakreslete v rovině, ze které chcete táhnout. Nejčastěji půdorys, Z ven z obrazovky.
- `_REGION` z uzavřené křivky udělá jednu plochu. Vysouvá se spolehlivěji než volné čáry.
- Po vysunutí profil smíte smazat, nebo ho nechte na hladině PROFIL a tu zmrazte.
- Jeden profil, jedna výška. Osazení vznikne až druhým tělesem a booleanem v další lekci.

> Tip: otevřená křivka se vysune na plochu, ne na těleso. Pro součást potřebujete těleso.

## Zadání: destička jako těleso
*Cvičení*

- Profil 120 × 70, `_EXTRUDE` o 10 mm. To je tloušťka destičky.
- Díru zatím nekreslete. Ověřte `_MASSPROP`: objem má být 120 × 70 × 10 = 84 000 mm³.
- Přední pohled ukáže 120 × 10. Horní pohled 120 × 70.
- Uložte jako desticka-3d.dwg.

> Tip: když objem vyjde dvojnásobek, výška je 20 místo 10. Kóta textu to nespraví, `_MASSPROP` čte model.


# Kvíz

## Jaký poloměr má `_CYLINDER` o průměru 40?
- 40
- [x] 20
- 80
- 10
> Příkaz bez přepnutí bere poloměr. Průměr je volba, nebo napište polovinu.

## Co `_EXTRUDE` potřebuje, aby vzniklo těleso?
- Otevřenou úsečku
- [x] Uzavřený profil
- Razítko
- Už hotový válec
> Neuzavřený profil vytvoří jen plochu. Těleso má mít objem.

## Kam míří kladná výška vysunutí?
- Vždy doprava na obrazovce
- [x] Ve směru osy Z aktuálního UCS
- Do záporného X
- Do razítka
> Špatně natočený UCS vysune destičku „na bok“. Před příkazem zkontrolujte osy.

## Objem destičky 120 × 70 × 10 mm je?
- 840 mm³
- [x] 84 000 mm³
- 200 mm³
- 12 000 mm³
> 120 × 70 = 8 400, krát 10 je 84 000. `_MASSPROP` má ukázat totéž.
