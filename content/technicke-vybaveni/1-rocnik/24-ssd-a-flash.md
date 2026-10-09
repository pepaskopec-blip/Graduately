---
id: hardware-1-ssd-a-flash
puvodni: hw.24
nazev: SSD a flash
popis: 3 snímky o SSD, TRIM, SSHD a USB a pak kvíz.
nazev_cviceni: Kvíz: SSD a flash
nadpis_kvizu: Kvíz k SSD a flash paměti
---

# Výklad

## SSD
*Bez pohybu*

- SSD je polovodičový disk bez pohyblivých částí.
- Je odolnější, tiché, úspornější a rychlejší než HDD.
- Buňky se opotřebovávají, počet zápisů je omezený.
- Bývá dražší než plotnový disk stejné kapacity.

> Tip: fragmentace u SSD problém není.

## FTL, TRIM a záchrana
*Buňky*

- FTL rovnoměrně rozkládá zápisy, aby se buňky sjížděly stejně.
- TRIM řekne disku, které bloky už nikdo nepoužívá, a zápis se tím zrychlí.
- Poškození může být hardwarové (mechanika, elektronika, oheň, voda) nebo softwarové (smazání, formát, virus).
- Smazání maže metadata, obsah na HDD často zůstane a dá se obnovit.
- U SSD je obnova kvůli TRIM obtížnější.

> Tip: smazání smaže metadata. U SSD TRIM záchranu ztěžuje.

## SSHD a USB flash
*Přenos*

- SSHD je pevný disk s malou SSD částí, zhruba 8 GB.
- Často používané soubory si přesune na rychlou část.
- Dává smysl, když chcete kapacitu i rychlost za rozumnou cenu.
- USB flash je malé médium na přenos dat po sběrnici USB.
- Životnost buněk záleží na typu SLC, MLC nebo TLC a na kvalitě hardwaru.

> Tip: konektor USB flash disku je počítaný asi na 1500 cyklů.


# Kvíz

## Co je hlavní výhoda SSD proti HDD?
- Má plotny a je hlučnější
- [x] Nemá pohyblivé části, je tiché a rychlejší
- Data ztratí při vypnutí
- Nejde ho použít jako systémový disk

## Jaká je nevýhoda SSD?
- [x] Buňky se opotřebovávají a disk bývá dražší
- Musí se defragmentovat každý den
- Nepřežije přesun po stole
- Nemá TRIM ani FTL

## Co dělá příkaz TRIM?
- Zvýší otáčky ploten
- Smaže BIOS
- [x] Oznámí SSD, které bloky už nejsou použité
- Zapne fantomové napájení

## Co je SSHD?
- Jen jiný název pro disketu
- [x] HDD doplněné o malou SSD část, asi 8 GB
- Optický disk s modrým laserem
- Paměťová karta SD
