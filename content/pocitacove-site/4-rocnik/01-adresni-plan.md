---
id: site-4-adresni-plan
puvodni: net4.1
nazev: Adresní plán
popis: Sítě se navrhují dřív, než se zapojí první kabel.
nazev_cviceni: Kvíz: Adresní plán
nadpis_kvizu: Kvíz k tématu Adresní plán
---

# Výklad

## Každá VLAN má svůj prefix
*Rozsahy*

- Nejdřív spočítejte stanice, tiskárny, AP a rezervu.
- Brána, síťová a broadcast adresa se do stanic nepočítají.
- Sousední VLAN nemají mít překryv.
- Dokument říká, která čísla jsou volná.

> Tip: nenechávejte v každé síti jen čtyři volné adresy.

## Škola není jedna velká síť
*Členění*

- Učebny, správa, servery, hosté a telefony patří odděleně.
- Stejný řád čísel ve všech budovách se líp pamatuje.
- Souhrn tras na páteři zmenší tabulku.
- Výjimka pro jednu tiskárnu se má zapsat, ne jen zapamatovat.

> Tip: /16 na celou budovu se snadno spravuje a špatně izoluje.

## Plán, který jde rozšířit
*Změna*

- Nechte v páteřním rozsahu díry pro další sítě.
- Přechod na nový prefix znamená DHCP, DNS, brány i firewall.
- IPv6 prefix se plánuje stejně vědomě, ne jako dodatek.
- Tabulka adres je součást maturity i běžného provozu.

> Tip: když přidáte budovu, neměli byste přečíslovat celou školu.


# Kvíz

## Co se v síti /24 nedá přiřadit stanici?
- [x] Adresa sítě a broadcast
- První použitelná adresa
- Adresa brány, pokud ji stanice nemá mít
- Adresa s lichým posledním oktetem
> Krajní adresy mají zvláštní význam. Brána se stanicím také nedává dvakrát.

## Proč nerozdat celé škole jednu velkou síť?
- [x] Špatně se izoluje porucha i provoz
- IPv4 to zakazuje
- Switch umí jen 8 adres
- DNS pak nejde spustit
> Menší sítě se líp hlídají a míň se ruší broadcastem.

## K čemu je souhrn tras?
- [x] Jedna trasa pokryje víc menších sítí
- Smaže VLAN
- Nahradí NAT
- Zapne WPA3
> Páteř nemusí znát každou učebnu zvlášť.

## Co patří do adresního plánu?
- [x] Prefix, VLAN, brána a k čemu síť je
- Jen heslo Wi-Fi
- Typ procesoru serveru
- Seznam učitelů
> Bez účelu sítě je tabulka jen sloupec čísel.
