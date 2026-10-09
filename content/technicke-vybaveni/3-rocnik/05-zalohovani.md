---
id: hardware-3-zalohovani
puvodni: hw3.5
nazev: Zálohování
popis: Kopie, kterou umíte vrátit.
nazev_cviceni: Kvíz: Zálohování
nadpis_kvizu: Kvíz k tématu Zálohování
---

# Výklad

## Tři místa
*Pravidlo*

- Jedna kopie je málo, když leží u originálu.
- Druhá má být mimo stroj a třetí mimo budovu, když na tom záleží.
- Verze v čase vrátí soubor z pátku, ne jen ze včerejška.
- Šifrovaná záloha bez klíče je hromada nic.

> Tip: kopie na tom samém disku přežije jen omyl v koši, ne pád disku.

## Ne jen plochu
*Co zálohovat*

- Data, účty a konfigurace sítě jsou různé věci.
- Poštovní schránka v cloudu má vlastní nastavení retence.
- Databáze se nezálohuje kopírováním otevřeného souboru naslepo.
- Seznam, co se nezálohuje, má být vědomý.

> Tip: obraz systému bez dat školy je pěkná, ale prázdná záchrana.

## Obnova je součást zálohy
*Zkouška*

- Jednou za čas obnovte soubor i celý systém nanečisto.
- Měřte, jak dlouho obnova trvá.
- Zápis říká, kdo má klíče a kde leží média.
- Po ransomware rozhoduje stáří poslední čisté kopie.

> Tip: záloha, kterou jste nikdy neobnovili, je hypotéza.


# Kvíz

## Proč nestačí kopie na stejném disku?
- [x] Pád disku vezme originál i kopii
- Systém druhou složku zakazuje
- RAID ji sám smaže
- NTFS umí jen jeden soubor
> Chyba média je společná.

## Co není pořádná zkouška zálohy?
- [x] Jen to, že noční úloha svítí zeleně
- Obnova jednoho souboru
- Změřený čas obnovy systému
- Ověření, že klíč k šifře existuje
> Zelená fajfka neznamená, že data jdou otevřít.

## Co rozhoduje po ransomware?
- [x] Jestli existuje čistá kopie starší než nákaza
- Barva skříně
- Počet ventilátorů
- Verze spořiče obrazovky
> Šifrovaná záloha z doby, kdy už malware běžel, je taky šifrovaná.

## Proč nekopírovat otevřenou databázi jako obyčejný soubor?
- [x] Rozpracovaný zápis bude nekonzistentní
- Databáze nemá soubor
- SMB databáze zakazuje
- Kopie by smazala RAID
> Běžící databáze se zálohuje nástrojem, který umí konzistentní snímek.
