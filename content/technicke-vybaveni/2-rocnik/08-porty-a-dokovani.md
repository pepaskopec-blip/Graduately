---
id: hardware-2-porty-a-dokovani
puvodni: hw2.8
nazev: Porty a dokování
popis: Co z notebooku opravdu vyleze.
nazev_cviceni: Kvíz: Porty a dokování
nadpis_kvizu: Kvíz k tématu Porty a dokování
---

# Výklad

## Jeden kabel, mnoho omezení
*Dok*

- Dok může nést obraz, síť i napájení.
- Limit je nejslabší článek: port, kabel nebo dok.
- Dva externí monitory chtějí port, který to umí, ne přání uživatele.
- Napájení přes dok nemusí stačit hernímu notebooku.

> Tip: levný USB hub není dok. Nepřenese obraz, který port neumí.

## Kabel z notebooku
*Síť*

- USB adaptér je síťová karta mimo šasi.
- Rychlost adaptéru bývá 1 Gb/s, ne rychlost školní páteře.
- Dok s vlastní síťovkou má vlastní MAC adresu.
- Rezervace DHCP na MAC notebooku pak neplatí pro dok.

> Tip: mnoho tenkých notebooků už ethernet nemá a potřebuje adaptér.

## Na co se ptát
*Výběr*

- Kolik obrazů, jaké rozlišení a jestli se notebook nabíjí.
- Jestli síť jede z doku, nebo se čeká Wi-Fi.
- Jestli čtečka karet a USB sedí verzi, kterou škola používá.
- Manuál portu je důležitější než fotka konektoru.

> Tip: stejný USB-C na dvou noteboocích nemá stejné schopnosti.


# Kvíz

## Kdy USB-C dok nepřenese dva monitory?
- [x] Když port notebooku umí jen data
- Když je dok černý
- Když je v učebně VLAN
- Když má disk NVMe
> Dok neumí vyrobit schopnost, kterou port nemá.

## Proč DHCP rezervace na notebook neplatí přes dok?
- [x] Dok má vlastní MAC adresu
- DHCP doky zakazuje
- VLAN maže rezervace
- Ethernet nemá MAC
> Rezervace sedí na adresu, kterou server vidí.

## Co je nejslabší článek řetězu obrazu?
- [x] Port, kabel nebo dok, podle toho, co umí nejmíň
- Vždy monitor
- Vždy procesor
- Vždy tapeta
> Datový tok musí projít všemi.

## Čím se liší USB hub od doku s obrazem?
- [x] Hub sám o sobě obraz z datového portu nevyrobí
- Hub je vždy rychlejší
- Dok neumí napájení
- Jsou to totéž
> Obraz chce alternativní režim nebo DisplayPort v kabelu.
