---
id: cs-3-zapouzdreni
nazev: Zapouzdření
popis: Soukromá data a veřejné metody.
---

# Výklad

## private
*schované*

- private pole vidí jen kód té třídy.
- public metoda je dveře, kterými se s objektem mluví.
- Zvenku z.penize nenapíšete. Zavoláte Pridej.

> Tip: Když je vše public, kdokoli rozbije pravidla objektu.

## Metoda
*Pridej*

- Pridej přičte a vrátí nový stav.
- Záporný vklad lze v metodě odmítnout.
- Výpis stavu jde přes Kolik, ne přes pole.

> Tip: Zkuste Pridej(-10). Stav se nemá zmenšit.

# Kód

```
Pokladna p = new Pokladna();
p.Pridej(50);
p.Pridej(-10);
Console.WriteLine(p.Kolik());

class Pokladna
{
    private int penize;

    public void Pridej(int castka)
    {
        if (castka > 0) penize = penize + castka;
    }

    public int Kolik()
    {
        return penize;
    }
}
```
