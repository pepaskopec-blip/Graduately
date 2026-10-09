---
id: hardware-1-shadery-a-render
puvodni: hw.18
nazev: Shadery a render
popis: 3 snímky o shaderech, renderu a CUDA a pak kvíz.
nazev_cviceni: Kvíz: shadery a render
nadpis_kvizu: Kvíz k shaderům a renderování
---

# Výklad

## Grafický procesor
*GPU*

- GPU přijímá data od CPU a počítá obraz.
- Je to složitý čip se stovkami milionů tranzistorů.
- Sleduje se takt v MHz, kapacita paměti a počet shaderů.
- Dřív se počítaly vertex a pixel shadery, dnes spíš unifikované, částečně programovatelné jednotky.

> Tip: pro výkon počítače je GPU stejně důležité jako CPU.

## Shader a renderování
*Programy*

- Shader je program, který řídí části grafického řetězce.
- Renderování tvoří obraz podle počítačového modelu, nejčastěji 3D.
- Render je vizualizace: model se přenese do 2D bitmapy.
- Softwarový render počítá procesor. Je přesnější, ale mnohonásobně pomalejší.
- Hardwarový render slouží pro náhledy a hry.
- Vertex shader transformuje vrcholy a nové nevytváří.
- Pixel shader potom mapuje texturu a přidává efekty, třeba ray tracing nebo bump mapping.

> Tip: vertex shader posouvá vrcholy, ale nové nevytváří.

## CUDA a architektura GPU
*Výpočty*

- CUDA je paralelní platforma NVIDIA a nechá GPU počítat i jiné úlohy než obraz.
- Zrychluje třeba lineární algebru, obraz, video a hluboké učení.
- Obdoba u AMD se jmenuje FireStream.
- GPU vzniklo pro hry: miliony polygonů a velké textury.
- Je stavěné na tisíce vláken, hodně počítání a málo podmínek a na sekvenční čtení paměti.
- Jednoduché skalární procesory jsou sdružené do streaming multiprocesorů.

> Tip: většinu čipu tvoří mnoho jednoduchých procesorů.


# Kvíz

## Co je shader?
- [x] Program řídící části grafického řetězce
- Napájecí konektor Molex
- Typ pevného disku
- Větrací mřížka

## Co vertex shader nedělá?
- Neposouvá vrcholy
- [x] Nevytváří nové vertexy
- Nesahá na grafickou kartu
- Nepoužívá se ve 3D

## Čím se liší softwarový render?
- Je rychlejší, protože ho počítá jen GPU
- Umí jen černobíle
- [x] Počítá ho CPU, je přesnější a mnohem pomalejší
- Nevytvoří bitmapu

## Co je CUDA?
- [x] Platforma NVIDIA pro výpočty na GPU
- Standard napájecího zdroje
- Starý slot ISA
- Formát diskety
