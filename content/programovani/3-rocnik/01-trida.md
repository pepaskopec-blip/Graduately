---
id: cs-3-trida
nazev: Třída a objekt
popis: Vlastní typ a jeho konkrétní kus.
---

# Výklad

## Třída
*návod*

- class Zak popisuje, co žák má: jméno a věk.
- new Zak() vytvoří objekt, jeden konkrétní kus.
- Dva new jsou dva objekty. Změna jednoho druhý nemění.

> Tip: Třída sama o sobě není žák. Je to popis.

## Pole objektu
*tečka*

- z.Jmeno se čte a zapisuje přes tečku.
- Na začátek dáme veřejná pole, ať je vidět, komu patří.
- Později je schováme za vlastnost.

> Tip: Vytvořte druhého žáka a vypište obě jména.

# Kód

```
Zak z = new Zak();
z.Jmeno = "Anna";
z.Vek = 17;
Console.WriteLine($"{z.Jmeno}, {z.Vek}");

class Zak
{
    public string Jmeno;
    public int Vek;
}
```
