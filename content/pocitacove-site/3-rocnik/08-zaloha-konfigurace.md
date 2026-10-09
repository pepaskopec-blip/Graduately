---
id: site-3-zaloha-konfigurace
puvodni: net3.8
nazev: Záloha konfigurace
popis: Co uložit, než se přepínač vymění.
nazev_cviceni: Kvíz: Záloha konfigurace
nadpis_kvizu: Kvíz k tématu Záloha konfigurace
---

# Výklad

## Restart není uložení
*Běžící a uložená*

- Běžící konfigurace je to, co zařízení právě dělá.
- Startovní se načte po zapnutí.
- Uložení zkopíruje běžící do startovní.
- Bez uložení druhý den síť vypadá jako včera.

> Tip: změna v běžící konfiguraci po restartu zmizí, dokud ji neuložíte.

## Nejen na zařízení
*Kopie ven*

- Konfigurace patří na server mimo to zařízení.
- Ukládejte i verzi, ať víte, co se změnilo.
- Hesla v exportu mohou být čitelná. Soubor chraňte.
- Automatická záloha po změně porazí dobrý úmysl.

> Tip: záloha na tom samém přepínači shoří spolu s ním.

## Aby to přečetl i kolega
*Popis*

- Popis portu říká, co je na druhém konci.
- Schéma VLAN a adres má sedět s konfigurací.
- Změna bez poznámky se špatně vrací.
- Nový switch postavíte z zálohy jen tehdy, když záloha existuje.

> Tip: port bez popisu je za půl roku hádanka.


# Kvíz

## Co se stane se změnou, kterou neuložíte?
- [x] Po restartu zmizí
- Zapíše se do VLAN navždy
- Přepíše firmware
- Pošle se sama do DNS
> Běžící konfigurace není totéž co startovní.

## Proč záloha jen na tom samém switchi nestačí?
- [x] S poškozeným switchem zmizí i ona
- Switch zálohy šifruje tak, že je nečitelné i pro vás
- STP zálohy maže
- IPv6 zálohy zakazuje
> Kopie musí žít jinde.

## Co má být u portu napsané?
- [x] Co je na druhém konci
- Heslo administrátora
- Celá směrovací tabulka
- Soukromý klíč VPN
> Popis portu šetří hodiny při poruše.

## Proč chránit soubor se zálohou?
- [x] Může obsahovat čitelná hesla
- Soubor sám otevírá porty
- DNS ho publikuje
- Je větší než disk
> Export není vždy v zašifrované podobě.
