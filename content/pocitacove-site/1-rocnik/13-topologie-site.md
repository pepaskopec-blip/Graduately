---
id: site-1-topologie-site
puvodni: net.13
nazev: Topologie sítě
popis: 4 snímky o sběrnici, hvězdě a kruhu a pak kvíz.
nazev_cviceni: Kvíz: topologie sítě
nadpis_kvizu: Kvíz k topologiím sítě
---

# Výklad

## Topologie sítě
*Přehled*

- Topologie popisuje, jak jsou uzly propojené.
- Základní typy: sběrnice, hvězda a kruh.

## Bus
*Sběrnice*

- Médium sdílí všichni – jednoduchá a levná topologie.
- Malá délka kabelů.
- Nevýhody: nízká bezpečnost (všichni slyší vše).
- Přerušení kabelu nebo terminátoru vyřadí celou síť.

> Tip: dříve typicky koaxiální kabel

## Star
*Hvězda*

- Uzly jdou přes centrální prvek (hub nebo switch).
- Výhody: odolnost proti poruchám kabelů k uzlům,
- snadná diagnostika.
- Nevýhody: aktivní prvky a více kabeláže.

> Tip: kroucená dvoulinka nebo optika

## Ring
*Kruh*

- Kabely tvoří souvislý kruh.
- Zprávy obíhají, dokud nenajdou adresáta.

> Tip: často zdvojený kruh pro odolnost


# Kvíz

## Topologie sběrnice (bus) znamená, že:
- Každý uzel má vlastní centrální switch
- [x] Médium sdílí všichni
- Kabely tvoří jen zdvojený kruh
- Používá jen OSPF

## Nevýhoda sběrnice je hlavně:
- Že potřebuje hub u každého uzlu
- [x] Nízká bezpečnost a náchylnost na poruchu kabelu
- Že nelze použít koaxiál
- Že zprávy neobíhají

## Topologie hvězda propojuje uzly přes:
- Jen jeden společný koaxiál bez centra
- [x] Centrální prvek (hub nebo switch)
- Jen Token Ring
- Jen ATM

## Výhoda hvězdy je:
- [x] Že přerušení jednoho kabelu k uzlu nevyřadí celou síť
- Že všichni slyší vše
- Že nepotřebuje žádnou kabeláž
- Že nemá aktivní prvky

## Hvězda typicky používá:
- Jen koaxiální kabel
- [x] Kroucenou dvoulinku nebo optiku
- Jen simplexní rádio
- Jen FDM

## V topologii kruh (ring):
- [x] Zprávy obíhají, dokud nenajdou adresáta
- Každý paket jde jen přes DNS
- Médium sdílí hub bez kabelů
- Neexistuje žádná kabeláž
