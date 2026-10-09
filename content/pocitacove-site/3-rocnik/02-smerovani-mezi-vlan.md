---
id: site-3-smerovani-mezi-vlan
puvodni: net3.2
nazev: Směrování mezi VLAN
popis: Jedna brána pro každou VLAN.
nazev_cviceni: Kvíz: Směrování mezi VLAN
nadpis_kvizu: Kvíz k tématu Směrování mezi VLAN
---

# Výklad

## Jeden spoj, víc VLAN
*Router na tyči*

- Switch pošle trunk do routeru.
- Každá VLAN má vlastní IP bránu.
- Stanice má jako bránu adresu své VLAN, ne cizí.
- Bez trasy mezi VLAN k sobě sítě nedosáhnou.

> Tip: trunk k routeru nese značky a každá VLAN má své podrozhraní.

## Směrování v přepínači
*L3 switch*

- SVI je virtuální rozhraní s adresou brány.
- Provoz mezi VLAN nemusí odejít na samostatný router.
- ACL na SVI hlídá, která VLAN kam smí.
- Zapomenuté SVI znamená, že VLAN nemá bránu.

> Tip: přepínač třetí vrstvy má pro VLAN rozhraní SVI.

## Když to nejde
*Chyby*

- Špatná maska na bráně rozhodí celou VLAN.
- Trunk, který VLAN nepropouští, bránu odstřihne.
- Stanice v access portu špatné VLAN dostane cizí DHCP.
- Kontrola: adresa, maska, brána, VLAN, trunk, ACL.

> Tip: nejdřív ověřte, že stanice je ve správné VLAN a má správnou bránu.


# Kvíz

## Co je brána pro stanici ve VLAN?
- [x] Adresa routeru nebo SVI v té samé VLAN
- Libovolná veřejná DNS adresa
- MAC adresa switche
- Číslo VLAN zapsané do masky
> Brána musí být dosažitelná na druhé vrstvě, tedy ve stejné VLAN.

## Co je SVI?
- [x] Virtuální rozhraní VLAN na L3 přepínači
- Typ optického vlákna
- Protokol pošty
- Záloha konfigurace
> SVI nese IP adresu brány dané VLAN.

## Jak router na tyči pozná VLAN?
- [x] Podle značky 802.1Q na trunku
- Podle barvy LED na počítači
- Podle jména v DNS
- Podle rychlosti ventilátoru
> Každé podrozhraní patří jedné značce.

## Co udělá trunk, který VLAN 20 nepropouští?
- [x] Střihne bránu VLAN 20 od zbytku sítě
- Smaže OSPF v celé škole
- Přepne Wi-Fi na WEP
- Zdvojí adresy DHCP
> Povolené VLAN na trunku jsou součást návrhu.
