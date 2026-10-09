---
id: site-1-podrobny-rozbor-7-vrstev-iso-osi
puvodni: net.11
nazev: Podrobný rozbor 7 vrstev ISO/OSI
popis: 8 snímků o jednotlivých vrstvách a pak kvíz.
nazev_cviceni: Kvíz: 7 vrstev ISO/OSI
nadpis_kvizu: Kvíz k 7 vrstvám ISO/OSI
---

# Výklad

## Tři skupiny vrstev
*Přehled*

- Vrstvy orientované na přenos: fyzická, spojová, síťová.
- Přizpůsobovací vrstva: transportní.
- Vrstvy orientované na aplikace: relační, prezentační,
- aplikační.

> Tip: přenos → přizpůsobení → aplikace

## Přenos bitů
*Fyzická*

- Přenos bitů po médiu.
- Definuje napětí, kabely, konektory, kódování a modulaci.
- Řeší také duplex / simplex.

## Rámce a MAC
*Spojová*

- Tvorba rámců a fyzické (MAC) adresy.
- Kontrola chyb (CRC) a řízení toku mezi sousedními uzly.
- Podvrstvy: MAC a LLC.

> Tip: podvrstvy MAC (přístup k médiu) a LLC (logické řízení)

## Směrování paketů
*Síťová*

- Směrování (routing) paketů.
- Funguje i v sítích bez přímého spojení mezi uzly.

## Segmenty end-to-end
*Transportní*

- Rozklad zpráv na segmenty a jejich zpětné složení.
- Kontrola pořadí a náprava chyb.
- Existuje jen v koncových uzlech – není v routerech.

> Tip: v routerech transportní vrstva není

## Relace a dialog
*Relační*

- Navazuje, udržuje a ukončuje relace.
- Řídí dialog („neskákat si do řeči“).
- Synchronizace – navázání po přerušení.

## Formát dat
*Prezentační*

- Formátování dat (např. ASCII).
- Komprese a šifrování.
- Převod mezi různými standardy.

## Rozhraní pro programy
*Aplikační*

- Rozhraní pro aplikace a služby.
- Například e-mail, HTTP, FTP nebo terminál.


# Kvíz

## Fyzická vrstva se stará o:
- [x] Přenos bitů
- Směrování paketů
- Navazování relací
- Formátování ASCII

## MAC a LLC jsou podvrstvy:
- Síťové vrstvy
- [x] Spojové (linkové) vrstvy
- Transportní vrstvy
- Aplikační vrstvy

## Směrování paketů v sítích bez přímého spojení řeší:
- Fyzická vrstva
- Relační vrstva
- [x] Síťová vrstva
- Prezentační vrstva

## Transportní vrstva existuje:
- [x] Jen v koncových uzlech
- I v routerech
- Jen ve switchech
- Jen na hubu

## Relační vrstva především:
- Definuje napětí a kabely
- [x] Navazuje, udržuje a ukončuje relace
- Přidává MAC adresy
- Směruje pakety

## Komprese a šifrování patří do vrstvy:
- Fyzické
- Spojové
- [x] Prezentační
- Síťové

## Aplikační vrstva poskytuje:
- CRC mezi sousedy
- [x] Rozhraní pro programy (HTTP, FTP, e-mail…)
- Jen duplex/simplex
- Jen směrování
