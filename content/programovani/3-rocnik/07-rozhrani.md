---
id: cs-3-rozhrani
nazev: Rozhraní
popis: Smlouva: které metody typ musí mít.
---

# Výklad

## interface
*I*

- interface nemá hotový kód, jen hlavičky metod.
- class Pes : IMluvici slíbí, že Mluv napíše.
- Jedna třída může splnit víc rozhraní. Dědit může jen jednu třídu.

> Tip: Jméno rozhraní se často píše s I na začátku: IMluvici.

## Společný typ
*seznam*

- Proměnná typu rozhraní udrží kohokoli, kdo smlouvu splnil.
- Pes a Radio nemají společného předka, ale oba umí Mluv.
- Smyčka jim rozumí přes smlouvu, ne přes původ.

> Tip: Rozhraní neříká, jak metoda funguje. To napíše až třída.

# Kód

```
IMluvici[] veci = { new Pes(), new Radio() };
foreach (IMluvici v in veci)
{
    Console.WriteLine(v.Mluv());
}

interface IMluvici
{
    string Mluv();
}

class Pes : IMluvici
{
    public string Mluv() { return "haf"; }
}

class Radio : IMluvici
{
    public string Mluv() { return "hudba"; }
}
```
