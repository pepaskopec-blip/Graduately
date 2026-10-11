---
id: cs-4-slovnik
nazev: Slovník
popis: Hodnota pod klíčem.
---

# Výklad

## Klíč
*jednou*

- Dictionary<string, int> má klíč text a hodnotu číslo.
- znamky["Anna"] = 1 uloží nebo přepíše.
- Stejný klíč je v slovníku jen jednou.

> Tip: Pole je podle čísla. Slovník je podle jména, které si zvolíte.

## Čtení
*ContainsKey*

- Čtení chybějícího klíče spadne.
- ContainsKey napřed zjistí, jestli klíč je.
- foreach projde dvojice Key a Value.

> Tip: Zeptejte se na jméno, které v slovníku není, až po ContainsKey.

# Kód

```
Dictionary<string, int> znamky = new Dictionary<string, int>();
znamky["Anna"] = 1;
znamky["Eva"] = 2;
znamky["Anna"] = 3;
if (znamky.ContainsKey("Eva"))
{
    Console.WriteLine(znamky["Eva"]);
}
Console.WriteLine(znamky["Anna"]);
```
