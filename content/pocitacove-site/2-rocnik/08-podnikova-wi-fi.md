---
id: site-2-podnikova-wi-fi
puvodni: net2.8
nazev: Podniková Wi-Fi
popis: SSID, WPA3 a proč jedno heslo na nástěnce nestačí.
nazev_cviceni: Kvíz: Podniková Wi-Fi
nadpis_kvizu: Kvíz k tématu Podniková Wi-Fi
---

# Výklad

## Více sítí, jedna infrastruktura
*SSID*

- Jedno rádio může ohlašovat víc SSID.
- Každé SSID se mapuje na VLAN.
- Schované SSID není zabezpečení, jméno jde pořád zachytit.
- Stejné SSID na více přístupových bodech umožní přechod.

> Tip: hostovské SSID má skončit v jiné VLAN než učitelé.

## WPA2 a WPA3
*Klíče*

- WPA2-Personal sdílí jedno heslo.
- WPA2-Enterprise ověřuje uživatele přes RADIUS.
- WPA3 lépe chrání i slabší hesla a má lepší dopředné utajení.
- Starý WEP se nepoužívá.

> Tip: společné heslo PSK se nedá vzít jednomu člověku, aniž by ho dostali všichni.

## Kanály a přechod
*Pokrytí*

- Pásmo 2,4 GHz má málo nepřekrývajících se kanálů.
- 5 GHz má víc kanálů a hůř prochází zdí.
- Výkon nesmí být zbytečně vysoký, jinak buňky slyší i cizí provoz.
- Roaming funguje, když se SSID a zabezpečení shodují.

> Tip: dva sousední body na stejném kanále si překážejí.


# Kvíz

## Kam má vést hostovské SSID?
- [x] Do vlastní VLAN, oddělené od školní sítě
- Do stejné sítě jako ředitelna
- Přímo do správy routeru
- Do sítě bez DHCP i bez izolace
> Host nemá vidět školní servery.

## Čím se liší WPA2-Enterprise od společného hesla?
- [x] Každý uživatel se ověřuje zvlášť, často přes RADIUS
- Nemá šifrování
- Funguje jen na 2,4 GHz
- Sdílí jeden klíč napsaný na tabuli
> Účet jde odebrat jednomu člověku.

## Proč schované SSID není ochrana?
- [x] Jméno sítě jde při připojení stejně zachytit
- Skryté SSID nejde vůbec použít
- WPA3 ho zakazuje
- Switch ho vyhlásí v STP
> Utajené jméno není šifra.

## Co vadí dvěma sousedním AP na stejném kanále?
- [x] Slyší se a kazí si přenos
- Smažou si VLAN
- Přestanou dělat DHCP
- Změní IPv6 na IPv4
> Kanály se mají plánovat, ne nechat náhodě.
