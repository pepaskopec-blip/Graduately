---
id: hardware-3-ups
puvodni: hw3.6
nazev: UPS
popis: Minuty na uložení, ne náhradní elektrárna.
nazev_cviceni: Kvíz: UPS
nadpis_kvizu: Kvíz k tématu UPS
---

# Výklad

## Překlenout výpadek
*Úkol*

- Při výpadku přejde na baterii.
- Čas běhu závisí na odběru, ne na nálepce bez zátěže.
- Server dostane signál a vypne se po pořádku.
- Tiskárna a monitor na UPS servery okrádají o minuty.

> Tip: malá UPS neutáhne učebnu. Utáhne server, než se uloží.

## Ne každá UPS je pro zdroj serveru
*Tvar*

- Sinusový výstup je jistější pro aktivní PFC zdroje.
- Výkon ve voltampérech není totéž co watty na štítku zdroje.
- Přetížená UPS spadne právě ve výpadku.
- Zkouška pod zátěží ukáže skutečné minuty.

> Tip: levná UPS s obdélníkovým průběhem modernímu zdroji vadí.

## Spotřební díl
*Baterie*

- Test baterie nečeká na první bouřku.
- Výměna se dělá dřív, než článek nabobtná.
- Teplá místnost baterii ničí rychleji.
- UPS není náhrada za zálohu dat.

> Tip: baterie po třech letech v teple už slíbené minuty nedá.


# Kvíz

## K čemu malá UPS u serveru je?
- [x] Aby se systém stihl uložit a vypnout
- Aby učebna jela celé odpoledne
- Aby nahradila zálohu disků
- Aby chladila rack
> Jsou to minuty, ne směna.

## Proč na UPS nepatří laserová tiskárna?
- [x] Má velký odběr a sebere čas serverům
- Tiskárna UPS zničí vždy okamžitě
- USB tiskárny UPS zakazuje
- Toner potřebuje jinou frekvenci
> Špička zapékání baterii vybíjí zbytečně.

## Co ukáže jen nálepka bez zátěže?
- [x] Optimistický čas, který v provozu neplatí
- Přesné minuty při plném serveru
- Stav RAID
- Teplotu procesoru
> Měří se s tím, co na UPS opravdu visí.

## Jak často baterie přestane držet slib?
- [x] Po letech, dřív v teple
- Nikdy, je součástí zdroje navždy
- Až po deseti minutách prvního provozu
- Jen když je UPS černá
> Je to spotřební díl a má se zkoušet.
