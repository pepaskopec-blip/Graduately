---
id: tg-2-hladiny
nazev: Hladiny
popis: Barva, typ čáry a tloušťka na jednom místě.
nazev_cviceni: Kvíz: hladiny
nadpis_kvizu: Rozhodněte, co patří na kterou hladinu
---

# Výklad

## Proč hladiny
*Pořádek*

- Hladina nese barvu, typ čáry i tloušťku. Objekt, který na ní vznikne, je zdědí.
- Obrys, osa, skrytá hrana, šrafa, kóta a rámeček jsou samostatné hladiny.
- Vypnutá hladina na obrazovce zmizí, ale tisknout se pořád může. Zmrazená se netiskne a nepřepočítává.
- Zamčená hladina je vidět, ale nejde omylem posunout.

> Tip: kóty nikdy nekreslete na OBRYS. Až budete obrys měnit, kóty by odjely s ním.

## Aktuální hladina
*Než kliknete*

- Nový objekt padne na aktuální hladinu. Je to ta, která je ve správci vybraná jako současná.
- `_LAYER` založí hladinu. Poklepáním ji nastavíte jako aktuální.
- Objekt na špatné hladině přesuňte ve vlastnostech, příkaz `_PROPERTIES` nebo horní lišta.
- Barvu a typ čáry nechte ByLayer. Barva nastavená na objektu se se hladinou přestane řídit.

> Tip: ByLayer znamená „ber to, co říká hladina“. Je to výchozí a na školním výkresu má tak zůstat.

## Názvy, které obstojí
*Úmluva*

- OBRYS, OSA, SKRYTE, SRAFA, KOTA, RAM, TEXT. Krátce a bez mezer.
- OBRYS je bílá nebo černá, tloušťka 0,50. Ostatní tenké, 0,25, každá svou barvou.
- SKRYTE má typ DASHED, OSA typ CENTER. Ostatní Continuous.
- Hladinu 0 nemažte a nekreslete na ni součást. Je systémová.

> Tip: hladina 0 je schovaná v blocích. Součást na ní se později špatně barví.

## Zadání: šest hladin
*Cvičení*

- V souboru destičky založte OBRYS, OSA, SKRYTE, SRAFA, KOTA a RAM.
- Obrys přesuňte na OBRYS, osy na OSA, rámeček na RAM.
- OSA musí mít typ CENTER a po `_LTSCALE` 0,5 nebo 1 musí být čerchování vidět.
- Vypněte OBRYS. Na obrazovce smí zůstat osy a rámeček, nic z tvaru.

> Tip: když osa vypadá plně, typ čáry není načtený, nebo je LTSCALE mimo rozumné měřítko.


# Kvíz

## Kam patří kóta?
- Na hladinu OBRYS
- [x] Na vlastní hladinu KOTA
- Na hladinu 0
- Kamkoli, hladina je jen barva pozadí
> Kóta má vlastní tenkou čáru a při úpravě obrysu nesmí odjet s ním.

## Co znamená ByLayer?
- Objekt barvu hladiny ignoruje
- [x] Objekt si bere barvu a typ čáry z hladiny
- Hladina je zamčená
- Objekt se netiskne
> ByLayer je správné nastavení. Barva „natvrdo“ na objektu se hladinou už neřídí.

## Čím se liší vypnutá a zmrazená hladina?
- Ničím
- [x] Zmrazená se navíc netiskne a výkres ji nepočítá
- Vypnutá se nedá znovu zapnout
- Zmrazená je vidět červeně
> Vypnutí je jen obrazovka. Zmrazení hladinu vyřadí i z tisku a z přepočtu.

## Proč nekreslit součást na hladinu 0?
- Hladina 0 nejde použít
- [x] Je systémová a v blocích se chová jinak
- Nepouští tlusté čáry
- Smí na ni jen razítko
> Hladina 0 má zvláštní roli uvnitř bloků. Součást patří na pojmenované hladiny.
