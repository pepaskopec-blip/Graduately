---
id: pg-3-export
nazev: Export videa
popis: H.264, Full HD, 25 sn/s a 10 Mb/s.
nazev_cviceni: Kvíz: export videa
nadpis_kvizu: Vyberte formát a datový tok
---

# Výklad

## Formát
*H.264*

- Odevzdává se MP4 s kodekem H.264. Otevře ho telefon, projektor i Windows.
- Formát projektu prproj nikdo bez Premiere nepustí. Není to výstup.
- Sekvence 1920 × 1080 a 25 sn/s se do exportu přenese. Neměňte ji při exportu na 1280 × 720, pokud to zadání nechce.
- Zvuk nechte v exportu zapnutý. Němý výstup je často jen odškrtnutý zvuk, ne záměr.

> Tip: Soubor > Export > Média. Zkratka Ctrl+M. Náhled vpravo ukáže první snímek, ne celý střih.

## Datový tok
*10 Mb/s*

- Cílový datový tok 10 Mb/s stačí na školní Full HD. Vyšší soubor zbytečně naroste.
- 1 Mb/s už na 1080p kostkuje, hlavně v pohybu. 10 je klidná střední hodnota.
- Nastavení zdroje, nebo shoda se sekvencí, podrží 25 sn/s a rozměr.
- Dva průchody export zpřesní, ale trvá déle. Na školní video stačí jeden.

> Tip: Mb/s je megabit za sekundu, ne megabajt. Minutové video při 10 Mb/s má zhruba 75 MB.

## Rozsah
*Celá sekvence*

- Exportujte celou sekvenci, ne jen výběr mezi body In a Out, pokud jste je nechali uprostřed.
- Zaškrtnutí, které exportuje jen vstup až výstup, umí oříznout film na pět sekund.
- Název souboru skolni-den.mp4. Cesta ať vede do složky projektu, ne na plochu.
- Po exportu soubor pusťte mimo Premiere. V náhledu programu chybu často nevidíte.

> Tip: Když je film moc krátký, otevřete body In a Out na ose. Ctrl+Shift+X je zruší.

## Zadání: první mp4
*Cvičení*

- Exportujte skolni-den.mp4, H.264, 1920 × 1080, 25 sn/s, cílový tok 10 Mb/s.
- Zvuk je v souboru. Úvodní titulek je hned na začátku, ne po deseti sekundách černé.
- Projekt prproj nechte vedle mp4. Mp4 samo o sobě nejde znovu střihnout po vrstvách.
- Soubor přehrajte celý. Jednou. Zastavte se na titulku, na prolínačce a na konci hudby.

> Tip: Když přehrávač hlásí, že soubor nejde otevřít, export nedoběhl. V Premiere je fronta, podívejte se, jestli neskončil chybou.

# Kvíz

## Který soubor se odevzdává jako video?
- prproj
- [x] mp4 v H.264
- psd
- svg
> MP4 otevře i člověk bez Premiere. Projekt je zdroj.

## Jaký datový tok má školní Full HD?
- [x] 10 Mb/s
- 1 Mb/s
- 300 ppi
- 72 Mb/s vždy
> 10 Mb/s je na 1080p25 dost. 1 Mb/s se v pohybu rozpadá.

## Co zkontrolujete, když je export dlouhý jen pár sekund?
- Spadávku
- [x] Jestli se neexportoval jen úsek mezi In a Out
- Režim CMYK
- Krytí pruhu
> Body In a Out omezí export na výběr. Zrušte je a exportujte celou sekvenci.

## Proč nestačí odevzdat jen prproj?
- Premiere ho neumí uložit
- [x] Bez Premiere ho nikdo nepřehraje a média mohou chybět
- Nemá 25 sn/s
- Je to totéž co mp4
> Projekt je odkaz na střih. Film je až mp4.
