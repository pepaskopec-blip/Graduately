---
id: site-1-icmp
puvodni: net.23
nazev: ICMP
popis: 5 snímků o ICMP zprávách a traceroute a pak kvíz.
nazev_cviceni: Kvíz: ICMP
nadpis_kvizu: Kvíz k ICMP
---

# Výklad

## Řídicí zprávy
*ICMP*

- Zasílá informace o chybách a stavu přenosu.
- ICMP paket jede přímo v IP datagramu.
- Směrovače s ním pracují i bez transportní vrstvy.

> Tip: síťová vrstva, ale nese se v IP datagramu

## Quench, TTL, Unreachable
*Zprávy I*

- Source Quench: hrozí zahlcení – zpomal odesílání.
- Time Exceeded: TTL kleslo na 0, paket zahozen (tracert).
- Destination Unreachable: cíl (síť/uzel/protokol/port)
- není dostupný.

## Redirect a Echo
*Zprávy II*

- Redirect: přesměruj přenos na jiný (rychlejší) router.
- Echo Request / Echo Reply: test dostupnosti (ping).

## Princip TTL
*Traceroute*

- Odešle paket s TTL = 1; první router odpoví Time Exceeded.
- Pak TTL = 2, 3… a tak se zjišťují skoky na cestě.
- Pokračuje, dokud paket nedorazí do cíle nebo nevyprší limit.

> Tip: typicky max. 30 skoků

## K čemu ICMP slouží
*Shrnutí*

- Diagnostika a hlášení chyb na síťové vrstvě.
- Základ příkazů ping a traceroute / tracert.


# Kvíz

## ICMP slouží především k:
- [x] Zasílání informací o chybách a stavu přenosu
- Jen směrování OSPF tabulek
- Jen lámání optiky
- Jen EtherType 0800h

## ICMP paket se přenáší:
- [x] Přímo v IP datagramu
- Jen jako Ethernet II bez IP
- Jen v UDP bez IP
- Jen jako MAC bez datagramu

## Source Quench znamená:
- [x] Varování před zahlcením – zpomal odesílání
- TTL kleslo na 0
- Cíl je nedostupný
- Echo Reply

## Time Exceeded nastane, když:
- [x] TTL v IP hlavičce klesne na 0
- Router nemá firewall
- Použijeme jen SNAP
- MTU je 1500

## Echo Request / Reply využívá příkaz:
- [x] ping
- lámačka
- cut-through
- hub

## Traceroute zjišťuje cestu tak, že:
- [x] Postupně zvyšuje TTL a čeká na Time Exceeded
- Posílá jen ARP bez IP
- Ignoruje směrovače
- Používá jen CAM tabulku
