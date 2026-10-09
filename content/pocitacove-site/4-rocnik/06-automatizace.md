---
id: site-4-automatizace
puvodni: net4.6
nazev: Automatizace
popis: Opakovanou práci ať dělá skript, ne deset kliknutí.
nazev_cviceni: Kvíz: Automatizace
nadpis_kvizu: Kvíz k tématu Automatizace
---

# Výklad

## Správa bez společného hesla na lístečku
*SSH*

- Klíč jde odebrat konkrétnímu člověku.
- Heslo v příkazu historie nemá ležet.
- Přístup omezte na správní síť.
- Záznam, kdo se přihlásil, patří do logu.

> Tip: Telnet posílá heslo čitelně. Na správu patří SSH.

## Stejná VLAN ve dvaceti switchích
*Opakování*

- Šablona drží názvy VLAN a popis portů.
- Skript změnu pošle na seznam zařízení.
- Nejdřív ho pusťte na jednom switchi.
- Záloha před hromadnou změnou je povinnost, ne ozdoba.

> Tip: ruční opis se v desátém switchi liší v jednom čísle.

## Skript není výmluva
*Mez*

- Kontrola po běhu srovná, co opravdu sedí.
- Tajné heslo nepatří do veřejného souboru.
- Člověk pořád schvaluje změnu, která shodí výuku.
- Verzovaný soubor konfigurace ukáže, kdo řádek změnil.

> Tip: automat neopraví špatný záměr, jen ho provede rychleji.


# Kvíz

## Proč na správu nepoužívat Telnet?
- [x] Heslo jde po síti čitelně
- Telnet neumí IPv4
- Switch Telnet zakazuje standardem STP
- Telnet maže VLAN
> SSH spojení chrání.

## Jak pustíte stejnou změnu do dvaceti switchů?
- [x] Šablonou a skriptem, neopisovat ji dvacetkrát
- Jen restartem všech naráz
- Přes DHCP
- Jedním broadcastem bez kontroly
> Nejdřív jeden switch, pak ostatní.

## Co před hromadnou změnou uděláte?
- [x] Zálohu konfigurací
- Vypnete dohled
- Smažete popisy portů
- Zakážete SSH
> Ať se máte kam vrátit.

## Co skript neumí?
- [x] Poznat, že záměr byl špatně
- Poslat stejný příkaz víckrát
- Uložit konfiguraci
- Připojit se přes SSH
> Rychlost neopraví chybný příkaz.
