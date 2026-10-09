---
id: site-1-tcp-a-udp
puvodni: net.25
nazev: TCP a UDP
popis: 6 snímků o transportní vrstvě a pak kvíz.
nazev_cviceni: Kvíz: TCP a UDP
nadpis_kvizu: Kvíz k TCP a UDP
---

# Výklad

## Základní rozdíly
*TCP vs UDP*

- TCP: spolehlivý a spojovaný přenos.
- Potvrzování, kontrola pořadí a řízení toku.
- UDP: nespolehlivý a nespojovaný.
- Minimální režie – neřeší potvrzení ani pořadí.

## Multiplex a demultiplex
*Porty*

- Porty rozlišují aplikace (procesy) na jednom uzlu.
- Multiplex: sběr dat od aplikací do segmentů/datagramů.
- Demultiplex: doručení příchozích dat správné aplikaci
- podle čísla portu.

## Spojení a vlastnosti
*TCP*

- Navázání spojení: třícestné podání ruky.
- Streamově orientovaný a full duplex.
- Jen unicast – bez multicastu a broadcastu.

> Tip: three-way handshake

## Režie a real-time
*Nevýhody TCP*

- Větší režie: hlavička 20 B (UDP má 8 B).
- Nevhodný pro real-time média: ztráta segmentu
- zastaví proud a čeká se na znovudoručení.
- Bez multihomingu – výpadek rozhraní shodí spojení.

## SCTP a DCCP
*Alternativy*

- SCTP: multihoming a multistreaming (až 64k proudů).
- DCCP: datagramy s řízením zahlcení
- (streaming, IP telefonie).

## Kdy co použít
*Shrnutí*

- TCP: spolehlivá data (web, pošta, soubory).
- UDP: rychlost a nízká režie (DNS, hry, real-time).


# Kvíz

## TCP je:
- [x] Spolehlivý a spojovaný
- Nespolehlivý a nespojovaný
- Jen fyzická vrstva
- Jen ARP cache

## UDP je:
- [x] Nespolehlivý a nespojovaný s malou režií
- Spolehlivý se třícestným handshake
- Jen store-and-forward
- Jen ICMP Time Exceeded

## Porty slouží k:
- [x] Rozlišení aplikací na jednom uzlu
- Jen směrování IP
- Jen MAC učení
- Jen lámání vlákna

## Navázání TCP spojení používá:
- [x] Třícestné podání ruky (three-way handshake)
- Jen broadcast ARP
- Jen Source Quench
- Jen cut-through

## Hlavička TCP má typicky:
- [x] 20 B (UDP 8 B)
- 8 B (UDP 20 B)
- 1500 B
- 64 B min. rámec

## TCP není vhodný pro real-time média hlavně proto, že:
- [x] Ztráta segmentu zastaví proud a čeká se na znovudoručení
- Nemá žádné porty
- Nepodporuje unicast
- Nemá hlavičku

## SCTP oproti TCP nabízí:
- [x] Multihoming a multistreaming
- Jen Ethernet II
- Jen RARP
- Jen hub flooding
