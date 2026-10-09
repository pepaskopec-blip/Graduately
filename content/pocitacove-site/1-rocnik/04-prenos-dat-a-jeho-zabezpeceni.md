---
id: site-1-prenos-dat-a-jeho-zabezpeceni
puvodni: net.4
nazev: Přenos dat a jeho zabezpečení
popis: 4 snímky o přenosu a zabezpečení a pak kvíz.
nazev_cviceni: Kvíz: přenos dat a zabezpečení
nadpis_kvizu: Kvíz k přenosu dat a zabezpečení
---

# Výklad

## Paralelní a sériový přenos
*Přenos*

- Paralelní: více bitů současně po více vodičích.
- Problém: přeslechy (rušení) při delších kabelech
- a vysokých frekvencích.
- Sériový: bit po bitu po jednom vodiči.
- Umožňuje vyšší frekvence i délky – celkově rychlejší.

## Asynchronní přenos
*Časování*

- Data se posílají v blocích (znaky 5–8 bitů).
- Start-bit (0) synchronizuje začátek znaku.
- Stop-bit (1) znak ukončuje.

> Tip: Start-bit = 0, Stop-bit = 1

## Synchronní přenos
*Časování*

- Řízeno společným hodinovým signálem.
- Přesné časování mezi vysílačem a přijímačem.

## Parita, checksum a CRC
*Zabezpečení*

- Parita: nejslabší – přidá bit pro sudý/lichý počet jedniček.
- Checksum: součet znaků jako dvojkových čísel.
- CRC (cyklické kódy): nejbezpečnější,
- počítá se z jednotlivých bitů v bloku.

> Tip: CRC je nejbezpečnější z těchto tří metod


# Kvíz

## Paralelní přenos znamená:
- Bit po bitu po jednom vodiči
- [x] Více bitů současně po více vodičích
- Jen Wi‑Fi bez kabelů
- Jen cloudové ukládání

## Hlavní problém paralelního přenosu při delších kabelech je:
- Absence serveru
- [x] Přeslechy (rušení)
- Chybějící SaaS
- Peer‑to‑peer režim

## Sériový přenos je oproti paralelnímu celkově:
- Vždy pomalejší
- [x] Rychlejší díky vyšším frekvencím a délkám
- Jen pro tiskárny
- Bez jakéhokoli kabelu

## V asynchronním přenosu start-bit má hodnotu:
- 1
- [x] 0
- 8
- 255

## Synchronní přenos je řízen:
- Jen stop-bitem
- [x] Společným hodinovým signálem
- Jen paritou
- Jen checksumem

## Nejslabší metoda zabezpečení z uvedených je:
- CRC
- Checksum
- [x] Parita
- Mainframe

## Checksum spočívá v:
- Přidání start-bitu
- [x] Součtu znaků jako dvojkových čísel
- Jen Wi‑Fi šifrování
- Výměně kabelů

## CRC (cyklické kódy) je z uvedených metod:
- Nejslabší
- Stejně slabá jako parita
- [x] Nejbezpečnější
- Jen pro paralelní přenos
