---
id: site-2-staticke-smerovani
puvodni: net2.6
nazev: Statické směrování
popis: Ruční cesta, metrika a kdy přestane stačit.
nazev_cviceni: Kvíz: Statické směrování
nadpis_kvizu: Kvíz k tématu Statické směrování
---

# Výklad

## Kam s paketem
*Trasa*

- Záznam říká síť, masku nebo prefix a další skok.
- Další skok musí být dosažitelný na přímo připojeném rozhraní.
- Výchozí trasa 0.0.0.0/0 chytí vše, co nemá přesnější záznam.
- Přesnější prefix vyhraje nad kratším.

> Tip: směrovací tabulka se dívá na síť, ne na jednu adresu stanice.

## Když je cest víc
*Metrika*

- Statická trasa má lepší vzdálenost než OSPF nebo RIP.
- Metrika porovnává trasy ze stejného zdroje.
- Plovoucí statická trasa má horší vzdálenost a čeká jako záloha.
- Špatný další skok vytvoří černou díru: paket odejde a ztratí se.

> Tip: nižší administrativní vzdálenost znamená důvěryhodnější zdroj trasy.

## Kdy ruční tabulka nestačí
*Meze*

- Statika je přehledná v malé síti a na okraji k poskytovateli.
- Neumí sama obejít spadlou cestu, pokud záloha není připravená.
- Na směrovači musí sedět trasa tam i zpět, jinak odpověď nepřijde.
- Změna adresace znamená obejít všechny ruční záznamy.

> Tip: deset poboček se statickými trasami neudržíte po každé změně.


# Kvíz

## Na co se směrovač dívá při výběru trasy?
- [x] Na síť a prefix, ne na jméno počítače
- Na barvu kabelu
- Na VLAN jméno v DNS
- Jen na MAC adresu cíle
> Třetí vrstva směruje podle prefixu.

## Která trasa je výchozí?
- [x] 0.0.0.0/0
- 255.255.255.255/32
- 192.168.1.1/32
- ::1/128
> Nulová síť s nulovým prefixem chytí zbytek.

## Co je plovoucí statická trasa?
- [x] Záložní ruční trasa s horší vzdáleností
- Trasa, která plave mezi VLAN bez routeru
- Automatický OSPF
- Záznam jen v ARP
> Nastoupí, až když zmizí lepší cesta.

## Proč odpověď ze serveru nedorazí, i když tam paket došel?
- [x] Chybí zpáteční trasa
- TCP nemá porty
- IPv4 neumí odpovědi
- Switch maže cílové MAC adresy
> Cesta musí existovat oběma směry.
