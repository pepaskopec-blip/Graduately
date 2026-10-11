---
id: cs-2-while
nazev: While
popis: Opakování, dokud podmínka platí.
---

# Výklad

## Smyčka
*while*

- Tělo se opakuje, dokud je podmínka true.
- Když podmínku nikdy nezměníte, smyčka neskončí.
- Aplikace dlouhý běh po chvíli zastaví.

> Tip: i = i + 1 nebo i++ posune čítač. Bez toho se i nemění.

## Součet
*1 až 4*

- Začněte součtem 0 a přičítejte i.
- 1+2+3+4 je 10.
- Podmínka i <= 4 pustí i číslo 4 dovnitř.

> Tip: Změňte 4 na 10. Součet 1 až 10 je 55.

# Kód

```
int i = 1;
int soucet = 0;
while (i <= 4)
{
    soucet = soucet + i;
    i++;
}
Console.WriteLine(soucet);
```
