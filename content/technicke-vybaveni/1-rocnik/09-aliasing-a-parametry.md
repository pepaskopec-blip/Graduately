---
id: hardware-1-aliasing-a-parametry
puvodni: hw.9
nazev: Aliasing a parametry
popis: 2 snímky o aliasingu a běžných formátech a pak kvíz.
nazev_cviceni: Kvíz: aliasing a parametry
nadpis_kvizu: Kvíz k aliasingu a parametrům záznamu
---

# Výklad

## Když je vzorkování moc řídké
*Aliasing*

- Aliasing vznikne, když je fs příliš nízká a poruší se Nyquistovo pravidlo.
- Vysoká frekvence se v záznamu jeví jako nižší tón.
- Příklad: tón 6 kHz vzorkovaný 8 kHz se jeví jako 2 kHz.
- Antialiasingový filtr před převodníkem vysoké frekvence ořízne.

> Tip: aliasing vznikne před uložením a z hotových vzorků už nejde spolehlivě odstranit.

## Běžné parametry záznamu
*Formáty*

- Audio CD: 44,1 kHz, 16 bitů, stereo.
- Klasický telefonní hovor: 8 kHz a 8 bitů – řeč se vejde do pásma do 4 kHz.
- Studio často používá 48 nebo 96 kHz a 24 bitů.
- Parametry se volí podle toho, co má záznam unést a jak velký smí být.

> Tip: vyšší fs a víc bitů znamená věrnější záznam a větší tok.


# Kvíz

## Kdy vzniká aliasing?
- [x] Když je vzorkovací frekvence příliš nízká
- Když má záznam 24 bitů
- Když je soubor ve stereu
- Když se použije D/A převodník

## K čemu je antialiasingový filtr?
- Zvětší bitovou hloubku po uložení
- [x] Před vzorkováním ořízne frekvence nad Nyquistovu
- Spočítá datový tok
- Převede mono na stereo

## Jaké parametry má audio CD?
- 8 kHz, 8 bitů, mono
- [x] 44,1 kHz, 16 bitů, stereo
- 96 kHz, 8 bitů, mono
- 1 kHz, 1 bit, stereo

## Proč telefonnímu hovoru stačí vzorkování 8 kHz?
- [x] Řeč se vejde do pásma zhruba do 4 kHz
- Telefon nahrává na audio CD
- 8 kHz je totéž co 44,1 kHz
- Řeč nemá žádnou frekvenci
