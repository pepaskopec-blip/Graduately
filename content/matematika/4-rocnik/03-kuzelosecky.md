---
id: matematika-4-kuzelosecky
puvodni: mat4.3
nazev: Kuželosečky
popis: Kružnice a elipsa v osové poloze.
nazev_cviceni: Příklady
nadpis_kvizu: Kuželosečky
---

# Výklad

## Střed a poloměr
*Kružnice*

- Kružnice je množina bodů stejně vzdálených od středu.
- (x − 1)² + (y + 2)² = 9 má střed [1; −2].
- y + 2 je totéž co y − (−2).
- Poloměr je √9 = 3, ne 9.

> Tip: (x − m)² + (y − n)² = r² má střed [m; n].

## Dvě poloosy
*Elipsa*

- Elipsa v osové poloze se středem v počátku má rovnici x²/a² + y²/b² = 1.
- U x²/25 + y²/9 = 1 je a = 5 a b = 3.
- Delší poloosa je hlavní, kratší vedlejší.
- Vrcholy na ose x jsou [±a; 0], na ose y [0; ±b].

> Tip: ve tvaru x²/a² + y²/b² = 1 je a poloosa na ose x.

## Vzdálenost ohniska od středu
*Excentricita*

- Lineární výstřednost e splňuje e² = a² − b², když je a hlavní poloosa.
- √(25 − 9) = √16 = 4.
- Ohniska elipsy jsou [±e; 0].
- Součet vzdáleností bodu elipsy od obou ohnisek je 2a.

> Tip: pro a > b je e = √(a² − b²).


# Příklady

## Kružnice má rovnici (x − 1)² + (y + 2)² = 9. Napište souřadnice středu jako x;y.
= pair:1;-2
> Střed je [1; −2]. Napište 1;-2.

## Jaký je poloměr kružnice (x − 1)² + (y + 2)² = 9?
= num:3
> Pravá strana je r², takže r = 3. Výsledek je 3.

## Elipsa má rovnici x²/25 + y²/9 = 1. Jak dlouhá je poloosa a na ose x?
= num:5
> a² = 25, takže a = 5. Výsledek je 5.

## Elipsa má rovnici x²/25 + y²/9 = 1. Jak dlouhá je poloosa b na ose y?
= num:3
> b² = 9, takže b = 3. Výsledek je 3.

## Elipsa má a = 5 a b = 3. Jaká je její lineární výstřednost e?
= num:4
> e = √(25 − 9) = √16 = 4. Výsledek je 4.
