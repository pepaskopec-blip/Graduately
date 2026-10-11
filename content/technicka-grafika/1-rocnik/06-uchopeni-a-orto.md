---
id: tg-1-uchopeni
nazev: Úchop, orto a polární trasování
popis: Jak trefit konec čáry a kreslit kolmo.
nazev_cviceni: Kvíz: přesné kreslení
nadpis_kvizu: Vyberte úchop a přepínač
---

# Výklad

## Úchopy
*Koncový bod*

- Úchop přitáhne kurzor ke geometrii. Bez něj minuta minete konec čáry.
- Zapíná se klávesou F3 nebo v stavovém řádku. Nastavení je `_OSNAP`.
- Na začátek stačí koncový bod, střed a střed kružnice.
- Kolmice a tečna se hodí, až budete napojovat čáru na kružnici.

> Tip: úchop Střed u úsečky je její polovina. Střed u kružnice je její osa. Jsou to dva různé úchopy.

## Orto a polární úhel
*F8 a F10*

- Orto, klávesa F8, pustí kurzor jen vodorovně a svisle.
- Polární trasování F10 drží i šikmé úhly, obvykle po 45 nebo po 15 stupních.
- Orto a volné šikmé kreslení nejdou dohromady. Na šikmou čáru orto vypněte.
- Délku při zapnutém ortu napište jen číslem a Enter. Směr určuje myš.

> Tip: vodorovná čára dlouhá 40 mm je F8, kurzor doprava, napsat 40 a Enter. Nemusíte počítat druhý bod.

## Proč se výkres rozpadne
*Minutí*

- Dvě čáry, které se „téměř“ dotýkají, nejdou oříznout ani vyšrafovat.
- Mezera vznikne, když kliknete vedle koncového bodu a úchop je vypnutý.
- `_ZOOM` na roh ukáže mezeru, která na celém výkresu není vidět.
- Opravený roh se napojí úchopem, ne posunem od oka.

> Tip: než odevzdáte výkres, přibližte každý roh. Mezera menší než milimetr na tisku vynikne.

## Zadání: kříž přesně ve středu
*Cvičení*

- Nakreslete obdélník 80 × 60 z bodu 0,0.
- Svislou osu veďte z úchopu Střed dolní hrany do úchopu Střed horní hrany.
- Vodorovnou osu veďte ze Středu levé hrany do Středu pravé.
- Osy se mají protnout v bodě 40,30. Ověřte to `_ID`.

> Tip: `_ID` vypíše souřadnice úchopu. Když průsečík není 40,30, jedna osa minula střed hrany.


# Kvíz

## Která klávesa zapíná úchopy?
- F8
- [x] F3
- F10
- Esc
> F3 jsou úchopy, F8 je orto, F10 je polární trasování.

## Co dělá orto?
- Chytá konce čar
- [x] Drží kreslení ve vodorovném a svislém směru
- Nastaví milimetry
- Maže poslední úsečku
> Se zapnutým ortem kurzor neuhne do šikma. Délku pak stačí napsat číslem.

## Proč dvě čáry, které se na obrazovce dotýkají, nejdou vyšrafovat?
- Šrafa vyžaduje kóty
- [x] Mezi nimi je mezera, konec minul úchop
- Hladina OSA to zakazuje
- Orto je zapnuté
> Od oka minutý konec nechá škvíru. Přiblížení a úchop Koncový bod ji ukážou.

## Který úchop najde polovinu úsečky?
- Střed kružnice
- [x] Střed
- Koncový bod
- Kolmice
> Úchop Střed na úsečce je její polovina. Na kružnici je to jiný úchop, střed kružnice.
