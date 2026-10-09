---
id: site-1-aplikacni-protokoly
puvodni: net.26
nazev: Aplikační protokoly
popis: 7 snímků o portech a službách a pak kvíz.
nazev_cviceni: Kvíz: Aplikační protokoly
nadpis_kvizu: Kvíz k aplikačním protokolům
---

# Výklad

## Aplikační protokoly
*Úvod*

- Na uzlu běží více síťových aplikací současně.
- Běžící aplikace = v operační paměti (1+ procesů).
- Pracují podle pravidel – protokolů aplikační vrstvy.
- Data posílají po síti přes TCP nebo UDP.

## Procesy a porty
*Porty*

- Port v TCP/UDP říká, které aplikaci data patří.
- Jeden proces může používat více portů.
- Dva procesy nesmí používat stejný port.

## Well-known, registrované, dynamické
*Rozsahy*

- Well-known: 0–1023 – vyhrazené službám (norma IANA).
- Registrované: 1024–49151 – IANA jen registruje použití.
- Dynamické/privátní: 49152–65535 – volně k použití.

> Tip: IANA

## Pětice hodnot
*Spojení*

- Aplikační spojení určuje pětice:
- (transport, IP1, port1, IP2, port2).
- Klient osloví server na well-known portu.

## Stejný server, různí klienti
*Více spojení*

- Na jeden uzel (např. web server) může být
- více aplikačních spojení z různých klientů.
- Rozlišují je IP a porty klientů (i transport).

## Časté well-known služby
*Porty I*

- 20/21 FTP, 22 SSH, 23 Telnet, 25 SMTP, 53 DNS.
- 67/68 DHCP, 80 HTTP, 110 POP3, 143 IMAP.
- 123 NTP, 161 SNMP, 389 LDAP, 443 HTTPS.

## Zabezpečené a registrované
*Porty II*

- 587 SMTPS, 636 LDAPS, 993 IMAPS, 995 POP3S.
- Registrované: 3306 MySQL, 3389 RDP, 5900 VNC.
- Dynamické porty: 49152–65535.


# Kvíz

## Aplikační protokoly typicky posílají data přes:
- [x] TCP nebo UDP
- Jen fyzickou vrstvu
- Jen ARP
- Jen CAM tabulku

## Dva běžící procesy:
- [x] Nesmí používat stejný port
- Musí sdílet jeden port
- Nesmí mít žádný port
- Používají jen MAC

## Well-known porty mají rozsah:
- [x] 0–1023
- 1024–49151
- 49152–65535
- Jen 80–443

## Dynamické (privátní) porty jsou:
- [x] 49152–65535
- 0–1023
- Jen 20–21
- Jen 3306

## Aplikační spojení určuje:
- [x] Pětice (transport, IP1, port1, IP2, port2)
- Jen MAC adresa
- Jen název souboru
- Jen VLAN ID

## HTTP a HTTPS používají porty:
- [x] 80 a 443
- 22 a 23
- 25 a 110
- 53 a 67

## SSH a DNS mají porty:
- [x] 22 a 53
- 80 a 443
- 3389 a 5900
- 20 a 21

## MySQL a RDP jsou typicky na portech:
- [x] 3306 a 3389
- 80 a 443
- 67 a 68
- 161 a 162
