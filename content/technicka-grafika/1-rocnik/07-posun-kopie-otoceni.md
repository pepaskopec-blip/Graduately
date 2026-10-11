---
id: tg-1-posun-kopie
nazev: Posun, kopie, otočení a zrcadlení
popis: Úprava hotového tvaru místo kreslení znovu.
nazev_cviceni: Kvíz: úpravy
nadpis_kvizu: Vyberte příkaz podle zadání
---

# Výklad

## Posun a kopie
*Z místa na místo*

- `_MOVE` objekt odnese. Na původním místě nic nezůstane.
- `_COPY` nechá vzor a přidá další. Ptá se na základní bod a pak na nové body.
- Základní bod vyberte úchopem, třeba roh nebo střed. Od oka se kopie rozjede.
- Víc kopií za sebou vznikne opakovaným zadáním dalšího bodu, bez nového příkazu.

> Tip: posun o 30 mm doprava je základní bod kdekoli, druhý bod `@30,0`.

## Otočení
*Kolem bodu*

- `_ROTATE` chce základní bod a úhel. Kladný úhel je proti směru hodin.
- 90 otočí doleva, -90 nebo 270 doprava.
- Volba Reference vezme současný úhel hrany a řekne, jaký má být nový.
- Otáčí se kolem vybraného bodu, ne kolem středu obrazovky.

> Tip: díru v rohu otočíte kolem středu destičky, ne kolem středu díry, jinak odjede z rozteče.

## Zrcadlení
*Souměrnost*

- `_MIRROR` chce dva body osy zrcadlení. Osa nemusí být nakreslená předem.
- Svislá osa jsou dva body se stejným X. Vodorovná dva body se stejným Y.
- Na otázku, zda smazat zdroj, dejte Ne, když má zůstat obě poloviny.
- Text se při zrcadlení může otočit naruby. Proměnná MIRRTEXT s hodnotou 0 nechá text čitelný.

> Tip: souměrnou součást nakreslete z poloviny a zrcadlete. Druhou polovinu neobkreslujte.

## Zadání: druhá díra zrcadlem
*Cvičení*

- Destička 100 × 40, levý dolní roh v 0,0.
- Díru Ø10 dejte středem do bodu 20,20.
- Zrcadlete ji přes svislou osu destičky. Osa prochází body 50,0 a 50,40.
- Zdroj nemažte. Druhá díra má střed v 80,20.

> Tip: 100 minus 20 je 80. Zrcadlo kolem x = 50 převede 20 na 80 a výšku 20 nechá.


# Kvíz

## Čím odnesete objekt, aby na původním místě nezůstal?
- `_COPY`
- [x] `_MOVE`
- `_MIRROR`
- `_ROTATE`
> Kopie zdroj nechá. Posun ho přestěhuje.

## Jaký úhel otočí tvar o 90 stupňů ve směru hodin?
- 90
- [x] -90
- 180
- 45
> Kladný úhel jde proti hodinám. Po směru hodin je záporný, tedy -90.

## Přes co se zrcadlí díra se středem 20,20 na destičce široké 100?
- Přes bod 0,0
- [x] Přes svislou osu x = 50
- Přes vodorovnou osu y = 20
- Přes hranu x = 100
> Osa souměrnosti destičky je uprostřed šířky. 20 a 80 jsou od ní stejně daleko.

## Co má být základní bod posunu?
- Libovolné místo vedle objektu, od oka
- [x] Úchop, třeba roh nebo střed
- Vždy počátek 0,0
- Střed obrazovky
> Úchop podrží rozměr. Kliknutí vedle rohu posune tvar o neznámý kousek navíc.
