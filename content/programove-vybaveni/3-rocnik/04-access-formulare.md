---
id: sw-3-access-formulare
nazev: Access: formulář a sestava
popis: Zápis přes formulář a tisk přes sestavu. Datový list není pro uživatele.
nazev_cviceni: Kvíz: formulář
nadpis_kvizu: Vyberte, čím se data zadávají a čím tisknou
---

# Výklad

## Formulář
*Jeden žák*

- Formulář ukáže jeden záznam jako políčka. Datový list ukáže mřížku a tam se běžný uživatel snadno trefí do špatného řádku.
- Formulář Zaci se založí z tabulky zaci. Pole id ať je vidět, ale zamčené. Automatické číslo se nepíše.
- trida_id je lépe rozbalovací seznam, který ukazuje název třídy a ukládá id.
- Formulář se jmenuje frmZaci. Výchozí Formulář1 se neodevzdává.

> Tip: Rozbalovací seznam bere řádky z tabulky tridy. Viditelný sloupec je nazev, vázaný sloupec je id.

## Sestava
*Tisk*

- Sestava je na tisk a na PDF. Formulář je na zápis. Neslouží k tomu totéž.
- Sestava žáků je seskupená podle třídy a v rámci třídy seřazená podle příjmení.
- Záhlaví skupiny ukáže 1.A jednou, ne na každém řádku jako jediný nadpis celé sestavy.
- Do sestavy nepatří tlačítka formuláře. Patří do ní nadpis Evidence žáků a číslo stránky.

> Tip: Náhled sestavy je před tiskem povinný. Prázdná druhá stránka znamená velkou zápatí nebo prázdný oddíl.

## Kdo kam smí
*Ne do návrhu*

- Uživatel, který jen zapisuje žáky, dostane formulář. Návrhové zobrazení ne.
- Mazání záznamu na formuláři je vypnuté, dokud učitel neřekne jinak.
- Sestava se needituje. Když v ní přepíšete jméno, často přepisujete data, a to z sestavy dělat nechcete.
- Záloha je kopie accdb před mazáním. Jediný soubor na ploše záloha není.

> Tip: Soubor, Uložit jako, a datum v názvu: evidence-2026-06-12.accdb. Pak teprve mažete zkušební řádky.

## Zadání: formulář a sestava
*Cvičení*

- frmZaci zapíše jméno, příjmení a třídu. id nejde přepsat.
- Sestava seskupí žáky podle třídy. Anna Nováková je pod 1.A.
- Z formuláře přidejte jednoho žáka do 1.B a v sestavě ho najděte.
- Uložte vše do evidence.accdb. PDF sestavy je evidence-zaci.pdf vedle souboru.

> Tip: Když v rozbalovacím seznamu vidíte čísla místo 1.A, zobrazujete id. Přepněte šířku sloupců, ať je vidět nazev.

# Kvíz

## Čím se ve škole zadává nový žák?
- Přímo v návrhovém zobrazení tabulky
- [x] Formulářem frmZaci
- Přepsáním sestavy
- Excelem místo Accessu bez uložení accdb
> Formulář ukáže jeden záznam a umí seznam tříd. Návrh tabulky je pro autora databáze.

## K čemu je sestava?
- K mazání sloupců
- [x] K tisku a PDF, seskupenému podle třídy
- K nastavení automatického čísla
- K instalaci Accessu
> Sestava formátuje výstup. Zápis zůstává formuláři.

## Proč je id na formuláři zamčené?
- [x] Automatické číslo se nepíše ručně
- Access id neumí ukázat
- Jinak nejde sestava
- Protože je to text
> Číslo přidělí Access. Ruční přepsání by rozbilo klíč.

## Co uděláte před mazáním zkušebních řádků?
- Nic, accdb se sám verzuje
- [x] Kopie souboru s datem v názvu
- Smažete vztah
- Export jen do JPG
> Jedna kopie accdb je záloha. Mazání bez ní nejde vzít zpět tak snadno jako Ctrl+Z ve Wordu.
