---
id: hardware-3-virtualizace
puvodni: hw3.4
nazev: Virtualizace
popis: Více systémů na jednom železe.
nazev_cviceni: Kvíz: Virtualizace
nadpis_kvizu: Kvíz k tématu Virtualizace
---

# Výklad

## Kdo rozděluje železo
*Hypervizor*

- Hypervizor pouští více systémů nad jedním hardwarem.
- Každý systém má vlastní diskový soubor a virtuální síťovku.
- Paměť, kterou slíbíte navíc, se ve špičce potká.
- Pád fyzického serveru shodí všechny hosty naráz.

> Tip: virtuální počítač nemá vlastní zdroj. Dělí se o ten fyzický.

## Virtuální switch
*Síť*

- Virtuální switch propojí hosty mezi sebou.
- Most je pustí ven fyzickým portem.
- Izolovaná síť jen mezi virtuály ven nevede.
- Špatný trunk vystaví správní virtuál do učebny.

> Tip: VLAN ve virtuálu musí sedět s trunkem na fyzické kartě.

## Soubor není kouzlo
*Disk*

- Tenké provizionování slíbí víc, než je na začátku obsazeno.
- Snímek před změnou umí vrátit systém.
- Zapomenuté snímky zpomalí disk a zaplní úložiště.
- Záloha virtuálu patří mimo ten samý disk.

> Tip: tenký disk roste a může zaplnit svazek, na kterém leží.


# Kvíz

## Co se stane, když spadne fyzický server s pěti virtuály?
- [x] Spadnou všechny, pokud nemají kam utéct
- Přežije aspoň správní virtuál
- Hypervizor je přenese na notebook
- Nic, virtuál zdroj nepotřebuje
> Sdílejí jedno železo.

## K čemu je most virtuálního switche?
- [x] Pustí virtuál do fyzické sítě
- Smaže VLAN
- Nahradí RAID
- Zapne Secure Boot hostitele
> Bez mostu virtuál ven neodejde.

## Čím je tenký disk nebezpečný?
- [x] Doroste a zaplní úložiště pod sebou
- Nejde zálohovat
- Nemá souborový systém
- Vypne hypervizor
> Slíbená kapacita není volné místo.

## Co udělá zapomenutý snímek?
- [x] Roste a zpomaluje disk
- Smaže heslo BIOSu
- Opraví RAID 0
- Přidá paměť
> Snímek je dočasná pojistka, ne trvalý režim.
