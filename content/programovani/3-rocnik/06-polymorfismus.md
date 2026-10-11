---
id: cs-3-polymorfismus
nazev: Polymorfismus
popis: Stejné volání, jiná odpověď potomka.
---

# Výklad

## virtual a override
*přepsání*

- virtual u předka dovolí potomkovi metodu vyměnit.
- override u potomka řekne, že jde o tu samou metodu.
- Bez virtual by potomek jen schoval jméno a přes typ předka by se volala stará verze.

> Tip: Proměnná je Zvire, objekt je Pes. Zavolá se štěk, ne obecné zvíře.

## Seznam
*víc tvarů*

- Do pole Zvire lze dát psa i kočku.
- foreach volá Zvuk a každý objekt odpoví po svém.
- Nové zvíře přidáte třídou, ne úpravou smyčky.

> Tip: Přidejte do pole kočku a třetí řádek výstupu bude mňau.

# Kód

```
Zvire[] zvirata = { new Pes(), new Kocka() };
foreach (Zvire z in zvirata)
{
    Console.WriteLine(z.Zvuk());
}

class Zvire
{
    public virtual string Zvuk() { return "..."; }
}

class Pes : Zvire
{
    public override string Zvuk() { return "haf"; }
}

class Kocka : Zvire
{
    public override string Zvuk() { return "mňau"; }
}
```
