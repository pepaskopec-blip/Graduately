---
id: site-1-paket-ip
puvodni: net.22
nazev: Paket IP
popis: 8 snímků o IP datagramu a hlavičce a pak kvíz.
nazev_cviceni: Kvíz: paket IP
nadpis_kvizu: Kvíz k IP paketu
---

# Výklad

## Header a data
*Paket*

- Paket/segment: hlavička (pro partnerskou vrstvu) + data.
- Protokol určuje strukturu a pravidla stejnolehlých vrstev.
- Může běžet na různých platformách.

## Protokoly po vrstvách
*TCP/IP*

- Aplikační: aplikační protokoly.
- Transportní: TCP, UDP.
- Síťová: IP, ARP, RARP, ICMP, IGMP, RIP, OSPF.
- Síťové rozhraní: Ethernet, Token Ring, ATM, PPP…

## Univerzální přenos
*IP*

- Jediný přenosový protokol TCP/IP nad libovolnou technologií.
- Pracuje s virtuálními pakety – IP datagramy.
- Stará se o směrování a přenos; velikost volí odesílatel.
- Nezaručuje pořadí, dobu ani nepoškozené doručení.

> Tip: nespojovaný a nespolehlivý

## Routing vs forwarding
*Routing*

- Routing: rozhodnutí o dalším směru datagramu.
- Forwarding: vložení do linkového rámce a odeslání.
- Linkové adresy se mění podle konkrétní sítě.

## Velikost a HLEN
*Datagram*

- Velikost: od 576 B do 64 kB.
- Části: hlavička + data.
- HLEN: délka hlavičky ve 32bitových slovech.
- TOTAL LENGTH: délka celého paketu v bytech.

> Tip: Length 5 = 20 B hlavičky

## Klíčová pole
*Hlavička*

- VERSION (IPv4 = 4), LENGTH, TOTAL LENGTH.
- IDENTIFICATION + FLAGS + OFFSET = fragmentace.
- TTL: čítač směrovačů proti zacyklení.
- PROTOCOL, HEADER CHECKSUM, zdrojová/cílová IP.

## Čísla protokolů
*PROTOCOL*

- 1 = ICMP, 2 = IGMP, 6 = TCP.
- 17 = UDP, 89 = OSPF.
- Položka je 8bitová → max. 256 hodnot.

## MTU a Ethernet
*Fragmentace*

- Fragmentace, když se datagram nevejde do menšího rámce.
- MTU udává max. velikost rámce podle hardwaru.
- DF = nefragmentuj; MF = další fragmenty.

> Tip: Ethernet MTU = 1500 B


# Kvíz

## IP protokol je:
- [x] Nespojovaný a nespolehlivý přenos datagramů
- Spolehlivý a spojovaný jako TCP
- Jen fyzické kódování
- Jen EtherType bez IP

## Routing u IP znamená:
- [x] Rozhodnutí o dalším směru datagramu
- Jen CRC na lince
- Jen MAC učení ve switchi
- Jen lámání vlákna

## Forwarding u IP znamená:
- [x] Vložení do linkového rámce a odeslání
- Jen výpočet metriky OSPF
- Jen DNS překlad
- Jen Aloha

## HLEN (LENGTH) s hodnotou 5 znamená:
- [x] Hlavičku dlouhou 20 B
- TTL 5
- PROTOCOL = 5
- MTU 5

## TTL v IP hlavičce:
- [x] Počítá průchody směrovači a brání zacyklení
- Udává EtherType
- Nahrazuje MAC adresu
- Je vždy 1500

## Číslo protokolu 6 v IP hlavičce znamená:
- [x] TCP
- UDP
- ICMP
- OSPF

## Číslo protokolu 17 znamená:
- [x] UDP
- TCP
- IGMP
- ARP

## Ethernet MTU je typicky:
- [x] 1500 B
- 64 kB
- 576 B bez dat
- 20 B hlavičky
