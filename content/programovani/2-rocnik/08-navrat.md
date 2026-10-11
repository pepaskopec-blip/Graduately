---
id: cs-2-navrat
nazev: Parametry a návrat
popis: Metoda dostane hodnoty a může jednu vrátit.
---

# Výklad

## Parametr
*vstup*

- int a v závorce je parametr. Volající dodá argument.
- Součet(2, 5) uloží 2 do a a 5 do b.
- Jména parametrů platí jen uvnitř metody.

> Tip: Změňte argumenty a výsledek se změní, metoda ne.

## return
*výstup*

- int před jménem je typ výsledku.
- return hodnotu vrátí a metodu ukončí.
- Volající si výsledek může uložit: int s = Soucet(2, 5);

> Tip: return 0 omylem na začátku by zbytek metody nespustil.

# Kód

```
int s = Soucet(2, 5);
Console.WriteLine(s);
Console.WriteLine(Soucet(10, 1));

static int Soucet(int a, int b)
{
    return a + b;
}
```
