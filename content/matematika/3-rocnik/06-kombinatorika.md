---
id: matematika-3-kombinatorika
puvodni: mat3.6
nazev: Kombinatorika
popis: Permutace, variace a kombinace.
nazev_cviceni: Příklady
nadpis_kvizu: Kombinatorika
---

# Výklad

## Pořadí všech prvků
*Permutace*

- Permutace je uspořádání všech n různých prvků.
- Počet je n!.
- 4! = 24 a 3! = 6.
- Na prvním místě je n možností, na dalším n − 1, a tak dál.

> Tip: n! = 1 · 2 · … · n a 0! = 1.

## Pořadí vybraných prvků
*Variace*

- Variace vybírá k prvků z n a záleží na pořadí.
- V(5, 2) = 5 · 4 = 20.
- Opakování by dovolilo vybrat stejný prvek víckrát.
- Permutace je variace, která vybírá všech n prvků.

> Tip: variace bez opakování V(n, k) = n! / (n − k)!.

## Pořadí nehraje roli
*Kombinace*

- Kombinace vybírá k prvků a dvě vybrané skupiny se stejným obsahem jsou jedna.
- C(5, 2) = 10.
- C(n, k) = C(n, n − k).
- Počet variací je k! krát větší než počet kombinací.

> Tip: C(n, k) = n! / (k! · (n − k)!).


# Příklady

## Kolik je permutací čtyř různých prvků?
= num:24
> 4! = 24. Výsledek je 24.

## Kolik je kombinací 2 prvků z 5?
= num:10
> C(5, 2) = (5 · 4) / 2 = 10. Výsledek je 10.

## Kolik je variací 2 prvků z 5 bez opakování?
= num:20
> V(5, 2) = 5 · 4 = 20. Výsledek je 20.

## Kolik je 3!?
= num:6
> 3 · 2 · 1 = 6. Výsledek je 6.
