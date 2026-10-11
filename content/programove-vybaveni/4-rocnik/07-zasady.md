---
id: sw-4-zasady
nazev: Místní zásady
popis: Minimální heslo 8 znaků a zámek po 300 sekundách. Doména školy místní zásadu přebije.
nazev_cviceni: Kvíz: zásady
nadpis_kvizu: Doplňte hodnotu a příkaz
---

# Výklad

## Kde se zásady berou
*gpedit*

- Místní zásady otevře gpedit.msc. Je ve Windows Pro, Education a Enterprise, v edici Home není.
- Konfigurace počítače platí pro stroj. Konfigurace uživatele platí pro účty, které se přihlásí.
- Školní cvičení mění dvě věci: délku hesla a zámek při nečinnosti. Ostatní zásady nechte být.
- Když je počítač v doméně školy, doménová zásada vyhraje nad místní. Místní změna se pak jakoby ztratí a není to chyba klikání.

> Tip: Než něco změníte, napište si původní hodnotu. Vrátit Neznámé číslo zpět nejde, když jste si ho nezapsali.

## Heslo
*Osm znaků*

- Cesta vede přes Konfigurace počítače, Nastavení systému Windows, Nastavení zabezpečení, Zásady účtů, Zásady hesel.
- Minimální délka hesla se nastaví na 8.
- Nula znamená, že heslo smí být prázdné. To školní účet nechce.
- Zásada neopraví heslo, které už je krátké, dokud ho uživatel příště nemění. Platí pro nová hesla.

> Tip: Heslo v zásadě nenastavujete. Nastavujete pravidlo. Samotné heslo účtu zaci se zadává při net user, ne tady.

## Zámek
*300 sekund*

- Nečinnost: Konfigurace počítače, Nastavení systému Windows, Nastavení zabezpečení, Místní zásady, Možnosti zabezpečení.
- Položka Interaktivní přihlášení: limit nečinnosti počítače se nastaví na 300 sekund, tedy 5 minut.
- Nula nebo nenastaveno znamená, že tato zásada zámek nevynucuje.
- Po pěti minutách bez myši a klávesnice se má počítač zamknout, ne vypnout a ne smazat Dokumenty.

> Tip: Zámek je Win+L hned. Zásada totéž udělá sama, když odejdete od stroje a zapomenete.

## Zadání: dvě hodnoty
*Cvičení*

- Pokud gpedit.msc máte, nastavte minimální délku hesla na 8 a limit nečinnosti na 300 sekund. Původní hodnoty si zapište.
- gpupdate /force zásady načte hned, bez čekání na restart. Příkaz chce okno spuštěné jako správce.
- Do zasady.txt napište obě hodnoty a jestli je počítač v doméně. Heslo nepište.
- V edici Home napište, že gpedit.msc v ní není, a hodnoty jen popište. Nevymýšlejte, že jste je nastavili.

> Tip: Když po gpupdate hodnota v gpedit zůstane a systém se nechová podle ní, počítač je v doméně a doména místní číslo přebíjí.

# Kvíz

## Jaká je školní minimální délka hesla?
- 0
- [x] 8
- 300
- Prázdné heslo
> Osm znaků je pravidlo z první hodiny o účtech. Nula by prázdné heslo dovolila.

## Na kolik sekund se nastaví limit nečinnosti?
- 5
- [x] 300
- 8
- Na nulu, ať se počítač nezamyká
> 300 sekund je pět minut. Číslo 5 by bylo pět sekund, to v této položce není.

## Čím se místní zásady načtou hned?
- Smazáním účtu zaci
- [x] gpupdate /force
- mkdir
- SELECT * FROM zaci
> gpupdate zásady znovu načte. Bez něj si počítač může počkat na své vlastní obnovení.

## Co platí, když je počítač v doméně školy a místní heslo nastavíte jinak?
- Vždy vyhraje místní gpedit
- [x] Vyhraje doménová zásada
- Hesla se vypnou
- WordPress přestane běžet
> Doména je nad místním počítačem. Místní změna, která jí odporuje, se neuplatní.
