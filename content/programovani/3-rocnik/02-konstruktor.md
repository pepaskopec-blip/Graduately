---
id: cs-3-konstruktor
nazev: Konstruktor
popis: Metoda, která objekt hned naplní.
---

# Výklad

## Jméno třídy
*new Zak*

- Konstruktor se jmenuje jako třída a nemá návratový typ.
- new Zak("Anna", 17) ho zavolá.
- Prázdný new Zak() přestane fungovat, jakmile napíšete vlastní konstruktor.

> Tip: Hodnoty patří do new, ne až na další řádky.

## this
*tento objekt*

- this.Jmeno je pole objektu. Jmeno bez this by v konstruktoru byl parametr.
- Když se jmenují stejně, this je nutný.
- Mimo konstruktor this často není potřeba.

> Tip: Přidejte do výpisu věk z konstruktoru.

# Kód

```
Zak z = new Zak("Anna", 17);
Console.WriteLine($"{z.Jmeno}, {z.Vek}");

class Zak
{
    public string Jmeno;
    public int Vek;

    public Zak(string jmeno, int vek)
    {
        Jmeno = jmeno;
        Vek = vek;
    }
}
```
