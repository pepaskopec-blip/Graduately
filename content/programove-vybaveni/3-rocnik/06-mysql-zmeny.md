---
id: sw-3-mysql-zmeny
nazev: MySQL: INSERT, UPDATE a DELETE
popis: Nový řádek, oprava a smazání. UPDATE a DELETE bez WHERE se ve škole nepíšou.
nazev_cviceni: Kvíz: změny dat
nadpis_kvizu: Poznejte bezpečný příkaz
---

# Výklad

## INSERT
*Nový žák*

- INSERT INTO zaci (jmeno, prijmeni, trida_id) VALUES ('Anna', 'Nováková', 1); přidá jeden řádek.
- Sloupce v závorce a hodnoty za VALUES musí být ve stejném pořadí a stejném počtu.
- id s automatickým číslem do výčtu nedávejte, server ho doplní.
- Jeden INSERT, jeden žák. Balit pět žáků do nepřehledného příkazu ve cvičení nemusíte.

> Tip: Když server hlásí neznámý sloupec, název nesedí s tabulkou. SHOW COLUMNS FROM zaci; ukáže pravdu.

## UPDATE
*Jen jeden řádek*

- UPDATE zaci SET trida_id = 2 WHERE id = 15; přepíše třídu jen žáku 15.
- Bez WHERE by stejný příkaz přepsat třídu všem. Ve škole se neodevzdává.
- Nejdřív SELECT s stejnou podmínkou WHERE. Až když vidíte jeden správný řádek, pusťte UPDATE.
- SET mění hodnoty. Není to SELECT a nic nevrací kromě počtu ovlivněných řádků.

> Tip: Klient ukáže, kolik řádků se změnilo. Když vidíte desítky a čekali jste jeden, zastavte se a volejte učitele. Záloha se hodí předtím, ne potom.

## DELETE
*Také s WHERE*

- DELETE FROM zaci WHERE id = 15; smaže jeden řádek.
- DELETE FROM zaci; bez WHERE smaže všechny žáky. To není čištění tabulky, to je ztráta dat.
- Smazaný řádek Ctrl+Z v MySQL nevrátí. Vrátí ho záloha, nebo ho vložíte znovu, když znáte hodnoty.
- Mazat třídu, na kterou ještě ukazují žáci, referenční pravidlo nemá dovolit. Nejdřív žáci, nebo třídu nemazat.

> Tip: Před DELETE udělejte SELECT se stejnou podmínkou. Počet řádků musí být ten, který čekáte, obvykle jeden.

## Zadání: tři změny na kopii
*Cvičení*

- Na cvičných datech vložte žáka Petr Svoboda do třídy 1, tedy trida_id 1.
- UPDATE mu změňte třídu na 2, ale jen přes jeho id, které jste předtím přečetli SELECTem.
- Téhož žáka DELETE smažte zase jen podle id. Ostatní žáci v tabulce zůstanou.
- Tři příkazy připojte do dotazy.sql pod komentář, který říká, že UPDATE a DELETE mají WHERE.

> Tip: Komentář v SQL začíná dvěma pomlčkami. Do příkazu se nepočítá, příštímu čtenáři řekne, proč tam WHERE je.

# Kvíz

## Který příkaz přidá Annu do třídy 1?
- [x] INSERT INTO zaci (jmeno, prijmeni, trida_id) VALUES ('Anna', 'Nováková', 1);
- SELECT Anna INTO zaci;
- UPDATE zaci SET Anna;
- DELETE FROM zaci WHERE jmeno = 'Anna';
> INSERT vkládá řádek. Sloupce a hodnoty drží pořadí.

## Jak se změní třída jen žáku s id 15?
- UPDATE zaci SET trida_id = 2;
- [x] UPDATE zaci SET trida_id = 2 WHERE id = 15;
- SELECT trida_id = 2;
- INSERT INTO zaci VALUES (15);
> Bez WHERE by se třída přepsala všem. WHERE id = 15 drží jeden řádek.

## Co udělá DELETE FROM zaci; bez WHERE?
- Nesmaže nic, WHERE je volitelné a server se zeptá
- [x] Smaže všechny řádky v zaci
- Smaže jen poslední řádek
- Jen vypíše žáky
> Bez podmínky je cílem celá tabulka. Ve školním cvičení takový příkaz nepište.

## Co uděláte těsně před UPDATE?
- DELETE bez WHERE
- [x] SELECT se stejnou podmínkou, ať vidíte ty řádky
- DROP tabulky
- Vypnete integritu
> SELECT ukáže, na koho UPDATE dopadne. Až pak má smysl stejné WHERE použít ke změně.
