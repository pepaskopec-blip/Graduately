---
id: hardware-1-pamet-a-sbernice-gpu
puvodni: hw.19
nazev: Paměť a sběrnice GPU
popis: 3 snímky o RAMDAC, GDDR a PCIe a pak kvíz.
nazev_cviceni: Kvíz: paměť a sběrnice GPU
nadpis_kvizu: Kvíz k paměti a sběrnici GPU
---

# Výklad

## RAMDAC a grafická paměť
*Převod*

- RAMDAC převádí digitální data na analogová.
- U dnešních karet je uvnitř GPU a má přednost při čtení paměti.
- Okolo GPU jsou grafické paměti, často DDR nebo GDDR5.
- Hlavní parametry jsou kapacita a frekvence.
- CAS latency je čekání, než se po zadání adresy sloupce data objeví na pinech.
- Čím nižší latence, tím lépe. Časy paměti se udávají v nanosekundách.

> Tip: nižší CAS latency znamená kratší čekání.

## Sběrnice grafické karty
*Spojení*

- Typická sběrnice karty je PCI Express ×16, dřív to bylo AGP.
- PCIe 4.0 zdvojnásobí propustnost proti předchozí verzi při stejném počtu linek.
- Rychlost jedné linky stoupla z 1 GB/s na 2 GB/s.
- Slot ×16 tak teoreticky dává 32 GB/s.
- M.2 se čtyřmi linkami dává 8 GB/s.

> Tip: PCIe 4.0 má na jedné lince 2 GB/s.

## Porty na kartě
*Výstupy*

- Běžné výstupy jsou D-Sub, DVI, HDMI a DisplayPort.
- Starší karty mají i TV výstup, S-Video nebo cinch.
- Výrobci parametry přikrášlují, proto se karty srovnávají benchmarkem.
- Známý test je 3DMark. Podíl má i procesor, rozhoduje ale grafická karta.

> Tip: stejný obraz může odejít analogově i digitálně.


# Kvíz

## Co dělá RAMDAC?
- Převádí analogový signál na digitální
- [x] Převádí digitální data na analogová a dnes je v GPU
- Počítá jen cache procesoru
- Formátuje pevný disk

## Co znamená nižší CAS latency?
- [x] Data se objeví dřív, takže je to lepší
- Karta má míň portů
- Sběrnice je pomalejší
- Monitor má nižší rozlišení

## Jakou teoretickou propustnost má PCIe 4.0 ×16?
- 2 GB/s
- 8 GB/s
- [x] 32 GB/s
- 264 MB/s

## Co se stalo s rychlostí jedné linky u PCIe 4.0?
- Klesla z 2 GB/s na 1 GB/s
- [x] Stoupla z 1 GB/s na 2 GB/s
- Zůstala 133 MB/s
- Záleží jen na barvě slotu
