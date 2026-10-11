---
id: sw-2-zadani-office
nazev: Zadání: protokol a tabulka
popis: Dopis ve Wordu a známky v Excelu v jedné složce.
nazev_cviceni: Kvíz: odevzdání Office
nadpis_kvizu: Zkontrolujte oba soubory
---

# Výklad

## Word
*Žádost*

- zadost.docx je jedna strana A4, okraje 2,5 cm, datum 12. 6. 2026 vpravo nahoře.
- Je v ní řádek Věc, oslovení, aspoň dvě věty a podpis. Není v ní obsah ani tabulka.
- K protokolu z dřívějška, pokud ho máte, patří styly Nadpis 1 a PDF.
- PDF se jmenuje stejně jako docx. Dvě verze final a final2 se neodevzdávají.

> Tip: Náhled tisku má ukázat jednu stránku. Podpis na druhé stránce znamená moc velké mezery.

## Excel
*Známky*

- znamky.xlsx má list Znamky: jméno, známka, stav.
- Stav je vzorec =KDYŽ(D2<=4;"prospěl";"neprospěl") nebo se sloupcem známky, který opravdu máte. Známka 5 je neprospěl.
- Pod tabulkou je PRŮMĚR známek, ne jejich součet.
- Čísla jsou čísla. Text 1 nebo 120 Kč ve stejné buňce se nepočítá.

> Tip: Klik na buňku stavu musí v řádku vzorců ukázat KDYŽ. Opsané slovo prospěl se při změně známky nehne.

## Kontrola vzorců
*Ne opis*

- V ceníku, pokud ho přikládáte, je částka =B2*C2 a sazba ukotvená jako $F$1.
- SVYHLEDAT, pokud ho máte, končí na NEPRAVDA a oblast je ukotvená.
- Graf tržby je spojnicový, má název a není to koláč dvanácti měsíců.
- Kontingenční tabulka, pokud je v sešitu, stojí na vlastním listu a sčítá částky po třídách.

> Tip: Stačí žádost a známky. Ceník, graf a kontingence jsou navíc, ale když je přiložíte, musí sedět.

## Složka
*Dva hlavní soubory*

- Ve složce Skola\1-rocnik\word je zadost.docx a zadost.pdf.
- Ve složce Skola\1-rocnik\excel je znamky.xlsx.
- Na ploše tyto soubory nejsou. Přípony jsou vidět.
- Otevřete oba znovu. Datum, věc a aspoň jedna pětka se stavem neprospěl mají být vidět.

> Tip: Když PDF nejde otevřít, export nedoběhl. Udělejte ho z docx znovu.

# Kvíz

## Jaký je školní dopis z tohoto zadání?
- Tři strany se styly a obsahem
- [x] Jedna strana A4 s řádkem Věc a datem 12. 6. 2026
- Jen tabulka 4×3
- Prezentace
> Žádost je jeden list. Obsah a Nadpis 1 patří protokolu, ne tomuto dopisu.

## Který vzorec označí pětku jako neprospěl?
- [x] =KDYŽ(D2<=4;"prospěl";"neprospěl")
- =KDYŽ(D2>=4;"prospěl";"neprospěl")
- napsané slovo prospěl
- =SUMA(D2)
> Prochází známka 1 až 4. Pětka padne do druhé větve. V češtině jsou středníky.

## Co patří pod sloupec známek?
- Součet známek
- [x] Průměr
- Kontingenční součet peněz
- Nic, známky se mažou
> Součet známek nemá význam. PRŮMĚR ano.

## Které soubory se odevzdávají jako základ?
- Jen PDF bez docx
- [x] zadost.docx, zadost.pdf a znamky.xlsx
- Jen xlsx na ploše
- prezentace a video
> Word se odevzdává i jako zdroj, i jako PDF. Excel je xlsx, protože v PDF by nebyly vzorce.
