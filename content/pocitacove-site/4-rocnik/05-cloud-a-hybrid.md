---
id: site-4-cloud-a-hybrid
puvodni: net4.5
nazev: Cloud a hybrid
popis: Část služeb není ve skříni na chodbě.
nazev_cviceni: Kvíz: Cloud a hybrid
nadpis_kvizu: Kvíz k tématu Cloud a hybrid
---

# Výklad

## Vlastní sál, nebo cizí
*Kam s tím*

- Veřejný cloud provozuje poskytovatel a vy si berete službu.
- Soukromý cloud je pořád vaše zařízení, jen obalené portálem.
- Hybrid nechá část dat ve škole a část venku.
- Účet a síťová cesta jsou pořád vaše odpovědnost.

> Tip: cloud neruší síť ve škole, mění jen to, kam vede spoj.

## Jak se tam školní síť dostane
*Cesta*

- VPN do cloudu vypadá jako další pobočka.
- Veřejná služba jede přes internet a firewall.
- DNS musí ukazovat na nové adresy.
- Výpadek WAN teď shodí i to, co dřív jelo z místního serveru.

> Tip: služba v cloudu bez cesty a bez jmen je jen faktura.

## Kdo zálohuje a kdo pustí dovnitř
*Hranice*

- Pravidla firewallu se stěhují s službou.
- Účty v cloudu mají mít stejný řád jako účty ve škole.
- Test obnovy platí i pro data, která nejsou ve vašem racku.
- Škola pořád řeší osobní údaje, i když disk je jinde.

> Tip: poskytovatel nezálohuje vaše data, dokud to není ve smlouvě.


# Kvíz

## Co hybridní síť znamená?
- [x] Část služeb je ve škole a část u poskytovatele
- Dva switche v jedné VLAN
- IPv4 i papírový sešit
- Wi-Fi bez hesla
> Hranice školy už není hranicí všech služeb.

## Co se pokazí, když WAN spadne a pošta je jen v cloudu?
- [x] Škola se k poště nedostane
- Místní switch přestane dělat VLAN
- STP zvolí cloud jako root
- Nic, cloud WAN nepotřebuje
> Cesta ven je součást dostupnosti služby.

## Kdo zálohuje data v cloudu?
- [x] Ten, koho to smlouva a nastavení opravdu určují
- Vždy automaticky stát
- STP
- Nikdo, cloud zálohy zakazuje
> Předpoklad, že poskytovatel zálohuje vše, často neplatí.

## Co škola řeší i u cizího disku?
- [x] Kdo má přístup a jak se chrání osobní údaje
- Jen barvu loga
- Jen typ ventilátoru v sále poskytovatele
- Nic, odpovědnost přechází celá
> Umístění dat nemaže pravidla přístupu.
