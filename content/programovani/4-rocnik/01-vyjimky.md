---
id: cs-4-vyjimky
nazev: Výjimky
popis: Chyba, kterou lze chytit a vysvětlit.
---

# Výklad

## try
*pokus*

- try je kus, který může spadnout.
- catch (Exception e) ho chytí, místo aby spadl celý program.
- e.Message je text chyby.

> Tip: Bez try by Parse na textu abc program ukončil.

## finally
*vždy*

- finally se spustí, ať try prošel, nebo ne.
- Hodí se na úklid. V příkladu jen vypíše, že končíme.
- catch může být víc, pro různé typy chyb.

> Tip: Změňte abc na 5. catch se přeskočí, finally ne.

# Kód

```
try
{
    int n = int.Parse("abc");
    Console.WriteLine(n);
}
catch (Exception e)
{
    Console.WriteLine("Chyba: " + e.Message);
}
finally
{
    Console.WriteLine("konec");
}
```
