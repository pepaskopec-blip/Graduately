---
id: cs-4-async
nazev: Async a await
popis: Práce, která počká, aniž by zamkla zbytek zápisu.
---

# Výklad

## await
*počkej*

- await Task.Delay čeká, ale zápis zůstane řádek za řádkem.
- Metoda s await má v hlavičce async a vrací Task.
- V krátkém programu smí await stát i na nejvyšší úrovni.

> Tip: Delay(300) čeká 300 milisekund. Výpis hotovo přijde až po něm.

## Task
*slib*

- Task je slib, že práce doběhne.
- await slib rozbalí. Bez await byste měli slib, ne výsledek.
- Na síť a soubory se await hodí. Na 2 + 2 ne.

> Tip: Dejte za první výpis ještě jeden await a druhý řádek.

# Kód

```
Console.WriteLine("začátek");
await Task.Delay(300);
Console.WriteLine("hotovo");
```
