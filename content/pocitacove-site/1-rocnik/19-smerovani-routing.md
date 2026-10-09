---
id: site-1-smerovani-routing
puvodni: net.19
nazev: Směrování (Routing)
popis: 7 snímků o směrovacích algoritmech a pak kvíz.
nazev_cviceni: Kvíz: směrování
nadpis_kvizu: Kvíz ke směrování
---

# Výklad

## Směrování paketů
*Princip*

- Rozhodování o dalším směru přenosu paketů do cílové sítě.
- Probíhá na síťové vrstvě modelu ISO/OSI.
- K rozhodování se používají směrovací algoritmy.

> Tip: 3. vrstva ISO/OSI – síťová

## Dynamické algoritmy
*Adaptivní*

- Reagují na změny v síti (výpadek linky, přetížení).
- Vyžadují pravidelnou výměnu informací mezi směrovači.

## Statické algoritmy
*Neadaptivní*

- Nereagují na změny v síti.
- Cesty pevně definuje správce.
- Při výpadku části sítě může spojení spadnout.

## Centralizované a izolované
*Řízení*

- Centralizované: centrum počítá cesty pro všechny směrovače.
- Výpadek centra = pád směrování; v praxi skoro nepoužívané.
- Izolované: směrovač se rozhoduje sám z lokálních informací.
- Izolované se používá spíš jako doplněk při přetížení.

## Výměna mezi směrovači
*Distribuované*

- Směrovače si vyměňují informace.
- Každý si udržuje vlastní směrovací tabulku.

## Next Hop a metrika
*Tabulka*

- Záznam: cílová síť, next hop a metrika.
- Next hop = nejbližší soused, kterému paket předáme.
- Metrika číselně ohodnocuje cestu.
- Při více cestách se volí ta s nejnižší metrikou.

> Tip: nižší metrika = lepší / rychlejší trasa

## Příklad algoritmu
*Distance Vector*

- Směrovače znají jen své přímé sousedy.
- Pravidelně si posílají kopie směrovacích tabulek.
- Informace se šíří krok za krokem po celé síti.
- Každý směrovač si spočítá nejlepší trasu do známých sítí.

> Tip: např. výměna tabulek každých 30 s


# Kvíz

## Směrování probíhá na vrstvě:
- Fyzické
- [x] Síťové (3. ISO/OSI)
- Jen aplikační
- Jen linkové

## Adaptivní (dynamické) algoritmy:
- [x] Reagují na změny a vyměňují informace mezi směrovači
- Nikdy nemění cesty
- Fungují jen na fyzické vrstvě
- Nepoužívají metriku

## Neadaptivní (statické) směrování:
- [x] Má cesty pevně nastavené správcem
- Samo opraví každý výpadek
- Nepotřebuje směrovací tabulku
- Běží jen na Wi-Fi

## Centralizované směrování v praxi:
- [x] Se skoro nepoužívá – výpadek centra shodí směrování
- Je jediný standard internetu
- Neexistuje žádné centrum
- Pracuje jen s MAC adresami

## Distribuované směrování znamená, že:
- [x] Směrovače si vyměňují informace a každý má vlastní tabulku
- Jedno centrum řídí všechny cesty
- Směrovač nezná žádné sousedy
- Pakety jdou jen přes hub

## Metrika ve směrovací tabulce:
- [x] Číselně hodnotí cestu – nižší je lepší
- Je vždy jen MAC adresa
- Nemá vliv na výběr trasy
- Nahrazuje next hop

## Distance Vector typicky:
- [x] Posílá sousedům kopie tabulek a šíří informace krok za krokem
- Počítá cesty jen v jednom centru
- Ignoruje přímé sousedy
- Pracuje jen na aplikační vrstvě
