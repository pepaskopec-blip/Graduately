---
id: cs-4-linq
nazev: LINQ
popis: Dotazy nad seznamem.
---

# Výklad

## Where
*filtr*

- Where nechá jen prvky, které splní podmínku.
- n => n >= 10 je krátká funkce: vezmi n a vrať, jestli je aspoň 10.
- Samotné Where ještě nic neprojde. ToList nebo foreach výsledek vezme.

> Tip: Šipka => není porovnání. Je to zápis malé funkce.

## Select
*převod*

- Select z každého prvku udělá jiný.
- OrderBy seřadí.
- Dotaz lze řetězit: Where a pak Select.

> Tip: Změňte 10 na 12 a dvojka z výpisu zmizí.

# Kód

```
int[] cisla = { 4, 11, 7, 20 };
var velka = cisla.Where(n => n >= 10).OrderBy(n => n);
foreach (int n in velka)
{
    Console.WriteLine(n);
}
```
