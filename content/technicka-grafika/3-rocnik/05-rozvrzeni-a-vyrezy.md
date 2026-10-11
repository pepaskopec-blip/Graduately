---
id: tg-3-rozvrzeni
nazev: Rozvržení a výřezy
popis: Papír, na kterém leží pohled do modelu.
nazev_cviceni: Kvíz: výřezy
nadpis_kvizu: Nastavte měřítko výřezu
---

# Výklad

## Model a papír
*Dva prostory*

- V modelu je součást v milimetrech 1:1. Papír je rozvržení, záložka nad příkazovým řádkem.
- Na papír přijde rámeček, razítko a výřez. Výřez je okno do modelu.
- Dvojklik dovnitř výřezu přepne do modelu. Dvojklik ven vrátí papír.
- Kóty součásti nechte v modelu. Razítko kreslete v rozvržení, ať má na papíře stálou velikost.

> Tip: rámeček nakreslený v modelu se při změně měřítka výřezu zmenší spolu se součástí. Proto patří na papír.

## Měřítko výřezu
*1:2 na papíře*

- Výřez má vlastní měřítko. Součást v modelu zůstává 1:1.
- Měřítko 1:2 znamená, že 1 mm na papíře je 2 mm na součásti.
- Ve stavovém řádku výřezu se měřítko vybírá ze seznamu. Starší zápis je `_ZOOM` a `1/2xp`.
- Výřez zamkněte, ať kolečko myši měřítko omylem nerozhodí.

> Tip: nezamčený výřez je nejčastější důvod, proč ráno výkres není 1:1, i když včera byl.

## Co je vidět
*Hladiny ve výřezu*

- Hladinu lze ve výřezu zmrazit a v modelu ji nechat. Pomocné čáry tak na papír nejdou.
- Rámeček výřezu dejte na hladinu VYREZ a tu před tiskem vypněte. Na papíře zůstane jen obsah.
- Jeden výkres smí mít víc výřezů: čelo, řez, detail v jiném měřítku.
- Detail 2:1 je druhý výřez. V razítku se pak píše měřítko hlavní, detail má vlastní popisku.

> Tip: detail zvětšený 2:1 nekreslete znovu. Je to stejný model v druhém výřezu.

## Zadání: A3 a měřítko 1:2
*Cvičení*

- Rozvržení nastavte na A3 na šířku. Rámeček a razítko nakreslete v rozvržení.
- Výřez ukazuje přírubu Ø100 v měřítku 1:2. Na papíře má mít průměr 50 mm.
- Výřez zamkněte. Hladinu jeho rámečku vypněte.
- `_DIST` na papíře přes průměr příruby má ukázat 50, ne 100.

> Tip: měření v rozvržení, mimo výřez, čte milimetry papíru. 100 mm součásti v měřítku 1:2 je na papíře 50 mm.


# Kvíz

## Kde se kreslí tvar součásti v měřítku 1:1?
- V rozvržení
- [x] V modelu
- V razítku
- V hladině VYREZ
> Model zůstává skutečný. Měřítko pro papír nastavuje až výřez.

## Součást Ø100 ve výřezu 1:2 má na papíře průměr?
- 100 mm
- [x] 50 mm
- 200 mm
- 1 mm
> 1:2 zmenšuje na polovinu. Kóta ve modelu pořád říká 100.

## Proč výřez zamknout?
- Aby nešel tisknout
- [x] Aby kolečko myši nezměnilo měřítko
- Zamknutí smaže model
- Aby šel rámeček smazat
> Nezamčený výřez se při zoomu rozhodí a výkres už není v měřítku z razítka.

## Kam patří rámeček a razítko?
- Do modelu, na hladinu OBRYS
- [x] Do rozvržení
- Do definice bloku šroubu
- Mimo papír
> V rozvržení mají stálou velikost bez ohledu na měřítko výřezu.
