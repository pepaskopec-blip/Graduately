---
id: site-2-dhcp-a-dns
puvodni: net2.3
nazev: DHCP a DNS
popis: Kdo přiděluje adresu a kdo překládá jméno.
nazev_cviceni: Kvíz: DHCP a DNS
nadpis_kvizu: Kvíz k tématu DHCP a DNS
---

# Výklad

## Adresa bez opisování
*DHCP*

- Klient pošle broadcast a hledá server.
- Nabídka obsahuje adresu, masku, bránu a DNS.
- Zapůjčení má dobu platnosti a klient ji obnovuje.
- Rezervace přiváže adresu ke konkrétní MAC adrese.

> Tip: DHCP dává adresu na čas, ne navždy.

## Co se nesmí překrývat
*Rozsah*

- Rozsah musí ležet v síti, kterou má brána.
- Statické adresy serverů do rozsahu nepatří, nebo se vyloučí.
- Špatná brána v nabídce odřízne celou VLAN od internetu.
- DHCP relay přenese požadavek z VLAN, kde server není.

> Tip: dva servery se stejnou adresou v jedné síti si škodí.

## Jméno na adresu
*DNS*

- A záznam ukazuje na IPv4, AAAA na IPv6.
- Reverzní PTR překládá adresu zpět na jméno.
- Rekurzivní resolver se ptá dál, autoritativní server zónu vlastní.
- TTL u záznamu říká, jak dlouho se smí odpověď cacheovat.

> Tip: DNS není totéž co DHCP. Jedno dává adresu, druhé jméno.


# Kvíz

## Co klient od DHCP obvykle dostane?
- [x] Adresu, masku, bránu a DNS
- Jen heslo k Wi-Fi
- Novou MAC adresu
- Kořenový certifikát webu
> To jsou čtyři údaje, bez kterých stanice v síti nepracuje.

## Proč se rezervace v DHCP hodí?
- [x] Zařízení s danou MAC dostane pořád stejnou adresu
- Vypne potřebu masky
- Nahradí směrování
- Zašifruje disk
> Rezervace je vhodná pro tiskárny a kamery.

## Čím se liší A a AAAA?
- [x] A je IPv4, AAAA je IPv6
- A je pošta, AAAA je web
- A je jméno VLAN, AAAA je číslo portu
- Oba ukazují jen na IPv4
> Čtyři A v názvu připomínají delší IPv6 adresu.

## K čemu je DHCP relay?
- [x] Pošle požadavek z jiné VLAN k serveru
- Překládá jména bez DNS
- Blokuje STP
- Mění rychlost portu na 10 Gb/s
> Broadcast DHCP jinak VLAN neopustí.
