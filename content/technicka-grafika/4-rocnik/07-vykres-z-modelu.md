---
id: tg-4-vykres-z-modelu
nazev: Výkres z 3D modelu
popis: Nárys a řez, které se s modelem aktualizují.
nazev_cviceni: Kvíz: pohledy z modelu
nadpis_kvizu: Propojte výkres s tělesem
---

# Výklad

## Základní pohled
*Z tělesa*

- V rozvržení příkaz základního pohledu vezme těleso a položí na papír nárys, půdorys nebo izometrii.
- Měřítko pohledu nastavíte jako u výřezu, třeba 1:1 nebo 1:2.
- Odvozené pohledy se položí nad, pod nebo vedle. Drží evropské promítání, když ho tak nastavíte.
- Pohled je svázaný s modelem. Změna tělesa a aktualizace pohledu přepíše čáry.

> Tip: plochý snímek se sám neopraví. Pohled z modelu ano. Na maturitní výkres použijte pohled.

## Řez pohledu
*A-A*

- Z základního pohledu uděláte řez tažením řezné čáry. Písmena A doplní AutoCAD, nebo je přepište na A.
- Šrafu řezu nastavte na ANSI31 a školní měřítko. Hřídel v podélném řezu pořád nešrafujte, i když to AutoCAD nabídne.
- Skryté čáry v pohledu vypněte, pokud řez vnitřek ukázal. Výkres zůstane čistý.
- Kóty klikejte na pohled, ne do modelu naslepo. Berou rozměr z tělesa.

> Tip: kóta napojená na pohled se po aktualizaci přepočítá. Kóta nakreslená vedle jako obyčejná čára ne.

## Papír
*A3*

- Rámeček a razítko jsou v rozvržení, měřítko 1:1 vůči papíru.
- Tisk je Layout, 1:1, monochrome.ctb, jako ve třetím ročníku.
- Izometrický pohled bez kót smí být třetí obraz. Nesmí být jediný.
- Než odevzdáte, aktualizujte pohledy. Starý pohled lže, když jste model mezitím změnili.

> Tip: ve stromu nebo přes aktualizaci pohledu uvidíte, jestli je obraz aktuální. Nevěřte čáře, která přežila smazanou díru.

## Zadání: pouzdro na A3
*Cvičení*

- Z 3D pouzdra vytvořte nárys jako řez A-A a půdorys nebo bokorys s roztečí, pokud na modelu díry jsou.
- Měřítko 1:1, formát A3. Kóty Ø60, Ø36 a 80.
- Změňte v modelu délku na 90, aktualizujte pohled a přesvědčte se, že kóta délky ukáže 90.
- Vraťte délku na 80, ať odevzdáte zadání, ne zkoušku. PDF pojmenujte pouzdro-z-modelu.pdf.

> Tip: když po změně modelu kóta zůstane 80, není napojená na pohled. Smažte ji a okótujte znovu.


# Kvíz

## Čím se pohled z modelu liší od `_FLATSHOT`?
- Ničím
- [x] Po změně tělesa se dá aktualizovat
- Umí jen izometrii
- Funguje v LT
> Snímek je jednorázová kopie čar. Pohled zůstává svázaný s tělesem.

## Kam patří rámeček výkresu z modelu?
- Dovnitř tělesa
- [x] Do rozvržení
- Na hladinu OBRYS modelu
- Do definice bloku
> Papír má vlastní měřítko. Rámeček v modelu by se zmenšil s pohledem.

## Co uděláte po změně délky pouzdra v modelu?
- Nic, kóty se nemění nikdy
- [x] Aktualizujete pohled
- Smažete razítko
- Přepnete na LT
> Neaktualizovaný pohled ukazuje starý tvar. Kóta napojená na něj se přepočítá spolu s ním.

## Jaké měřítko má tisk tohoto rozvržení?
- 1:2, protože model je velký
- [x] 1:1
- Podle `_MASSPROP`
- Fit
> Měřítko obrazu drží pohled. Tisk papíru je 1:1, jinak se měřítko zdvojí.
