---
id: site-4-redundance
puvodni: net4.2
nazev: Redundance
popis: Druhá cesta, která se opravdu použije.
nazev_cviceni: Kvíz: Redundance
nadpis_kvizu: Kvíz k tématu Redundance
---

# Výklad

## Dva boxy nestačí
*Dvojice*

- Redundance chce druhou cestu i protokol, který ji hlídá.
- STP řeší smyčku na druhé vrstvě.
- Na třetí vrstvě zálohu hlídá směrování nebo HSRP a příbuzné protokoly.
- Jedna brána v DHCP znamená, že pád routeru odstřihne všechny.

> Tip: dva switche se stejnou konfigurací nejsou záloha, dokud provoz neumí přejít.

## Virtuální adresa
*Brána*

- Dva směrovače sdílejí jednu adresu brány.
- Aktivní drží provoz, druhý čeká.
- Když aktivní ztichne, druhý adresu převezme.
- Stanice nemusí dostat nové DHCP.

> Tip: stanice mají jako bránu virtuální adresu, ne jednu fyzickou krabici.

## Nevyzkoušená záloha je teorie
*Zkouška*

- Ověřte, že záložní cesta opravdu přenáší data.
- Sledujte, za jak dlouho se provoz vrátí.
- Záložní zdroj má jinou pojistku a jinou fázi, když to jde.
- Zápis z zkoušky říká, co se má stát příště.

> Tip: vytáhněte kabel v okně, kdy smíte, ne až při maturitní písemce.


# Kvíz

## Proč dva stejné routery samy o sobě nestačí?
- [x] Stanice musí mít kam přejít, když jeden ztichne
- IPv4 povoluje jen jeden router v budově
- Druhý router maže VLAN
- STP dva routery zakazuje
> Bez společné nebo přepínané brány počítače zůstanou na mrtvé adrese.

## Co stanice používá při protokolu virtuální brány?
- [x] Virtuální adresu, ne konkrétní krabici
- Svou vlastní MAC jako bránu
- Adresu DNS
- Broadcast 255.255.255.255 jako další skok
> Převzetí adresy stanice nepozná jako výměnu DHCP.

## Kdy se záloha ověřuje?
- [x] Nanečisto, dřív než o ni opřete provoz
- Až když síť leží
- Jen na papíře
- Záloha se ověřovat nemá, ať se nepokazí
> Nevyzkoušený přepínač překvapí v nejhorší chvíli.

## Co na druhé vrstvě řeší náhradní kabel?
- [x] STP, jinak vznikne smyčka
- Jen NAT
- Jen WPA3
- Nic, ethernet smyčky umí sám
> Druhý kabel bez STP je recept na bouři.
