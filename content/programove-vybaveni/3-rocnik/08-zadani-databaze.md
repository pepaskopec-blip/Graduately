---
id: sw-3-zadani-databaze
nazev: Zadání: evidence žáků
popis: Třídy, žáci, vztah a jeden dotaz, který ukáže 1.A jménem.
nazev_cviceni: Kvíz: evidence
nadpis_kvizu: Projděte klíče i dotaz
---

# Výklad

## Struktura
*Dvě tabulky*

- tridy: id jako klíč, nazev. Řádky 1.A a 1.B.
- zaci: id jako klíč, jmeno, prijmeni, trida_id jako cizí klíč.
- Anna Nováková je ve třídě 1.A. Ve třídě 1.B je aspoň jeden jiný žák.
- Název 1.A není opsaný v tabulce žáků. Je jen v tridy.

> Tip: Ať děláte Access, nebo MySQL, tahle dvě pole a vztah jsou povinné. Nástroj je obal.

## Access
*Když odevzdáváte accdb*

- evidence.accdb má vztah 1:N, referenční integritu a vypnutou kaskádu.
- Dotaz zaci_1A a formulář frmZaci. Sestava je seskupená podle třídy.
- Žák s neexistující třídou nejde uložit. Vyzkoušeno na trida_id 99.
- PDF sestavy leží vedle accdb.

> Tip: Když učitel otevře jen accdb, formulář a dotaz v něm musí být. Samotné PDF bez souboru nestačí.

## MySQL
*Když odevzdáváte SQL*

- dotazy.sql obsahuje SELECT se WHERE, INSERT, UPDATE i DELETE, oba měnící příkazy s WHERE.
- JOIN vypíše příjmení a název 1.A, seřazené podle příjmení.
- V souboru není heslo k serveru a není DELETE bez WHERE.
- Příkazy jdou spustit znovu jen tam, kde to dává smysl. DELETE testovacího žáka v souboru má konkrétní id a komentář.

> Tip: Odevzdává se text příkazů, ne snímek obrazovky klienta. Snímek smí být navíc.

## Jedno odevzdání
*Ne obě poloviční*

- Stačí jeden nástroj, Access nebo MySQL, ale celý: struktura, data, dotaz na 1.A.
- V dotazu je Anna vidět jako 1.A, ne jako holé číslo, pokud je součástí této části zadání JOIN nebo dotaz s názvem.
- Záloha souboru má datum v názvu, třeba evidence-2026-06-12.accdb.
- Do textu úkolu nepište heslo a nepište, že jste kaskádu zapnuli, pokud jste ji zapnout neměli.

> Tip: Když umíte obojí, odevzdejte jedno pořádně. Dvě rozdělané databáze jsou horší než jedna hotová.

# Kvíz

## Kde leží text 1.A?
- V každém řádku žáka jako jediná pravda
- [x] V tabulce tridy, v poli nazev
- V názvu souboru jako jediném místě
- V hesle
> Žák má jen trida_id. Název je v tabulce tříd jednou.

## Jaký vztah mají třídy a žáci?
- [x] 1:N, jedna třída a víc žáků
- N:N bez třetí tabulky uprostřed školního cvičení
- Žádný, stačí Excel bez klíče
- 1:1, každý žák vlastní tabulku třídy
> Jedna třída, mnoho žáků. Na straně žáků je cizí klíč.

## Co musí mít UPDATE a DELETE v odevzdaném SQL?
- Žádnou podmínku, ať je příkaz krátký
- [x] WHERE, které trefí jen zamýšlené řádky
- Hvězdičku místo sloupců
- Kaskádu v příkazu SELECT
> Bez WHERE se mění nebo maže celá tabulka. Ve cvičení se to neodevzdává.

## Co Access odmítne, když je integrita zapnutá?
- Žáka se třídou 1.A
- [x] Žáka s trida_id 99, které v tridy není
- Formulář
- Sestavu podle příjmení
> Cizí klíč musí ukazovat na existující třídu. 99 v tabulce tříd není.
