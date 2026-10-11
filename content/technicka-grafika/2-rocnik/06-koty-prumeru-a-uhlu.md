---
id: tg-2-koty-prumeru
nazev: Kóty průměru, poloměru a úhlu
popis: Značka průměru, poloměr zaoblení a úhel zkosení.
nazev_cviceni: Kvíz: průměr a úhel
nadpis_kvizu: Doplňte značku ke správnému rozměru
---

# Výklad

## Průměr a poloměr
*Díra a oblouk*

- `_DIMDIAMETER` kótuje kružnici. Před číslo patří značka průměru.
- `_DIMRADIUS` kótuje oblouk. Před číslo patří R.
- Díra má kótu průměru, i když jste ji kreslili poloměrem. Výroba vrtá průměrem.
- Zaoblení rohu má kótu poloměru. Jedno R na výkresu platí pro všechny stejné rohy, když to poznámka říká.

> Tip: díru Ø8 nekótujte jako R4. Nástroj se jmenuje podle průměru.

## Úhel
*Zkosení a klín*

- `_DIMANGULAR` měří úhel mezi dvěma úsečkami nebo obloukem.
- Zkosení 2 × 45° se často zapíše poznámkou, ne obloukovou kótou v každém rohu.
- Úhel se měří ve stupních. 45 stupňů je polovina pravého úhlu.
- Kóta úhlu nesmí překrýt číslo sousední délky. Odsuňte ji dál od vrcholu.

> Tip: pravý úhel se na strojařském výkresu obvykle nekótuje. 90 stupňů se předpokládá.

## Kolik desetin
*Přesnost*

- Školní kóta celého milimetru nemá zbytečné desetinné nuly, pokud styl neříká jinak.
- Průměr a poloměr zaokrouhlete stejně jako délky. Nemíchejte 8 a 8,00 bez důvodu.
- `_DIMSTYLE` nastaví přesnost i značky najednou. Neupravujte každou kótu ručně.
- Kóta, která ukazuje 7,99 místo 8, minula střed. Geometrii opravte, text ne.

> Tip: 7,99 je skoro jistě úchop vedle středu. `_ID` středu díry to ukáže.

## Zadání: díra a roh
*Cvičení*

- Na destičce okótujte díru Ø8 příkazem `_DIMDIAMETER`.
- Zaoblení R8 na drážce okótujte `_DIMRADIUS` jednou. K ostatním stejné poznámkou.
- Zkosení 2 × 45° zapište k příslušné hraně.
- Žádná kóta průměru nesmí ukazovat 4 ani R8 u díry, která se vrtá.

> Tip: čtyři stejné díry se kótují jednou, s poznámkou 4× Ø8, plus rozteč středů.


# Kvíz

## Jak se kótuje vrtaná díra o průměru 8 mm?
- R8
- R4
- [x] Ø8
- 8 bez značky, jako délka
> Vrták se volí podle průměru. Poloměr by výrobce přepočítával a snadno spletl.

## Co znamená R8?
- Průměr 8
- [x] Poloměr zaoblení 8 mm
- Rozteč 8 mm
- Tloušťka čáry 8
> R je poloměr oblouku. U díry se R nepoužívá.

## Kóta díry ukazuje 7,99. Co s tím?
- Přepsat text na 8
- [x] Opravit geometrii, střed minul úchop
- Smazat všechny kóty
- Změnit měřítko na 1:2
> Text by lež zakryl. Střed díry má ležet přesně tam, kde ho kóty polohy uvádějí.

## Jak jednou kótou popsat čtyři stejné díry Ø8?
- Čtyřikrát opsat Ø8
- [x] 4× Ø8 a rozteč středů
- Jen jednu díru a ostatní nechat bez kóty
- R4 krát čtyři
> Počet, průměr a poloha středů stačí. Opakovat stejnou kótu u každé díry výkres zanáší.
