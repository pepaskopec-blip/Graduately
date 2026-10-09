---
id: site-1-arp
puvodni: net.24
nazev: ARP
popis: 5 snímků o ARP/RARP a cache a pak kvíz.
nazev_cviceni: Kvíz: ARP
nadpis_kvizu: Kvíz k ARP
---

# Výklad

## Převod adres
*ARP / RARP*

- ARP: z IP adresy na MAC (fyzickou) adresu.
- RARP: z MAC adresy na IP adresu.

## Jak zjistit MAC
*Způsoby*

- Tabulkový převod: ruční správa (malé sítě).
- Výpočet: funkce, kde MAC plyne z IP.
- Dotaz a odpověď: dynamické zjišťování (nejčastější).

## Broadcast a unicast
*Mechanismus*

- ARP využívá broadcast v Ethernetu.
- Odpoví jen uzel s danou IP.
- Odpověď jde unicastem a obsahuje MAC adresu.

> Tip: „Kdo má IP x.x.x.x?“

## Dočasná paměť
*Cache*

- Výsledky se ukládají do ARP cache.
- Dotazy se tak nemusí stále opakovat.
- Položky: dynamické (učené) nebo statické (ruční).

> Tip: zobrazení příkazem arp -a

## K čemu ARP slouží
*Shrnutí*

- Propojuje síťovou adresu (IP) s linkovou (MAC).
- Bez ARP by IP datagram nešel vložit do Ethernet rámce.


# Kvíz

## ARP převádí:
- [x] IP adresu na MAC adresu
- MAC adresu na IP adresu
- TCP na UDP
- TTL na MTU

## RARP převádí:
- [x] MAC adresu na IP adresu
- IP adresu na MAC adresu
- EtherType na SAP
- OSPF na RIP

## Nejčastější způsob ARP je:
- [x] Dotaz a odpověď v síti
- Jen ruční tabulka bez sítě
- Jen cut-through
- Jen ICMP Echo

## ARP dotaz v Ethernetu používá:
- [x] Broadcast
- Jen unicast bez broadcastu
- Jen laser
- Jen Token Ring bez IP

## ARP odpověď typicky přichází jako:
- [x] Unicast s MAC adresou uzlu
- Broadcast bez MAC
- Jen Time Exceeded
- Jen Source Quench

## ARP cache slouží k tomu, aby:
- [x] Se nemusely stále opakovat stejné dotazy
- Se nahradil směrovač
- Se zrušila IP adresa
- Se vypnul Ethernet
