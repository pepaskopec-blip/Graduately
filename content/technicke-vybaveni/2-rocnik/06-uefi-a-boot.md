---
id: hardware-2-uefi-a-boot
puvodni: hw2.6
nazev: UEFI a boot
popis: Co se děje, než naběhne systém.
nazev_cviceni: Kvíz: UEFI a boot
nadpis_kvizu: Kvíz k tématu UEFI a boot
---

# Výklad

## BIOS a UEFI
*Firmware*

- Firmware otestuje železo a najde disk se systémem.
- UEFI používá tabulku GPT, starý BIOS často MBR.
- Secure Boot pustí jen podepsaný zavaděč.
- Vypnutí Secure Bootu je výjimka, ne výchozí rada.

> Tip: UEFI není jen barevné menu. Umí jiné dělení disku a Secure Boot.

## Z čeho se startuje
*Pořadí*

- Nabídka bootu umí jednorázově vybrat zařízení.
- Disk bez zavaděče se v seznamu nechytí.
- Více systémů potřebuje bootovací nabídku, ne přepisování pořád dokola.
- Síťový boot PXE hledá obraz na serveru.

> Tip: flash disk v pořadí před systémem nabootuje omylem instalaci.

## Menu není pro celou třídu
*Heslo*

- Heslo na setup brání přehazování bootu.
- Heslo na disk je silnější a bez něj jsou data nedostupná.
- Clear CMOS heslo setupu často smaže a vezme i nastavení.
- Po změně profilu paměti ověřte, že systém naběhne dvakrát.

> Tip: heslo BIOSu bez záznamu je past, až se bude měnit disk.


# Kvíz

## Co Secure Boot hlídá?
- [x] Aby se spustil jen důvěryhodný zavaděč
- Aby měl disk aspoň 1 TB
- Aby ventilátor točil na maximum
- Aby nešel USB flash disk vůbec zapojit
> Nepodepsaný zavaděč se nespustí.

## Čím se liší GPT od MBR?
- [x] GPT je tabulka oddílů pro UEFI a větší disky
- GPT je typ pasty
- MBR je jen pro NVMe
- Jsou to totéž
> Starý MBR má omezení, na která velký disk naráží.

## Proč nenechat flash disk trvale první v bootu?
- [x] Omylem z něj naběhne cizí systém
- Flash disk vypne Secure Boot navždy
- UEFI flash disky zakazuje
- Smaže TPM
> Jednorázová nabídka bootu je bezpečnější.

## Co často udělá vymazání CMOS?
- [x] Smaže heslo setupu i ostatní volby
- Smaže jen soubory na ploše
- Přepne Windows na jiný jazyk
- Opraví ohnutý pin
> Je to návrat firmware k výchozímu stavu.
