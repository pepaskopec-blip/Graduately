---
id: sw-2-excel-funkce
nazev: Excel: funkce
popis: SUMA, KDYŽ a SVYHLEDAT. V české verzi se argumenty oddělují středníkem.
nazev_cviceni: Kvíz: funkce
nadpis_kvizu: Vyberte správný zápis
---

# Výklad

## Souhrny
*SUMA a spol.*

- SUMA(D2:D6) sečte částky. Průměr je PRŮMĚR, nejmenší MIN, největší MAX.
- POČET spočítá čísla. Prázdnou buňku nepočítá. Text nepočítá.
- Rozsah D2:D6 zahrnuje oba konce. D2:D6 jsou řádky 2 až 6, ne jen 2 a 6.
- Souhrn patří pod tabulku, nebo na list Součet. Nepočítá se na papír a neopisuje se.

> Tip: Anglický Excel má SUM a čárku. Český má SUMA a středník. Ve škole pište česky, jak to má školní Office.

## KDYŽ
*Známky*

- České známky: 1 je nejlepší, 5 je neprospěl. Prospěch je známka 1 až 4.
- =KDYŽ(D2<=4;"prospěl";"neprospěl") vrací text podle známky v D2.
- Středníky oddělují podmínku, větev ano a větev ne. Čárka v české verzi vzorec rozbije.
- Podmínka, která čeká známku aspoň 4 jako úspěch, je obráceně. Čtyřka ještě prochází, pětka ne.

> Tip: Uvozovky kolem textu ve funkci jsou povinné. Bez nich Excel hledá buňku nebo název.

## SVYHLEDAT
*Ceník*

- SVYHLEDAT najde hodnotu v levém sloupci oblasti a vrátí jiný sloupec stejného řádku.
- =SVYHLEDAT(A2;Ceník!$A$2:$C$20;3;NEPRAVDA) hledá A2, oblast je ukotvená, vrátí 3. sloupec, shoda je přesná.
- NEPRAVDA znamená přesnou shodu. PRAVDA by směla vzít nejbližší nižší a u textu plete.
- Hledaná hodnota musí být v prvním sloupci oblasti. Jinak funkce nehledá uprostřed tabulky.

> Tip: Když funkce vrátí #N/A, hodnota v prvním sloupci není. Zkontrolujte mezeru za jménem a stejný typ, text proti číslu.

## Zadání: známky a ceník
*Cvičení*

- Na list Znamky dejte pět žáků, sloupec Znamka a sloupec Stav se vzorcem KDYŽ, prospěl při známce 1 až 4.
- Pod známky SUMA nedávejte, součet známek nic neříká. Dejte PRŮMĚR.
- Na listu Ceník doplňte SVYHLEDAT, který podle kódu položky vrátí cenu. Oblast ukotvěte a dejte NEPRAVDA.
- Uložte znamky.xlsx do složky excel. Oba listy ať jsou v jednom sešitu.

> Tip: Průměr známek 1,0 až 5,0 vychází jako číslo. Když vyjde 0, počítáte prázdné buňky, nebo máte text místo čísel.

# Kvíz

## Jak se v českém Excelu zapíše součet D2 až D6?
- =SUM(D2:D6)
- [x] =SUMA(D2:D6)
- =SUMA(D2,D6)
- SUMA(D2:D6) bez rovnítka
> Česká funkce je SUMA a rozsah je D2:D6. SUM je anglická verze.

## Který vzorec označí známku 5 jako neprospěl?
- [x] =KDYŽ(D2<=4;"prospěl";"neprospěl")
- =KDYŽ(D2>=4;"prospěl";"neprospěl")
- =KDYŽ(D2<=4,"prospěl","neprospěl")
- =D2<=4
> Prochází 1 až 4, tedy menší nebo rovno 4. V češtině jsou středníky, ne čárky.

## Co znamená NEPRAVDA na konci SVYHLEDAT?
- Vypne vzorec
- [x] Chce přesnou shodu
- Sečte sloupec
- Vrátí vždy první řádek
> NEPRAVDA je přesně ta hodnota. PRAVDA bere přibližnou shodu a u kódů položek lže.

## Proč je oblast Ceník!$A$2:$C$20 ukotvená?
- [x] Aby se při kopírování vzorce dolů neposunula
- Protože jinak nejde uložit xlsx
- Dolar sčítá ceny
- SVYHLEDAT ukotvení zakazuje
> Každý řádek hledá v téže tabulce. Bez dolarů by se oblast sunula dolů s vzorcem.
