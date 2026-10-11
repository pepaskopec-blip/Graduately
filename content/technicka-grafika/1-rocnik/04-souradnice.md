---
id: tg-1-souradnice
nazev: Souřadnice
popis: Absolutní bod, relativní posun a polární zápis.
nazev_cviceni: Kvíz: souřadnice
nadpis_kvizu: Dopočítejte další bod
---

# Výklad

## Absolutní souřadnice
*Od počátku*

- Bod 40,25 leží 40 mm vpravo a 25 mm nahoru od počátku 0,0.
- Čárka odděluje X a Y. Desetinná čárka v čísle se v AutoCADu píše tečkou: 10.5
- Osa X míří vpravo, osa Y nahoru. Tak je natočený model při startu.
- Absolutní bod nezávisí na tom, kde jste skončili minule.

> Tip: když se výkres „rozjede“ mimo papír, někdo zadal absolutní bod místo posunu.

## Relativní souřadnice
*Zavináč*

- `@30,0` znamená 30 mm vpravo od posledního bodu, výška se nemění.
- `@0,40` je 40 mm nahoru. `@-20,0` je 20 mm doleva.
- Zavináč se píše před čísla. Bez něj AutoCAD čte absolutní bod.
- Při kreslení `_LINE` se tak skládá tvar bod za bodem.

> Tip: obdélník 70 × 40 z bodu 10,10: `@70,0` pak `@0,40` pak `@-70,0` a zavřít volbou Close.

## Polární souřadnice
*Délka a úhel*

- `@50<90` je úsečka dlouhá 50 mm směrem 90 stupňů, tedy kolmo nahoru.
- Úhel se měří od kladné osy X proti směru hodinových ručiček.
- `@40<0` míří vpravo, `@40<180` vlevo, `@40<270` dolů.
- Špičatá závorka se píše bez mezery. Mezera by příkaz ukončila.

> Tip: úhel 45 stupňů je šikmo vpravo nahoru. Délka je vždy skutečná, ne průmět.

## Zadání: obdélník ze souřadnic
*Cvičení*

- Spusťte `_LINE` v bodě 20,20.
- Pokračujte `@80,0`, potom `@0,50`, potom `@-80,0`.
- Útvar zavřete volbou Close, ne ručním bodem.
- Změřte `_DIST` mezi levým dolním a pravým horním rohem. Má vyjít 94,3 mm, tedy přepona 80 a 50.

> Tip: Close spojí poslední bod s prvním. Ruční bod 20,20 nemusí trefit počátek a obrys zůstane otevřený.


# Kvíz

## Co znamená zápis `@0,40`?
- Bod 0,40 od počátku výkresu
- [x] 40 mm nahoru od posledního bodu
- 40 mm vpravo od počátku
- Úhel 40 stupňů
> Zavináč čte posun od posledního bodu. Nula v X a 40 v Y je čistě nahoru.

## Kam míří `@60<180`?
- 60 mm nahoru
- [x] 60 mm doleva
- 180 mm vpravo
- Do počátku
> Úhel 180 stupňů je záporná osa X, tedy doleva. 60 je délka úsečky.

## Jak AutoCAD pozná desetinné číslo?
- Čárkou: 10,5,0
- [x] Tečkou: 10.5
- Zlomkem 21/2
- Mezerou 10 5
> Čárka odděluje X a Y. Desetina se píše tečkou, třeba 10.5,20.

## Proč útvar zavřít volbou Close?
- Aby se smazala první úsečka
- [x] Aby se poslední bod trefil do prvního
- Close nastaví měřítko
- Close je totéž co Esc
> Ručně opsaný první bod často mine o zlomek milimetru a obrys nejde vyšrafovat.
