---
id: hardware-3-nas-a-site-disku
puvodni: hw3.2
nazev: NAS a sítě disků
popis: Disky, které nejsou uvnitř jednoho počítače.
nazev_cviceni: Kvíz: NAS a sítě disků
nadpis_kvizu: Kvíz k tématu NAS a sítě disků
---

# Výklad

## Souborový server v krabici
*NAS*

- Nabízí složky po síti.
- Má vlastní účty nebo se ptá školního adresáře.
- Dva porty se dají spojit, ale jen když to umí i switch.
- Hlučný diskový provoz nepatří do tiché učebny.

> Tip: NAS v učebně bez zálohy je jen jeden košík na všechna data.

## Disk jako místní
*SAN*

- Server vidí disk, i když leží jinde.
- Síť pro SAN se nemíchá s provozem žáků.
- Špatné zónování vystaví disk cizímu serveru.
- Výpadek této sítě shodí systémy, které na disku jedou.

> Tip: SAN nedává složku, dává blokové zařízení.

## Práva a snímky
*Soubor*

- Práva na složce mají sedět se skupinami, ne s jedním účtem učitele.
- Snímek umí vrátit soubor smazaný dopoledne.
- Plný svazek zastaví i zdánlivě nesouvisející službu.
- Kapacita se hlídá dřív, než svítí červeně.

> Tip: snímek disku není záloha, dokud leží na stejném poli.


# Kvíz

## Co NAS obvykle nabízí?
- [x] Složky po síti
- Blokový disk celé škole bez oprávnění
- Náhradu procesoru v noteboocích
- Obraz monitoru
> Je to souborová služba.

## Čím se SAN liší?
- [x] Server dostane disk jako bloky, ne jako složku
- Je to jen jiné jméno pro USB flash disk
- Jede jen po Wi-Fi
- Nemá disky
> Souborový systém si na těch blocích dělá až server.

## Proč snímek na stejném poli není záloha?
- [x] Pád pole vezme i snímek
- Snímek nejde vytvořit
- Snímek maže RAID
- SMB snímky zakazuje
> Záloha musí přežít ztrátu původního úložiště.

## Kam nepatří hlučné diskové pole?
- [x] Do tiché učebny vedle žáků
- Do racku s odtahem
- Do místnosti se zámek
- Do sítě oddělené od hostů
> Hluk a přístup jsou součást návrhu.
