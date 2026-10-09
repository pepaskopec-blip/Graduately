# Obsah aplikace

Všechny texty lekcí, cvičení, kvízů a knih jsou tady v Markdownu. Aplikace pro
Android, iOS, macOS, Linux i Windows z nich při sestavení vyrobí jeden soubor
`content.json` (skript `scripts/build-content.py`). Na kód se kvůli obsahu sahat
nemusí.

Kontrola po úpravě:

```sh
python3 scripts/build-content.py
```

Skript vypíše počet lekcí, nebo chyby s cestou a číslem řádku
(`content/fyzika/1-rocnik/02-pohyb.md:41: …`). Při chybě se aplikace nesestaví.

## Kde co je

| Složka | Obsah |
|---|---|
| `pocitacove-site/`, `technicke-vybaveni/`, `obcanska-nauka/`, `fyzika/` | `N-rocnik/`, v nich lekce |
| `prirodni-vedy/chemie`, `prirodni-vedy/biologie` | lekce |
| `cestina/literatura/N-rocnik` | lekce literatury |
| `cestina/mluvnice` | cvičení z mluvnice |
| `cestina/cetba` | knihy k maturitní četbě |
| `matematika/0-opakovani`, `matematika/N-rocnik` | lekce s příklady |
| `anglictina/N-rocnik`, `nemcina/N-rocnik` | lekce s výkladem, čtením, poslechem, psaním |
| `nemcina/ucebnice` | učebnice němčiny po kapitolách |
| `aplikace/texty.md` | texty rozhraní (tlačítka, nadpisy obrazovek) |
| `aplikace/predmety.md` | dlaždice předmětů na úvodní obrazovce |
| `aplikace/novinky.txt` | seznam změn zobrazovaný v aplikaci |

Jedna lekce je jeden soubor `NN-nazev.md`. Pořadí v aplikaci určuje číslo na
začátku názvu. Soubory, které začínají `_`, nejsou lekce (např. `_lekce.md`
v učebnici popisuje kapitolu).

## Angličtina

Anglická verze leží vedle české jako `NN-nazev.en.md`. Co v ní chybí, zobrazí
se česky. Obsahuje:

- hlavičku se stejnými poli jako česká (`nazev`, `popis`, …), ale anglicky,
- volitelně tabulku `# Překlady`, kde je každý český text z lekce přeložený:

```md
---
nazev: Physical quantities
popis: SI, scalar and vector, measurement error.
---

# Překlady

| Česky | Anglicky |
|---|---|
| Fyzikální veličina má číselnou hodnotu a jednotku. | A physical quantity has a value and a unit. |
```

Český text musí v tabulce sedět přesně (včetně interpunkce). Pole `id`
a `puvodni` do `.en.md` nepatří.

## Společná pravidla zápisu

Hlavička mezi `---` obsahuje pole `klíč: hodnota`. Pak následují oddíly `#`,
položky `##` a podle potřeby `###`.

| Zápis | Význam |
|---|---|
| `- text` | odrážka; v kvízu možnost |
| `- [x] text` | správná možnost |
| `1. text` | číslovaná položka (např. části děje) |
| `> text` | vysvětlení, nápověda, tip nebo význam (podle oddílu) |
| `= odpověď` | správná odpověď; více variant odděluje `\|`: `= hasn't\|has not` |
| `[odpověď]` | mezera k doplnění uvnitř věty: `Ich [bin\|heiße] Anna.` |
| `\| a \| b \|` | tabulka; v buňce se píše `\|` jako `\\|` a nový řádek jako `\n` |
| `<!-- … -->` | poznámka, do aplikace se nedostane |

Hranaté závorky, které nemají být mezerou, se píšou `\[` a `\]`. Řádek, který
by jinak vypadal jako značka (začíná `-`, `>`, `=` …), se uvede zpětným
lomítkem: `\- tohle je obyčejný text`.

## ID a postup žáků

```md
id: fyzika-1-fyzikalni-veliciny
puvodni: fyz.1
```

