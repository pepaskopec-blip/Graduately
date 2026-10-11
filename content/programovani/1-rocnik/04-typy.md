---
id: cs-1-typy
nazev: Datové typy
popis: int, double, bool, char a string.
---

# Výklad

## Čísla
*int a double*

- int je celé číslo: 42.
- double je číslo s desetinnou tečkou: 3.14.
- V kódu je desetinná tečka, ne čárka.

> Tip: 3,14 by C# přečetl jako dva údaje oddělené čárkou.

## Text a ano/ne
*string, char, bool*

- string je text v uvozovkách.
- char je jeden znak v apostrofech: 'A'.
- bool je jen true nebo false.

> Tip: Apostrof a uvozovky nejsou totéž. 'A' je znak, "A" je text.

# Kód

```
int cele = 42;
double desetinne = 3.14;
bool prsi = false;
char pismeno = 'A';
string text = "ahoj";
Console.WriteLine(cele);
Console.WriteLine(desetinne);
Console.WriteLine(prsi);
Console.WriteLine(pismeno);
Console.WriteLine(text);
```
