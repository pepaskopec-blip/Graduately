---
id: site-1-vrstevnate-modely-a-protokoly
puvodni: net.9
nazev: Vrstevnaté modely a protokoly
popis: 4 snímky o vrstvách a protokolech a pak kvíz.
nazev_cviceni: Kvíz: vrstevnaté modely a protokoly
nadpis_kvizu: Kvíz k vrstevnatým modelům a protokolům
---

# Výklad

## Standardizace a dekompozice
*Proč*

- Standardizace zajišťuje kompatibilitu mezi výrobci.
- Dekompozice: rozklad složitého problému přenosu na vrstvy.

## Komunikace mezi vrstvami
*Vrstvy*

- Vrstvy komunikují jen se sousedy přes rozhraní (SAP).

> Tip: SAP = Service Access Point

## Pravidla komunikace
*Protokol*

- Protokol: pravidla komunikace mezi stejnými vrstvami
- na různých uzlech.

## Hlavičky a PDU
*Jednotky*

- Každá vrstva přidá k datům hlavičku (header).
- Rámec – linková vrstva.
- Paket – síťová vrstva.
- Segment – transportní vrstva.


# Kvíz

## Standardizace v sítích především:
- Zvyšuje nekompatibilitu
- [x] Zajišťuje kompatibilitu mezi výrobci
- Ruší protokoly
- Nahrazuje kabely

## Dekompozice znamená:
- Spojení všeho do jedné vrstvy
- [x] Rozklad složitého přenosu na vrstvy
- Jen FDM
- Jen CRC

## Vrstvy komunikují:
- Se všemi vrstvami najednou
- [x] Jen se sousedy přes rozhraní (SAP)
- Jen přes satelit
- Jen bez protokolu

## Protokol je:
- Fyzický kabel
- [x] Pravidla komunikace mezi stejnými vrstvami
- Jen amplituda signálu
- Jen tiskový server

## Každá vrstva k datům typicky přidá:
- Jen stop-bit
- [x] Hlavičku (header)
- Jen šířku pásma
- Jen mainframe

## Jednotka linkové vrstvy se nazývá:
- Segment
- Paket
- [x] Rámec
- Baud

## Jednotka síťové vrstvy se nazývá:
- Rámec
- [x] Paket
- Segment
- Simplex

## Jednotka transportní vrstvy se nazývá:
- Rámec
- Paket
- [x] Segment
- Trunk
