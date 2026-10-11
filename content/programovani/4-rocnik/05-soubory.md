---
id: cs-4-soubory
nazev: Soubory
popis: Zápis a čtení textu.
---

# Výklad

## WriteAllText
*jednou*

- File.WriteAllText zapíše celý soubor. Když existuje, přepíše ho.
- Cesta pozdrav.txt je vedle programu, v jeho složce.
- ReadAllText celý text vrátí jako string.

> Tip: Na telefonu se soubor nespustí. Na počítači s .NET ano.

## Řádky
*Append*

- AppendAllText přidá na konec a soubor nesmaže.
- ReadAllLines vrátí pole řádků.
- Po skončení programu soubor ve složce laboratoře zůstane.

> Tip: Spusťte dvakrát. Druhý zápis WriteAllText starý text nahradí.

# Kód

```
File.WriteAllText("pozdrav.txt", "Ahoj ze souboru");
string zpet = File.ReadAllText("pozdrav.txt");
Console.WriteLine(zpet);
```
