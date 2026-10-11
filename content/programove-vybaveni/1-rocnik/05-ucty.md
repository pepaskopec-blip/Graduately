---
id: sw-1-ucty
nazev: Uživatelské účty
popis: Běžný uživatel, správce a heslo, které není jméno účtu.
nazev_cviceni: Kvíz: účty
nadpis_kvizu: Vyberte typ účtu a heslo
---

# Výklad

## Dva typy
*Správce a uživatel*

- Správce smí instalovat programy a měnit systém. Ve škole je to účet učitele nebo správce učebny.
- Běžný uživatel smí pracovat se svými soubory. Systémové složky mu systém zamkne.
- Denní práce nepatří na účet správce. Virus by dostal stejná práva jako vy.
- Nový školní účet pro cvičení se jmenuje zaci a je běžný uživatel, ne správce.

> Tip: Když systém požádá o heslo správce, běžný účet na to nestačí. To je záměr, ne porucha.

## Heslo
*Osm znaků*

- Heslo má aspoň 8 znaků a není to jméno účtu, ani slovo heslo, ani 12345678.
- Do příkazu se heslo nepíše. Příkaz net user zaci * /add se na heslo zeptá a na obrazovce ho neukáže.
- Heslo na papírek na monitor nepatří. Patří do hlavy, nebo do školního sejfu hesel, když ho učitel určí.
- Cizí heslo se nezkouší. Ani když je účet spolužáka odhlášený a počítač je společný.

> Tip: Hvězdička v net user znamená dotaz na heslo. Heslo napsané za příkazem zůstane v historii příkazů.

## Vlastní soubory
*Profil*

- Profil je složka C:\Users\zaci. Jsou v ní Dokumenty, Plocha a Stažené soubory.
- Soubor uložený do Dokumentů tohoto účtu jiný běžný uživatel neotevře.
- Společná složka C:\Users\Public je pro věci, které mají vidět všichni. Úkoly tam nedávejte.
- Odhlášení účet zavře. Vypnutí počítače taky. Přepnutí uživatele nechá první účet otevřený v paměti.

> Tip: Na sdíleném počítači se odhlašujte. Přepnutí uživatele nechá otevřené dokumenty v paměti.

## Zadání: účet na papíře
*Cvičení*

- Napište, jak by se založil běžný účet zaci: příkaz net user zaci * /add spuštěný jako správce.
- Napište heslo, které pravidlům nevyhovuje, a druhé, které má aspoň 8 znaků a není jméno účtu. Skutečné školní heslo nepište.
- Účet nesmí být ve skupině Administrators. Patří mezi běžné Users.
- Uložte ucet.txt do Skola\1-rocnik\system. Účet v systému zakládejte jen když to učitel výslovně řekne.

> Tip: Kdo jsem, zjistí příkaz whoami. Když vypíše správce, na denní práci jste na špatném účtu.

# Kvíz

## Jaký má být cvičný účet zaci?
- Správce, ať jde všechno
- [x] Běžný uživatel
- Účet bez hesla
- Účet hosta sdílený celou třídou bez hesla
> Denní práce běží jako běžný uživatel. Správce je na instalaci a správu, ne na Word.

## Jak se heslo předá příkazu net user?
- Napíše se za příkaz, ať je v sešitě vidět
- [x] Hvězdičkou, příkaz se zeptá a heslo neukáže
- Do názvu účtu
- Do souboru na plochu
> net user zaci * /add se zeptá. Heslo v příkazu zůstane v historii.

## Proč se denně nepracuje jako správce?
- Word jinak nejde otevřít
- [x] Program, který spustíte, dostane právo měnit systém
- Správce nemá Dokumenty
- Excel jinak nepočítá
> Škodlivý program dědí vaše práva. U správce smí instalovat a měnit systém.

## Kam patří osobní úkol?
- Do C:\Users\Public
- [x] Do Dokumentů vlastního účtu
- Do složky Windows
- Na účet spolužáka
> Public vidí každý účet na počítači. Úkol patří do vlastního profilu.
