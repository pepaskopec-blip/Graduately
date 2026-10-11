---
id: sw-4-batch
nazev: Dávkové soubory
popis: Soubor bat, který založí složky školy a nic nemaže.
nazev_cviceni: Kvíz: batch
nadpis_kvizu: Vyberte bezpečný příkaz
---

# Výklad

## Soubor bat
*Příkazy za sebou*

- Dávkový soubor je text s příponou bat. Příkazový řádek ho spustí řádek po řádku.
- První řádek cvičení je @echo off, ať se příkazy nevypisují podruhé do okna.
- rem je komentář. Do příkazu se nepočítá.
- Soubor se jmenuje slozky.bat a leží v Dokumentech, ne na cizím flash disku, kterému nerozumíte.

> Tip: Dvojklik spustí bat v té složce, kde soubor je. Neznámý bat z mailu se nespouští, stejně jako pdf.exe.

## Složky
*mkdir*

- mkdir založí složku. Když už existuje, příkaz ohlásí chybu a pokračuje se dál, složku nesmaže.
- set "ROOT=%USERPROFILE%\Documents\Skola" uloží cestu. %USERPROFILE% je složka účtu, u každého jiná.
- mkdir "%ROOT%\1-rocnik\word" založí i mezistupně, když je napíšete takto v nových Windows. Když ne, založte nejdřív 1-rocnik a pak word.
- Uvozovky kolem cesty jsou povinné v okamžiku, kdy v cestě je mezera. V Documents mezera není, v názvu účtu být může.

> Tip: echo Hotovo vypíše větu. pause nechá okno otevřené, ať výsledek přečtete. Bez pause dvojklik okno hned zavře.

## Co se nepouští
*del*

- del smaže soubor. Ve školním bat na založení složek žádný del není.
- Příkaz, který maže podle masky celý disk, se nepíše ani z legrace. Obnova není Ctrl+Z.
- cd /d přejde na jiný disk a složku. Než něco mažete, echo vypíše, kde jste. V tomto cvičení nemažete.
- Bat, kterému nerozumíte řádek po řádku, nespouštějte. Krátký soubor se čte celý.

> Tip: Když chcete zkusit del, zkuste ho na jeden soubor test.txt, který jste sami právě vytvořili. Ne na složku Windows.

## Zadání: slozky.bat
*Cvičení*

- slozky.bat nastaví ROOT na %USERPROFILE%\Documents\Skola a založí 1-rocnik\word, 1-rocnik\excel a 1-rocnik\system.
- Na konci je echo s cestou a pause. Není tam del.
- Spusťte ho jednou. Složky v Průzkumníku opravdu jsou.
- Spusťte ho podruhé. Smí ohlásit, že složka už existuje. Nemá nic smazat.

> Tip: Úprava bat je v Poznámkovém bloku, ne ve Wordu. Word do něj doplní formátování a příkazový řádek pak řádek nepochopí.

# Kvíz

## Co má být první řádek školního bat?
- del C:\
- [x] @echo off
- formát disku
- heslo účtu
> @echo off jen ztiší opisování příkazů. Nic nemaže.

## Co dělá mkdir?
- Smaže složku
- [x] Založí složku
- Otevře Word
- Změní heslo
> mkdir je make directory. Mazání je jiný příkaz a v tomto cvičení není.

## Kam ukazuje %USERPROFILE%?
- Na stejnou složku na všech účtech počítače
- [x] Na profil přihlášeného účtu
- Na web WordPressu
- Na tabulku tridy
> Každý účet má vlastní profil. Stejný bat tedy založí Skola v Dokumentech toho, kdo ho spustil.

## Který bat ve cvičení nespouštíte?
- Ten, který jen zakládá složky a na konci čeká
- [x] Neznámý soubor z mailu
- slozky.bat, který jste sami přečetli
- Soubor, který ohlásí, že složka už existuje
> Dávka je program. Z mailu se nespouští, stejně jako soubor s příponou exe.
