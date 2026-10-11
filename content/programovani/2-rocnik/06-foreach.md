---
id: cs-2-foreach
nazev: Foreach
popis: Projít každý prvek bez indexu.
---

# Výklad

## Projítí
*foreach*

- foreach (int z in znamky) vezme prvky od prvního po poslední.
- z je kopie hodnoty. U int se zápisem do z pole nezmění.
- Když index nepotřebujete, foreach je kratší než for.

> Tip: Součet ve smyčce přičítá aktuální z.

## Texty
*string[]*

- Pole může být i ze stringů.
- Stejný foreach funguje pro oba typy, jen se liší typ prvku.
- Pořadí je pořadí v závorce při vytvoření.

> Tip: Přidejte do pole třetí jméno.

# Kód

```
string[] jmena = { "Anna", "Eva", "Jan" };
foreach (string jmeno in jmena)
{
    Console.WriteLine(jmeno);
}
```
