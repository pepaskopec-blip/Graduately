---
id: site-1-metody-pristupu
puvodni: net.16
nazev: Metody přístupu
popis: 4 snímky o kolizích a CSMA a pak kvíz.
nazev_cviceni: Kvíz: metody přístupu
nadpis_kvizu: Kvíz k metodám přístupu
---

# Výklad

## Sdílené médium
*Kolize*

- Kolize nastane, když na sdíleném médiu vysílá více uzlů najednou.

## Vysílej kdykoliv
*Aloha*

- Uzel vysílá kdykoliv chce.
- Když nepřijde potvrzení, pošle data znovu.
- Kolize jsou časté.

> Tip: časté kolize, primitivní metoda

## Ethernet
*CSMA/CD*

- Nejdřív se poslouchá nosná (Carrier Sense).
- Je-li ticho, uzel vysílá.
- Při kolizi se vysílání zastaví a zkusí se znovu
- po náhodném čase.

> Tip: Carrier Sense + Collision Detection

## Wi-Fi
*CSMA/CA*

- Předchází kolizím (Collision Avoidance).
- RTS (Request to Send) a CTS (Clear to Send)
- rezervují médium před vysíláním.

> Tip: Collision Avoidance přes RTS/CTS


# Kvíz

## Kolize nastane, když:
- [x] Na sdíleném médiu vysílá více uzlů najednou
- Router vypne OSPF
- Vlákno má ochranu 250 µm
- Použijeme jen TCP

## Metoda Aloha znamená především:
- [x] Vysílej kdykoliv; bez potvrzení pošli znovu
- Vždy rezervuj médium RTS/CTS
- Nikdy nevysílej při tichu
- Jen směrování RIP

## CSMA/CD se typicky pojí s:
- Wi-Fi
- [x] Ethernetem
- Jen SMTP
- Jen ATM

## Při CSMA/CD uzel před vysíláním:
- Ignoruje médium
- [x] Monitoruje nosnou (Carrier Sense)
- Vždy čeká přesně 1 hodinu
- Použije jen laser

## Když CSMA/CD detekuje kolizi:
- Pokračuje vysíláním bez změny
- [x] Zastaví vysílání a zkusí to po náhodném čase
- Přepne síť na sběrnici
- Smaže IP adresu

## CSMA/CA na Wi-Fi předchází kolizím pomocí:
- [x] RTS a CTS
- Jen CRC bez nosné
- Jen hubu
- Jen koaxiálu
