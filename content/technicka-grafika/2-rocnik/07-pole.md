---
id: tg-2-pole
nazev: Obdélníkové a polární pole
popis: Opakování děr v řadě nebo na kružnici.
nazev_cviceni: Kvíz: pole
nadpis_kvizu: Dopočítejte rozteč a počet
---

# Výklad

## Obdélníkové pole
*Řady a sloupce*

- `_ARRAYRECT` rozkopíruje objekt do řádků a sloupců.
- Zadává se počet sloupců, počet řádků a rozteč, ne celkový rozměr.
- Rozteč 30 při třech sloupcích dá od první osy ke třetí 60 mm.
- Pole může zůstat svázané. Úprava jedné díry se propíše do ostatních.

> Tip: tři díry v řadě mají dvě mezery. Celková délka je dvojnásobek rozteče, ne trojnásobek.

## Polární pole
*Na kružnici*

- `_ARRAYPOLAR` chce střed otáčení, počet prvků a úhel vyplnění.
- Čtyři díry po 360 stupních jsou po 90 stupních.
- Vzorová díra musí ležet na roztečné kružnici dřív, než pole spustíte.
- Střed pole je střed příruby, ne střed první díry.

> Tip: roztečná kružnice Ø60 má poloměr 30. Vzorová díra leží třeba v bodě 30,0.

## Roztečná kružnice se nekreslí tlustě
*Osa*

- Kružnice, na které leží středy děr, je tenká čerchovaná osa.
- Není to hrana součásti a nepatří na OBRYS.
- Kótuje se jejím průměrem, často jako Ø60, a počtem děr.
- Samotné díry mají svou kótu průměru jen jednou.

> Tip: na výkresu čtete 4× Ø8 na Ø60. Osm je díra, 60 je kružnice středů.

## Zadání: čtyři díry na kružnici
*Cvičení*

- Příruba je kružnice Ø80, střed 0,0.
- Vzorovou díru Ø8 dejte středem do 30,0.
- `_ARRAYPOLAR` kolem 0,0, čtyři prvky, 360 stupňů.
- Roztečnou kružnici Ø60 dejte na hladinu OSA. Ověřte `_DIST` mezi sousedními středy.

> Tip: sousední středy svírají 90 stupňů na poloměru 30. Vzdálenost je 30 × √2, asi 42,4 mm.


# Kvíz

## Tři díry v řadě mají rozteč 30. Jak daleko je první osa od třetí?
- 30
- 90
- [x] 60
- 120
> Mezi třemi dírami jsou dvě mezery. 2 × 30 = 60.

## Kam se umísťuje vzorová díra pro pole na Ø60?
- Do středu příruby
- [x] Na poloměr 30 od středu
- 60 mm od středu
- Kamkoli, pole si poloměr dopočítá
> Ø60 znamená poloměr 30. Střed vzorové díry musí na této kružnici už ležet.

## Čím je nakreslená kružnice středů děr?
- Tlustým obrysem
- [x] Tenkou čerchovanou osou
- Šrafou
- Kótovací čarou
> Není to hrana materiálu. Je to osa rozteče a patří na hladinu OSA.

## Co znamená zápis 4× Ø8 na Ø60?
- Čtyři díry o průměru 60
- [x] Čtyři díry Ø8, jejich středy leží na Ø60
- Osazení 4 mm
- Zkosení 8 × 60
> První průměr je díra, druhý je roztečná kružnice.
