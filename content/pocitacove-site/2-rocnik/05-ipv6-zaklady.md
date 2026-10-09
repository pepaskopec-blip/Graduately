---
id: site-2-ipv6-zaklady
puvodni: net2.5
nazev: IPv6 základy
popis: Delší adresa, jiný zápis a co se nemění.
nazev_cviceni: Kvíz: IPv6 základy
nadpis_kvizu: Kvíz k tématu IPv6 základy
---

# Výklad

## 128 bitů
*Adresa*

- Adresa má osm skupin po čtyřech hexadecimálních číslicích.
- Prefix se píše jako /64, ne jako stará maska 255.255.255.0.
- Link-local adresa fe80:: platí jen na tom spoji.
- Globální adresa je vidět i mimo místní spoj.

> Tip: nuly v jedné skupině za sebou smí zkrátit :: jen jednou.

## SLAAC a DHCPv6
*Přidělení*

- Router advertisement řekne prefix a jestli se stanice smí nastavit sama.
- SLAAC složí adresu z prefixu a identifikátoru rozhraní.
- DHCPv6 přidá adresu nebo jen DNS, podle příznaků.
- Obě varianty mohou běžet vedle sebe.

> Tip: výchozí bránu v IPv6 často ohlašuje router, ne DHCP.

## Dual stack
*Vedle IPv4*

- Počítač může mít IPv4 i IPv6 naráz.
- DNS AAAA záznam rozhoduje, jestli se klient zkusí spojit po IPv6.
- NAT u IPv6 není nutný tak jako u IPv4, ale firewall ano.
- Ping na IPv6 je jiný příkaz než ping na IPv4, podle systému.

> Tip: IPv6 nevypínejte jen proto, že mu nerozumíte. Rozbitý IPv6 zpomalí i weby.


# Kvíz

## Jak je IPv6 adresa dlouhá?
- [x] 128 bitů
- 32 bitů
- 48 bitů
- 256 bitů
> IPv4 má 32 bitů, IPv6 čtyřikrát tolik.

## Která adresa je link-local?
- [x] fe80::1
- 192.168.1.1
- 8.8.8.8
- 255.255.255.255
> fe80:: platí jen na místním spoji.

## Kdo v IPv6 často oznámí prefix?
- [x] Router advertisement
- ARP request
- STP hello
- Jen ruční zápis do hosts
> Router říká stanicím, jaký prefix na spoji platí.

## Co znamená dual stack?
- [x] Zařízení používá IPv4 i IPv6
- Dva switche v jedné VLAN
- Dvě masky u jedné IPv4 adresy
- Záloha jen na pásku
> Oba protokoly běží současně.
