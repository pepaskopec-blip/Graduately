---
id: site-3-qos
puvodni: net3.6
nazev: QoS
popis: Když je linka plná, někdo musí dostat přednost.
nazev_cviceni: Kvíz: QoS
nadpis_kvizu: Kvíz k tématu QoS
---

# Výklad

## Ne všechno snese čekání
*Fronty*

- Hlas a videohovor kazí zpoždění a kolísání.
- Záloha disku snese čekání líp než telefonát.
- Fronta na úzké lince řadí pakety.
- Bez označení provozu switch neví, co je hlas.

> Tip: QoS nevytvoří rychlost, jen rozhodne, kdo počká.

## Kdo si řekne o přednost
*Značky*

- IP hlavička umí nést DSCP.
- Ethernet umí prioritu na VLAN značce.
- Na vstupu se značka přepíše podle politiky.
- Hlasová VLAN se často značí automaticky.

> Tip: značce z koncového portu nevěřte, uživatel si ji může nastavit sám.

## Kde to má smysl
*Praxe*

- Úzké hrdlo bývá WAN, ne páteřní switch.
- Rezervace pro hlas nesmí sežrat celou linku.
- Testujte hovor ve chvíli, kdy běží velká kopie.
- Špatná politika umí zpomalit i správu sítě.

> Tip: na prázdné gigabitové lince QoS nic viditelného neudělá.


# Kvíz

## Co QoS neumí?
- [x] Vyrobit větší rychlost linky
- Dát přednost hlasu před zálohou
- Omezit kolísání hovoru
- Označit provoz
> Dělí existující kapacitu, nepřidá ji.

## Proč se značka z počítače přepisuje?
- [x] Uživatel by si jinak dal nejvyšší prioritu
- DSCP nejde číst
- IPv4 značky zakazuje
- STP značky maže
> Důvěryhodná značka vznikne na vstupu podle pravidel.

## Kde QoS obvykle pomůže nejdřív?
- [x] Na pomalé WAN lince
- Na prázdném 10Gb portu mezi switchi
- Uvnitř jednoho počítače na smyčce
- Na vypnutém Wi-Fi
> Přednost se projeví, až když se linka plní.

## Který provoz čekání snese hůř?
- [x] Telefonní hovor
- Noční kopie disku
- Stahování aktualizace
- Archiv pošty
> Hlas potřebuje malé a stálé zpoždění.
