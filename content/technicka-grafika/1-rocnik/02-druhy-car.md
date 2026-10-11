---
id: tg-1-druhy-car
nazev: Druhy a tloušťky čar
popis: Obrys, skryté hrany, osy a kóty podle normy.
nazev_cviceni: Kvíz: čáry
nadpis_kvizu: Poznejte druh čáry
---

# Výklad

## Tlustá plná čára
*Obrys*

- Viditelné hrany a obrysy se kreslí souvislou tlustou čarou.
- V AutoCADu jí patří vlastní hladina, třeba OBRYS, s tloušťkou 0,5 mm.
- Tloušťka se nastavuje na hladině, ne u každé čáry zvlášť.
- Obrys nesmí být stejně tenký jako kóta, jinak se výkres nedá číst.

> Tip: hladinu OBRYS nastavte jako aktuální dřív, než začnete kreslit tvar.

## Tenké čáry
*Kóty a hrany*

- Kóty, odkazy, šrafy a pomocné čáry jsou souvislé a tenké, kolem 0,25 mm.
- Skryté hrany, které z daného pohledu nejsou vidět, jsou čárkované a tenké.
- Osa díry nebo válce je čerchovaná: dlouhá čárka, tečka, dlouhá čárka.
- Osa přesahuje obrys jen o kousek, nepřetahuje se přes celý výkres.

> Tip: čárkovaná čára je skrytá hrana. Čerchovaná je osa. Nejsou totéž.

## Jak to nastavit v AutoCADu
*Hladiny*

- Příkaz `_LAYER` otevře správce hladin. Založte OBRYS, SKRYTE, OSA a KOTA.
- U SKRYTE zvolte typ čáry DASHED, u OSA typ CENTER.
- Typ čáry se načítá příkazem `_LINETYPE`. Bez načtení ho hladina nenabídne.
- Měřítko typu čáry `_LTSCALE` zvětší nebo zmenší mezery v čárkování, ať je čára čitelná.

> Tip: když čárkování vypadá jako plná čára, je LTSCALE moc velké nebo moc malé.

## Zadání: tři čáry jedné díry
*Cvičení*

- Nakreslete obdélník 60 × 40 tlustou plnou čarou.
- Svislou osu veďte středem a nechte ji přesáhnout obrys o 3 mm na každou stranu.
- Jednu zadní hranu, která z pohledu není vidět, nakreslete tenkou čárkovanou čarou.
- Každý druh čáry musí ležet na své hladině.

> Tip: osa dlouhá 46 mm při výšce tělesa 40 mm přesahuje o 3 mm nahoře i dole.


# Kvíz

## Jakou čarou se kreslí viditelný obrys?
- Tenkou čárkovanou
- [x] Tlustou plnou
- Tenkou čerchovanou
- Tlustou čerchovanou
> Viditelné hrany jsou souvislé a tlustší než kóty, osy i skryté hrany.

## Co znamená tenká čárkovaná čára?
- Osu díry
- [x] Skrytou hranu
- Obrys
- Šrafu
> Čárkovaná čára je hrana, kterou v daném pohledu nevidíme. Osa je čerchovaná.

## Jakou čarou se kreslí osa válce?
- Tlustou plnou
- Tenkou čárkovanou
- [x] Tenkou čerchovanou
- Vlnovkou
> Osa je dlouhá čárka, tečka, dlouhá čára. V AutoCADu je to typ CENTER.

## Proč má mít obrys vlastní hladinu?
- Aby šel smazat jen razítko
- [x] Aby měla tloušťka a typ čáry jedno místo
- Hladiny v AutoCADu nejsou
- Obrys se kreslí jen od ruky
> Tloušťka i typ se nastaví na hladině. Všechny obrysy je pak mají stejné.
