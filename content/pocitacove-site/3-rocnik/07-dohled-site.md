---
id: site-3-dohled-site
puvodni: net3.7
nazev: Dohled sítě
popis: SNMP, syslog a co z grafu opravdu plyne.
nazev_cviceni: Kvíz: Dohled sítě
nadpis_kvizu: Kvíz k tématu Dohled sítě
---

# Výklad

## Čísla z zařízení
*SNMP*

- Dohled se ptá na vytížení, chyby a stav portů.
- Komunita není ozdoba, je to přístup.
- SNMPv3 umí uživatele a šifrování.
- Zařízení má poslouchat SNMP jen ze sítě dohledu.

> Tip: SNMPv2c posílá jméno komunity skoro jako heslo v čistém textu.

## Věty o tom, co se stalo
*Syslog*

- Syslog odnáší hlášky na server.
- Úrovně od ladění po nouzi se nemají všechny sypat do jedné hromady.
- Čas na zařízeních musí sedět, jinak nejde srovnat události.
- NTP je proto součást dohledu, ne jen hodiny na zdi.

> Tip: log na zařízení se při restartu může ztratit. Posílejte ho pryč.

## Graf není diagnóza
*Čtení*

- Chyby na portu často znamenají kabel nebo duplex.
- Vytrvalé vytížení WAN řekne, jestli je čas na silnější spoj.
- Výpadek, který nikdo nevidí, se bude opakovat.
- Upozornění má chodit člověku, který může reagovat.

> Tip: špička jednou za den může být záloha, ne útok.


# Kvíz

## Proč SNMPv2c nestačí na nezabezpečené síti?
- [x] Jméno komunity jde odposlechnout
- Neumí číst vytížení
- Funguje jen na IPv6
- Maže syslog
> Komunita cestuje bez pořádné ochrany. Lepší je SNMPv3.

## Proč posílat syslog pryč ze zařízení?
- [x] Lokální log se při restartu může ztratit
- Syslog nahrazuje heslo
- Switch jinak vypne porty
- DNS bez logu nefunguje
> Po restartu už příčinu pádu na zařízení nemusíte najít.

## K čemu je při dohledu NTP?
- [x] Aby časy událostí šly srovnat
- Aby přidělil VLAN
- Aby šifroval Wi-Fi
- Aby nahradil SNMP
> Bez společného času nejde říct, co bylo dřív.

## Co často znamenají rostoucí chyby na portu?
- [x] Špatný kabel nebo duplex
- Že je VLAN správně
- Že OSPF konverguje
- Že DNS TTL vypršelo
> Fyzická vrstva se v počítadlech chyb projeví dřív než v teorii.
