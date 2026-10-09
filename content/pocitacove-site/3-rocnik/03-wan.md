---
id: site-3-wan
puvodni: net3.3
nazev: WAN
popis: Spoj mezi pobočkami a k poskytovateli.
nazev_cviceni: Kvíz: WAN
nadpis_kvizu: Kvíz k tématu WAN
---

# Výklad

## Kde končí vaše síť
*Okraj*

- WAN spojuje vzdálené sítě přes síť, kterou nevlastníte.
- Poskytovatel vám dá adresu, bránu a často i VLAN.
- Výchozí trasa obvykle míří právě sem.
- Výpadek poslední míle odřízne pobočku, i když jádro žije.

> Tip: za routerem k poskytovateli už neřídíte fyzickou cestu.

## Čím se to vozí
*Technologie*

- Starší spoje používaly pronajaté linky a PPP.
- MPLS u poskytovatele spojí pobočky jako jednu službu.
- Metro Ethernet přivede do budovy běžný ethernetový port.
- Rychlost, odezva a ztráty jsou parametry, které se sledují.

> Tip: dnešní WAN často vypadá jako ethernet, i když pod ním je jiná síť.

## Co domluvit
*Návrh*

- Jedna přípojka je jednoduchá a křehká.
- Druhý poskytovatel sníží riziko, když nejde stejnou trasou.
- Adresy od poskytovatele musí sedět s vaším NAT a firewallem.
- Na okraji filtrujte, co do školy z internetu lézt nemá.

> Tip: záložní spoj má smysl jen s cestou, která se na něj umí přepnout.


# Kvíz

## Co je WAN z pohledu školy?
- [x] Spoj mimo budovu, často přes poskytovatele
- Kabel mezi dvěma porty jednoho switche
- VLAN pro tiskárny
- Diskové pole
> WAN překračuje místní síť.

## Kam obvykle míří výchozí trasa?
- [x] K poskytovateli
- Na tiskárnu
- Do STP rootu
- Na adresu 127.0.0.1
> Co směrovač nezná, pošle ven.

## Proč dva spoje stejnou ulicí nemusí být záloha?
- [x] Jedna porucha překope oba naráz
- OSPF dva spoje zakazuje
- IPv4 umí jen jednu bránu na světě
- Ethernet nesmí mít zálohu
> Fyzicky oddělená trasa je součást redundance.

## Co je metro Ethernet?
- [x] WAN služba, která u vás končí ethernetovým portem
- Wi-Fi ve vlaku
- Protokol místo DNS
- Typ procesoru
> Pod portem může být síť poskytovatele, vy vidíte ethernet.
