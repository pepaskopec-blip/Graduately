---
id: sw-1-zadani-windows
nazev: Zadání: školní účet a pořádek
popis: Složky, běžný účet, aktualizace a ochrana v jednom zápisu.
nazev_cviceni: Kvíz: školní účet
nadpis_kvizu: Zkontrolujte celé zadání
---

# Výklad

## Složky
*Strom*

- V Dokumentech je Skola\1-rocnik a v ní word, excel a system.
- Na ploše nezůstane pracovní soubor. Plocha není archiv.
- Přípony názvů jsou vidět. poznámka se nejmenuje poznámka.txt.txt.
- Názvy jsou bez znaků \ / : * ? a datum v názvu je rok-měsíc-den.

> Tip: Cestu opište z řádku adresy. Od oka se lomítka pletou.

## Účet
*zaci*

- Cvičný účet zaci je běžný uživatel. Není ve skupině Administrators.
- Heslo má aspoň 8 znaků a nezadává se do příkazu, jen přes hvězdičku v net user zaci * /add.
- Do zápisu nepatří skutečné heslo. Patří tam pravidlo a příkaz bez hesla.
- whoami na denní práci nemá ukázat účet správce.

> Tip: Účet zakládejte jen pod dohledem. V zápisu stačí postup, když učitel účet na počítači nechce.

## Stav systému
*Tři řádky*

- winver: edice a verze.
- Windows Update: aktuální, nebo čeká restart. Datum stačí.
- Zabezpečení Windows: ochrana v reálném čase zapnutá, firewall zapnutý.
- Ze Správce úloh tři nejtěžší aplikace podle paměti. Systémové procesy neukončené.

> Tip: Když je něco vypnuté, zapište to jako nález. Neopravujte firewall naslepo.

## Odevzdání
*Jeden soubor*

- Všechno je v ucto-windows.txt ve složce Skola\1-rocnik\system.
- Jsou tam cesta ke složce word, popis účtu, verze Windows a stav ochrany.
- Není tam heslo, není tam cizí soubor a není tam vypnutý Defender.
- Soubor otevřete znovu a přečtěte ho. Cesta v textu musí po vložení do adresy opravdu vyjít.

> Tip: Když cesta z textu skončí chybou, složka neexistuje, nebo je v cestě obyčejné lomítko.

# Kvíz

## Jaký účet je zaci?
- Správce
- [x] Běžný uživatel mimo skupinu Administrators
- Účet bez hesla v Public
- Sdílený účet celé třídy
> Denní účet není správce. Heslo má a do skupiny Administrators nepatří.

## Kam se ukládá pracovní soubor?
- Na plochu
- [x] Do Skola\1-rocnik a příslušné složky
- Do C:\Windows
- Do stažených souborů bez názvu
> Plocha se nearchivuje. Strom Skola drží word, excel a system odděleně.

## Co má být zapnuté?
- Jen spořič
- [x] Ochrana v reálném čase a firewall
- Dva antiviry
- Sdílení celé složky Users
> Defender a firewall zůstávají zapnuté. Do úkolu se opisuje jejich stav.

## Co v souboru ucto-windows.txt být nesmí?
- Verze z winver
- [x] Skutečné heslo
- Cesta ke složce
- Stav Windows Update
> Heslo se do textového úkolu nepíše. Příkaz ho bere přes hvězdičku, ne do sešitu.
