---
id: sw-4-power-bi-prurez
nazev: Power BI: průřez a sestava
popis: Průřez třídy přefiltruje celou stránku. Filtr jednoho vizuálu ostatní nechá.
nazev_cviceni: Kvíz: průřez
nadpis_kvizu: Řekněte, co průřez ovlivní
---

# Výklad

## Průřez
*Celá stránka*

- Průřez na pole Trida je ovládací prvek na plátně. Čtenář klikne na 1.A.
- Sloupcový graf i karta se mají změnit. Celková částka pak není celá škola, ale jen 1.A.
- Když se karta nehne, průřez na ni není napojený, nebo karta bere jinou tabulku.
- Výběr se zruší gumou na průřezu, nebo volbou všech tříd. Data se tím nesmažou.

> Tip: Průřez filtruje. Není to nový součet uložený do Excelu.

## Filtr vizuálu
*Jen jeden graf*

- Filtr v podokně konkrétního vizuálu ovlivní jen ten vizuál.
- Školní sestava pro porovnání tříd filtr vizuálu nepotřebuje. Má ukázat obě, dokud čtenář nezvolí průřez.
- Filtr stránky schová třídu všem vizuálům na stránce, i bez klikání. Na odevzdání ho nenechávejte zapnutý omylem.
- Než sestavu vyexportujete, zrušte výběr průřezu, pokud má PDF ukázat celou školu.

> Tip: PDF zachytí průřez tak, jak je zrovna vybraný. Vybraná 1.A v PDF už druhou třídu neukáže.

## Stránka
*Jedna otázka*

- Stránka se jmenuje Prodej. Název Stránka 1 se neodevzdává.
- Nadpis na plátně říká Prodej po třídách. Graf má popisky tříd.
- Na jedné stránce jsou nejvýš průřez, jeden graf a jedna karta. Druhý graf se stejným obsahem je navíc.
- Barvy nechte klidné. Červená a zelená těsně vedle sebe se špatně čte.

> Tip: Zobrazte sestavu na projektoru, nebo aspoň v celém okně. Drobný popisek, který na vašem monitoru ujde, vzadu ve třídě zanikne.

## Zadání: stránka s průřezem
*Cvičení*

- Na stránku Prodej přidejte průřez Trida.
- Klik na 1.A změní graf i kartu. Po zrušení výběru je zase vidět celý součet.
- PDF vyexportujte bez vybrané třídy, ať ukazuje 1.A i 1.B.
- pbix uložte znovu. V modelu je tabulka Prodej pořád jen jednou.

> Tip: Když průřez nabízí prázdný řádek, v Excelu je prázdný řádek tabulky. Smažte ho ve zdroji a obnovte.

# Kvíz

## Co má průřez na Trida změnit?
- Jen barvu pozadí
- [x] Graf i kartu na stránce
- Soubor xlsx na disku
- Heslo k WordPressu
> Průřez filtruje vizuály na stránce. Karta s celkovou částkou se musí změnit taky.

## Čím se liší filtr jednoho vizuálu od průřezu?
- Ničím
- [x] Filtr vizuálu platí jen pro něj, průřez je ovládání pro stránku
- Filtr vizuálu smaže Excel
- Průřez nejde zrušit
> Průřez je na plátně pro čtenáře. Filtr v podokně vizuálu ostatní grafy nechá.

## Jaké PDF se odevzdává, když mají být vidět obě třídy?
- S vybranou jen 1.A
- [x] Bez výběru v průřezu
- Prázdná stránka
- Jen karta bez grafu a bez tříd
> PDF drží aktuální výběr. Než ho uděláte, výběr zrušte.

## Jak se má stránka jmenovat?
- Stránka 1
- [x] Prodej
- pbix
- DELETE
> Název říká, co na stránce je. Výchozí Stránka 1 se neodevzdává.
