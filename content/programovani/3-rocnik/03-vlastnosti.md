---
id: cs-3-vlastnosti
nazev: Vlastnosti
popis: get a set místo holého pole.
---

# Výklad

## get a set
*tečka zůstává*

- public string Jmeno { get; set; } se zvenku používá jako pole.
- Uvnitř může set odmítnout špatnou hodnotu.
- get hodnotu vrátí, set ji uloží.

> Tip: Zápis z.Vek = -1 může set zarazit, holé pole ne.

## Kontrola
*set*

- V příkladu set u věku záporné číslo neuloží.
- get pořád vrátí poslední platnou hodnotu.
- Tak se objekt nepokazí zvenku.

> Tip: Zkuste z.Vek = -3 a vypište věk. Zůstane 17.

# Kód

```
Zak z = new Zak("Anna", 17);
z.Vek = -3;
Console.WriteLine(z.Vek);

class Zak
{
    public string Jmeno { get; set; }
    private int vek;
    public int Vek
    {
        get { return vek; }
        set { if (value >= 0) vek = value; }
    }

    public Zak(string jmeno, int vek)
    {
        Jmeno = jmeno;
        Vek = vek;
    }
}
```
