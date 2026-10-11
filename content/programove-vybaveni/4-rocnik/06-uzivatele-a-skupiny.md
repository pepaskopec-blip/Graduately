---
id: sw-4-uzivatele
nazev: Uživatelé a skupiny ve Windows
popis: Účet zaci je ve Users. Ve skupině Administrators není.
nazev_cviceni: Kvíz: skupiny
nadpis_kvizu: Zařaďte účet do skupiny
---

# Výklad

## Skupina
*Práva najednou*

- Skupina sdružuje účty, které mají mít stejná práva. Neměníte právo každému účtu zvlášť po jednom souboru.
- Users jsou běžní uživatelé. Administrators smí měnit systém a instalovat.
- Účet zaci patří do Users. Do Administrators ne.
- Nový účet je často v Users sám. Práce navíc je nestrkat ho mezi správce, ne dopisovat Users.

> Tip: Kdo je správce, vypíše net localgroup Administrators. Jméno zaci v tom seznamu být nemá.

## Příkazy
*Bez hesla v řádku*

- net user zaci * /add účet založí a na heslo se zeptá. Hvězdička znamená dotaz, ne heslo zapsané do bat.
- net user zaci vypíše účet. Je tam vidět, jestli je aktivní.
- net localgroup Users zaci /add ho přidá do Users, kdyby tam nebyl.
- net localgroup Administrators zaci /delete ho ze správců odebere, pokud tam omylem je. Příkaz chce spustit správce.

> Tip: lusrmgr.msc otevře stejné účty v okně, ale jen ve Windows Pro, Education a Enterprise. V edici Home toto okno není a zůstává net user.

## Kdo příkaz spouští
*Správce*

- Běžný uživatel účet nezaloží. Okno se musí spustit jako správce, nebo systém požádá o souhlas.
- Souhlas správce není důvod založit účet rovnou jako správce. zaci zůstává běžný.
- whoami na účtu zaci nesmí skončit na Administrators.
- Cvičný účet se po hodině dá odebrat: net user zaci /delete. Nejdřív si ověřte, že nemaže účet učitele.

> Tip: Jméno účtu zaci je cvičné. Účet, který se jmenuje jako učitel nebo správce, se nedeaktivuje a nemaže.

## Zadání: zápis, ne cizí účty
*Cvičení*

- Do ucty.txt napište příkaz, kterým se zaci založí, včetně hvězdičky místo hesla.
- Napište, v které skupině má být a ve které být nemá.
- Pokud učitel dovolí, účet opravdu založte a net localgroup Administrators opište. zaci v seznamu není.
- Heslo do ucty.txt nepište. Cizí účty na počítači neměňte.

> Tip: Když lusrmgr.msc hlásí, že neexistuje, máte edici Home. Použijte net user a do zápisu napište, že grafické okno v této edici není.

# Kvíz

## Do které skupiny patří zaci?
- Administrators
- [x] Users
- Do žádné, účet bez skupiny smí instalovat
- Do skupiny hesel
> Běžná práce je skupina Users. Správci jsou Administrators.

## Jak se účet založí, aniž by heslo zůstalo v příkazu?
- [x] net user zaci * /add
- net user zaci heslo123 /add
- mkdir zaci
- DELETE FROM zaci
> Hvězdička se zeptá. Heslo napsané v příkazu zůstane v historii.

## Kde lusrmgr.msc není?
- Ve Windows Pro
- [x] Ve Windows Home
- Ve Windows Education
- Ve Windows Enterprise
> Grafická správa účtů je v Pro, Education a Enterprise. Home ji nemá.

## Co whoami nesmí ukázat při denní práci na účtu zaci?
- Že jde o běžného uživatele
- [x] Že účet je ve skupině Administrators
- Název počítače
- To, že účet existuje
> Denní účet není správce. whoami to prozradí dřív, než něco nainstalujete.
