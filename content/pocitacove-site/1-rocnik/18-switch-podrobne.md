---
id: site-1-switch-podrobne
puvodni: net.18
nazev: Switch (podrobně)
popis: 5 snímků o CAM a přepínání a pak kvíz.
nazev_cviceni: Kvíz: switch podrobně
nadpis_kvizu: Kvíz ke switchi podrobně
---

# Výklad

## Učení MAC adres
*Funkce*

- Switch se učí MAC adresy odesílatelů.
- Ukládá je do CAM tabulky.
- Rámce pak posílá jen na konkrétní port.

> Tip: CAM tabulka = adresa → port

## Tři způsoby přepínání
*Metody*

- Cut-through, store-and-forward a fragment-free.
- Liší se rychlostí a kontrolou chyb.

## Hned po adrese
*Cut-through*

- Čte jen adresu příjemce a hned posílá dál.
- Nejrychlejší metoda.
- Nevýhoda: posílá i chybné rámce.

## Celý rámec + CRC
*Store-and-forward*

- Přijme celý rámec a zkontroluje CRC.
- Teprve pak rámec pošle.
- Nejpomalejší, ale nejbezpečnější metoda.

## Modifikovaný cut-through
*Fragment-free*

- Čte prvních 64 bytů rámce.
- To stačí k odhalení kolizních fragmentů.
- Kompromis mezi rychlostí a bezpečností.

> Tip: prvních 64 bytů = detekce kolizí


# Kvíz

## Switch se učí MAC adresy a ukládá je do:
- [x] CAM tabulky
- Jen DNS cache
- Jen ARP bez portů
- Jen OSPF

## Díky CAM tabulce switch posílá rámce:
- [x] Jen na konkrétní port
- Vždy na všechny porty jako hub
- Jen na routery
- Jen přes laser

## Cut-through přepínání:
- [x] Čte jen adresu příjemce a hned posílá
- Vždy čeká na celé CRC
- Nikdy neposílá rámce
- Pracuje jen s IP směrováním

## Nevýhoda cut-through je:
- [x] Že posílá i chybné rámce
- Že je nejpomalejší
- Že nemá MAC adresy
- Že vyžaduje gateway

## Store-and-forward:
- [x] Přijme celý rámec, zkontroluje CRC a pak pošle
- Čte jen první 2 byty
- Nikdy nekontroluje chyby
- Funguje jen jako Aloha

## Fragment-free (modifikovaný cut-through) čte:
- [x] Prvních 64 bytů (detekce kolizí)
- Jen poslední byte
- Celý paket IP včetně směrování
- Jen SMTP hlavičku
