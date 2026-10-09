---
id: hardware-2-sbernice-a-disky
puvodni: hw2.3
nazev: Sběrnice a disky
popis: USB, SATA a NVMe nejsou zaměnitelné jen konektorem.
nazev_cviceni: Kvíz: Sběrnice a disky
nadpis_kvizu: Kvíz k tématu Sběrnice a disky
---

# Výklad

## Versie a napájení
*USB*

- USB 2.0, 3.x a 4 se liší tokem dat.
- USB-C je tvar, ne záruka rychlosti.
- Thunderbolt po stejném tvaru umí obraz, síť i disk.
- Hub bez napájení nestačí pro víc disků.

> Tip: port USB-A 2.0 fyzicky vezme i rychlý flash disk a stáhne ho na svou rychlost.

## Klasické disky
*SATA*

- HDD i starší SSD sedí na SATA.
- Rychlost rozhraní je strop, disk může být pomalejší.
- Kabel bez západky vypadne a disk zmizí z BIOSu.
- Externí box schová SATA za USB.

> Tip: napájení SATA a datový kabel jsou dva různé vodiče.

## SSD přímo na PCIe
*NVMe*

- NVMe jede po PCIe a obchází starý řadič SATA.
- Slot M.2 na desce může umět jen jednu z variant.
- Dlouhá destička do krátkého slotu nepatří.
- Bez chlazení rychlé SSD ve špičce zpomalí.

> Tip: M.2 je rozměr destičky. NVMe a SATA na M.2 nejsou totéž.


# Kvíz

## Co říká samotný tvar USB-C?
- [x] Jen tvar konektoru, ne rychlost ani obraz
- Vždy Thunderbolt
- Vždy 40 Gb/s
- Vždy nabíjení 100 W a DisplayPort
> Schopnosti určuje řadič a kabel, ne obrys.

## Čím se NVMe liší od SATA SSD?
- [x] Jede po PCIe, ne po starém SATA
- Nemá paměťové buňky
- Jde zapojit jen do VGA
- Nepotřebuje desku
> Protokol a sběrnice jsou rychlejší cesta.

## Co může umět slot M.2?
- [x] Jen SATA, jen NVMe, nebo obojí, podle desky
- Vždy obojí
- Jen grafickou kartu
- Jen operační paměť
> Manuál desky je závaznější než tvar zářezu.

## Proč rychlé SSD ve špičce zpomalí?
- [x] Přehřeje se a stáhne výkon
- SATA mu sebere VLAN
- BIOS maže buňky
- USB-C ho přepne na 2.0 vždy
> Řadič hlídá teplotu.
