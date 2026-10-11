---
id: tg-2-koty-linearni
nazev: Lineární kóty
popis: Skutečný rozměr, pomocné čáry a styl kót.
nazev_cviceni: Kvíz: lineární kóty
nadpis_kvizu: Přečtěte kótu správně
---

# Výklad

## Co kóta říká
*Skutečný rozměr*

- Kóta je skutečný rozměr součásti, ne délka čáry na papíře.
- Na výkresu 1:2 je čára poloviční a číslo zůstává celé.
- `_DIMLINEAR` chce počátek první pomocné čáry, druhé a polohu kótovací čáry.
- Body chytejte úchopem. Kóta z bodu vedle rohu je o milimetr špatně a tváří se přesně.

> Tip: číslo na kótě nepřepisujte ručně. Když nesedí, je špatně geometrie, ne text.

## Vzhled
*Čitelné číslo*

- Pomocné čáry odstupují od obrysu asi o 1 mm a přesahují kótovací čáru.
- Sousední kóty mají stejný odstup, třeba 10 mm od obrysu a 7 mm mezi sebou.
- Číslo se píše nad kótovací čáru, ne přes obrys. Kóta neprotíná šrafu, když to jde jinak.
- `_DIMSTYLE` nastaví šipky, písmo a jednotky jednou pro celý výkres.

> Tip: výška písma na A4 bývá 3,5 mm. Menší text se na tisku nedá číst.

## Řetězec a základna
*Víc rozměrů*

- Řetězec klade kóty za sebou. Součet dílčích kót má dát celkový rozměr.
- Kóta od základny měří všechno od jedné hrany. Hodí se na rozteče od dorazu.
- Celkový rozměr a součet dílčích se na jedné hraně neopakují bez důvodu.
- Kóta patří tam, odkud se rozměr bude vyrábět, ne doprostřed šrafy.

> Tip: 20 + 45 + 15 = 80. Když součet nesedí s celkovou kótou, jedna pomocná čára minula roh.

## Zadání: destička 120 × 70
*Cvičení*

- Okótujte šířku 120 a výšku 70.
- Vodorovně přidejte 15 od levého kraje ke středu díry a 90 mezi středy děr.
- Svisle 15 od dolní hrany ke středu díry.
- Všechny kóty na hladinu KOTA. `_DIST` mezi rohy musí ukázat stejná čísla jako kóty.

> Tip: kótujte středy děr, ne jejich okraje. Okraj určí až kóta průměru.


# Kvíz

## Co se píše do lineární kóty na výkresu 1:2?
- Polovina skutečného rozměru
- [x] Skutečný rozměr
- Dvojnásobek
- Měřítko 1:2 místo čísla
> Měřítko mění délku čáry na papíře. Číslo kóty je rozměr součásti.

## Proč nepřepisovat text kóty ručně?
- AutoCAD to zakazuje vždy
- [x] Špatné číslo skrývá chybu geometrie
- Text musí být v palcích
- Kóta by zmizela z hladiny
> Správná kóta měří geometrii. Ruční text 120 na čáře dlouhé 119 je lež.

## Od čeho se kótuje poloha díry?
- Od okraje kružnice
- [x] Od středu
- Od osy razítka
- Od nejbližší šrafy
> Výroba potřebuje střed. Průměr díry je samostatná kóta.

## Co má platit pro řetězec 20, 45 a 15 na součásti dlouhé 80?
- Součet má být 100
- [x] Součet má být 80
- Celková kóta se vynechá vždy
- Řetězec se kreslí čerchovaně
> 20 + 45 + 15 = 80. Nesoulad znamená minutý úchop na některé pomocné čáře.
