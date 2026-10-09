---
id: site-1-smerovac-a-brana
puvodni: net.20
nazev: Směrovač a brána
popis: 7 snímků o routeru a gateway a pak kvíz.
nazev_cviceni: Kvíz: směrovač a brána
nadpis_kvizu: Kvíz ke směrovači a bráně
---

# Výklad

## Síťová vrstva
*Router*

- Pracuje na 3. (síťové) vrstvě.
- Segmentuje síť na podsítě nebo propojuje LAN do většího celku.
- Umožňuje přístup na WAN (např. internet).
- Podporuje soustavy protokolů (např. TCP/IP) a má vlastní adresu.

> Tip: má vlastní IP a je vidět např. pingen

## Firewall na routeru
*Bezpečnost*

- Router s firewallem filtruje pakety.
- Může zakázat nebo povolit určité síťové služby.

## Různé standardy
*Multiprotokol*

- Multiprotokolový router spojuje různé soustavy protokolů.
- Například TCP/IP se sítěmi IPX/SPX.
- Umí i různý síťový hardware (Ethernet ↔ Token Ring).
- Konvertuje formát rámců i paketů.

## Směrování a tabulky
*Routing*

- Každému paketu hledá vhodnou cestu k cíli.
- Zohledňuje např. okamžité zatížení sítí.
- Používá směrovací tabulky a musí znát topologii.
- Směrovače si tabulky vyměňují (např. broadcastem).

## Broadcastové domény
*Broadcast*

- Router rozděluje síť na broadcastové domény.
- Broadcasty lze filtrovat – nešíří se na všechny porty.

## Brána
*Gateway*

- Připojuje LAN na zcela odlišné prostředí.
- Například mainframe nebo GSM síť.
- Často PC s kartou/emulací terminálu nebo s modemem na WAN.

> Tip: pracuje až na aplikační vrstvě (i nižších)

## Gateway vs router
*Poznámka*

- Někdy se pod pojmem brána rozumí spíše router.
- Gateway také označuje připojení LAN na WAN / internet.


# Kvíz

## Směrovač (router) pracuje na vrstvě:
- Fyzické
- [x] Síťové (3.)
- Jen aplikační
- Jen prezentační

## Router mimo jiné umožňuje:
- [x] Segmentaci na podsítě a přístup na WAN
- Jen lámání optiky
- Jen Aloha bez IP
- Jen cut-through bez MAC

## Firewall na routeru dokáže:
- [x] Filtrovat pakety a řídit služby
- Jen zesílit bitový signál
- Jen uložit CAM bez IP
- Jen nahradit fotodiodu

## Multiprotokolový router:
- [x] Propojí sítě různých protokolů/standardů
- Umí jen jeden protokol
- Nepracuje s IP adresou
- Nikdy nekonvertuje rámce

## Router rozděluje síť na:
- [x] Broadcastové domény
- Jen fyzické kabely bez domén
- Jen aplikační servery
- Jen Token Ring bez Ethernetu

## Brána (gateway) typicky:
- [x] Připojuje LAN na odlišné prostředí (mainframe, GSM, WAN)
- Pracuje jen jako hub
- Nikdy nemá software
- Funguje jen na fyzické vrstvě

## Poznámka k pojmu brána:
- [x] Někdy se jí myslí spíše router / LAN–WAN připojení
- Vždy znamená jen fotodiodu
- Nikdy nesouvisí s internetem
- Nahrazuje jen lámačku vláken
