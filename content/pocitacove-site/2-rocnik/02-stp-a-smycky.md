---
id: site-2-stp-a-smycky
puvodni: net2.2
nazev: STP a smyčky
popis: Proč smyčka na druhé vrstvě shodí síť a jak ji STP přeruší.
nazev_cviceni: Kvíz: STP a smyčky
nadpis_kvizu: Kvíz k tématu STP a smyčky
---

# Výklad

## Broadcastová bouře
*Smyčka*

- Dva kabely mezi stejnými přepínači vytvoří smyčku.
- Broadcast se kopíruje na všechny porty a vrátí se zpět.
- Přepínač se zahltí a přestane přenášet užitečný provoz.
- Na třetí vrstvě paket TTL snižuje, ethernetový rámec ne.

> Tip: na druhé vrstvě není TTL, rámec může kroužit donekonečna.

## Jedna aktivní cesta
*STP*

- Přepínače si volí root bridge, obvykle nejnižší bridge ID.
- Některé porty zůstanou ve stavu blocking a rámce nepřeposílají.
- Když aktivní linka spadne, blocking port se může otevřít.
- RSTP reaguje rychleji než původní STP.

> Tip: STP nevybírá nejrychlejší cestu pro data, brání smyčce.

## Porty, které nemají přepínat
*Ochrana*

- PortFast na access portu zrychlí připojení počítače.
- BPDU Guard port shodí, když na něm přijde BPDU.
- Root Guard brání tomu, aby se cizí switch stal rootem.
- Záložní kabel má smysl, ale jen se zapnutým STP.

> Tip: port k tiskárně nemá poslouchat cizí BPDU.


# Kvíz

## Proč je smyčka na přepínačích nebezpečná?
- [x] Broadcast může kroužit bez omezení TTL
- IPv4 přestane mít masku
- DNS přejmenuje domény
- Procesor v počítači se vypne
> Ethernetový rámec nemá TTL.

## Co STP s nadbytečným portem udělá?
- [x] Nechá ho blokovat, aby nevznikla smyčka
- Zdvojnásobí rychlost obou kabelů
- Smaže MAC tabulku navždy
- Přesměruje port na VLAN 1 a vypne ostatní
> Blocking port rámce nepřeposílá, dokud není potřeba.

## K čemu je BPDU Guard?
- [x] Shodí access port, na kterém se objeví cizí přepínač
- Šifruje heslo k Wi-Fi
- Přiděluje IPv6 adresy
- Měří útlum optiky
> Na portu pro počítač nemá co dělat protokol přepínačů.

## Které tvrzení o RSTP sedí?
- [x] Na výpadek reaguje rychleji než původní STP
- Ruší potřebu VLAN
- Funguje jen na IPv6
- Nahrazuje směrovací tabulku
> RSTP zkracuje čekání při změně topologie.
