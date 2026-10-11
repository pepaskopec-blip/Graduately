---
id: cs-4-delegaty
nazev: Delegáty
popis: Proměnná, do které uložíte metodu.
---

# Výklad

## Func
*funkce*

- Func<int, int> je metoda, která dostane int a vrátí int.
- x => x * x je taková metoda bez jména.
- Do proměnné ji uložíte a zavoláte později.

> Tip: Závorky pocet(4) metodu spustí. Bez nich byste ji jen předali dál.

## Action
*bez výsledku*

- Action nic nevrací. Hodí se na kroky.
- Metodu můžete předat jiné metodě jako argument.
- Tak se píše i Where z minulé lekce.

> Tip: Vyměňte x * x za x + 1.

# Kód

```
Func<int, int> pocet = x => x * x;
Console.WriteLine(pocet(4));
Action pozdrav = () => Console.WriteLine("ahoj");
pozdrav();
```
