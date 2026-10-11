---
id: cs-2-pole
nazev: Pole
popis: Víc hodnot stejného typu pod jedním jménem.
---

# Výklad

## Index
*od nuly*

- new int[] { 2, 4, 6 } je pole tří čísel.
- První prvek je znamky[0], poslední znamky[2].
- Length je počet prvků, ne index posledního.

> Tip: znamky[3] u pole délky 3 spadne. Poslední index je 2.

## Zápis
*prvku*

- Do prvku se dá uložit nová hodnota: znamky[1] = 5.
- Ostatní prvky zůstanou.
- Pole má pevnou délku. Přidat čtvrtý prvek nejde, na to je později List.

> Tip: Vyměňte první známku a spusťte znovu.

# Kód

```
int[] znamky = { 1, 3, 2 };
Console.WriteLine(znamky[0]);
Console.WriteLine(znamky.Length);
znamky[1] = 5;
Console.WriteLine(znamky[1]);
```
