---
id: site-3-ospf
puvodni: net3.1
nazev: OSPF
popis: Směrovače si samy řeknou, jaké sítě znají.
nazev_cviceni: Kvíz: OSPF
nadpis_kvizu: Kvíz k tématu OSPF
---

# Výklad

## Stejná oblast
*Sousedé*

- Směrovače v jednom spoji si vymění hello.
- Hello interval a dead interval musí sedět.
- Oblast 0 je páteř. Ostatní oblasti se k ní připojují.
- Pasivní rozhraní hello neposílá, třeba k počítačům.

> Tip: OSPF soused nevznikne, když se neshoduje oblast nebo hello.

## Kratší neznamená míň skoků
*Cena*

- Rychlejší linka má obvykle nižší cenu.
- Součet cen rozhodne, kudy paket půjde.
- Ruční cena umí odtáhnout provoz ze špatné linky.
- Změna topologie se přepočítá bez přepisování statických tras.

> Tip: OSPF sčítá cenu linek, ne počet routerů jako RIP.

## Co se z OSPF stane
*Tabulka*

- Naučená síť se vloží do směrovací tabulky.
- Když lepší cesta spadne, použije se náhradní.
- Redistribuce natáhne do OSPF i statiku nebo jiný protokol.
- Špatná redistribuce umí do sítě pustit cizí trasy.

> Tip: v tabulce uvidíte zdroj trasy, ne jen další skok.


# Kvíz

## Co musí u OSPF sousedů sedět?
- [x] Oblast a časy hello
- Stejná MAC adresa
- Stejné SSID
- Stejný počet VLAN na switchi
> Nesoulad hello nebo oblasti sousedství nesestaví.

## Podle čeho OSPF vybírá cestu?
- [x] Podle součtu cen linek
- Podle počtu písmen v názvu routeru
- Podle VLAN ID
- Podle stáří DNS záznamu
> Cena vyjadřuje náklad cesty, často odvozený z rychlosti.

## K čemu je pasivní rozhraní?
- [x] Neposílá hello tam, kde žádný soused být nemá
- Vypne směrování celé sítě
- Zapne NAT
- Skryje SSID
> K uživatelům se hello zbytečně neposílá.

## Co je oblast 0?
- [x] Páteř, ke které se ostatní oblasti připojují
- VLAN pro hosty
- Výchozí heslo WPA
- Číslo konzolového portu
> Bez páteře se víc oblastí OSPF nespojí správně.
