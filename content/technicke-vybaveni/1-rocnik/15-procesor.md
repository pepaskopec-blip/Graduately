---
id: hardware-1-procesor
puvodni: hw.15
nazev: Procesor
popis: 3 snímky o CPU, jádrech a chlazení a pak kvíz.
nazev_cviceni: Kvíz: procesor
nadpis_kvizu: Kvíz k procesoru
---

# Výklad

## Procesor
*Mozek*

- CPU je hlavní výpočetní jednotka a řídí ostatní části.
- Je to křemíkový čip z miliard tranzistorů, které pracují jako spínače.
- Frekvence je počet cyklů za sekundu (Hz, v praxi GHz).
- Takt vznikne jako součin base clock (100 MHz) a násobiče.

> Tip: takt = základní hodiny 100 MHz × násobič.

## Více jader, vlákna a cache
*Jádra*

- Více jader jsou samostatné jednotky pro souběžnou práci.
- Hyper-Threading / SMT nechá jedno jádro zpracovat dvě vlákna najednou.
- Cache je vyrovnávací paměť mezi rychlým jádrem a pomalejšími součástmi.
- Bývá ve třech úrovních: L1, L2 a L3.

> Tip: cache L1, L2 a L3 zmenšují čekání na pomalejší paměť.

## Chlazení procesoru
*Teplo*

- Chladič odvádí teplo z čipu.
- Vzduchové chlazení je pasivní blok a ventilátor.
- Je levné a spolehlivé.
- Vodní all-in-one vede teplo kapalinou do radiátoru.
- U procesoru tak zabere méně místa.

> Tip: chlazení prodlužuje životnost a dovolí přetaktování.


# Kvíz

## Jak vzniká takt procesoru?
- Jen z napětí 12 V
- [x] Jako součin base clock 100 MHz a násobiče
- Jen z počtu USB portů
- Z rychlosti ventilátoru ve skříni

## Co umí Hyper-Threading / SMT?
- [x] Jedno jádro zpracuje dvě vlákna současně
- Vypne chlazení
- Zdvojnásobí napětí zdroje
- Nahradí operační paměť diskem

## K čemu je cache L1, L2 a L3?
- K napájení grafické karty
- K uložení BIOSu na baterii
- [x] Vyrovnává rychlostní rozdíl mezi procesorem a ostatními částmi
- K větrání zadní stěny

## Čím se vyznačuje vzduchové chlazení CPU?
- Jen kapalinou bez ventilátoru
- [x] Pasivním blokem a ventilátorem; je levné a spolehlivé
- Tím, že procesor nehřeje
- Tím, že se montuje jen na disk
