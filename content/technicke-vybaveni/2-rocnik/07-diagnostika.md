---
id: hardware-2-diagnostika
puvodni: hw2.7
nazev: Diagnostika
popis: POST, pípání a minimum, které ještě naběhne.
nazev_cviceni: Kvíz: Diagnostika
nadpis_kvizu: Kvíz k tématu Diagnostika
---

# Výklad

## Test hned po zapnutí
*POST*

- Firmware zkouší procesor, paměť a grafiku.
- Kódy na displeji desky jsou přesnější než staré pípání.
- Reproduktor skříně musí být zapojený, jinak ticho nic neřekne.
- Manuál desky překládá kódy. Nejsou u všech stejné.

> Tip: žádný obraz a žádné pípnutí neznamená automaticky mrtvý procesor.

## Uberte, co může lhát
*Minimum*

- Ven grafika, pokud má procesor vlastní obraz.
- Ven disky a karty.
- Zkuste jiný kabel obrazu a jiný port.
- Až jádro naběhne, vracejte díly po jednom.

> Tip: jeden modul paměti v doporučeném slotu porazí čtyři najednou.

## Náhodné pády
*Paměť a disk*

- Test paměti běží déle než jedno pípnutí.
- Chyby čtení disku se projeví v jeho vlastních statistikách.
- Přehřátí shodí stroj až při zátěži, ne v menu BIOSu.
- Vyměněný díl ověřte stejnou zkouškou, která předtím selhala.

> Tip: pád až ve Windows může být paměť, ne jen ovladač.


# Kvíz

## Co POST dělá?
- [x] Po zapnutí zkouší základní díly
- Instaluje systém
- Formátuje SSD
- Stahuje ovladače z internetu
> Je to první test firmware, ne operační systém.

## Proč začít s jedním modulem paměti?
- [x] Špatný modul nebo slot se snáz odhalí
- Deska víc modulů zakazuje
- BIOS umí jen 4 GB
- Druhý modul maže UEFI
> Minimum dílů zužuje chybu.

## Kdy se přehřátí často neprojeví?
- [x] V menu BIOSu bez zátěže
- Při dlouhém testu procesoru
- Ve hře
- Při kódování videa
> Teplota roste až s odběrem.

## Co ověří výměnu dílu?
- [x] Stejná zkouška, která předtím selhala
- Jen to, že ventilátor se točí
- Nová tapeta
- Jiná barva skříně
> Bez opakování testu nevíte, jestli to byla příčina.
