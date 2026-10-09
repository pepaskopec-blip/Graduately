---
id: hardware-1-graficka-karta
puvodni: hw.17
nazev: Grafická karta
popis: 3 snímky o obrazu, typech karet a cestě obrazu a pak kvíz.
nazev_cviceni: Kvíz: grafická karta
nadpis_kvizu: Kvíz ke grafické kartě
---

# Výklad

## Z čeho se skládá obraz
*Obraz*

- Digitalizace obrazu je vzorkování, kvantování a kódování.
- Jas může mít třeba 256 úrovní, nebo jen 2.
- Pixel je bod v obraze.
- Polygon je 2D mnohoúhelník; ve hrách často trojúhelník, protože ho GPU zpracuje nejjednodušeji.
- Vertex je vrchol, edge je hrana.
- Textura je bitmapa namapovaná na 3D model.
- Ray tracing je metoda sledování paprsku.

> Tip: hry skládají scénu hlavně z trojúhelníků.

## Integrovaná a samostatná grafika
*Typy*

- Integrovaná grafika je čip přímo na základní desce.
- Bývá v levnějších a kancelářských sestavách i v noteboocích.
- Má nižší výkon a nižší cenu a může brzdit zbytek systému.
- Samostatná karta se dává do rozšiřujícího slotu.
- Míří na hráče a náročnější uživatele.

> Tip: integrovaný čip šetří peníze, samostatná karta přidá výkon.

## Co je na kartě a kam jde obraz
*Cesta*

- Na kartě je GPU, RAMDAC, paměti, sběrnice, porty a často konektor přídavného napájení (Molex).
- GPU mívá aktivní chladič, okolo něj sedí paměťové čipy.
- Datový tok začíná v procesoru počítače a jde do grafické paměti.
- GPU data zpracuje a pošle je do převodníku signálu.
- Odtud obraz pokračuje do monitoru (framebuffer).

> Tip: obraz jde z CPU do grafické paměti, pak do GPU a dál na monitor.


# Kvíz

## Co je pixel?
- [x] Bod v obraze
- Hrana polygonu
- Napájecí konektor
- Typ pevného disku

## Proč hry často používají trojúhelník?
- Protože má víc hran než čtverec
- [x] Pro grafickou kartu je nejjednodušší na zpracování
- Protože nepotřebuje texturu
- Protože nahradí monitor

## Čím se vyznačuje integrovaná grafika?
- Je vždy rychlejší než samostatná karta
- Sedí jen v mechanice DVD
- [x] Čip je na desce, bývá levnější a má nižší výkon
- Nemá vliv na zbytek počítače

## Jakou cestou jde obraz?
- [x] CPU → grafická paměť → GPU → převodník → monitor
- Monitor → disk → zdroj → skříň
- Jen z BIOSu přímo do reproduktoru
- Z F_PANEL rovnou na tiskárnu
