---
id: cs-1-operatory
nazev: Operátory
popis: Počítání, zbytek, porovnání a a zároveň.
---

# Výklad

## Počet
*+ - * / %*

- Sčítání, odčítání a násobení fungují, jak čekáte.
- Dělení dvou int je celočíselné: 17 / 5 je 3.
- % je zbytek po dělení: 17 % 5 je 2.

> Tip: Když chcete desetinný podíl, jedno číslo udělejte double: 17 / 5.0.

## Porovnání
*true a false*

- == je rovnost. = je uložení. Nejsou totéž.
- && znamená a zároveň. || znamená nebo.
- ! otočí bool: !true je false.

> Tip: Jedno = v podmínce je častá chyba. Podmínka chce ==.

# Kód

```
int a = 17;
int b = 5;
Console.WriteLine(a + b);
Console.WriteLine(a / b);
Console.WriteLine(a % b);
Console.WriteLine(a > b);
Console.WriteLine(a > 10 && b < 10);
```
