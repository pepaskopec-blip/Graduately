---
id: site-3-vpn
puvodni: net3.4
nazev: VPN
popis: Šifrovaný tunel přes síť, které nevěříte.
nazev_cviceni: Kvíz: VPN
nadpis_kvizu: Kvíz k tématu VPN
---

# Výklad

## Cizí síť jako kabel
*Tunel*

- Mezi dvěma body se postaví šifrovaný tunel.
- Uvnitř mohou téct soukromé adresy.
- Venku je vidět jen obal mezi konci tunelu.
- Bez shodných klíčů nebo certifikátů tunel nevstane.

> Tip: VPN neschová provoz před sítí, ve které tunel končí.

## Pobočka a člověk
*Dva tvary*

- Site-to-site propojí dvě pobočky trvale.
- Remote access pustí notebook do školní sítě.
- Uživatel se ověřuje účtem, ne jen společným heslem na zdi.
- Split tunnel posílá tunelem jen školní sítě, zbytek jde přímo.

> Tip: site-to-site spojuje sítě, remote access jednoho uživatele.

## Když tunel stojí a data ne
*Chyby*

- Musí sedět návrh šifry na obou koncích.
- Firewall po cestě musí pustit port VPN.
- Uvnitř musí existovat trasa do cílových sítí.
- Příliš široký tunel stáhne na školu i provoz, který tam být neměl.

> Tip: tunel může svítit zeleně a přesto chybí trasa dovnitř.


# Kvíz

## Co VPN před cizí sítí skrývá?
- [x] Obsah a vnitřní adresy, venku je vidět obal tunelu
- I to, že nějaký provoz existuje
- MAC adresu switche ve třídě
- Úplně všechno včetně konců tunelu
> Koncové adresy tunelu vidět jsou, náklad ne.

## Čím se liší site-to-site od remote access?
- [x] První spojuje sítě, druhá připojuje uživatele
- První je Wi-Fi, druhá je VLAN
- Obě jen mění DNS
- Remote access nejde šifrovat
> Notebook není druhá pobočka, i když tunel vypadá podobně.

## Proč tunel svítí, ale server nehraje?
- [x] Chybí trasa nebo firewall uvnitř
- IPv4 zakazuje VPN
- STP tunely maže
- DNS přejmenuje šifru
> Šifra a dosažitelnost jsou dvě vrstvy.

## Co je split tunnel?
- [x] Tunelem jde jen provoz do vnitřních sítí
- Každý paket se rozpůlí
- VPN bez šifry
- Dva SSID na jednom kanále
> Internet z notebooku pak nemusí téct přes školu.
