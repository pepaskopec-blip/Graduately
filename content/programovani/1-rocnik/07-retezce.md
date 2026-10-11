---
id: cs-1-retezce
nazev: Řetězce
popis: Skládání textu, délka a výřez.
---

# Výklad

## Interpolace
*$*

- $"Ahoj, {jmeno}" dosadí hodnotu do složených závorek.
- Je přehlednější než spojování plusem.
- Do závorek může i výpočet: {vek + 1}.

> Tip: Bez dolaru před uvozovkami se {jmeno} vypíše doslova.

## Metody textu
*Length*

- Length je počet znaků.
- ToUpper vrátí velká písmena a původní text nezmění.
- Substring(0, 2) vezme dva znaky od začátku.

> Tip: První znak má index 0, ne 1.

# Kód

```
string jmeno = "Anna";
int vek = 17;
Console.WriteLine($"Ahoj, {jmeno}. Za rok ti bude {vek + 1}.");
Console.WriteLine(jmeno.Length);
Console.WriteLine(jmeno.ToUpper());
Console.WriteLine(jmeno.Substring(0, 2));
```