- **`id`** je trvalé jméno lekce. Pod ním se ukládá, co má žák hotové. Musí být
  v celém obsahu jedinečné. **Po vydání ho neměňte**, jinak se žákům postup
  v lekci ztratí. Název souboru, číslo pořadí i nadpis měnit lze.
- **`puvodni`** je adresa lekce ze starších verzí aplikace. Podle ní se při
  prvním spuštění nové verze převede starý postup. Neměňte ji a u nových lekcí
  ji nepište.

Nová lekce: zkopírujte podobný soubor, dejte mu číslo podle pořadí, nové `id`
a smažte `puvodni`.

## Běžná lekce

Sítě, hardware, občanka, literatura, chemie, biologie, fyzika.

```md
---
id: fyzika-1-fyzikalni-veliciny
nazev: Fyzikální veličiny
popis: SI, skalár a vektor, chyba měření.
nazev_cviceni: Kvíz: veličiny
nadpis_kvizu: Ověřte si jednotky a chyby
---

# Výklad

## Veličina a jednotka
*Měření*

- Fyzikální veličina má číselnou hodnotu a jednotku.
- Soustava SI má sedm základních jednotek.

> Tip: základní jednotka hmotnosti je kilogram, ne gram.

# Kvíz

## Která jednotka je základní jednotka SI pro hmotnost?
- Gram
- [x] Kilogram
- Newton
> Gram je tisícina kilogramu. Newton je jednotka síly.
```

Každá `##` ve výkladu je jeden snímek. Řádek `*Měření*` je štítek snímku,
počítadlo „1 / 3“ doplní aplikace sama.

V první lekci sítí je navíc oddíl `# Úlohy na podsítě`: `## Úloha N` se
zadáním, `### Řešení` s textem řešení a `### Kontrola` s tabulkou
`Prefix | Síť | Broadcast | První adresa | Poslední adresa`.

## Matematika

Stejná hlavička a `# Výklad` jako běžná lekce, místo kvízu `# Příklady`:

```md
# Příklady

## Jaký je největší společný dělitel čísel 24 a 36?
= num:12
> Společní dělitelé jsou 1, 2, 3, 4, 6 a 12.
```

Druhy odpovědí:

| Odpověď | Kdy je správně |
|---|---|
| `num:12`, `num:1/2` | číslo nebo výraz (zlomek, odmocnina, π); stačí stejná hodnota v jakémkoli tvaru |
| `set:2;3` | všechna čísla v libovolném pořadí |
| `pair:2;-1` | čísla v daném pořadí (např. x; y) |
| `word:ano\|yes` | jedno ze slov |
| `square:2,3;5,3`, `right:…`, `mid:…`, `para:…`, `points:1,4;fix:1,1` | rýsování do mřížky 10 × 8; `fix:` jsou body předem zakreslené |

## Angličtina a němčina po ročnících

```md
---
id: anglictina-1-ja-a-moje-rodina
nazev: Já a moje rodina
popis: Sloveso be, have got. Doplňování a kvíz.
---

# Výklad
(snímky jako u běžné lekce)

# Čtení
Text článku jedním odstavcem.

## The bus is ___ than the train.
- faster
- [x] slower
> cheaper, but slower.

# Poslech
Přepis nahrávky, pak otázky stejně jako u čtení.

# Doplňování

## My sister ___ sixteen.
= is
> Věk: be, ne have.

# Psaní
Zadání.

## Vzorová odpověď
Text.

## Musí obsahovat
- would
- because

## Minimum slov
35

# Kvíz
(jako u běžné lekce)
```

Oddíly, které lekce nepotřebuje, se vynechají.

## Mluvnice (`cestina/mluvnice`)

Hlavička: `id`, `typ`, `zkratka` (krátký název na mapě), `nazev`, `popis`,
volitelně `poznamka`. Úlohy jsou v oddílu `# Úlohy`, podle typu:

```md
typ: doplnovani

## Stará bab_čka sed_la na lav_čce.
- i/í/y/ý = babička
- i/í/y/ý = seděla|sedla
> babička, seděla, lavičce
```

