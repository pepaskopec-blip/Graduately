---
id: site-1-vypocet-ip-adres
puvodni: net.1
nazev: Výpočet IP adres
popis: 4 snímky s postupem a pak cvičení.
nazev_cviceni: Cvičení: výpočet IP adres
---

# Výklad

## Podsíťování sítí
*VLSM*

- Cíl: rozdělit jednu IP síť na menší podsítě podle počtu uzlů.
- Pamatuj si tři role adres v každém bloku:
- síťová = první adresa bloku
- broadcast = poslední adresa bloku
- uzly = vše mezi nimi (síť + 1 až broadcast − 1)

> Tip: prefix /n = počet jedniček v masce.  /24 = 255.255.255.0

## Jak velký blok potřebuji?
*Velikosti*

- Pro X uzlů potřebuješ blok, do kterého se vejde X + 2 adresy (síť a broadcast).
- Velikost bloku je vždy mocnina dvojky – vezmi nejmenší blok, který je větší nebo roven X + 2.
- 60 uzlů → 62 → blok 64 → /26
- 30 uzlů → 32 → blok 32 → /27
- 14 uzlů → 16 → blok 16 → /28
- 7 uzlů  → 9  → blok 16 → /28

> Počet bitů pro uzly h = log₂(velikost),  prefix = 32 − h

## Jak na to krok za krokem
*Postup*

- 1. Seřaď podsítě od největší po nejmenší.
- 2. Pro každou najdi potřebný prefix.
- 3. Začni na síťové adrese ze zadání.
- 4. Rozsah podsítě = od síťové adresy po broadcast.
- 5. Rozsah uzlů = síťová + 1 až broadcast − 1.
- 6. Posuň se hned za broadcast a pokračuj dál.

## Ukázka: 10.0.0.0/24
*Příklad*

- A  → 10.0.0.0/26    uzly 10.0.0.1–10.0.0.62    broadcast 10.0.0.63
- B  → 10.0.0.64/27   uzly 10.0.0.65–10.0.0.94   broadcast 10.0.0.95
- C  → 10.0.0.96/28   uzly 10.0.0.97–10.0.0.110  broadcast 10.0.0.111
- D  → 10.0.0.112/28  uzly 10.0.0.113–10.0.0.126 broadcast 10.0.0.127

> A: 60 uzlů, B: 30, C: 14, D: 7


# Úlohy na podsítě

## Úloha 1

1.) Síť má adresu 10.0.0.0/24. Rozdělte ji do podsítí s následujícími požadavky:

Podsíť A: 60 uzlů
Podsíť B: 30 uzlů
Podsíť C: 14 uzlů
Podsíť D: 7 uzlů

### Řešení

A: 10.0.0.0/26   síť 10.0.0.0   uzly 10.0.0.1–10.0.0.62   broadcast 10.0.0.63
B: 10.0.0.64/27  síť 10.0.0.64  uzly 10.0.0.65–10.0.0.94   broadcast 10.0.0.95
C: 10.0.0.96/28  síť 10.0.0.96  uzly 10.0.0.97–10.0.0.110  broadcast 10.0.0.111
D: 10.0.0.112/28 síť 10.0.0.112 uzly 10.0.0.113–10.0.0.126  broadcast 10.0.0.127

### Kontrola

| Prefix | Síť | Broadcast | První adresa | Poslední adresa |
|---|---|---|---|---|
| 26 | 10.0.0.0 | 10.0.0.63 | 10.0.0.1 | 10.0.0.62 |
| 27 | 10.0.0.64 | 10.0.0.95 | 10.0.0.65 | 10.0.0.94 |
| 28 | 10.0.0.96 | 10.0.0.111 | 10.0.0.97 | 10.0.0.110 |
| 28 | 10.0.0.112 | 10.0.0.127 | 10.0.0.113 | 10.0.0.126 |

## Úloha 2

2.) Síť má adresu 172.16.0.0/16. Rozdělte ji do podsítí s následujícími požadavky:

Podsíť A: 500 uzlů
Podsíť B: 200 uzlů
Podsíť C: 100 uzlů
Podsíť D: 50 uzlů

### Řešení

A: 172.16.0.0/23    síť 172.16.0.0   uzly 172.16.0.1–172.16.1.254  broadcast 172.16.1.255
B: 172.16.2.0/24    síť 172.16.2.0   uzly 172.16.2.1–172.16.2.254  broadcast 172.16.2.255
C: 172.16.3.0/25    síť 172.16.3.0   uzly 172.16.3.1–172.16.3.126  broadcast 172.16.3.127
D: 172.16.3.128/26  síť 172.16.3.128 uzly 172.16.3.129–172.16.3.190 broadcast 172.16.3.191

### Kontrola

| Prefix | Síť | Broadcast | První adresa | Poslední adresa |
|---|---|---|---|---|
| 23 | 172.16.0.0 | 172.16.1.255 | 172.16.0.1 | 172.16.1.254 |
| 24 | 172.16.2.0 | 172.16.2.255 | 172.16.2.1 | 172.16.2.254 |
| 25 | 172.16.3.0 | 172.16.3.127 | 172.16.3.1 | 172.16.3.126 |
| 26 | 172.16.3.128 | 172.16.3.191 | 172.16.3.129 | 172.16.3.190 |

## Úloha 3

3.) Síť má adresu 192.168.10.0/24. Rozdělte ji do podsítí s následujícími požadavky:

Podsíť A: 120 uzlů
Podsíť B: 70 uzlů
Podsíť C: 35 uzlů
Podsíť D: 15 uzlů

### Řešení

Do sítě /24 se tyto požadavky nevejdou: A i B potřebují /25 (2 × 128 = 256 adres), na C a D už nezbude žádný blok. Při zachování velikostí by bylo nutné větší síť (např. /23).

### Kontrola

| Prefix | Síť | Broadcast | První adresa | Poslední adresa |
|---|---|---|---|---|
| -1 |  |  |  |  |
| -1 |  |  |  |  |
| -1 |  |  |  |  |
| -1 |  |  |  |  |

## Úloha 4

4.) Síť má adresu 192.168.2.0/25. Rozdělte ji do podsítí s následujícími požadavky:

Podsíť A: 50 uzlů
Podsíť B: 25 uzlů
Podsíť C: 12 uzlů
Podsíť D: 5 uzlů

### Řešení

A: 192.168.2.0/26    síť 192.168.2.0    uzly 192.168.2.1–192.168.2.62 broadcast 192.168.2.63
B: 192.168.2.64/27   síť 192.168.2.64   uzly 192.168.2.65–192.168.2.94   broadcast 192.168.2.95
C: 192.168.2.96/28   síť 192.168.2.96   uzly 192.168.2.97–192.168.2.110  broadcast 192.168.2.111
D: 192.168.2.112/28  síť 192.168.2.112  uzly 192.168.2.113–192.168.2.126  broadcast 192.168.2.127

### Kontrola

| Prefix | Síť | Broadcast | První adresa | Poslední adresa |
|---|---|---|---|---|
| 26 | 192.168.2.0 | 192.168.2.63 | 192.168.2.1 | 192.168.2.62 |
| 27 | 192.168.2.64 | 192.168.2.95 | 192.168.2.65 | 192.168.2.94 |
| 28 | 192.168.2.96 | 192.168.2.111 | 192.168.2.97 | 192.168.2.110 |
| 28 | 192.168.2.112 | 192.168.2.127 | 192.168.2.113 | 192.168.2.126 |
