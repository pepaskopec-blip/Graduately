---
id: cs-2-metody
nazev: Metody
popis: Pojmenovaný kus programu, který lze zavolat.
---

# Výklad

## Volání
*jméno()*

- static void Pozdrav() je metoda bez výsledku.
- Zavolá se Pozdrav(); včetně závorek.
- Bez závorek by to bylo jen jméno, ne volání.

> Tip: Metodu lze zavolat víckrát a pokaždé udělá totéž.

## Proč
*jednou*

- Kód, který se opakuje, patří do metody.
- Jméno metody říká, co dělá: Pozdrav, ne Metoda1.
- void znamená, že nevrací hodnotu.

> Tip: Přidejte druhé volání Pozdrav().

# Kód

```
Pozdrav();
Pozdrav();

static void Pozdrav()
{
    Console.WriteLine("Ahoj");
}
```
