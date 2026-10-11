---
id: cs-1-promenne
nazev: Proměnné
popis: Pojmenovaná hodnota, kterou lze změnit.
---

# Výklad

## Deklarace
*typ jméno*

- Nejdřív typ, pak jméno, pak hodnota: int vek = 17;
- Jméno začíná písmenem. Mezery v něm být nesmí.
- Do stejné proměnné lze později uložit novou hodnotu stejného typu.

> Tip: int drží celé číslo. string drží text.

## Čtení
*jméno*

- Jméno bez uvozovek je proměnná. Uvozovky by z něj udělaly text.
- WriteLine umí vypsat proměnnou i text spojený plusem.
- Hodnota se dosadí až při běhu, ne při psaní.

> Tip: Změňte vek na 18 a spusťte znovu.

# Kód

```
string jmeno = "Anna";
int vek = 17;
jmeno = "Eva";
Console.WriteLine(jmeno);
Console.WriteLine(vek);
Console.WriteLine(jmeno + " má " + vek + " let");
```
