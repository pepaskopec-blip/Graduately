---
id: cs-2-for
nazev: For
popis: Smyčka s čítačem na jednom místě.
---

# Výklad

## Tři části
*start; podmínka; krok*

- for (int i = 0; i < 3; i++) má začátek, podmínku a krok.
- i < 3 pustí 0, 1 a 2. Trojku už ne.
- Krok i++ se provede až po těle.

> Tip: Když potřebujete přesný počet opakování, for je přehlednější než while.

## Násobilka
*výpis*

- V těle můžete počítat i * 2.
- WriteLine v těle vypíše jeden řádek na každé opakování.
- Čítač nemusí začínat nulou.

> Tip: Přepište podmínku na i <= 5 a sledujte další řádek.

# Kód

```
for (int i = 1; i <= 3; i++)
{
    Console.WriteLine($"{i} krát 2 je {i * 2}");
}
```
