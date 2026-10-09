---
id: site-2-vlan-a-trunk
puvodni: net2.1
nazev: VLAN a trunk
popis: Logické sítě na jednom přepínači a značkovaný spoj.
nazev_cviceni: Kvíz: VLAN a trunk
nadpis_kvizu: Kvíz k tématu VLAN a trunk
---

# Výklad

## Oddělení provozu
*VLAN*

- VLAN sdružuje porty do jedné vysílací domény.
- Počítače v různých VLAN spolu přímo na druhé vrstvě nemluví.
- Číslo VLAN se na trunku vozí ve značce 802.1Q.
- Nativní VLAN na trunku jede neznačkovaná. Na obou koncích musí sedět.

> Tip: VLAN není samostatný kabel, je to značka v rámci.

## Access a trunk
*Porty*

- Access port se připojuje ke koncovému zařízení.
- Trunk spojuje přepínače nebo přepínač s routerem.
- Povolené VLAN na trunku omezí, co smí přejít.
- Špatně nastavený trunk pustí cizí VLAN do špatné sítě.

> Tip: access port patří do jedné VLAN, trunk jich nese víc.

## K čemu se VLAN hodí
*Provoz*

- Hosté, škola a správa mohou sdílet jeden přepínač.
- Hlasová VLAN oddělí telefony od dat.
- Mezi VLAN se dostane jen směrovač nebo přepínač třetí vrstvy.
- VLAN zmenšuje broadcast, nenahrazuje firewall.

> Tip: oddělení hostů od školy nesmí stát jen na hesle Wi-Fi.


# Kvíz

## Co VLAN na přepínači dělá?
- [x] Sdruží porty do jedné vysílací domény
- Zrychlí procesor serveru
- Nahradí šifrování WPA3
- Přidělí veřejnou IPv4 adresu
> VLAN odděluje provoz na druhé vrstvě.

## Jak se VLAN pozná na trunku?
- [x] Značkou 802.1Q v rámci
- Jinou MAC adresou zdroje
- Jiným konektorem RJ11
- Bitem v ICMP
> 802.1Q vloží do ethernetového rámce číslo VLAN.

## Kam patří access port?
- [x] Ke koncovému počítači v jedné VLAN
- Jen mezi dvěma routery
- Na optický spoj s vlnovým multiplexem
- Do napájecího zdroje
> Access port má jednu VLAN a značku obvykle sundá.

## Jak spolu mluví dvě různé VLAN?
- [x] Přes směrování, ne přímo na druhé vrstvě
- Samy od sebe, VLAN je jen barva kabelu
- Jedině přes ARP broadcast do všech VLAN
- Jen když mají stejnou MAC adresu
> Mezi VLAN je potřeba router nebo L3 switch.
