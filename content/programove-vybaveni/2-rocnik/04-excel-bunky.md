---
id: sw-2-excel-bunky
nazev: Excel: buňky a vzorce
popis: Adresa A1, vzorec se rovnítkem a ukotvení dolarama.
nazev_cviceni: Kvíz: buňky
nadpis_kvizu: Doplňte vzorec
---

# Výklad

## Buňka
*Adresa*

- Sloupce jsou písmena, řádky čísla. B2 je druhý sloupec a druhý řádek.
- Do buňky patří jedna hodnota. Jméno a třída do jedné buňky nepatří.
- Číslo je číslo, text je text. Jeden apostróf před číslem z něj udělá text a přestane se sčítat.
- Sešit je soubor xlsx. List je záložka dole. Školní sešit má listy Ceník a Součet.

> Tip: Klik na záhlaví sloupce označí celý sloupec. Na číslo řádku celý řádek.

## Vzorec
*Rovnítko*

- Vzorec začíná =. Bez rovnítka je  B2*C2 jen text a nic nespočítá.
- V ceníku je v D2 vzorec =B2*C2. B je počet, C je cena, D je částka.
- Český Excel odděluje argumenty funkcí středníkem. Krát je hvězdička, děleno lomítko.
- Vzorec se kopíruje za úchyt v pravém dolním rohu buňky. Neopisuje se do pěti buněk ručně.

> Tip: Když buňka ukáže vzorec místo výsledku, je sloupec úzký, nebo je buňka text. Zkontrolujte rovnítko.

## Posun a dolar
*A1 a $A$1*

- Zkopírování =B2*C2 o řádek níž udělá =B3*C3. Adresy se posunou.
- =$B$2 se neposune ani ve sloupci, ani v řádku. Dolar kotví.
- Sazba DPH v buňce F1 se ve vzorcích bere jako $F$1, ať kopírování neodjede na F2.
- Smazání hodnoty, na kterou vzorec ukazuje, udělá chybu. Vzorec na prázdnou buňku není nula sám od sebe, když čekal číslo a dostal text.

> Tip: Dolar jen u písmene, $B2, kotví sloupec. Dolar jen u čísla, B$2, kotví řádek.

## Zadání: ceník
*Cvičení*

- List Ceník: sloupce Polozka, Pocet, Cena, Castka. Záhlaví v řádku 1, pět položek v řádcích 2 až 6.
- Castka v D2 je =B2*C2 a dolů se táhne. Žádná částka není napsaná ručně.
- Sazba v F1 je 0,21 a v E2 je =D2*$F$1. Po zkopírování dolů pořád ukazuje na F1.
- Uložte cenik.xlsx do Skola\1-rocnik\excel.

> Tip: Klik na D2 ukáže vzorec v řádku vzorců. Když tam je jen číslo, částka je opsaná a při změně počtu se nehne.

# Kvíz

## Čím začíná vzorec?
- Závorkou
- [x] Rovnítkem
- Středníkem
- Dolarama bez rovnítka
> Bez = Excel nic nepočítá. Text B2*C2 zůstane text.

## Co je v D2, když počet je v B2 a cena v C2?
- [x] =B2*C2
- B2*C2 bez rovnítka
- =B2+C2
- Ručně spočítané číslo
> Částka je součin. Sčítání by sečetlo počet s cenou, což není částka.

## Proč je sazba ve vzorci $F$1?
- [x] Aby se při kopírování dolů adresa neposunula
- Protože dolar je měna tabulky
- Aby vzorec přestal počítat
- $ se v Excelu nesmí
> F1 je jedna buňka pro všechny řádky. Bez dolarů by druhý řádek četl F2.

## Co udělá zkopírování =B2*C2 o řádek níž?
- Nechá B2 a C2
- [x] Vznikne =B3*C3
- Smaže list
- Změní se na text
> Relativní adresa se posune o stejný počet řádků jako kopírování.
