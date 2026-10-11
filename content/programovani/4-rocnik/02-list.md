---
id: cs-4-list
nazev: List
popis: Seznam, který umí růst.
---

# Výklad

## Přidávání
*Add*

- List<string> začne prázdný. Add přidá na konec.
- Count je počet. Indexy jsou zase od nuly.
- Na rozdíl od pole délku dopředu neříkáte.

> Tip: List<string> potřebuje špičaté závorky s typem prvku.

## Odebrání
*Remove*

- Remove("Eva") odebere první Evu.
- RemoveAt(0) odebere podle indexu.
- foreach pak projde, co zůstalo.

> Tip: Přidejte třetí jméno před odebráním.

# Kód

```
List<string> jmena = new List<string>();
jmena.Add("Anna");
jmena.Add("Eva");
jmena.Remove("Eva");
foreach (string j in jmena)
{
    Console.WriteLine(j);
}
Console.WriteLine(jmena.Count);
```
