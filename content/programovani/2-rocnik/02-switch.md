---
id: cs-2-switch
nazev: Switch
popis: Víc větví podle jedné hodnoty.
---

# Výklad

## Větve
*case*

- switch vybere case, který se rovná hodnotě.
- break ukončí větev. Bez něj se překlad v klasickém zápisu zastaví.
- default je větev, když nesedí žádný case.

> Tip: Na známku 1 až 5 se switch hodí víc než řada else if.

## Nový zápis
*šipka*

- case 1 => "výborně" je krátká větev bez break.
- Výraz switch umí hodnotu rovnou vrátit.
- V lekci je klasický zápis, ať je vidět break.

> Tip: Přidejte case 5 a spusťte se znamkou 5.

# Kód

```
int znamka = 2;
switch (znamka)
{
    case 1:
        Console.WriteLine("výborně");
        break;
    case 2:
        Console.WriteLine("chvalitebně");
        break;
    default:
        Console.WriteLine("jiná známka");
        break;
}
```
