---
id: sw-1-operacni-system
nazev: Operační systém
popis: Co systém dělá mezi uživatelem a hardwarem.
nazev_cviceni: Kvíz: operační systém
nadpis_kvizu: Poznejte roli systému
---

# Výklad

## Mezi člověkem a strojem
*Role*

- Systém spouští programy, hlídá paměť a přiděluje čas procesoru.
- Souborový systém ukládá data na disk. Ve Windows je to NTFS.
- Ovladač si systém načte, aby mluvil s tiskárnou, diskem a sítí.
- Uživatel vidí plochu a okna. Jádro pod tím nevidí a do něj nesahá.

> Tip: Když dva programy chtějí tiskárnu najednou, rozhodne systém. Samy se nedomluví.

## Tři systémy
*Windows, Linux, macOS*

- Školní učebna je Windows. Příkazy a obrazovky v tomto předmětu jsou z Windows.
- Linux je častý na serverech. Stejný dokument v Excelu tam sám od sebe není.
- macOS běží na počítačích Apple. Zkratka Ctrl je tam často Cmd.
- Soubor .docx je dokument Wordu. Není to program a není to systém.

> Tip: Přípona souboru říká, čím ho otevřít. Systém podle ní vybere aplikaci.

## Start počítače
*Až po přihlášení*

- Po zapnutí naběhne firmware, pak systém, pak přihlašovací obrazovka.
- Dokud se nepřihlásíte, pracujete s účtem nikoho. Soubory uživatele jsou zamčené.
- Každý účet má vlastní plochu a složku C:\Users\jméno.
- Vypnutí přes nabídku Start uloží práci systému. Dlouhý stisk tlačítka je až poslední možnost.

> Tip: Restart po aktualizaci není trest. Systém si domlouvá soubory, které za běhu nevymění.

## Zadání: jaký systém to je
*Cvičení*

- Spusťte winver a opište edici a verzi, třeba Windows 11 Pro, verzi z okna.
- Otevřete C:\Users a napište, které složky uživatelů tam jsou. Do cizích nesaháte.
- Najděte jeden ovladač v názvu: ve Správci zařízení rozbalte Monitory nebo Tiskárny.
- Zápis uložte jako system.txt do Dokumenty. Hesla ani sériová čísla nepište.

> Tip: Správce zařízení je ve Windows devmgmt.msc. Žlutý vykřičník znamená chybějící nebo vadný ovladač.

# Kvíz

## Co dělá operační systém s pamětí?
- Nic, paměť si berou programy samy
- [x] Přiděluje ji programům
- Maže ji při každém uložení dokumentu
- Patří do Excelu
> Program požádá, systém přidělí. Dva programy nesmí psát do stejného místa.

## Jaký souborový systém používá školní Windows na disku?
- [x] NTFS
- Jen názvy souborů bez systému
- SVG
- MySQL
> NTFS drží soubory, oprávnění i větší disky. Názvy složek jsou až uvnitř něj.

## Kdy smíte sáhnout do složky jiného uživatele v C:\Users?
- Kdykoli, je to jeden počítač
- [x] Nesmíte, je to jeho účet
- Jen když je složka na ploše
- Když má dokument příponu docx
> Každý účet má vlastní soubory. Cizí složka není společný šuplík.

## Čím zjistíte edici Windows?
- [x] Příkazem winver
- Otevřením Excelu
- Smazáním koše
- Souborem .bat
> winver otevře okno O systému Windows s edicí a verzí.
