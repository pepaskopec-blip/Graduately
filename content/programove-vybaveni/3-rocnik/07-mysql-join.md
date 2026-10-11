---
id: sw-3-mysql-join
nazev: MySQL: spojení tabulek
popis: INNER JOIN přes trida_id a proč se název třídy neopakuje v každém řádku.
nazev_cviceni: Kvíz: JOIN
nadpis_kvizu: Doplňte spojení
---

# Výklad

## Jedna pravda
*Název jen jednou*

- Název 1.A je v tabulce tridy jednou. V zaci je jen trida_id.
- Když by název ležel i u žáka, oprava překlepu znamená projít všechny žáky.
- To je důvod dvou tabulek. Excel to umí jen ručně, databáze vztahem.
- JOIN tabulky spojí ve výsledku dotazu. Nesloučí je do jedné natrvalo.

> Tip: Po JOIN pořád existují dvě tabulky. Výsledek dotazu je jen pohled pro tento příkaz.

## INNER JOIN
*Shoda klíčů*

- INNER JOIN vrátí řádky, kde se klíče sejdou. Žák a jeho třída.
- ON z.trida_id = t.id je podmínka spojení. Není to totéž co WHERE na název.
- Bez ON server buď příkaz odmítne, nebo spojí každý řádek s každým.
- Přezdívky z a t jsou v dotazu proto, aby bylo jasné, která tabulka je která.

> Tip: WHERE t.nazev = '1.A' přijde až po spojení. Nejdřív se tabulky potkají, pak se filtruje.

## Celý příkaz
*Jak má vypadat*

- SELECT z.prijmeni, z.jmeno, t.nazev FROM zaci AS z INNER JOIN tridy AS t ON z.trida_id = t.id;
- WHERE t.nazev = '1.A' z toho udělá jen jednu třídu.
- ORDER BY z.prijmeni výsledek seřadí.
- Sloupce pište s přezdívkou tabulky. Samotné id by bylo nejasné, mají ho obě tabulky.

> Tip: Když příkaz hlásí nejednoznačný sloupec id, napište z.id nebo t.id. Server nehádá.

## Zadání: jeden JOIN
*Cvičení*

- Do dotazy.sql přidejte dotaz, který vypíše příjmení, jméno a název třídy.
- Jen třída 1.A, seřazené podle příjmení.
- Anna Nováková v něm je včetně textu 1.A, ne jen čísla 1.
- Příkaz spusťte dvakrát. Výsledek má být stejný, JOIN data nemění.

> Tip: Když vidíte číslo místo 1.A, vybíráte trida_id a ne t.nazev. Doplňte sloupec z tabulky tridy.

# Kvíz

## Které spojení ukáže žáka i název jeho třídy?
- SELECT * FROM zaci;
- [x] SELECT z.prijmeni, t.nazev FROM zaci AS z INNER JOIN tridy AS t ON z.trida_id = t.id;
- DELETE FROM tridy;
- INSERT INTO tridy VALUES ('1.A');
> JOIN vezme příjmení z žáků a název z tříd tam, kde se id shoduje.

## Co patří do ON?
- [x] z.trida_id = t.id
- t.nazev = '1.A'
- ORDER BY z.prijmeni
- USE skola
> ON je shoda klíčů. Filtr na název 1.A je WHERE, ne spojení.

## Proč není název třídy uložený u každého žáka?
- MySQL to neumí
- [x] Aby se název měnil na jednom místě
- Protože text v MySQL nejde uložit
- Aby nešel JOIN
> Jedna pravda v tabulce tridy. Žáci drží jen číslo.

## Co JOIN udělá s uloženými tabulkami?
- Sloučí je natrvalo do jedné
- [x] Jen složí výsledek dotazu, tabulky zůstanou dvě
- Smaže cizí klíče
- Převede je na Excel
> Výsledek je odpověď na příkaz. tridy a zaci na serveru zůstávají.
