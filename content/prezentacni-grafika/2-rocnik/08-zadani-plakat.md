---
id: pg-2-zadani-plakat
nazev: Zadání: plakát
popis: A3 se spadávkou, maskou, křivkami a dvěma exporty.
nazev_cviceni: Kvíz: hotový plakát
nadpis_kvizu: Zkontrolujte tisk i web
---

# Výklad

## Zadání akce
*Obsah*

- Plakát zve na školní den 12. 6. v tělocvičně. Název si zvolte jeden a držte ho.
- Formát A3 na výšku, plátno 303 × 426 mm, 300 ppi, RGB, spadávka 3 mm.
- Na plakátu je fotka, jeden hlavní nápis, datum, místo a malé logo nebo značka školy.
- Text je aspoň 10 mm od čistého ořezu. Ve spadávce smí být jen pozadí.

> Tip: Jeden plakát, jedna akce. Nepřidávejte druhé datum a druhý sál.

## Jak je složený
*Vrstvy*

- Skupina foto: fotka, maska předmětu, křivky jako vrstva úprav, pozadí rozostřené 8 px.
- Skupina grafika: tvarový pruh 75 % a případný další tvar. Stín pruhu 8 / 16 / 40 %.
- Skupina text: název a údaje, pořád jako text, ne jako pixely.
- Vypnutí kterékoli skupiny musí být vidět. Když ne, je skupina prázdná nebo je všechno sloučené.

> Tip: Křivka má bod světel kolem 245 a stínů kolem 15. Doostření předmětu 80 %, 1 px, práh 2.

## Dva výstupy
*Soubory*

- plakat.psd s vrstvami.
- plakat-web.jpg, delší strana 1600 px, sRGB, kvalita 10, bez ořezových značek.
- plakat-tisk.pdf, 303 × 426 mm, ořezové značky, bez vrstev.
- Mockup na zdi je navíc, ne místo PDF. Když ho přidáte, je to mockup.psd.

> Tip: Soubory leží v jedné složce plakat. Název Bez názvu v odevzdání neberte.

## Kontrola
*Než to odevzdáte*

- Na 100 % nemá retuš opakovaný vzor a maska nemá barevný lem.
- Datum 12. 6. je celé a čitelné. Logo není rozmazaný JPG zvětšený z 80 px.
- Webový JPG otevřený na celou obrazovku nemá značky ani šedé okraje plátna.
- PDF má stejný název akce jako PSD. Dvě verze textu jsou chyba.

> Tip: Vytiskněte si PDF na A4 jako náhled. Když je text u kraje už na A4, na A3 bude u řezu taky.

# Kvíz

## Jaké je plátno tiskového plakátu?
- 1920 × 1080 px, 72 ppi
- [x] 303 × 426 mm, 300 ppi
- A4 bez spadávky
- 1600 px
> A3 je 297 × 420, spadávka 3 mm z každé strany přidá 6 mm. 300 ppi je tisk.

## Jak je rozostřené pozadí?
- [x] 8 px, předmět zůstane ostrý
- 8 px přes celý plakát včetně textu
- 80 % Doostření na pozadí
- Nijak
> Rozostření oddělí pozadí. Text a hlavní předmět jsou ostré.

## Co je ve webovém JPG navíc chyba?
- sRGB a 1600 px
- [x] Ořezové značky
- Kvalita kolem 10
- Název akce
> Značky jsou pro tiskárnu. Na webu vypadají jako čáry v rozích.

## Který soubor musí obsahovat vrstvy?
- plakat-web.jpg
- [x] plakat.psd
- plakat-tisk.pdf
- Žádný
> PSD je práce. JPG a PDF jsou ploché kopie.
