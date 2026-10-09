---
id: hardware-3-raid
puvodni: hw3.1
nazev: RAID
popis: Více disků jako jeden svazek, ne jako záloha.
nazev_cviceni: Kvíz: RAID
nadpis_kvizu: Kvíz k tématu RAID
---

# Výklad

## RAID 1
*Zrcadlo*

- Stejná data jsou na dvou discích.
- Kapacita je jako jeden disk.
- Čtení může být rychlejší, zápis ne dvojnásobný.
- Po výměně disku se pole musí obnovit.

> Tip: zrcadlo přežije pád jednoho disku, ne smazání souboru.

## RAID 0 a 5
*Prokládání*

- RAID 0 skládá rychlost a kapacitu a nemá ochranu.
- RAID 5 snese pád jednoho disku z aspoň tří.
- Parita zabere kapacitu jednoho disku.
- Obnova velkého pole trvá dlouho a druhý pád v té době je zkáza.

> Tip: RAID 0 při pádu jednoho disku ztratí všechno.

## Hardware a software
*Řadič*

- Hardwarový řadič má vlastní cache a baterii.
- Softwarové pole řeší systém a je přenosnější.
- Cache bez ochrany při výpadku proudu rozbije zápis.
- RAID nenahrazuje zálohu na jiné místo.

> Tip: pole z jiného řadiče v novém serveru nemusí naběhnout.


# Kvíz

## Co RAID 1 přežije?
- [x] Pád jednoho z dvojice disků
- Smazání souboru uživatelem
- Požár místnosti
- Ransomware na obou discích naráz
> Oba disky mají stejná data, včetně chyby, kterou tam zapíšete.

## Co je RAID 0?
- [x] Prokládání bez ochrany při pádu disku
- Zrcadlo
- Záloha na pásku
- Šifrování disku
> Rychlost a kapacita, nulová odolnost.

## Proč RAID není záloha?
- [x] Neochrání před smazáním, malwarem ani požárem skříně
- Protože neumí víc než dva disky
- Protože Windows RAID zakazuje
- Protože parita maže soubory
> Záloha je další kopie jinde a v jiném čase.

## Co hrozí velkému poli při obnově?
- [x] Druhý disk spadne dřív, než se obnova dopočítá
- Pole se samo přepne na RAID 0
- BIOS smaže paritu
- NIC se při obnově nesmí točit
> Obnova je dlouhá a disky jsou stejně staré.
