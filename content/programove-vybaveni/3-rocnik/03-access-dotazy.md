---
id: sw-3-access-dotazy
nazev: Access: dotazy
popis: Dotaz, který ukáže jen třídu 1.A, a kritérium v uvozovkách.
nazev_cviceni: Kvíz: dotazy
nadpis_kvizu: Doplňte kritérium
---

# Výklad

## Dotaz není kopie
*Pohled*

- Dotaz data nekopíruje do nové tabulky. Přečte je a ukáže výsledek.
- Když ve třídě přibude žák, stejný dotaz ho příště ukáže taky.
- Dotaz se dělá v návrhovém zobrazení. Tabulky se přidají a pole se přetáhnou dolů.
- Uložený dotaz se jmenuje zaci_1A. Dotaz1 se neodevzdává.

> Tip: Když dotaz otevřete a data změníte, měníte zdrojovou tabulku. Na prohlížení to vadit nemusí, na omyl ano.

## Kritérium
*1.A*

- Pod pole nazev z tabulky tridy patří kritérium "1.A" včetně uvozovek.
- Bez uvozovek Access text nepozná a dotaz selže, nebo se zeptá na parametr.
- Číselné kritérium uvozovky nemá. id 1 je prostě 1.
- Chcete-li žáky obou tříd, kritérium 1.A tam nedávejte. Filtr, který tam zbyl z minula, výsledek ořeže.

> Tip: Řádek Kritéria je filtr. Řádek Zobrazit schová sloupec, který v dotazu být musí, ale ve výsledku ho nechcete.

## Spojení v dotazu
*Obě tabulky*

- Do dotazu přidejte tridy i zaci. Access použije vztah, který už máte.
- Ven dejte prijmeni, jmeno a nazev třídy. id ven dávat nemusíte.
- Řazení podle prijmeni je vzestupně. V návrhové mřížce je na to řádek Řadit.
- Žák bez třídy se při vnitřním spojení neukáže. Díky integritě takový řádek stejně nemá vzniknout.

> Tip: Když se každý žák ukáže dvakrát, tabulky nejsou spojené a Access udělal všechny kombinace. Vraťte vztah.

## Zadání: jen 1.A
*Cvičení*

- Dotaz zaci_1A ukáže jméno, příjmení a název třídy, jen pro 1.A, seřazené podle příjmení.
- Anna Nováková v něm je. Žák z 1.B v něm není.
- Uložte dotaz do evidence.accdb. Výsledek nekopírujte do Excelu jako jediný výstup.
- Změňte v tabulce příjmení Anny na Novák a dotaz otevřete znovu. Má ukázat Novák.

> Tip: Když se zeptá na parametr 1.A, kritérium nemá uvozovky a Access ho bere jako otázku.

# Kvíz

## Kam patří podmínka "1.A"?
- Do názvu souboru
- [x] Do řádku Kritéria pod polem nazev
- Do primárního klíče místo id
- Do kaskádového odstranění
> Kritéria filtrují řádky. Pole nazev drží text 1.A.

## Proč jsou u 1.A uvozovky?
- [x] Access jinak text pochopí jako parametr nebo chybu
- Protože 1.A je číslo
- Uvozovky se v Accessu nepoužívají
- Aby se zapnula kaskáda
> Textové kritérium se píše v uvozovkách. Číslo ne.

## Co se stane s dotazem, když do třídy 1.A přidáte žáka?
- Dotaz je stará kopie a žáka neukáže
- [x] Po novém otevření ho ukáže
- Access databázi zavře
- Musíte ho opsat do Excelu
> Dotaz čte tabulky. Není to zmrazený export.

## Proč se žák v dotazu objeví dvakrát u každé třídy?
- Má dva primární klíče
- [x] Tabulky v dotazu nejsou spojené vztahem
- Je seřazený podle příjmení
- Filtr je vypnutý
> Bez spojení Access zkombinuje každý řádek s každým. Vztah tomu zabrání.
