---
id: tg-1-zadani-desticka
nazev: Zadání: destička
popis: Celý výkres z prvního ročníku na jedné součásti.
nazev_cviceni: Kvíz: destička
nadpis_kvizu: Zkontrolujte hotový výkres
---

# Výklad

## Co se odevzdává
*Zadání*

- Destička 120 × 70 mm, tloušťka se nekreslí, jde o jeden pohled.
- Čtyři díry Ø8. Středy jsou 15 mm od levého i pravého kraje a 15 mm od dolního i horního kraje.
- Uprostřed je podélná drážka 40 × 16 se zaoblenými konci. Zatím stačí obdélník, zaoblení přijde ve druháku.
- Výkres je na A4, měřítko 1:1, rámeček a razítko z první lekce.

> Tip: středy děr jsou body 15,15, pak 105,15, pak 15,55 a 105,55. 120 minus 15 je 105, 70 minus 15 je 55.

## Hladiny a čáry
*Pořádek*

- OBRYS je tlustá plná čára. OSA je tenká čerchovaná. KOTA zatím nechte prázdnou.
- Každá díra má svislou i vodorovnou osu, která přesahuje obrys o 3 mm.
- Destička má jednu vodorovnou a jednu svislou osu souměrnosti přes celý tvar.
- Skryté hrany tu nejsou. Pohled je zepředu a díry jsou průchozí, jejich okraj je vidět.

> Tip: průchozí díra v tomto pohledu není čárkovaná. Čárkovaná by byla hrana schovaná za materiálem.

## Postup
*Po řadě*

- Nejdřív rámeček a razítko, potom obrys `_RECTANG` 120 × 70.
- Jednu díru `_CIRCLE` o průměru 8 a další tři `_COPY` nebo dvě `_MIRROR`.
- Drážku `_RECTANG` 40 × 16 se středem ve středu destičky. Její levý dolní roh je 40,27.
- Osy dokreslete úchopem Střed. `_DIST` mezi protilehlými dírami ve vodorovném směru má být 90.

> Tip: střed destičky je 60,35. Drážka sahá 20 mm doleva a doprava a 8 mm nahoru a dolů.

## Kontrola před odevzdáním
*Co musí sedět*

- V razítku je název Destička, měřítko 1:1 a materiál, třeba ocel.
- Žádná čára obrysu neleží na hladině OSA a žádná osa není plná.
- Přiblížené rohy nemají mezeru. Kružnice se dotýká svých os ve středu.
- Soubor se jmenuje desticka-120x70.dwg a je v milimetrech.

> Tip: vypněte hladinu OBRYS. Zůstat mají jen osy a rámeček. Zapněte ji zpět a totéž udělejte s OSA.


# Kvíz

## Kde leží střed pravé horní díry?
- 15,15
- [x] 105,55
- 120,70
- 60,35
> Od pravého kraje 15 mm je x = 105. Od horního kraje 15 mm je y = 55.

## Jaká je vodorovná rozteč levé a pravé díry?
- 120
- 15
- [x] 90
- 105
> 105 minus 15 je 90. Tolik má ukázat `_DIST` mezi středy.

## Kam patří obrys destičky?
- Na hladinu OSA
- [x] Na hladinu OBRYS, tlustou plnou čarou
- Čárkovaně, protože díry jsou průchozí
- Do razítka
> Viditelný obrys je tlustá plná čára. Osa je samostatná hladina.

## Proč se průchozí díra v tomto pohledu nekreslí čárkovaně?
- Čárkovaná čára je zakázaná
- [x] Její okraj je z pohledu vidět
- Díra se kreslí jen osou
- Měřítko je 1:2
> Čárkovaně se kreslí hrana, kterou materiál zakrývá. Okraj díry v čelním pohledu vidíme.
