---
id: cs-4-generika
nazev: Generika
popis: Jedna metoda pro víc typů.
---

# Výklad

## T
*typový parametr*

- static void Vypis<T>(T hodnota) přijme int i string.
- T se doplní při volání podle argumentu.
- Jedna metoda tak nenutí kopírovat kód pro každý typ.

> Tip: Když metoda s T má umět jen porovnávat, T se omezí. Tady stačí výpis.

## List
*už ho znáte*

- List<string> je generická třída. string je dosazený T.
- Stejný List<int> drží čísla a metody jsou stejné.
- Generika hlídá typ. Do List<int> string nedáte.

> Tip: Zavolejte Vypis s číslem i s textem.

# Kód

```
Vypis(7);
Vypis("ahoj");

static void Vypis<T>(T hodnota)
{
    Console.WriteLine(hodnota);
}
```
