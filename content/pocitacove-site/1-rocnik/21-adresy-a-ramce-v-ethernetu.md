---
id: site-1-adresy-a-ramce-v-ethernetu
puvodni: net.21
nazev: Adresy a rámce v ethernetu
popis: 8 snímků o MAC a typech rámců a pak kvíz.
nazev_cviceni: Kvíz: ethernet adresy a rámce
nadpis_kvizu: Kvíz k ethernetovým adresám a rámcům
---

# Výklad

## Síťový hardware
*Ethernet*

- Jeden ze standardů síťového hardware.
- Dnes nejpoužívanější.

## 48bitová MAC
*Adresy*

- Adresa má 48 bitů (6 bytů) a je celosvětově jedinečná.
- Je pevně v síťové kartě už od výroby.
- Výrobci dostávají bloky adres od IEEE.

> Tip: IEEE přidělí první 3 B, výrobce další 3 B

## Typy hlaviček
*Rámce*

- Rámec = skupina bitů na linkové vrstvě.
- Hlavička má adresy odesílatele/příjemce a typ obsahu.
- Typy: Ethernet II, IEEE 802.3, 802.3 SNAP, raw 802.3.

## DIX + EtherType
*Ethernet II*

- Původní DIX Ethernet.
- Hlavička: příjemce 6 B, odesílatel 6 B, EtherType 2 B.
- EtherType identifikuje protokol (IP, IPX…).

> Tip: EtherType > 1500 (např. IP = 0800h)

## Délka místo typu
*IEEE 802.3*

- Místo EtherType je údaj o délce (vždy ≤ 1500).
- Uvnitř je rámec 802.2 (stejný i pro Token Ring).
- Typ protokolu je v SAP mezi linkovou a síťovou vrstvou.

## Novell / IPX
*Raw 802.3*

- „Holý“ rámec 802.3 bez 802.2 (Novell).
- Funguje jen v prostředí IPX.
- Paket začíná dvěma byty FFFFh.

## 802.2 SNAP
*SNAP*

- Vložen do rámce 802.3.
- Rozšiřuje identifikaci protokolu až na 5 bytů.

> Tip: byty 15–16 za délkou = AAAAh

## Min / max / MTU
*Velikost*

- Max. Ethernet II: 1500 + 18 = 1518 B.
- Min. velikost: 64 B (kolizní okénko).
- MTU je obvyklé omezení velikosti na routerech.


# Kvíz

## Ethernetová adresa má:
- [x] 48 bitů (6 bytů) a je jedinečná
- Jen 16 bitů
- Jen 32 bitů IP
- Jen 8 bitů SAP

## První 3 byty MAC adresy přiděluje:
- [x] IEEE výrobci
- Jen uživatel v BIOS
- Jen DNS server
- Jen Aloha

## Ethernet II v hlavičce používá:
- [x] EtherType (např. IP = 0800h, hodnota > 1500)
- Jen délku ≤ 1500 bez typu
- Jen FFFFh bez adres
- Jen AAAAh bez MAC

## IEEE 802.3 místo EtherType má:
- [x] Údaj o délce (≤ 1500) a uvnitř 802.2
- Jen cut-through
- Jen 64 B MTU bez délky
- Jen laserovou hlavičku

## Raw 802.3 (Novell):
- [x] Bez 802.2, jen IPX, začíná FFFFh
- Funguje jen s HTTP
- Má vždy EtherType 0800h
- Nemá žádné adresy

## Maximální rámec Ethernet II je:
- [x] 1518 B (1500 + 18)
- 64 B
- 900 µm
- 250 µm

## Minimální velikost rámce plyne z:
- [x] Kolizního okénka (64 B)
- Jen z MTU routeru
- Jen z SNAP AAAAh
- Jen z Token Ring
