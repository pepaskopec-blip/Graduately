---
id: site-2-nat-a-pat
puvodni: net2.4
nazev: NAT a PAT
popis: Jedna veřejná adresa pro mnoho počítačů.
nazev_cviceni: Kvíz: NAT a PAT
nadpis_kvizu: Kvíz k tématu NAT a PAT
---

# Výklad

## Soukromé adresy ven
*NAT*

- NAT přepíše zdrojovou adresu při odchodu z lokální sítě.
- Zpět router podle tabulky překladů provoz vrátí.
- Zvenku nikdo sám od sebe spojení dovnitř nenaváže.
- To je vedlejší efekt, ne náhrada firewallu.

> Tip: 192.168.0.0/16 a 10.0.0.0/8 na internetu směrovat nejdou.

## Mnoho počítačů, jedna adresa
*PAT*

- Více vnitřních adres sdílí jednu veřejnou.
- Rozliší je zdrojový port.
- Domácí router dělá téměř vždy PAT, ne čistý NAT jedna ku jedné.
- Dojdou-li porty, nová spojení se neotevřou.

> Tip: PAT si pamatuje i číslo portu, ne jen adresu.

## Když má server být vidět
*Dovnitř*

- Port forward pošle příchozí port na vnitřní adresu.
- Na serveru pak musí poslouchat daná služba a firewall ji pustit.
- DMZ na domácím routeru vystaví jeden počítač hodně otevřeně.
- Lepší je konkrétní port než celá DMZ.

> Tip: přesměrování portu otevře jen tu službu, kterou opravdu chcete.


# Kvíz

## Proč stanice s adresou 192.168.1.20 není z internetu vidět přímo?
- [x] Je to soukromá adresa a ven jde až přes NAT
- IPv4 takové adresy nezná
- MAC adresa se na internetu nesmí použít jako jméno
- DNS soukromé adresy maže
> Soukromé rozsahy se mezi poskytovateli nesměrují.

## Čím se PAT liší od NAT jedna ku jedné?
- [x] Více vnitřních adres sdílí jednu veřejnou a liší se portem
- PAT šifruje provoz
- PAT funguje jen pro IPv6
- PAT ruší potřebu brány
> Překlad zahrnuje adresu i port.

## K čemu je přesměrování portu?
- [x] Příchozí spojení na daný port pošle na vnitřní server
- Přidělí VLAN
- Zapne STP
- Změní masku celé školy na /8
> Bez něj zvenku na vnitřní web sami nedosáhnete.

## Proč NAT není firewall?
- [x] Jen překládá adresy, pravidla provozu řeší firewall
- NAT umí totéž co antivir
- NAT kontroluje hesla uživatelů
- NAT šifruje Wi-Fi
> Nechtěný publikovaný port NAT klidně pustí.
