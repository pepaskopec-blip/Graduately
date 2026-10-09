---
id: hardware-1-pcm-a-datovy-tok
puvodni: hw.8
nazev: PCM a datový tok
popis: 2 snímky o PCM a velikosti záznamu a pak kvíz.
nazev_cviceni: Kvíz: PCM a datový tok
nadpis_kvizu: Kvíz k PCM a datovému toku
---

# Výklad

## Pulzně kódová modulace
*PCM*

- PCM uloží každý vzorek jako binární číslo.
- Je to základní nekomprimovaný záznam (WAV, audio CD).
- Datový tok = vzorkovací frekvence × bitová hloubka × počet kanálů.
- Mono má jeden kanál, stereo dva (levý a pravý).

> Tip: tok = fs × bity na vzorek × počet kanálů.

## Kolik dat zabere CD
*Velikost*

- CD stereo: 44 100 Hz × 16 bit × 2 = 1 411 200 bit/s.
- To je 176 400 B/s.
- Za minutu je to 10 584 000 B, tedy přes 10 MB.
- Delší záznam, vyšší fs, více bitů nebo více kanálů soubor zvětší.
- MP3 a AAC soubor zmenší za cenu ztráty části informace.

> Tip: komprese (MP3, AAC) soubor zmenší, ale část informace zahodí.


# Kvíz

## Co je PCM?
- [x] Každý vzorek uložený jako binární číslo
- Jen komprimovaný formát MP3
- Filtr před mikrofonem
- Jednotka hlasitosti

## Jak se spočítá datový tok nekomprimovaného zvuku?
- fs + bity + kanály
- [x] fs × bitová hloubka × počet kanálů
- jen délka skladby v minutách
- počet hladin děleno dvěma

## Jaký je datový tok audio CD ve stereu?
- 44 100 bit/s
- 256 bit/s
- [x] 1 411 200 bit/s
- 8 bit/s

## Co udělá komprese MP3 se záznamem?
- Zvětší soubor a přidá vzorky
- [x] Zmenší soubor a část informace zahodí
- Změní jen název souboru
- Převede digitál zpět na analog
