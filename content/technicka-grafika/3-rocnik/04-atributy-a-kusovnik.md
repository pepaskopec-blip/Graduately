---
id: tg-3-atributy
nazev: Atributy a kusovník
popis: Text v bloku, který se mění s každým vložením.
nazev_cviceni: Kvíz: atributy
nadpis_kvizu: Přečtěte kusovník
---

# Výklad

## Atribut je popisek bloku
*Značka*

- Atribut je text uložený v bloku. Při každém vložení se zeptá znovu.
- `_ATTDEF` založí značku, výzvu a výchozí hodnotu. Značka je třeba OZN.
- Do bloku se atribut vloží spolu s geometrií. Samotný atribut na výkresu nic neopakuje.
- Hodnotu už vloženého bloku změníte `_ATTEDIT` nebo dvojklikem, ne přepsáním čáry.

> Tip: značka OZN není text, který se tiskne. Tiskne se hodnota, třeba 1 nebo M8×20.

## Kusovník
*Co je na výkresu*

- Kusovník je tabulka nad razítkem. Řádek je jedna položka sestavy.
- Sloupce bývají pozice, název, počet, materiál nebo norma a rozměr.
- Pozice v kolečku na výkresu musí souhlasit s řádkem tabulky.
- Počet se počítá z vložení, ne od oka. Dva stejné šrouby jsou počet 2, ne dva řádky.

> Tip: tabulku `_TABLE` dejte na hladinu TEXT nebo RAM. Čáry tabulky nejsou obrys součásti.

## Pořadí
*Nejdřív definice*

- Nejdřív nakreslete díl, přidejte atribut, teprve pak z toho udělejte blok.
- Atribut přidaný až po vytvoření bloku se do starých vložení sám nedostane.
- `_BATTMAN` nebo nové definování bloku vložení srovná. Na škole je jistější blok založit znovu.
- Výchozí hodnota atributu je nápověda. U šroubu třeba M8×20, a při vložení ji smíte změnit.

> Tip: prázdná hodnota v kusovníku znamená, že jste při vložení výzvu odklepli bez textu.

## Zadání: tři položky
*Cvičení*

- Sestava má přírubu (pozice 1, 1 kus, ocel), šroub M8×20 (pozice 2, 4 kusy) a matici M8 (pozice 3, 4 kusy).
- Šroub je blok s atributem OZN. Čtyři vložení mají stejnou hodnotu.
- Kusovník nad razítkem má tři řádky. Pozice 2 má počet 4, ne 1.
- Odkazové čáry z koleček 1, 2 a 3 míří na správný díl a nekříží se bez důvodu.

> Tip: čtyři matice jsou pořád jedna položka. Do tabulky nepište čtyři řádky se stejným šroubem.


# Kvíz

## Co se při vložení bloku s atributem ptá znovu?
- Barva hladiny
- [x] Hodnota atributu, třeba označení
- Základní bod definice
- Měřítko výkresu
> Geometrie bloku zůstává. Mění se text, který jste založili jako atribut.

## Čtyři stejné šrouby v kusovníku jsou?
- Čtyři řádky
- [x] Jeden řádek s počtem 4
- Pozice 1 až 4
- Atribut bez bloku
> Stejný díl je jedna položka. Počet říká, kolikrát je vložený.

## Kdy se atribut musí založit?
- Až po tisku
- [x] Před vytvořením bloku, spolu s geometrií
- Jen v razítku
- Atribut nejde uložit do bloku
> Atribut přidaný později se do existující definice sám nepřidá.

## Kam patří kusovník?
- Přes nárys
- [x] Nad razítko
- Na hladinu OBRYS
- Mimo rámeček
> Čte se spolu s razítkem. Čáry tabulky nejsou hrany součásti.