Každá odrážka je jedno políčko: vlevo nápověda, za `=` správné odpovědi.

```md
typ: vyber

## Narodil se v ______ v malé vesnici.
- [x] Čechách
- čechách
```

```md
typ: reseni

## napsali
= na- / ps- / -a- / -li
```

U `reseni` si žák řešení jen odkryje.

## Četba (`cestina/cetba`)

```md
---
id: 1984
autor: George Orwell
nazev: 1984
zanr: Próza
preklad: Šimečková
---

# O knize
- Autor: George Orwell

# Postavy
- Winston Smith – hlavní hrdina.

# Kvíz
## Kdo je hlavní hrdina?
- [x] Winston Smith
- O'Brien
> Vysvětlení.

# Děj
## Přetáhněte části příběhu do správného pořadí:
1. První část děje.
2. Druhá část děje.
> Volitelně význam celého děje.
```

Všechny oddíly kromě `Kvíz` a `Děj` jsou poznámky ke knize, zobrazí se
v uvedeném pořadí. Kvíz i děj jsou nepovinné. Volitelná pole v hlavičce:
`popis`, `kviz_nadpis`, `kviz_popis`, `dej_nadpis`, `dej_popis` (jinak se
použijí obecné texty) a `stranka`/`plna` pro knihy s vlastní obrazovkou.

## Učebnice němčiny (`nemcina/ucebnice`)

Každá kapitola je složka `NN-nazev/` se souborem `_lekce.md`:

```md
---
id: ucebnice-1
nazev: Neue Freunde
popis: Vyberte cvičení a dokončete je.
zamceno: ano
---
```

(`zamceno: ano` kapitolu v aplikaci zamkne.) V kapitole jsou cvičení
`NN-nazev.md` a slovíčka `slovicka.md`. Každé cvičení má v hlavičce `typ`.
Nejčastější typy:

| Typ | Oddíly |
|---|---|
| `choice` | `# Otázky` jako kvíz |
| `typed` | `# Úlohy`: tabulka `Zadání \| Odpověď \| Význam` |
| `verb`, `fill` | `# Nabídka` (odrážky slov), `# Úlohy`: tabulka s větami a mezerami `[…]` |
| `dialog` | `# Nabídka`, `# Rozhovory` s `## názvem` a tabulkou `Kdo \| Věta` |
| `assign` | `# Skupiny` (odrážky), `# Slova`: tabulka `Obrázek \| Slovo \| Skupina` |
| `free` | `# Otázky`: tabulka `Otázka \| Překlad otázky \| Vzorová odpověď \| Překlad odpovědi` |
| `vocab` | `# Slovíčka`, `## Strana N` a tabulka `Německy \| Odpověď \| Význam` |

Další typy (`assembly`, `number`, `count`, `seq`, `verbclue`, `profile`,
`hangman`, `ordne`, `letters`, `ex2`, `table`, `kw` …) mají vždy aspoň jeden
vzor v existujících kapitolách; nejjednodušší je zkopírovat ho. Oddíl
`# Překlad` obsahuje české překlady položek ve stejném pořadí.

Volitelná pole hlavičky cvičení: `tip`, `ukazka` (vzorový příklad),
`vyznam_predem`, `preklad_predem`, `odhalit_nemcinu`, `ukazat_nemcinu`
(`ano`/`ne`) a `poznamka_k_odpovedim`.

## Texty rozhraní a předměty

`aplikace/texty.md` obsahuje tabulky `Klíč | Česky | Anglicky`. Klíč používá
kód aplikace, neměňte ho; texty ve sloupcích měnit lze. `%s` a `%d` jsou místa,
kam aplikace doplní hodnotu.

`aplikace/predmety.md` má tabulku
`Klíč | Česky | Anglicky | Ikona | Cíl | Otevřeno`. `Cíl` je obrazovka, která se
otevře, `Otevřeno: ne` dlaždici zamkne.
