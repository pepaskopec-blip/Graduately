---
id: site-1-bezdratove-site
puvodni: net.27
nazev: Bezdrátové sítě
popis: 8 snímků o družicích, Wi-Fi a zabezpečení a pak kvíz.
nazev_cviceni: Kvíz: Bezdrátové sítě
nadpis_kvizu: Kvíz k bezdrátovým sítím
---

# Výklad

## Bezdrátové sítě
*Úvod*

- Patří sem hlavně: družicové, mikrovlnné a optické spoje.
- Dále Wi-Fi (LAN), WiMAX/LTE (MAN/WAN) a další.

## Geostacionární spoje
*Družice*

- Geostacionární družice: ~36 000 km, stále nad stejným bodem.
- Pasivní odrážejí; aktivní mají transpondéry (převod + zisk).
- C-band ~6/4 GHz; KU-band 12–14 GHz (menší antény).
- Nevýhoda: zpoždění ~250–300 ms (RTT až ~600 ms).

## Přístup a topologie
*VSAT*

- Bod–bod, broadcast (TV) i multiple access.
- VSAT: terminály + centrální hub.
- Komunikace jen terminál ↔ hub (ne přímo mezi terminály).

## Pozemní spoje
*Mikrovlny*

- Frekvence 1–12 GHz, směrování parabolou, malý rozptyl.
- Dosah typicky do ~50 km; retranslační stanice.
- Troposférické spoje: odraz ve ~16 km, dosah až ~500 km.

> Tip: přímá viditelnost

## AP, BSS, ESS
*Wi-Fi pojmy*

- AP = přístupový bod; STA komunikují přes AP (ne přímo).
- BSS = stanice v buňce BSA; ESS = více buněk přes DS.
- Roaming při překrytí; ad-hoc = P2P bez AP; hotspot = Wi-Fi.
- ESSID identifikuje síť při přístupu k AP.

## 802.11 a … ax
*Standardy*

- b: 11 Mb/s, 2,4 GHz; a/g: 54 Mb/s; n: MIMO, až 600 Mb/s.
- ac (Wi-Fi 5): 256-QAM, široké kanály; ax (Wi-Fi 6): OFDMA.
- OFDM: mnoho ortogonálních subnosných v kanálu.

## WEP → WPA3
*Zabezpečení*

- WEP: RC4, krátký IV – prolomeno (2001), nepoužívat.
- WPA: TKIP; WPA2: AES (doporučený základ).
- WPA3: SAE místo PSK, silnější ochrana hesla.
- 802.1X/EAP: ověření klienta přes AP (RADIUS).

## Dosahy a další
*Shrnutí*

- PAN <10 m (Bluetooth); LAN = Wi-Fi; MAN = WiMAX.
- WAN/mobilita: 802.16e, LTE; licencovaná pásma = méně rušení.
- Používej WPA2/WPA3 + AES, ne WEP.


# Kvíz

## Geostacionární družice létají asi:
- [x] 36 000 km nad Zemí
- 16 km v troposféře
- 50 km nad povrchem
- Jen v nízké orbitě LEO

## Hlavní nevýhoda družicového spoje je:
- [x] Velké zpoždění (~250–300 ms)
- Žádný broadcast
- Jen kabelové médium
- Absence portů

## V síti VSAT komunikace probíhá:
- [x] Jen mezi terminálem a hubem
- Přímo mezi dvěma terminály
- Jen přes ARP
- Jen přes WEP

## Mikrovlnný spoj typicky vyžaduje:
- [x] Přímou viditelnost (dosah ~50 km)
- Jen metalický kabel
- Jen port 80
- Jen WEP klíč

## V infrastruktuře Wi-Fi stanice (STA):
- [x] Komunikují přes AP, ne přímo mezi sebou
- Vždy jen ad-hoc bez AP
- Nepoužívají ESSID
- Běží jen na 36 000 km

## 802.11ax (Wi-Fi 6) přináší zejména:
- [x] OFDMA a vysokou kapacitu
- Jen WEP
- Jen C-band 4/6 GHz
- Jen troposférický odraz

## WEP je dnes:
- [x] Prolomený a nevhodný
- Nejbezpečnější volba
- Totéž co WPA3-SAE
- Povinný od roku 2006

## Doporučené zabezpečení Wi-Fi je:
- [x] WPA2/WPA3 s AES (ne WEP)
- Jen Open system WEP
- Jen Shared key WEP
- Bez hesla vždy
