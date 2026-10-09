---
id: hardware-1-vzorkovani
puvodni: hw.6
nazev: Vzorkování
popis: 2 snímky o vzorkovací frekvenci a Nyquistovi a pak kvíz.
nazev_cviceni: Kvíz: vzorkování
nadpis_kvizu: Kvíz ke vzorkování zvuku
---

# Výklad

## Vzorkování
*Vzorky*

- Vzorkování měří velikost signálu v pravidelných okamžicích.
- Mezi vzorky se hodnota neukládá.
- Vzorkovací frekvence fs je počet vzorků za sekundu.
- Jednotka je hertz (Hz). 1 kHz = 1000 vzorků za sekundu.
- Vyšší fs zachytí rychlejší změny, tedy vyšší tóny.

> Tip: vzorkovací frekvence fs = počet vzorků za sekundu (Hz).

## Nyquistův–Shannonův teorém
*Nyquist*

- Pro věrnou rekonstrukci musí být fs vyšší než dvojnásobek nejvyšší frekvence ve signálu.
- Nyquistova frekvence je fs / 2 – vyšší tón už záznam neunese.
- Člověk slyší zhruba do 20 kHz, dvojnásobek je 40 kHz.
- CD proto používá 44,1 kHz: o něco víc než 40 kHz, aby zbyl prostor pro filtr.

> Tip: fs musí být vyšší než 2 × fmax. Nyquistova frekvence je fs / 2.


# Kvíz

## Co je vzorkovací frekvence?
- Počet bitů v jednom vzorku
- [x] Počet vzorků za sekundu
- Délka skladby v minutách
- Počet kanálů (mono nebo stereo)

## V jakých jednotkách se udává vzorkovací frekvence?
- V bytech
- [x] V hertzech
- Ve voltech
- V pixelech

## Co říká Nyquistův–Shannonův teorém?
- [x] fs musí být vyšší než dvojnásobek nejvyšší frekvence
- fs musí být vždy 8 kHz
- Stačí jeden vzorek na celou skladbu
- Bitová hloubka musí být 1

## Proč má audio CD vzorkovací frekvenci 44,1 kHz?
- Protože byte má 8 bitů
- [x] Sluch sahá asi do 20 kHz a fs musí být vyšší než dvojnásobek
- Protože stereo má dva kanály
- Protože minutu tvoří 60 sekund
