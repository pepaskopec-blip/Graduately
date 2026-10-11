---
id: cs-3-enum
nazev: Výčet
popis: Pevný seznam pojmenovaných hodnot.
---

# Výklad

## enum
*Den*

- enum Den { Po, Ut, St } jsou tři pojmenované hodnoty.
- Místo magického čísla 1 píšete Den.Po.
- Překladač nepustí hodnotu, která ve výčtu není.

> Tip: Čísla 0, 1, 2 by za měsíc nikdo nepřečetl. Jména ano.

## switch
*přes výčet*

- switch na enum má case pro každou hodnotu.
- Když přidáte čtvrtek a zapomenete case, překlad na to v novém zápisu upozorní.
- Výčet je typ. Není to string.

> Tip: Změňte dnes na Den.St.

# Kód

```
Den dnes = Den.Po;
switch (dnes)
{
    case Den.Po:
        Console.WriteLine("pondělí");
        break;
    case Den.Ut:
        Console.WriteLine("úterý");
        break;
    default:
        Console.WriteLine("jiný den");
        break;
}

enum Den
{
    Po,
    Ut,
    St
}
```
