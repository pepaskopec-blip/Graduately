---
id: tg-2-srafy
nazev: Šrafy
popis: Řez materiálem, měřítko šrafy a uzavřená oblast.
nazev_cviceni: Kvíz: šrafy
nadpis_kvizu: Rozhodněte, co se šrafuje
---

# Výklad

## Co šrafa říká
*Materiál*

- Šrafa v řezu znamená materiál, který řezná rovina proťala.
- Ocel se na školním výkresu šrafuje tenkými čarami pod 45 stupňů.
- Dva sousední díly mají šrafu na opačnou stranu, nebo jinou rozteč.
- Díra, drážka a prázdno se nešrafují. Není tam materiál.

> Tip: šrafa pod 45 stupni nesmí být rovnoběžná s obrysem. Když je obrys šikmý, úhel šrafy změňte.

## Příkaz
*Uzavřená oblast*

- `_HATCH` a vzor ANSI31. Klikněte dovnitř uzavřené oblasti.
- Oblast musí být dovřená. Mezera v rohu šrafu zastaví nebo ji vylije ven.
- Měřítko vzoru zvětší rozteč čar. Moc hustá šrafa na tisku zčerná.
- Šrafa patří na hladinu SRAFA, tenkou čarou. Ne na OBRYS.

> Tip: asociativní šrafa se přizpůsobí, když obrys později natáhnete. Na školní práci ji nechte zapnutou.

## Co se v podélném řezu nešrafuje
*Výjimky*

- Hřídel, žebro, šroub a pero v podélném řezu zůstávají bez šrafy.
- V příčném řezu se tatáž hřídel šrafuje. Rozhoduje směr řezu, ne součást.
- Tenká stěna řezaná podélně se šrafuje. Žebro ne, aby nepůsobilo jako plný kus.
- Obrys šrafované plochy je pořád tlustá čára. Šrafa ho nenahrazuje.

> Tip: podélný řez hřídele ukáže osazení obrysem. Šrafa by z hřídele udělala zdánlivě plný hranol.

## Zadání: řez destičkou
*Cvičení*

- Destičku 80 × 20 v řezu A-A vyšrafujte ANSI31, měřítko tak, aby rozteč byla zhruba 2 až 3 mm.
- Díru Ø10 uprostřed nešrafujte. Šrafa končí na její hraně.
- Šrafu dejte na hladinu SRAFA. Vypnutím této hladiny musí zmizet jen šrafa, obrys zůstane.
- Když šrafa vyteče ven, najděte mezeru v obrysu a roh napojte.

> Tip: jedna oblast vlevo od díry a jedna vpravo. Obě mohou být jedna šrafa, pokud příkaz umí víc oblastí.


# Kvíz

## Co znamená šrafa v řezu?
- Skrytou hranu
- [x] Materiál proťatý řeznou rovinou
- Závit
- Měřítko 1:2
> Šrafuje se jen to, co řez skutečně prochází. Díra je prázdná.

## Jaký vzor se na škole používá pro ocel?
- Plná výplň
- [x] ANSI31, tenké čáry pod 45 stupňů
- Tečkování
- Čerchovaná osa
> ANSI31 je běžný strojařský vzor. Úhel změňte, jen když by splynul s obrysem.

## Proč šrafa vyteče mimo součást?
- Je zapnuté orto
- [x] Obrys není uzavřený
- Chybí kóta
- Hladina je ByLayer
> I malá mezera v rohu oblast otevře. Napojte roh a šrafu opakujte.

## Šrafuje se hřídel v podélném řezu?
- Ano, vždy
- [x] Ne
- Jen když má drážku
- Jen ve 3D
> Hřídel, žebro a šroub se v podélném řezu nechávají prázdné. V příčném řezu se hřídel šrafuje.
