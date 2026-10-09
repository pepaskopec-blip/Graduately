---
id: site-3-firewall-a-zony
puvodni: net3.5
nazev: Firewall a zóny
popis: Kdo s kým smí mluvit, ne jen jestli kabel svítí.
nazev_cviceni: Kvíz: Firewall a zóny
nadpis_kvizu: Kvíz k tématu Firewall a zóny
---

# Výklad

## Důvěra není stejná
*Zóny*

- Rozhraní se sdruží do zón podle důvěry.
- Z důvěryhodnější zóny ven se často povoluje snáz.
- Z internetu dovnitř se povoluje jen to, co je schválené.
- Pravidlo se píše od zóny ke zóně, ne jen na jednu adresu.

> Tip: LAN, WAN a DMZ nemají stejná pravidla.

## Pamatuje si spojení
*Stav*

- Nové spojení se kontroluje proti politice.
- Odpověď patřící ke spojení se pustí.
- Bezestavový filtr zkoumá každý paket zvlášť.
- Ukončené spojení se z tabulky smaže.

> Tip: stavový firewall pustí odpověď, aniž byste psali pravidlo nazpátek.

## Server, který má být vidět
*DMZ*

- Veřejná služba sedí v oddělené zóně.
- Z internetu se otevře jen její port.
- Z DMZ do LAN se pustí jen to, co služba opravdu potřebuje.
- Kompromitovaný web pak neotevře celou školu.

> Tip: web v DMZ nemá mít cestu ke všem diskům ve škole.


# Kvíz

## Proč se zóny liší?
- [x] Mají jinou důvěru a tedy jiná pravidla
- Každá zóna musí mít vlastní DNS jméno switche
- Zóny nahrazují IP adresy
- Bez zón nejde STP
> Internet a učebna nejsou stejně důvěryhodné.

## Co umí stavový firewall?
- [x] Pustí odpověď k už otevřenému spojení
- Smaže VLAN
- Přidělí MAC adresu
- Nahradí směrovací protokol
> Pamatuje si, kdo spojení začal.

## Kam patří veřejný web, který nemá vidět celou LAN?
- [x] Do DMZ
- Do stejné sítě jako účtárna
- Na access port ředitele
- Do nativní VLAN trunku bez pravidel
> DMZ omezí, kam se útočník dostane dál.

## Co samotná zóna ještě není?
- [x] Náhrada za konkrétní pravidla služeb
- Způsob, jak sdružit rozhraní
- Hranice důvěry
- Místo, kam se píše politika
> Zóna bez pravidel nic nechrání.
