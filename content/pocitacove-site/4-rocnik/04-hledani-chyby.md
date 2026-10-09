---
id: site-4-hledani-chyby
puvodni: net4.4
nazev: Hledání chyby
popis: Postup zdola nahoru, ne náhodné restarty.
nazev_cviceni: Kvíz: Hledání chyby
nadpis_kvizu: Kvíz k tématu Hledání chyby
---

# Výklad

## Nejdřív světlo na portu
*Vrstvy*

- Fyzická vrstva: kabel, port, napájení, duplex, chyby.
- Druhá vrstva: VLAN, MAC tabulka, trunk, STP.
- Třetí vrstva: adresa, maska, brána, trasa, ACL.
- Výš: DNS, port služby, firewall na konci.

> Tip: nehledejte chybu v DNS, když port nesvítí.

## Ať víte, co pomohlo
*Jedna změna*

- Zapište, co vidíte, než něco přepnete.
- Ověřte jednu hypotézu.
- Ping na bránu, pak za ni, pak jméno.
- Když oprava sedí, uložte konfiguraci a dopište důvod.

> Tip: pět změn naráz nic nenaučí.

## Koho to postihlo
*Rozsah*

- Jeden stroj: jeho kabel, port, adresa, účet.
- Celá VLAN: brána, DHCP, trunk, SVI.
- Celá škola: jádro, firewall, poskytovatel.
- Čas začátku často sedí se změnou, kterou někdo neoznámil.

> Tip: jeden počítač je jiná chyba než celá učebna.


# Kvíz

## Co ověříte jako první, když port nesvítí?
- [x] Kabel, port a napájení
- Záznam AAAA
- Cenu OSPF
- Pravidlo QoS pro hlas
> Vyšší vrstvy nemají na čem jet.

## Proč měnit jen jednu věc?
- [x] Ať je vidět, která změna pomohla
- Switch víc změn zakazuje
- IPv4 umí jen jednu opravu denně
- DNS jinak smaže zónu
> Více zásahů naráz zamaskuje příčinu.

## Celá učebna nedostane adresu. Kde začnete?
- [x] DHCP, VLAN a brána té sítě
- Heslem jednoho studenta k webu
- Jasem monitoru
- Verzí prohlížeče
> Společný příznak ukazuje na společnou službu.

## Co uděláte, když oprava sedí?
- [x] Uložíte konfiguraci a zapíšete důvod
- Restartujete všechno ještě jednou
- Smažete popis portů
- Vypnete dohled, ať neruší
> Neuložená oprava do rána zmizí.
