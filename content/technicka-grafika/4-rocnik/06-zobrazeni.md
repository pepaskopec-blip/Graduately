---
id: tg-4-zobrazeni
nazev: Vizuální styly
popis: Drát, skryté hrany a stínování pro kontrolu tvaru.
nazev_cviceni: Kvíz: zobrazení
nadpis_kvizu: Vyberte styl podle úkolu
---

# Výklad

## Drát a skryté hrany
*Kontrola*

- Drátový model ukáže každou hranu. Hodí se, když saháte na zadní vrchol.
- Styl skrytých hran schová, co je za materiálem. Tvar se čte jako na fotografii bez stínu.
- `_VSCURRENT` styly přepíná. Na pásu karet je stejná nabídka u pohledu.
- Styl nemění těleso. Špatně odečtená díra zůstane špatně v každém stylu.

> Tip: nejdřív drát, ať trefíte hranu, potom skryté hrany, ať zkontrolujete, že díra opravdu prochází.

## Stínování
*Objem*

- Konceptuální nebo stínovaný styl ukáže, jestli je díra průchozí, nebo je to jen namalovaná kružnice.
- Barva tělesa se bere z hladiny. OBRYS a těleso smí mít vlastní hladinu MODEL.
- Materiál a render na škole stačí náznak. Odevzdává se výkres, ne obrázek.
- Stínování zpomalí otáčení. Na slabším počítači se vraťte do drátu, než budete modelovat dál.

> Tip: když v řezu vidíte dno díry, válec byl krátký. Rozdíl stěnu neprošel.

## Řezy bez rozbití
*Pohled*

- Živý řez v kombinaci se stínováním je nejrychlejší kontrola tloušťky stěny.
- Pohled uložte `_VIEW`, ať se k němu vrátíte. Název třeba REZ-AA.
- Izometrický stínovaný pohled smí přijít na výkres jako názorný doplněk, ne jako jediný obraz.
- Barvy na obrazovce pořád tiskněte černě. Styl zobrazení není tabulka per.

> Tip: uložený pohled nese i styl. Když REZ-AA otevřete a je z něj drát, někdo styl přepsal.

## Zadání: tři kontroly destičky
*Cvičení*

- Destičku s dírou Ø16 zobrazte jako drát, jako skryté hrany a stínovaně.
- Ve stínovaném horním pohledu musí být skrz díru vidět. Když není, opakujte rozdíl.
- Uložte pohled HORNI a pohled PREDNI.
- Do výkresu tyto pohledy zatím nevkládejte. Jde o kontrolu modelu.

> Tip: díra, která ve stínu má dno stejné barvy jako destička, je slepá. Výška válce byla menší než 10 mm.


# Kvíz

## Který styl nechá vidět i zadní hrany?
- Stínovaný
- [x] Drátový
- Skryté hrany
- Monochrome.ctb
> Drát nekryje nic. Skryté hrany a stín zadní geometrii schovají.

## Změní vizuální styl objem tělesa?
- Ano, stín ubere materiál
- [x] Ne
- Jen u rozdílu
- Jen při tisku
> Styl je způsob kresby na obrazovce. `_MASSPROP` zůstane stejný.

## Co máte vidět skrz průchozí díru ve stínovaném pohledu?
- Dno stejné barvy
- [x] Prostor za destičkou
- Šrafu ANSI31
- Razítko
> Dno znamená, že válec díry byl krátký a rozdíl stěnu neprošel.

## Čím se na papír dostane černá čára, když je model na obrazovce modrý?
- Vizuálním stylem
- [x] Tabulkou per při tisku
- Smazáním hladiny
- `_ORBIT`
> Barva na monitoru je pracovní. monochrome.ctb z ní na PDF udělá černou.
