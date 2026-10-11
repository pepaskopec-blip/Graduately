---
id: cs-1-prevody
nazev: Převody typů
popis: Z double na int a z textu na číslo.
---

# Výklad

## Ořez desetin
*(int)*

- (int)3.9 je 3. Desetinná část se odřízne, nezaokrouhlí.
- Opačně int do double ztrátu nemá: 3 se stane 3.0.
- Převod napište před hodnotu do závorky.

> Tip: Když potřebujete zaokrouhlit, (int) na to není.

## Text na číslo
*Parse*

- int.Parse("20") udělá z textu číslo 20.
- Když text číslo není, program spadne. To později chytíme výjimkou.
- Číslo na text udělá ToString nebo zápis do řetězce.

> Tip: Vstup z konzole je vždy text, i když uživatel napíše číslici.

# Kód

```
double cena = 19.9;
int cele = (int)cena;
Console.WriteLine(cele);
string vstup = "20";
int cislo = int.Parse(vstup);
Console.WriteLine(cislo + 1);
```
