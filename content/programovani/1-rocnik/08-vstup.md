---
id: cs-1-vstup
nazev: Vstup z konzole
popis: Console.ReadLine přečte řádek.
---

# Výklad

## ReadLine
*text*

- ReadLine počká na řádek a vrátí ho jako string.
- V této lekci řádek napište do pole Vstup a pak Spustit.
- Každé další ReadLine vezme další řádek vstupu.

> Tip: Když vstup necháte prázdný, ReadLine dostane konec a program může spadnout.

## Číslo ze vstupu
*Parse*

- ReadLine nikdy nevrací int. Vrací text.
- int.Parse z něj číslo udělá.
- Napište do vstupu třeba 17.

> Tip: Zkuste ve vstupu Anna a uvidíte chybu. Parse čeká číslo.

# Kód

```
Console.WriteLine("Kolik ti je?");
string radek = Console.ReadLine();
int vek = int.Parse(radek);
Console.WriteLine($"Za rok ti bude {vek + 1}.");
```
