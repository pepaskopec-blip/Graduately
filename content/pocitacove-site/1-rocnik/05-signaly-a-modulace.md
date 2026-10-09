---
id: site-1-signaly-a-modulace
puvodni: net.5
nazev: Signály a modulace
popis: 4 snímky o signálech a modulaci a pak kvíz.
nazev_cviceni: Kvíz: signály a modulace
nadpis_kvizu: Kvíz k signálům a modulaci
---

# Výklad

## Analogový a digitální signál
*Signál*

- Analogový signál je spojitý. Mezi dvěma hodnotami nabývá i všech hodnot mezi nimi, třeba napětí na mikrofonu nebo teplota.
- Digitální signál je diskrétní. V počítači nabývá jen hodnot 0 a 1, takže se dá uložit a zkopírovat bez postupného šumu.
- Perioda T je doba jednoho opakování. Frekvence f = 1/T říká, kolikrát za sekundu se průběh zopakuje. Jednotka je hertz.
- Amplituda A je velikost výchylky od středu. Fázový posun Φ říká, o kolik je průběh posunutý proti jinému signálu se stejnou frekvencí.

> Tip: frekvence a perioda jsou převrácené hodnoty. Platí f = 1/T.

## Základní typy modulace
*Modulace*

- AM, amplitudová modulace, mění velikost nosné podle zprávy. Používá ji třeba rozhlas na středních vlnách. Rušení amplitudu snadno pokazí.
- FM, frekvenční modulace, mění kmitočet nosné a amplitudu nechává. VKV rádio je odolnější proti šumu než AM.
- PM, fázová modulace, posouvá fázi nosné. Změna fáze a změna frekvence spolu souvisejí, proto se FM a PM často popisují podobně.
- Bez modulace by pomalá zpráva, třeba hlas, na rádiové frekvenci sama neodešla. Nosná je rychlá vlna, zpráva ji jen tvaruje.

> Modulace nasadí zprávu na nosnou vlnu, aby prošla kabelem nebo éterem. Tomu se říká přenos v přeloženém pásmu.

## QPSK a 256-QAM
*Kombinované*

- QPSK má čtyři fázové stavy. Čtyři možnosti jsou dva bity, takže jeden symbol nese 2 bity.
- 256-QAM skládá amplitudu i fázi do 256 stavů. 256 = 2^8, takže jeden symbol nese 8 bitů.
- Wi-Fi a kabelové modemy QAM používají, protože za stejnou šířku pásma přenesou víc dat než prosté AM.
- Hustší konstelace chce čistší signál. Když je rušení velké, přijímač spadne na řidší modulaci s menším počtem bitů na symbol.

> Jeden stav signálu, symbol, může nést víc než jeden bit. Čím víc stavů, tím víc bitů, ale tím snáz se stavy zamění.

## Modulační, přenosová a šířka pásma
*Rychlosti*

- Modulační rychlost je počet změn signálu za sekundu. Jednotka je baud, Bd/s. U QPSK jedna změna nese 2 bity, takže bitů je víc než baudů.
- Přenosová rychlost je množství informace za sekundu, v bit/s. Když symbol nese víc bitů, přenosová rychlost je vyšší než modulační.
- Šířka pásma je rozsah frekvencí, který kanál propustí. U vozovky je to počet pruhů: užší pásmo znamená nižší strop rychlosti.
- Nyquistův a Shannonův vztah z šířky pásma a šumu odvozují, kolik bitů za sekundu kanál vůbec unese. Větší pásmo nebo čistší signál strop zvedne.

> Baud a bit za sekundu nejsou totéž. Baud počítá změny signálu, bit za sekundu počítá přenesenou informaci.


# Kvíz

## Digitální signál nabývá:
- Libovolných spojitých hodnot
- [x] Diskrétních hodnot 0 a 1
- Jen frekvence FM
- Jen šířky pásma

## Frekvence f je:
- Stejná jako amplituda
- [x] f = 1/T
- Jen fázový posun
- Počet bitů v QPSK

## AM, FM a PM jsou:
- Typy kabelů
- [x] Základní typy modulace
- Jen CRC metody
- Typy serverů

## QPSK přenáší v jednom prvku:
- 1 bit
- [x] 2 bity
- 8 bitů
- 256 bitů

## 256-QAM přenáší v jednom prvku:
- 2 bity
- 4 bity
- [x] 8 bitů
- 16 bitů

## Modulační rychlost se udává v:
- Jen bit/s
- [x] Baud (Bd/s)
- Jen metrech
- Jen hertzech amplitudy

## Přenosová rychlost vyjadřuje:
- Počet změn signálu za sekundu
- [x] Velikost přenesené informace [bit/s]
- Jen fázový posun
- Jen periodu T

## Šířka pásma určuje:
- Jen barvu kabelu
- [x] Maximální možnou přenosovou rychlost
- Jen počet serverů
- Jen start-bit
