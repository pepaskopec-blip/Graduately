---
id: sw-3-mysql-select
nazev: MySQL: SELECT
popis: Výběr sloupců, podmínka WHERE a řazení. Text je v jednoduchých uvozovkách.
nazev_cviceni: Kvíz: SELECT
nadpis_kvizu: Vyberte správný příkaz
---

# Výklad

## Databáze skola
*Dvě tabulky*

- Na serveru je databáze skola. Tabulka tridy má id a nazev. Tabulka zaci má id, jmeno, prijmeni a trida_id.
- Příkazy se píšou v MySQL, ne do Excelu. Středník příkaz ukončí.
- Nejdřív USE skola; ať další příkazy nemíří do jiné databáze.
- Heslo k serveru do sešitu nepatří. Patří učiteli a do klienta, který ho neukazuje v příkazu.

> Tip: Když klient hlásí, že tabulka neexistuje, jste v jiné databázi, nebo je v názvu překlep. SHOW TABLES; ukáže, co tam je.

## SELECT
*Jen to, co potřebujete*

- SELECT prijmeni, jmeno FROM zaci; vrátí dva sloupce všech žáků.
- Hvězdička SELECT * FROM zaci; je na průzkum. Do odevzdaného dotazu vyberte sloupce jménem.
- WHERE trida_id = 1 nechá jen jednu třídu. Číslo je bez uvozovek.
- Text se píše v jednoduchých uvozovkách: WHERE nazev = '1.A'.

> Tip: Dvojité uvozovky kolem 1.A záleží na režimu serveru. Ve škole pište jednoduché, ty platí spolehlivě.

## Pořadí
*ORDER BY*

- ORDER BY prijmeni; seřadí vzestupně, tedy A až Ž.
- ORDER BY prijmeni DESC; je sestupně.
- WHERE patří před ORDER BY. Obrácené pořadí příkazů server odmítne.
- LIMIT 5 vrátí nejvýš pět řádků. Na kontrolu ano, na úplný seznam třídy ne.

> Tip: Nejdřív FROM, pak WHERE, pak ORDER BY. SELECT je na začátku, i když se sloupce vybírají jako první v hlavě.

## Zadání: tři dotazy
*Cvičení*

- Do souboru dotazy.sql napište USE skola; a tři SELECT.
- První: příjmení a jméno všech žáků podle příjmení.
- Druhý: jen žáci s trida_id = 1.
- Třetí: název třídy 1.A z tabulky tridy. Text '1.A' je v jednoduchých uvozovkách.

> Tip: Každý příkaz ukončete středníkem. Bez něj klient čeká na pokračování.

# Kvíz

## Který příkaz vrátí příjmení žáků třídy s id 1?
- [x] SELECT prijmeni FROM zaci WHERE trida_id = 1;
- SELECT prijmeni WHERE trida_id = 1;
- SELECT * FROM tridy WHERE nazev = 1.A;
- WHERE trida_id = 1;
> SELECT říká sloupce, FROM tabulku, WHERE podmínku. Číslo je bez uvozovek.

## Jak se v podmínce zapíše název třídy?
- nazev = 1.A
- [x] nazev = '1.A'
- nazev = "bez uvozovek a bez čísla"
- nazev := 1.A
> Text patří do jednoduchých uvozovek. Bez nich to není řetězec.

## Kam patří ORDER BY?
- Před SELECT
- [x] Za WHERE
- Místo FROM
- Do názvu tabulky
> Pořadí je WHERE, potom ORDER BY. SELECT zůstává první.

## K čemu je ve škole SELECT * ?
- Na odevzdaný tisk všech osobních údajů
- [x] Na rychlý pohled, co v tabulce je
- Místo primárního klíče
- Na smazání řádků
> Hvězdička je průzkum. Odevzdaný dotaz jmenuje sloupce, které mají být vidět.
