---
id: pg-2-krivky
nazev: Úrovně a křivky
popis: Světlost a kontrast jako vrstva úprav, ne jako přepis pixelů.
nazev_cviceni: Kvíz: křivky
nadpis_kvizu: Poznejte nedestruktivní úpravu
---

# Výklad

## Vrstva úprav
*Nad pixely*

- Vrstva úprav mění vzhled všeho pod sebou a pixely fotky nepřepisuje.
- Přidá se kolečkem dole v panelu Vrstvy. Položka Křivky nebo Úrovně.
- Když úpravu vypnete okem, fotka je jako na začátku.
- Úprava přímo přes Obraz > Přizpůsobení fotku přepíše. Ve školní práci ji neberte.

> Tip: Vrstva úprav má vlastní masku. Černou v masce úpravu na části obrazu vypnete.

## Úrovně
*Tři body*

- Černý bod říká, co má být nejtmavší. Bílý bod, co má být nejsvětlejší.
- Šedý bod srovná nádech. Klik do místa, které má být neutrální šedé.
- Bod světel nechte kolem 245, ne na 255, ať v obloze nezmizí kresba.
- Bod stínů kolem 15, ne na 0, ať černá není slitá.

> Tip: Když histogram na obou koncích nic nemá, fotka je mdlá. Body přitáhněte k místu, kde data začínají.

## Křivka S
*Kontrast*

- Křivka zdola zleva nahoru doprava je světlost od stínů ke světlům.
- Jemná křivka do S zvedne kontrast: stíny o trochu níž, světla o trochu výš.
- Ostré S přepálí tváře a zabije stíny. Stačí malý oblouk.
- Jedna vrstva Křivky na celou fotku. Druhou přidejte jen na oblohu, a maskou ji tam omezte.

> Tip: Na křivce táhněte málo. Oko si na přepálený náhled rychle zvykne a původní fotka pak vypadá špatně, i když byla lepší.

## Zadání: dvě úpravy
*Cvičení*

- Nad fotku dejte vrstvu úprav Křivky. Bod stínů nechte u 15, bod světel u 245, mezi tím jemné S.
- Druhou křivku omezte maskou jen na pozadí, ať tvář nejde do přepalu.
- Obě úpravy pojmenujte. Fotka vespod ať nemá žádné Přizpůsobení zapékané v pixelech.
- Uložte krivky.psd. Vypnutí obou úprav vrátí původní světlost.

> Tip: Alt při tahu černého a bílého bodu v Úrovních ukáže, které místo už je slité. U křivek stačí oko a 100 %.

# Kvíz

## Kam patří školní zesvětlení fotky?
- Obraz > Přizpůsobení, přímo do pixelů
- [x] Vrstva úprav nad fotkou
- Do textové vrstvy
- Do exportu JPG jako jediná úprava
> Vrstva úprav jde vypnout. Přizpůsobení přes nabídku Obraz pixely přepíše.

## Kam dát bod světel, aby obloha neztratila kresbu?
- 0
- [x] kolem 245
- 255 a přes
- 28 pt
> 255 je čistá bílá bez kresby. 245 nechá ve světlech detail.

## Co dělá jemná křivka ve tvaru S?
- Sníží rozlišení
- [x] Zvedne kontrast
- Převede RGB na vektor
- Přidá spadávku
> Stíny jdou trochu níž a světla trochu výš. Kontrast stoupne, barva se nemění v jiný režim.

## Čím se křivka omezí jen na pozadí?
- Gumou na fotce
- [x] Maskou vrstvy úprav
- Exportem
- Krytím textu
> Černá v masce úpravy řekne: tady křivku nepoužívej. Tvář může zůstat bílá, tedy upravená jinou vrstvou, nebo naopak.
