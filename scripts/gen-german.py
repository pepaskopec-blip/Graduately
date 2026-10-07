#!/usr/bin/env python3
"""German skill course: four years, same activity mix as English.

Textbook units (Neue Freunde and the next two) stay as they are.
This course follows a typical Czech secondary-school ŠVP for German
as a second foreign language: A1, A2, then B1 exam skills.
"""

from importlib.machinery import SourceFileLoader
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
eng = SourceFileLoader(
    "gen_english", str(Path(__file__).with_name("gen-english.py"))
).load_module()

L, S, Q, G, W = eng.L, eng.S, eng.Q, eng.G, eng.W
c_str = eng.c_str
emit_lesson = eng.emit_lesson

YEARS = [
    {
        "sub_cs": "Pozdravy, rodina, škola, jídlo a den. Úroveň A1.",
        "sub_en": "Greetings, family, school, food and the day. Level A1.",
        "lessons": [
            L(
                "Pozdravy a představení",
                "Greetings and introductions",
                "Sein, heißen a woher. Doplňování a kvíz.",
                "Sein, heißen and woher. Gaps and a quiz.",
                [
                    S("1 / 2   •   Sein", "Kdo jsem",
                      "Tip: věk je Ich bin 16, ne Ich habe 16 Jahre.",
                      [
                          "ich bin, du bist, er/sie/es ist, wir/ihr/sie sind.",
                          "Ich heiße Anna. Wie heißt du?",
                          "Woher kommst du? — Ich komme aus Prag.",
                          "Guten Tag, Hallo, Tschüss, Danke, Bitte.",
                      ]),
                    S("2 / 2   •   Otázka", "Sloveso na druhém místě",
                      "Tip: v otázce je sloveso hned za tázacím slovem.",
                      [
                          "Wie heißt du? Wo wohnst du? Was machst du?",
                          "Zápor: Ich heiße nicht Peter. Er ist nicht hier.",
                          "Sie s velkým S je vykání. sie s malým je ona nebo oni.",
                          "Zdvořilost: Guten Morgen, Frau Novak.",
                      ]),
                ],
                gaps=[
                    G("Ich ___ Anna.", "heiße|heisse", "heißen ve 1. osobě."),
                    G("___ kommst du?", "Woher", "Odkud."),
                    G("Wir ___ Schüler.", "sind", "wir + sein."),
                    G("Er ___ mein Bruder.", "ist", "er + sein."),
                    G("Wie ___ du?", "heißt|heisst", "du + heißen."),
                ],
                quiz=[
                    Q("Jak se německy řekne věk?",
                      ["Ich habe 16 Jahre.", "Ich bin 16 Jahre alt.", "Ich ist 16.", "Ich bin 16 Jahre."],
                      1, "bin + Jahre alt."),
                    Q("Woher kommst du?",
                      ["Ich bin gut.", "Ich komme aus Brno.", "Ich heiße aus Brno.", "Ich wohne kommst."],
                      1, "kommen aus + město."),
                    Q("du + sein",
                      ["du ist", "du bist", "du sind", "du bin"],
                      1, "du bist."),
                    Q("Který pozdrav je odpoledne neutrální?",
                      ["Gute Nacht", "Guten Tag", "Tschüss gleich", "Bitte sehr Tag"],
                      1, "Guten Tag."),
                ],
            ),
            L(
                "Rodina",
                "Family",
                "Haben a přivlastňovací zájmena. Čtení a doplňování.",
                "Haben and possessives. Reading and gaps.",
                [
                    S("1 / 2   •   Haben", "Co mám",
                      "Tip: haben není věk. Ich habe einen Bruder.",
                      [
                          "ich habe, du hast, er hat, wir haben.",
                          "Přivlastňovací: mein, dein, sein, ihr, unser, euer.",
                          "Člen der/die/das se v 1. pádu u jmen učí s podstatným jménem.",
                          "die Mutter, der Vater, die Schwester, der Bruder, die Eltern.",
                      ]),
                    S("2 / 2   •   Text", "Krátký medailon",
                      "Tip: v textu hledejte čísla a kdo s kým bydlí.",
                      [
                          "und spojuje, aber staví proti.",
                          "nicht stojí před tím, co popíráme.",
                          "auch = taky.",
                          "zu Hause = doma.",
                      ]),
                ],
                reading=(
                    "Ich heiße Lukas und ich bin fünfzehn. Ich wohne mit meinen Eltern "
                    "in Pilsen. Ich habe eine Schwester. Sie heißt Eva und sie ist zwölf. "
                    "Mein Vater ist Lehrer und meine Mutter arbeitet im Krankenhaus. "
                    "Wir haben auch einen Hund. Er heißt Max. Am Wochenende besuchen "
                    "wir meine Oma. Sie wohnt nicht bei uns, aber sie wohnt in derselben Stadt."
                ),
                readq=[
                    Q("Wie alt ist Lukas?",
                      ["Zwölf", "Fünfzehn", "Sechzehn", "Der Text sagt es nicht"],
                      1, "ich bin fünfzehn."),
                    Q("Was ist der Vater von Beruf?",
                      ["Arzt", "Lehrer", "Koch", "Schüler"],
                      1, "Mein Vater ist Lehrer."),
                    Q("Wo wohnt die Oma?",
                      ["Bei der Familie", "In einer anderen Stadt", "In derselben Stadt", "Im Krankenhaus"],
                      2, "in derselben Stadt."),
                    Q("Wie heißt der Hund?",
                      ["Lukas", "Eva", "Max", "Der Text nennt keinen Namen"],
                      2, "Er heißt Max."),
                ],
                gaps=[
                    G("Ich ___ eine Schwester.", "habe", "haben, 1. osoba."),
                    G("___ Vater ist Lehrer.", "Mein", "Přivlastňovací u maskulina."),
                    G("Sie ___ zwölf.", "ist", "Věk: sein."),
                    G("Wir ___ einen Hund.", "haben", "wir haben."),
                ],
            ),
            L(
                "Ve škole",
                "At school",
                "Přítomný čas. Poslech a doplňování.",
                "The present tense. Listening and gaps.",
                [
                    S("1 / 2   •   Präsens", "Pravidelné sloveso",
                      "Tip: kmen + koncovka. spielen: ich spiele, du spielst, er spielt.",
                      [
                          "ich -e, du -st, er/sie/es -t, wir -en, ihr -t, sie -en.",
                          "Otázka bez tázacího slova: Spielst du Fußball?",
                          "Zápor: Ich spiele nicht Klavier.",
                          "Sloveso je ve větě na druhém místě: Heute spiele ich Tennis.",
                      ]),
                    S("2 / 2   •   Poslech", "Rozvrh",
                      "Tip: nejdřív otázky, pak nahrávka. Chyťte předměty a dny.",
                      [
                          "am Montag, am Dienstag, in der Pause.",
                          "das Fach, die Stunde, die Hausaufgabe.",
                          "gern = rád. Ich lerne gern Deutsch.",
                          "lieber = radši.",
                      ]),
                ],
                listening=(
                    "Hallo, ich bin Mia. Am Montag haben wir Deutsch, Mathe und Biologie. "
                    "Deutsch mag ich gern, weil die Lehrerin nett ist. Mathe mag ich nicht. "
                    "In der Pause esse ich ein Brötchen und spreche mit Eva. Nach der Schule "
                    "mache ich Hausaufgaben oder ich spiele Volleyball. Am Abend lese ich "
                    "oder ich höre Musik. Um zehn gehe ich ins Bett."
                ),
                listenq=[
                    Q("Welches Fach mag Mia gern?",
                      ["Mathe", "Deutsch", "Volleyball", "Musik nur"],
                      1, "Deutsch mag ich gern."),
                    Q("Warum?",
                      ["Weil es leicht ist", "Weil die Lehrerin nett ist", "Weil sie keine Hausaufgaben hat", "Weil es am Abend ist"],
                      1, "weil die Lehrerin nett ist."),
                    Q("Was macht sie in der Pause?",
                      ["Sie schläft", "Sie isst ein Brötchen und spricht mit Eva", "Sie spielt Volleyball", "Sie lernt Mathe"],
                      1, "esse ich ein Brötchen und spreche mit Eva."),
                    Q("Wann geht sie ins Bett?",
                      ["Um acht", "Um neun", "Um zehn", "Um elf"],
                      2, "Um zehn."),
                ],
                gaps=[
                    G("Ich ___ (spielen) gern Tennis.", "spiele", "ich + -e."),
                    G("___ du heute Deutsch?", "Lernst", "Otázka: Lernst du heute Deutsch?"),
                    G("Er ___ (wohnen) in Prag.", "wohnt", "er + -t."),
                    G("Wir ___ (machen) Hausaufgaben.", "machen", "wir + -en."),
                ],
            ),
            L(
                "Jídlo",
                "Food",
                "Möchten a 4. pád. Krátký dialog a kvíz.",
                "Möchten and the accusative. A short dialogue and a quiz.",
                [
                    S("1 / 2   •   Möchten", "Chtěl bych",
                      "Tip: ich möchte, du möchtest, er möchte. Je to zdvořilé chtít.",
                      [
                          "Ich möchte einen Apfel. die → eine, der → einen v akuzativu.",
                          "Maskulinum: der Salat → einen Salat. Neutrum: das Brot → ein Brot.",
                          "Femininum se v akuzativu nemění: die Suppe → eine Suppe.",
                          "Was möchten Sie? je vykání v restauraci.",
                      ]),
                    S("2 / 2   •   Psaní", "Objednávka",
                      "Tip: začněte Guten Tag a skončete Danke.",
                      [
                          "Ich möchte … und …",
                          "Für mich bitte einen Tee.",
                          "Die Rechnung, bitte.",
                          "Es schmeckt gut.",
                      ]),
                ],
                writing=W(
                    "Schreib mindestens 25 Wörter. Bestell im Café etwas zu essen und zu trinken. Benutze möchte und bitte.",
                    "Guten Tag, ich möchte eine Suppe und ein Brot, bitte. Für mich bitte auch einen Tee. Meine Schwester möchte einen Apfelsaft. Die Suppe schmeckt gut. Die Rechnung, bitte. Danke, das ist alles.",
                    "möchte|bitte",
                    25,
                ),
                quiz=[
                    Q("der Salat im Akkusativ",
                      ["eine Salat", "einen Salat", "einem Salat", "der Salat bleibt immer"],
                      1, "Maskulinum: einen."),
                    Q("Was ist höflich?",
                      ["Ich will sofort Pizza.", "Ich möchte eine Suppe, bitte.", "Gib mir Essen.", "Du bist Hunger."],
                      1, "möchte + bitte."),
                    Q("die Suppe im Akkusativ",
                      ["einen Suppe", "einem Suppe", "eine Suppe", "ein Suppe"],
                      2, "Femininum se nemění: eine Suppe."),
                    Q("Ich ___ einen Tee.",
                      ["möchte", "möchtest", "möchten du", "bist möchte"],
                      0, "ich möchte."),
                ],
            ),
            L(
                "Můj den",
                "My day",
                "Čas a odlučitelné předpony. Čtení a doplňování.",
                "Time and separable verbs. Reading and gaps.",
                [
                    S("1 / 2   •   Čas", "Um, am, im",
                      "Tip: um + hodina, am + den, im + měsíc nebo roční období.",
                      [
                          "Um sieben stehe ich auf. Am Montag habe ich Deutsch.",
                          "Im Sommer fahre ich ans Meer.",
                          "Odlučitelné: aufstehen → Ich stehe um sieben auf.",
                          "Předpona jde na konec věty.",
                      ]),
                    S("2 / 2   •   Pořadí", "Nejdřív, potom, nakonec",
                      "Tip: zuerst, dann, danach, zum Schluss.",
                      [
                          "Zuerst stehe ich auf. Dann frühstücke ich.",
                          "Danach gehe ich zur Schule.",
                          "Am Abend sehe ich fern.",
                          "fernsehen → Ich sehe abends fern.",
                      ]),
                ],
                reading=(
                    "An einem Schultag stehe ich um halb sieben auf. Zuerst dusche ich, "
                    "dann frühstücke ich Brot und Tee. Um sieben fahre ich mit der "
                    "Straßenbahn zur Schule. Der Unterricht beginnt um acht. Nach der "
                    "Schule mache ich Hausaufgaben. Am Abend sehe ich manchmal fern "
                    "oder ich rufe eine Freundin an. Um zehn gehe ich ins Bett."
                ),
                readq=[
                    Q("Wann steht sie auf?",
                      ["Um zehn", "Um halb sieben", "Um acht", "Am Abend"],
                      1, "um halb sieben."),
                    Q("Womit fährt sie zur Schule?",
                      ["Mit dem Auto", "Mit der Straßenbahn", "Zu Fuß nur", "Mit dem Zug"],
                      1, "mit der Straßenbahn."),
                    Q("Was macht sie nach der Schule?",
                      ["Sie schläft sofort", "Sie macht Hausaufgaben", "Sie fährt ans Meer", "Sie hat frei"],
                      1, "mache ich Hausaufgaben."),
                    Q("anrufen v větě Ich ___ eine Freundin ___.",
                      ["rufe … an", "anrufe …", "rufe … auf", "stehe … an"],
                      0, "Předpona an jde na konec."),
                ],
                gaps=[
                    G("Ich stehe um sieben ___.", "auf", "aufstehen."),
                    G("___ Montag habe ich Deutsch.", "Am", "am + den."),
                    G("Der Unterricht beginnt ___ acht.", "um", "um + hodina."),
                    G("Ich sehe abends ___.", "fern", "fernsehen."),
                ],
            ),
            L(
                "Test 1. ročníku",
                "Year 1 test",
                "Čtení, poslech, gramatika A1 a krátké psaní.",
                "Reading, listening, A1 grammar and a short text.",
                [
                    S("1 / 2   •   A1", "Co už má sedět",
                      "Tip: bin pro věk, habe pro vlastnictví, sloveso na druhém místě.",
                      [
                          "sein, haben, heißen, wohnen, möchten.",
                          "mein/dein, ein/einen.",
                          "um, am, odlučitelná předpona na konci.",
                          "Psaní: Guten Tag a bitte.",
                      ]),
                    S("2 / 2   •   Kontrola", "Nejčastější chyby",
                      "Tip: po kontrole si přečtěte nápovědy.",
                      [
                          "Ich bin 15, ne Ich habe 15 Jahre.",
                          "Ich komme aus Prag, ne Ich bin aus Prag kommen.",
                          "einen Apfel, ne ein Apfel jako předmět.",
                          "Ich stehe auf, ne Ich aufstehe.",
                      ]),
                ],
                reading=(
                    "Nina ist sechzehn und wohnt in Olomouc. Sie hat einen Bruder. "
                    "Er heißt Tom und er ist zehn. Am Morgen isst Nina Brot. In der "
                    "Schule lernt sie gern Deutsch, aber sie mag Mathe nicht. Nach "
                    "der Schule ruft sie ihre Freundin an."
                ),
                readq=[
                    Q("Wo wohnt Nina?",
                      ["In Prag", "In Olomouc", "In Brno", "Der Text sagt es nicht"],
                      1, "wohnt in Olomouc."),
                    Q("Wie alt ist Tom?",
                      ["Sechzehn", "Zehn", "Zwölf", "Fünfzehn"],
                      1, "er ist zehn."),
                    Q("Was macht Nina nach der Schule?",
                      ["Sie schläft", "Sie ruft ihre Freundin an", "Sie kocht für Tom", "Sie fährt nach Wien"],
                      1, "ruft sie ihre Freundin an."),
                ],
                listening=(
                    "Guten Tag, ich möchte einen Salat und ein Wasser, bitte. "
                    "Haben Sie auch Brot? Ja, natürlich. Dann bitte noch ein Brot. "
                    "Möchten Sie Kaffee? Nein, danke, nur Wasser. Die Rechnung "
                    "mache ich später."
                ),
                listenq=[
                    Q("Was bestellt die Person zuerst?",
                      ["Kaffee und Kuchen", "Einen Salat und ein Wasser", "Nur Brot", "Eine Suppe"],
                      1, "einen Salat und ein Wasser."),
                    Q("Möchte sie Kaffee?",
                      ["Ja", "Nein", "Nur am Abend", "Der Text sagt es nicht"],
                      1, "Nein, danke."),
                    Q("Was kommt noch dazu?",
                      ["Ein Brot", "Ein Tee", "Ein Hund", "Die Oma"],
                      0, "noch ein Brot."),
                ],
                quiz=[
                    Q("Ich ___ fünfzehn Jahre alt.",
                      ["habe", "bin", "ist", "hat"],
                      1, "Alter mit sein."),
                    Q("Sie hat ___ Bruder.",
                      ["eine", "einen", "einem", "der"],
                      1, "der Bruder → einen Bruder."),
                    Q("Ich stehe um sieben ___.",
                      ["auf", "an", "ein", "um"],
                      0, "aufstehen."),
                    Q("___ du aus Prag?",
                      ["Kommst", "Wohnst", "Bist", "Hast"],
                      0, "kommen aus: Kommst du aus Prag?"),
                ],
                writing=W(
                    "Schreib mindestens 30 Wörter über deine Familie. Benutze bin und habe.",
                    "Ich bin sechzehn und ich wohne in Prag. Ich habe eine Schwester und meine Eltern. Mein Vater ist nett und meine Mutter arbeitet viel. Wir haben keine Katze, aber ich habe einen Freund in der Schule. Am Sonntag besuchen wir oft meine Oma.",
                    "bin|habe",
                    30,
                ),
            ),
        ],
    },
    {
        "sub_cs": "Město, nákupy, volný čas, bydlení a perfektum. Úroveň A2.",
        "sub_en": "Town, shopping, free time, home and the perfect. Level A2.",
        "lessons": [
            L(
                "Ve městě",
                "In town",
                "Wo a wohin, předložky. Doplňování a kvíz.",
                "Wo and wohin, prepositions. Gaps and a quiz.",
                [
                    S("1 / 2   •   Místo", "Kde a kam",
                      "Tip: wo ptá kde to je, wohin ptá kam někdo jde.",
                      [
                          "Wo ist der Bahnhof? — Er ist neben der Post.",
                          "Wohin gehst du? — Ich gehe zum Bahnhof.",
                          "in, an, auf, neben, zwischen, gegenüber.",
                          "zu + der = zur, zu + dem = zum.",
                      ]),
                    S("2 / 2   •   Popis cesty", "Krátké věty",
                      "Tip: zuerst, dann, dann links, dann rechts.",
                      [
                          "Gehen Sie geradeaus.",
                          "Nehmen Sie die erste Straße links.",
                          "Die Apotheke ist gegenüber vom Park.",
                          "Es ist nicht weit.",
                      ]),
                ],
                gaps=[
                    G("Wo ___ der Bahnhof?", "ist", "Wo + sein."),
                    G("Ich gehe ___ Bahnhof.", "zum", "zu + dem = zum."),
                    G("Die Post ist ___ der Schule.", "neben", "vedle."),
                    G("___ gehst du?", "Wohin", "Kam."),
                    G("Gehen Sie ___.", "geradeaus", "rovně."),
                ],
                quiz=[
                    Q("Wohin bedeutet…",
                      ["wo", "woher", "kam", "wann"],
                      2, "wohine = kam."),
                    Q("zu + dem",
                      ["zur", "zum", "zu dem bleibt immer zwei Wörter", "beim"],
                      1, "zum."),
                    Q("zu + der",
                      ["zum", "zur", "zu die", "am"],
                      1, "zur."),
                    Q("Die Apotheke ist ___ vom Park.",
                      ["gegenüber", "weil", "um", "aufstehe"],
                      0, "gegenüber."),
                ],
            ),
            L(
                "Nákupy",
                "Shopping",
                "Ceny a 4. pád v obchodě. Čtení a kvíz.",
                "Prices and the accusative in a shop. Reading and a quiz.",
                [
                    S("1 / 2   •   Obchod", "Kolik to stojí",
                      "Tip: Wie viel kostet das? Es kostet fünf Euro.",
                      [
                          "Ich suche einen Pullover.",
                          "Haben Sie das auch in Blau?",
                          "Das ist mir zu teuer. Haben Sie etwas Billigeres?",
                          "Ich nehme den Pullover.",
                      ]),
                    S("2 / 2   •   Barvy a velikost",
                      "Velikost",
                      "Tip: die Größe M, zu klein, zu groß.",
                      [
                          "groß, klein, billig, teuer, bequem.",
                          "die Jacke, der Schuh, das Hemd.",
                          "Zahlen: zweiundzwanzig Euro.",
                          "Bar oder mit Karte?",
                      ]),
                ],
                reading=(
                    "Anna sucht eine Jacke. Die rote Jacke kostet neunzig Euro. "
                    "Das ist ihr zu teuer. Eine blaue Jacke kostet fünfundvierzig Euro "
                    "und sie ist bequem. Anna nimmt die blaue Jacke. Dazu kauft sie "
                    "noch ein Hemd für ihren Bruder. Das Hemd kostet zwanzig Euro. "
                    "Sie zahlt mit Karte."
                ),
                readq=[
                    Q("Warum nimmt Anna die rote Jacke nicht?",
                      ["Sie ist zu klein", "Sie ist zu teuer", "Es gibt sie nicht", "Sie ist nicht rot"],
                      1, "zu teuer."),
                    Q("Was kostet die blaue Jacke?",
                      ["90 Euro", "45 Euro", "20 Euro", "22 Euro"],
                      1, "fünfundvierzig Euro."),
                    Q("Für wen ist das Hemd?",
                      ["Für Anna", "Für den Bruder", "Für die Lehrerin", "Für niemanden"],
                      1, "für ihren Bruder."),
                    Q("Wie zahlt sie?",
                      ["Bar", "Mit Karte", "Sie zahlt nicht", "Mit dem Hemd"],
                      1, "mit Karte."),
                ],
                quiz=[
                    Q("Ich suche ___ Pullover.",
                      ["eine", "einen", "einem", "der"],
                      1, "der Pullover → einen."),
                    Q("Das ist mir zu ___. Ich habe nur zwanzig Euro.",
                      ["billig", "teuer", "bequem", "blau"],
                      1, "zu teuer."),
                    Q("Wie viel ___ das?",
                      ["kostet", "kosten du", "bist", "heißt"],
                      0, "kostet."),
                    Q("Sie zahlt ___ Karte.",
                      ["mit", "zu", "am", "auf"],
                      0, "mit Karte."),
                ],
            ),
            L(
                "Volný čas",
                "Free time",
                "Modální slovesa. Poslech a doplňování.",
                "Modal verbs. Listening and gaps.",
                [
                    S("1 / 2   •   Modály", "können, müssen, wollen",
                      "Tip: modální sloveso je na druhém místě, významové na konci.",
                      [
                          "Ich kann schwimmen. Du musst lernen. Wir wollen ins Kino.",
                          "können: ich kann, du kannst, er kann, wir können.",
                          "müssen: ich muss, du musst, er muss.",
                          "Zápor: Ich kann heute nicht kommen.",
                      ]),
                    S("2 / 2   •   Koníčky", "gern a lieber",
                      "Tip: Ich schwimme gern. Ich spiele lieber Fußball als Tennis.",
                      [
                          "das Hobby, die Mannschaft, das Training.",
                          "am Wochenende, in der Freizeit.",
                          "zu laut, zu spät.",
                          "Darf ich mitkommen?",
                      ]),
                ],
                listening=(
                    "Am Samstag muss ich vormittags lernen, weil wir am Montag einen "
                    "Test haben. Nachmittags kann ich aber Fußball spielen. Mein Freund "
                    "Paul will auch kommen. Er kann nicht so gut spielen, aber er ist "
                    "nett. Abends wollen wir einen Film sehen. Ich darf nicht zu spät "
                    "nach Hause kommen. Um elf muss ich zu Hause sein."
                ),
                listenq=[
                    Q("Warum muss er vormittags lernen?",
                      ["Weil er krank ist", "Weil am Montag ein Test ist", "Weil Paul nicht kann", "Weil der Film lang ist"],
                      1, "am Montag einen Test."),
                    Q("Was kann er nachmittags machen?",
                      ["Schlafen", "Fußball spielen", "Nach Wien fahren", "Kochen"],
                      1, "Fußball spielen."),
                    Q("Wann muss er zu Hause sein?",
                      ["Um neun", "Um zehn", "Um elf", "Um zwölf"],
                      2, "Um elf."),
                    Q("Was wollen sie abends machen?",
                      ["Einen Film sehen", "Lernen", "Fußball spielen", "Einkaufen"],
                      0, "einen Film sehen."),
                ],
                gaps=[
                    G("Ich ___ heute nicht kommen.", "kann", "können, ich kann."),
                    G("Du ___ lernen.", "musst", "müssen, du musst."),
                    G("Wir ___ ins Kino.", "wollen", "wollen, wir wollen."),
                    G("Das Modalverb steht, das andere Verb steht am ___.", "Ende|Satzende", "Významové sloveso na konci."),
                ],
            ),
            L(
                "Bydlení",
                "Home",
                "Místnosti a předložky. Popis bytu.",
                "Rooms and prepositions. A description of a flat.",
                [
                    S("1 / 2   •   Byt", "Wo steht was",
                      "Tip: Es gibt + akuzativ. Es gibt einen Balkon.",
                      [
                          "die Küche, das Wohnzimmer, das Bad, das Schlafzimmer.",
                          "Das Sofa steht vor dem Fenster.",
                          "an der Wand, auf dem Tisch, unter dem Bett.",
                          "hell, dunkel, ruhig, laut, gemütlich.",
                      ]),
                    S("2 / 2   •   Psaní", "Můj pokoj",
                      "Tip: nejdřív velikost, pak nábytek a místo.",
                      [
                          "Mein Zimmer ist klein, aber hell.",
                          "Es gibt ein Bett und einen Schreibtisch.",
                          "Das Buch liegt auf dem Tisch.",
                          "Ich mag mein Zimmer, weil es ruhig ist.",
                      ]),
                ],
                writing=W(
                    "Beschreib dein Zimmer in mindestens 35 Wörtern. Benutze es gibt und weil.",
                    "Mein Zimmer ist klein, aber hell. Es gibt ein Bett, einen Schreibtisch und ein Fenster. Das Bett steht an der Wand und der Schreibtisch steht vor dem Fenster. Meine Bücher liegen auf dem Tisch. Ich mag das Zimmer, weil es ruhig ist und ich dort gut lernen kann.",
                    "weil|gibt",
                    35,
                ),
                quiz=[
                    Q("Es gibt ___ Balkon.",
                      ["ein", "einen", "einem", "der"],
                      1, "der Balkon → einen."),
                    Q("Das Buch liegt ___ dem Tisch.",
                      ["auf", "weil", "um", "zum"],
                      0, "auf dem Tisch."),
                    Q("gemütlich bedeutet…",
                      ["teuer", "příjemný, útulný", "hlasitý", "prázdný"],
                      1, "útulný."),
                    Q("Das Bad ist…",
                      ["die Küche", "koupelna", "ložnice", "balkon"],
                      1, "das Bad = koupelna."),
                ],
            ),
            L(
                "Perfektum",
                "The perfect tense",
                "Haben nebo sein a příčestí. Doplňování a kvíz.",
                "Haben or sein plus the participle. Gaps and a quiz.",
                [
                    S("1 / 2   •   Perfekt", "Minulost v hovoru",
                      "Tip: haben/sein je na druhém místě, příčestí na konci.",
                      [
                          "Ich habe gestern Fußball gespielt.",
                          "Ich bin nach Wien gefahren. Pohyb a změna stavu berou sein.",
                          "ge- + kmen + t u pravidelných: gemacht, gespielt.",
                          "Nepravidelné: gesehen, gegessen, getrunken, genommen.",
                      ]),
                    S("2 / 2   •   Signály", "Kdy",
                      "Tip: gestern, letzte Woche, vor zwei Tagen, im Urlaub.",
                      [
                          "gehen → ist gegangen. kommen → ist gekommen.",
                          "bleiben → ist geblieben.",
                          "kaufen → hat gekauft. Lernen bere sein, protože to není pohyb.",
                          "Otázka: Hast du das gesehen? Bist du gekommen?",
                      ]),
                ],
                gaps=[
                    G("Ich ___ gestern Tennis gespielt.", "habe", "spielen bere haben."),
                    G("Wir ___ nach Berlin gefahren.", "sind", "fahren bere sein."),
                    G("Er hat einen Film ___.", "gesehen", "sehen → gesehen."),
                    G("___ du gut geschlafen?", "Hast", "schlafen s haben v této učebnicové řadě často haben; otázka Hast du."),
                    G("Sie ist um acht ___ Hause gekommen.", "nach", "nach Hause."),
                ],
                quiz=[
                    Q("gehen im Perfekt",
                      ["hat gegangen", "ist gegangen", "ist gehen", "hat gegehen"],
                      1, "ist gegangen."),
                    Q("kaufen im Perfekt",
                      ["ist gekauft", "hat gekauft", "hat kaufen", "ist gekaufen"],
                      1, "hat gekauft."),
                    Q("Wo steht das Partizip?",
                      ["Am Anfang", "Am Ende", "Vor dem Subjekt", "Es verschwindet"],
                      1, "Na konci."),
                    Q("gestern bedeutet…",
                      ["zítra", "včera", "vždycky", "nikdy"],
                      1, "včera."),
                ],
            ),
            L(
                "Test 2. ročníku",
                "Year 2 test",
                "Čtení, poslech, A2 a krátké psaní.",
                "Reading, listening, A2 and a short text.",
                [
                    S("1 / 2   •   A2", "Nové jevy",
                      "Tip: wohin, modální sloveso, perfektum, es gibt.",
                      [
                          "zum a zur.",
                          "Ich kann / muss / will + infinitiv na konci.",
                          "habe gemacht, bin gegangen.",
                          "weil přijde ve 3. ročníku pořádně, tady stačí protože v jednoduché větě.",
                      ]),
                    S("2 / 2   •   Strategie",
                      "Jedna věta",
                      "Tip: v psaní ať je sloveso na druhém místě.",
                      [
                          "Nejdřív podmět, pak sloveso.",
                          "Čísla si zapište.",
                          "einen u maskulina ve 4. pádu.",
                          "Partizip na konec.",
                      ]),
                ],
                reading=(
                    "Letztes Wochenende bin ich mit Paul in die Stadt gefahren. "
                    "Zuerst haben wir einen Pullover gesucht. Er war zu teuer. "
                    "Dann sind wir ins Café gegangen. Ich habe einen Kuchen gegessen "
                    "und Paul hat einen Tee getrunken. Danach sind wir nach Hause "
                    "gefahren, weil Paul lernen musste."
                ),
                readq=[
                    Q("Wohin sind sie zuerst gefahren?",
                      ["Nach Hause", "In die Stadt", "Ins Krankenhaus", "Nach Wien"],
                      1, "in die Stadt."),
                    Q("Warum haben sie den Pullover nicht genommen?",
                      ["Er war zu teuer", "Er war blau", "Paul war krank", "Das Café war zu"],
                      0, "zu teuer."),
                    Q("Warum sind sie nach Hause gefahren?",
                      ["Weil der Tee kalt war", "Weil Paul lernen musste", "Weil es geregnet hat", "Weil der Zug weg war"],
                      1, "weil Paul lernen musste."),
                ],
                listening=(
                    "Entschuldigung, wo ist die Apotheke? Gehen Sie geradeaus und "
                    "dann links. Sie ist neben der Post. Ist es weit? Nein, nur "
                    "fünf Minuten. Danke! Kann ich dort auch Tee kaufen? Nein, "
                    "Tee gibt es im Supermarkt gegenüber."
                ),
                listenq=[
                    Q("Wo ist die Apotheke?",
                      ["Im Bahnhof", "Neben der Post", "Im Café", "Weit vom Zentrum"],
                      1, "neben der Post."),
                    Q("Wie weit ist es?",
                      ["Fünf Minuten", "Eine Stunde", "Zu weit", "Neben dem Tee"],
                      0, "fünf Minuten."),
                    Q("Wo gibt es Tee?",
                      ["In der Apotheke", "Im Supermarkt gegenüber", "Im Zug", "Nirgends"],
                      1, "im Supermarkt gegenüber."),
                ],
                quiz=[
                    Q("Ich ___ schwimmen.",
                      ["kann", "kannst", "bin kann", "können"],
                      0, "ich kann."),
                    Q("Wir sind nach Prag ___.",
                      ["gefahren", "gefahrt", "fahren", "gegeht"],
                      0, "gefahren."),
                    Q("Es gibt ___ Balkon.",
                      ["ein", "einen", "einem", "eine"],
                      1, "einen Balkon."),
                    Q("Wohin ___ du?",
                      ["gehst", "kommst aus", "bist wo", "hast wohin"],
                      0, "Wohin gehst du?"),
                ],
                writing=W(
                    "Schreib mindestens 30 Wörter. Was hast du gestern gemacht? Benutze habe und bin.",
                    "Gestern bin ich in die Schule gegangen und ich habe einen Test geschrieben. Danach habe ich mit Eva gesprochen. Am Abend bin ich nach Hause gekommen und ich habe Musik gehört. Ich bin um elf ins Bett gegangen.",
                    "habe|bin",
                    30,
                ),
            ),
        ],
    },
    {
        "sub_cs": "Zdraví, cestování, věty s weil, práce a srovnání. Úroveň A2+.",
        "sub_en": "Health, travel, weil-clauses, work and comparison. Level A2+.",
        "lessons": [
            L(
                "U lékaře",
                "At the doctor's",
                "Tělo, tut a rozkaz. Čtení a kvíz.",
                "The body, tut and the imperative. Reading and a quiz.",
                [
                    S("1 / 2   •   Körper", "Co mě bolí",
                      "Tip: Mir tut der Kopf weh. Ich habe Fieber.",
                      [
                          "der Kopf, der Hals, der Bauch, der Rücken.",
                          "Ich fühle mich nicht gut.",
                          "Du sollst viel trinken. Sie müssen im Bett bleiben.",
                          "Rozkaz: Trink Wasser! Bleiben Sie zu Hause!",
                      ]),
                    S("2 / 2   •   Rada", "sollen",
                      "Tip: sollen je rada. müssen je nutné.",
                      [
                          "Du sollst heute nicht trainieren.",
                          "Sie müssen die Tabletten zweimal am Tag nehmen.",
                          "Gute Besserung!",
                          "die Apotheke, das Rezept, die Tablette.",
                      ]),
                ],
                reading=(
                    "Tom hat Halsschmerzen und ein bisschen Fieber. Die Ärztin sagt: "
                    "Sie sollen viel trinken und heute zu Hause bleiben. Sie müssen "
                    "nicht im Bett liegen, aber Sie sollen nicht Fußball spielen. "
                    "Hier ist ein Rezept. Die Tabletten nehmen Sie zweimal am Tag. "
                    "Wenn es am Freitag nicht besser ist, kommen Sie wieder."
                ),
                readq=[
                    Q("Was hat Tom?",
                      ["Nur Kopfschmerzen", "Halsschmerzen und Fieber", "Ein gebrochenes Bein", "Keine Symptome"],
                      1, "Halsschmerzen und Fieber."),
                    Q("Was soll er heute nicht machen?",
                      ["Trinken", "Fußball spielen", "Zum Arzt gehen", "Schlafen"],
                      1, "nicht Fußball spielen."),
                    Q("Wie oft nimmt er die Tabletten?",
                      ["Einmal", "Zweimal am Tag", "Nur am Freitag", "Gar nicht"],
                      1, "zweimal am Tag."),
                    Q("Wann kommt er wieder?",
                      ["Wenn es am Freitag nicht besser ist", "Jeden Morgen", "Nie", "Nur mit dem Rezept"],
                      0, "am Freitag."),
                ],
                quiz=[
                    Q("Mir tut der Kopf ___.",
                      ["weh", "gut", "auf", "zum"],
                      0, "weh tun."),
                    Q("Du ___ viel trinken. Das ist ein Rat.",
                      ["sollst", "bist", "heißt", "wohinst"],
                      0, "sollen."),
                    Q("Gute Besserung znamená…",
                      ["dobrou chuť", "brzké uzdravení", "na shledanou navždy", "účet prosím"],
                      1, "uzdravení."),
                    Q("Rozkaz pro du od trinken",
                      ["Trinkst!", "Trink!", "Trinken du!", "Du trink bitte nicht Form"],
                      1, "Trink!"),
                ],
            ),
            L(
                "Cesta",
                "A trip",
                "Perfektum v vyprávění. Poslech a doplňování.",
                "The perfect in a story. Listening and gaps.",
                [
                    S("1 / 2   •   Vyprávění", "Jedna věc po druhé",
                      "Tip: dann, danach, später, am Ende.",
                      [
                          "Zuerst sind wir zum Bahnhof gegangen.",
                          "Der Zug hatte Verspätung, ale v A2 stačí: Der Zug war zu spät.",
                          "Wir haben Tickets gekauft.",
                          "angekommen, eingestiegen, ausgestiegen.",
                      ]),
                    S("2 / 2   •   Vlak", "Slovník",
                      "Tip: der Bahnsteig, die Fahrkarte, umsteigen.",
                      [
                          "eine einfache Fahrt, hin und zurück.",
                          "der Sitzplatz, das Abteil.",
                          "Entschuldigung, ist dieser Platz frei?",
                          "in Wien umsteigen.",
                      ]),
                ],
                listening=(
                    "Letzten Samstag sind wir nach Wien gefahren. Zuerst haben wir "
                    "am Automaten zwei Fahrkarten gekauft, hin und zurück. Der Zug "
                    "war zwanzig Minuten zu spät. Im Zug habe ich ein Buch gelesen "
                    "und meine Schwester hat geschlafen. In Wien sind wir am "
                    "Hauptbahnhof ausgestiegen. Danach haben wir die Innenstadt "
                    "gesehen. Am Abend sind wir wieder nach Hause gefahren."
                ),
                listenq=[
                    Q("Wohin sind sie gefahren?",
                      ["Nach Berlin", "Nach Wien", "Nach Prag", "Ans Meer"],
                      1, "nach Wien."),
                    Q("Wie viel Verspätung hatte der Zug?",
                      ["Keine", "Zehn Minuten", "Zwanzig Minuten", "Zwei Stunden"],
                      2, "zwanzig Minuten."),
                    Q("Was hat die Schwester gemacht?",
                      ["Sie hat gelesen", "Sie hat geschlafen", "Sie hat gekocht", "Sie ist zu Hause geblieben"],
                      1, "hat geschlafen."),
                    Q("Wo sind sie ausgestiegen?",
                      ["Am Hauptbahnhof", "Am Flughafen", "In der Schule", "Im Café"],
                      0, "am Hauptbahnhof."),
                ],
                gaps=[
                    G("Wir haben zwei Fahrkarten ___.", "gekauft", "kaufen → gekauft."),
                    G("Der Zug war zwanzig Minuten zu ___.", "spät", "zu spät."),
                    G("Wir sind in Wien ___.", "ausgestiegen", "aussteigen."),
                    G("Am Abend sind wir nach Hause ___.", "gefahren", "fahren → gefahren."),
                ],
            ),
            L(
                "Weil, dass, wenn",
                "Weil, dass, wenn",
                "Vedlejší věta a sloveso na konci. Doplňování a kvíz.",
                "Subordinate clauses and the verb at the end. Gaps and a quiz.",
                [
                    S("1 / 2   •   Sloveso na konci", "weil",
                      "Tip: po weil, dass a wenn jde sloveso na konec.",
                      [
                          "Ich bleibe zu Hause, weil ich krank bin.",
                          "Sie sagt, dass sie morgen kommt.",
                          "Wenn es regnet, bleiben wir hier. V wenn-větě je sloveso na konci, v hlavní na druhém místě.",
                          "Čárka mezi větami.",
                      ]),
                    S("2 / 2   •   denn a weil",
                      "Dvě možnosti",
                      "Tip: denn nemění pořádek. weil ano.",
                      [
                          "Ich lerne, denn ich habe einen Test. Sloveso habe je druhé.",
                          "Ich lerne, weil ich einen Test habe.",
                          "dass není das. das je člen nebo vztažné.",
                          "ob = jestli v otázce: Ich weiß nicht, ob er kommt.",
                      ]),
                ],
                gaps=[
                    G("Ich bleibe hier, weil ich krank ___.", "bin", "Sloveso na konci."),
                    G("Sie sagt, dass sie morgen ___.", "kommt", "dass + sloveso na konci."),
                    G("___ es regnet, bleiben wir zu Hause.", "Wenn", "když."),
                    G("Ich lerne, ___ ich einen Test habe.", "weil", "důvod se slovesem na konci."),
                    G("Ich weiß nicht, ___ er kommt.", "ob", "jestli."),
                ],
                quiz=[
                    Q("Welche Variante ist richtig?",
                      ["weil ich bin krank", "weil ich krank bin", "weil bin ich krank", "weil ich krank ist"],
                      1, "bin na konci."),
                    Q("denn…",
                      ["schickt das Verb ans Ende", "ändert die Wortstellung nicht", "bedeutet wenn", "ist ein Artikel"],
                      1, "Po denn zůstává běžný pořádek."),
                    Q("dass oder das? Sie sagt, ___ sie kommt.",
                      ["das", "dass", "weil", "den"],
                      1, "spojka dass."),
                    Q("Wenn es regnet, ___ wir zu Hause.",
                      ["bleiben", "bleiben wir", "wir bleiben am Ende nur", "bleibt du"],
                      1, "V hlavní větě po wenn je sloveso první: bleiben wir."),
                ],
            ),
            L(
                "Práce a vykání",
                "Work and Sie",
                "Formální věta. E-mail a kvíz.",
                "A formal sentence. An email and a quiz.",
                [
                    S("1 / 2   •   Sie", "Vykání",
                      "Tip: Sie, Ihr, Ihnen se píšou velké.",
                      [
                          "Wie heißen Sie? Was machen Sie beruflich?",
                          "Ich möchte mich bewerben.",
                          "der Lebenslauf, die Stelle, das Vorstellungsgespräch.",
                          "Mit freundlichen Grüßen.",
                      ]),
                    S("2 / 2   •   E-mail", "Krátká žádost",
                      "Tip: Sehr geehrte Frau …, na konci jméno.",
                      [
                          "Ich schreibe Ihnen, weil ich die Stelle interessant finde.",
                          "Im Anhang ist mein Lebenslauf.",
                          "Ich freue mich auf Ihre Antwort.",
                          "Ne Piš Hi a Tschüss ve formálním mailu.",
                      ]),
                ],
                writing=W(
                    "Schreib eine kurze formale E-Mail, mindestens 40 Wörter. Benutze Sie a Grüßen.",
                    "Sehr geehrte Frau Berger, ich schreibe Ihnen, weil ich die Stelle im Café interessant finde. Ich bin siebzehn und ich habe schon im Sommer in einem Geschäft gearbeitet. Ich kann am Samstag arbeiten. Können Sie mich anrufen? Im Anhang ist mein Lebenslauf. Mit freundlichen Grüßen, Eva Malá.",
                    "Sie|Grüßen",
                    40,
                ),
                quiz=[
                    Q("Vykání od heißen",
                      ["Wie heißt du?", "Wie heißen Sie?", "Wie heißt Sie?", "Wie heißen du?"],
                      1, "Wie heißen Sie?"),
                    Q("Formální konec",
                      ["Tschüss, dein Freund", "Mit freundlichen Grüßen", "Hi und Kuss", "Bye weil"],
                      1, "Mit freundlichen Grüßen."),
                    Q("der Lebenslauf je…",
                      ["účtenka", "životopis", "nádraží", "recept"],
                      1, "životopis."),
                    Q("Ihr v Ich freue mich auf ___ Antwort.",
                      ["deine", "Ihre", "sein", "mein Sie"],
                      1, "Ihre s velkým I."),
                ],
            ),
            L(
                "Srovnání a příroda",
                "Comparison and nature",
                "Komparativ a superlativ. Čtení a doplňování.",
                "Comparative and superlative. Reading and gaps.",
                [
                    S("1 / 2   •   Stupňování", "-er a am -sten",
                      "Tip: billig → billiger → am billigsten. gern → lieber → am liebsten.",
                      [
                          "klein, kleiner, am kleinsten.",
                          "gut, besser, am besten. viel, mehr, am meisten.",
                          "als po komparativu: Prag ist größer als Pilsen.",
                          "so … wie: nicht so teuer wie.",
                      ]),
                    S("2 / 2   •   Prostředí", "Jednoduchá slova",
                      "Tip: der Müll, trennen, sparen, die Natur, das Klima.",
                      [
                          "zu Fuß gehen statt mit dem Auto fahren.",
                          "weniger Plastik, mehr Gemüse.",
                          "Ich finde die Bahn besser als das Flugzeug.",
                          "weil es die Umwelt schont.",
                      ]),
                ],
                reading=(
                    "Die Bahn ist oft besser für die Umwelt als das Flugzeug, aber "
                    "nicht immer billiger. Ein Ticket nach Berlin kann teurer sein "
                    "als ein Flug. Trotzdem fahren viele Jugendliche lieber mit dem "
                    "Zug, weil sie unterwegs lesen können. Am liebsten fahre ich "
                    "nachts, weil der Sitz dann so ruhig ist wie zu Hause."
                ),
                readq=[
                    Q("Was ist oft besser für die Umwelt?",
                      ["Das Flugzeug", "Die Bahn", "Das Auto immer", "Nichts"],
                      1, "Die Bahn."),
                    Q("Ist die Bahn immer billiger?",
                      ["Ja", "Nein", "Nur nachts", "Nur für Jugendliche"],
                      1, "nicht immer billiger."),
                    Q("Warum fahren viele lieber mit dem Zug?",
                      ["Weil sie unterwegs lesen können", "Weil es immer gratis ist", "Weil es kein Flugzeug gibt", "Weil Berlin klein ist"],
                      0, "lesen können."),
                    Q("am liebsten bedeutet…",
                      ["nejradši", "málo", "včera", "naproti"],
                      0, "nejradši."),
                ],
                gaps=[
                    G("Prag ist ___ (groß) als Pilsen.", "größer", "Komparativ + als."),
                    G("Das ist am ___.", "besten", "gut → am besten."),
                    G("Ich fahre ___ mit dem Zug als mit dem Flugzeug.", "lieber", "gern → lieber."),
                    G("Nicht so teuer ___ ein Flug.", "wie", "so … wie."),
                ],
            ),
            L(
                "Test 3. ročníku",
                "Year 3 test",
                "Čtení, poslech, weil a krátký názor.",
                "Reading, listening, weil and a short opinion.",
                [
                    S("1 / 2   •   A2+", "Věta",
                      "Tip: po weil je sloveso na konci. V perfektu je příčestí taky na konci.",
                      [
                          "sollen a müssen.",
                          "weil, dass, wenn, ob.",
                          "Sie s velkým S.",
                          "größer als, am liebsten.",
                      ]),
                    S("2 / 2   •   Psaní", "Jeden důvod",
                      "Tip: jedna věta s weil stačí, když je sloveso správně.",
                      [
                          "Ich finde …, weil …",
                          "Neweil ich bin.",
                          "Konkrétní příklad.",
                          "Na konec Mit freundlichen Grüßen jen ve formálním textu.",
                      ]),
                ],
                reading=(
                    "Mia bleibt heute zu Hause, weil sie Fieber hat. Die Ärztin hat "
                    "gesagt, dass sie viel trinken soll. Wenn es morgen besser ist, "
                    "geht Mia wieder in die Schule. Sie findet die Bahn besser als "
                    "das Auto, aber heute kann sie nicht fahren."
                ),
                readq=[
                    Q("Warum bleibt Mia zu Hause?",
                      ["Weil sie Fieber hat", "Weil die Bahn teuer ist", "Weil morgen Samstag ist", "Weil sie Fußball spielt"],
                      0, "weil sie Fieber hat."),
                    Q("Was soll sie machen?",
                      ["Viel trinken", "Fußball spielen", "Nach Wien fahren", "Nichts sagen"],
                      0, "viel trinken."),
                    Q("Was findet sie besser als das Auto?",
                      ["Das Flugzeug", "Die Bahn", "Das Fieber", "Die Schule"],
                      1, "die Bahn."),
                ],
                listening=(
                    "Ich habe mich für die Stelle beworben, weil ich gern mit Menschen "
                    "arbeite. Die Chefin hat gesagt, dass ich am Samstag kommen soll. "
                    "Wenn das Gespräch gut ist, kann ich im Juli anfangen. Ich bin "
                    "nervös, aber ich freue mich."
                ),
                listenq=[
                    Q("Warum hat sie sich beworben?",
                      ["Weil sie gern mit Menschen arbeitet", "Weil sie Fieber hat", "Weil der Zug zu spät war", "Weil sie nicht arbeiten will"],
                      0, "gern mit Menschen."),
                    Q("Wann soll sie kommen?",
                      ["Am Montag", "Am Samstag", "Im Zug", "Nie"],
                      1, "am Samstag."),
                    Q("Wann kann sie anfangen?",
                      ["Im Juli, wenn das Gespräch gut ist", "Gestern", "Nur im Winter", "Sie weiß es nicht"],
                      0, "im Juli."),
                ],
                quiz=[
                    Q("weil ich krank ___",
                      ["bin", "bin ich", "ist", "habe bin"],
                      0, "bin na konci."),
                    Q("Mir tut der Hals ___.",
                      ["weh", "weil", "auf", "gern"],
                      0, "weh."),
                    Q("größer ___ Berlin? Nein, Prag ist kleiner.",
                      ["wie", "als", "dass", "ob"],
                      1, "Komparativ + als. Otázka je schválně o vztahu als."),
                    Q("Wie heißen ___?",
                      ["Sie", "du Sie", "dein", "dir"],
                      0, "Sie."),
                ],
                writing=W(
                    "Schreib mindestens 35 Wörter. Was findest du besser, Bahn oder Auto, und warum? Benutze weil.",
                    "Ich finde die Bahn besser als das Auto, weil ich unterwegs lesen kann und nicht selbst fahren muss. Das Auto ist manchmal schneller, aber die Bahn ist ruhiger. Am liebsten fahre ich mit dem Zug, weil ich dabei Musik hören kann.",
                    "weil|besser",
                    35,
                ),
            ),
        ],
    },
    {
        "sub_cs": "Média, formální dopis, čtení, konjunktiv a maturitní témata. Úroveň B1.",
        "sub_en": "Media, a formal letter, reading, the subjunctive and exam topics. Level B1.",
        "lessons": [
            L(
                "Média",
                "The media",
                "Poslech rozhovoru a kvíz k slovům.",
                "An interview and a vocabulary quiz.",
                [
                    S("1 / 2   •   Medien", "Co sleduju",
                      "Tip: die Nachrichten, die Serie, das Handy, der Account.",
                      [
                          "Ich schaue selten fern, aber ich bin oft online.",
                          "eine Nachricht teilen, einen Kommentar schreiben.",
                          "Das stimmt nicht. Das ist ein Gerücht.",
                          "die Quelle, seriös, falsch.",
                      ]),
                    S("2 / 2   •   Poslech", "Dvě otázky předem",
                      "Tip: čísla a zápor si zapište hned.",
                      [
                          "seit zwei Jahren, jeden Abend, fast nie.",
                          "nicht mehr = už ne.",
                          "zu viel Zeit.",
                          "Ich habe gemerkt, dass …",
                      ]),
                ],
                listening=(
                    "Früher habe ich jeden Abend ferngesehen. Seit zwei Jahren mache "
                    "ich das nicht mehr. Ich höre einen Podcast auf dem Weg zur Schule "
                    "und ich lese die Nachrichten auf dem Handy. Manche Videos sind "
                    "lustig, aber nicht jede Quelle ist seriös. Letzte Woche habe ich "
                    "ein Gerücht geteilt und später gemerkt, dass es falsch war. "
                    "Seitdem prüfe ich den Text, bevor ich ihn schicke."
                ),
                listenq=[
                    Q("Seit wann sieht er nicht mehr jeden Abend fern?",
                      ["Seit zwei Wochen", "Seit zwei Jahren", "Seit gestern", "Er sieht immer noch fern"],
                      1, "Seit zwei Jahren."),
                    Q("Was hört er auf dem Weg zur Schule?",
                      ["Einen Podcast", "Nur Musik aus dem Fernseher", "Nichts", "Die Lehrerin"],
                      0, "einen Podcast."),
                    Q("Was ist letzte Woche passiert?",
                      ["Er hat ein Gerücht geteilt", "Er hat den Fernseher gekauft", "Er hat die Prüfung bestanden", "Er ist nach Wien gefahren"],
                      0, "ein Gerücht geteilt."),
                    Q("Was macht er seitdem?",
                      ["Er teilt alles sofort", "Er prüft den Text, bevor er ihn schickt", "Er sieht wieder jeden Abend fern", "Er schreibt keine Nachrichten"],
                      1, "prüft den Text."),
                    Q("seriös bedeutet…",
                      ["vtipný", "důvěryhodný", "zdarma", "hlasitý"],
                      1, "seriózní, důvěryhodný."),
                ],
            ),
            L(
                "Formální e-mail",
                "A formal email",
                "Stížnost nebo žádost. Psaní a kvíz.",
                "A complaint or a request. Writing and a quiz.",
                [
                    S("1 / 2   •   Kostra", "Čtyři části",
                      "Tip: oslovení, důvod, prosba, rozloučení.",
                      [
                          "Sehr geehrte Damen und Herren,",
                          "Ich schreibe Ihnen, weil …",
                          "Ich bitte Sie um eine Antwort.",
                          "Mit freundlichen Grüßen und jméno.",
                      ]),
                    S("2 / 2   •   Tón", "Klidně a konkrétně",
                      "Tip: datum, spoj, co chcete. Bez nadávek.",
                      [
                          "Der Zug am 3. Mai hatte Verspätung.",
                          "Deshalb habe ich den Termin verpasst.",
                          "Ich bitte um eine Erklärung oder um Geld zurück.",
                          "Im Anhang finden Sie das Ticket.",
                      ]),
                ],
                writing=W(
                    "Schreib eine formelle E-Mail, mindestens 45 Wörter. Der Zug hatte Verspätung. Benutze Ihnen a Grüßen.",
                    "Sehr geehrte Damen und Herren, ich schreibe Ihnen, weil der Zug am 3. Mai nach Berlin zwanzig Minuten zu spät war. Deshalb habe ich meinen Termin verpasst. Ich bitte Sie um eine Erklärung oder um das Geld zurück. Im Anhang ist mein Ticket. Mit freundlichen Grüßen, Tereza Malá.",
                    "Ihnen|Grüßen",
                    45,
                ),
                quiz=[
                    Q("Neznámé jméno",
                      ["Hi Leute", "Sehr geehrte Damen und Herren", "Tschüss Chef", "Liebe du"],
                      1, "Damen und Herren."),
                    Q("Ich schreibe ___, weil …",
                      ["dir du", "Ihnen", "dein", "euch Sie"],
                      1, "Ihnen."),
                    Q("im Anhang znamená…",
                      ["v příloze", "na nástupišti", "včera", "zdarma"],
                      0, "v příloze."),
                    Q("Který konec sedí?",
                      ["Bye!", "Mit freundlichen Grüßen", "Dein Kumpel", "Kuss"],
                      1, "Mit freundlichen Grüßen."),
                ],
            ),
            L(
                "Čtení: noční vlak",
                "Reading: the night train",
                "Delší text a otázky jako u maturity.",
                "A longer text and exam-style questions.",
                [
                    S("1 / 2   •   Čtení", "Nejdřív otázka",
                      "Tip: nehledejte stejné slovo, hledejte stejný význam.",
                      [
                          "Hlavní myšlenka bývá na začátku nebo na konci.",
                          "obwohl = ačkoli. Sloveso po obwohl je na konci.",
                          "trotzdem = přesto. Po trotzdem je běžný pořádek.",
                          "die Verspätung, das Abteil, der Schaffner.",
                      ]),
                    S("2 / 2   •   Slovo z kontextu",
                      "Neznámé slovo",
                      "Tip: věta před ním a za ním ho ohraničí.",
                      [
                          "umsteigen = přestupovat.",
                          "pünktlich = včas.",
                          "sich lohnen = vyplatit se.",
                          "lieber = radši.",
                      ]),
                ],
                reading=(
                    "Der Nachtzug von Prag nach Berlin ist für viele Jugendliche "
                    "interessanter als ein billiger Flug, obwohl das Ticket oft mehr "
                    "kostet. Man kann lesen, schlafen und muss nicht um fünf Uhr am "
                    "Flughafen sein. Kritiker sagen, dass die Verspätung ein Problem "
                    "ist und dass die Abteile klein sind. Trotzdem lohnt sich die Fahrt "
                    "für Lea. Sie ist schon dreimal gefahren. Sie sagt, dass sie nur "
                    "fliegt, wenn sie den Abendzug verpasst. Für sie ist die Reise ein "
                    "Teil des Urlaubs und nicht nur eine Strecke."
                ),
                readq=[
                    Q("Was ist die Hauptidee?",
                      ["Flüge sind verboten", "Manche Leute nehmen den Nachtzug, obwohl er mehr kosten kann", "Berlin hat keinen Bahnhof", "Lea fliegt jeden Tag"],
                      1, "obwohl das Ticket oft mehr kostet, přesto vlak volí."),
                    Q("Was kann man im Zug machen?",
                      ["Nur stehen", "Lesen und schlafen", "Das Flugzeug steuern", "Nichts"],
                      1, "lesen, schlafen."),
                    Q("Was sagen die Kritiker?",
                      ["Der Zug ist immer gratis", "Verspätung und kleine Abteile sind ein Problem", "Es gibt keine Tickets", "Lea ist der Schaffner"],
                      1, "Verspätung, Abteile klein."),
                    Q("Wann fliegt Lea?",
                      ["Immer", "Nur wenn sie den Abendzug verpasst", "Nie, das steht nicht so", "Jeden Morgen um fünf"],
                      1, "wenn sie den Abendzug verpasst."),
                    Q("obwohl bedeutet…",
                      ["protože", "ačkoli", "potom", "bez"],
                      1, "ačkoli."),
                ],
            ),
            L(
                "Kdyby",
                "If I were",
                "Konjunktiv II. Doplňování a kvíz.",
                "Konjunktiv II. Gaps and a quiz.",
                [
                    S("1 / 2   •   wäre a hätte", "Neskutečná podmínka",
                      "Tip: Wenn ich Zeit hätte, würde ich mehr lesen.",
                      [
                          "haben → hätte, sein → wäre.",
                          "würde + infinitiv pro ostatní slovesa.",
                          "Wenn ich reich wäre, würde ich reisen.",
                          "Sloveso po wenn je na konci: Wenn ich Zeit hätte.",
                      ]),
                    S("2 / 2   •   Zdvořilost", "würde a könnte",
                      "Tip: Könnten Sie mir helfen? je měkčí než Können Sie.",
                      [
                          "Ich hätte gern einen Kaffee.",
                          "Würden Sie das bitte wiederholen?",
                          "An deiner Stelle würde ich warten.",
                          "Není to minulost, i když tvar vypadá jako minulý.",
                      ]),
                ],
                gaps=[
                    G("Wenn ich Zeit ___, würde ich mehr lesen.", "hätte", "haben → hätte."),
                    G("Wenn ich reich ___, würde ich reisen.", "wäre", "sein → wäre."),
                    G("Ich ___ gern einen Kaffee.", "hätte", "Ich hätte gern."),
                    G("An deiner Stelle ___ ich warten.", "würde", "würde + infinitiv."),
                    G("___ Sie das bitte wiederholen?", "Würden|Könnten", "Zdvořilá prosba."),
                ],
                quiz=[
                    Q("Wenn ich du wäre, ___ ich das nicht machen.",
                      ["werde", "würde", "bin", "hast"],
                      1, "würde."),
                    Q("hätte je od slovesa…",
                      ["sein", "haben", "werden", "gehen"],
                      1, "haben."),
                    Q("Könnten Sie… je…",
                      ["rozkaz", "zdvořilejší otázka", "minulý čas perfekta", "člen"],
                      1, "zdvořilost."),
                    Q("Welche Satz ist richtig?",
                      ["Wenn ich hätte Zeit", "Wenn ich Zeit hätte", "Wenn hätte ich Zeit", "Wenn ich Zeit habe würde nur so"],
                      1, "hätte na konci."),
                ],
            ),
            L(
                "Témata k maturitě",
                "Exam topics",
                "Kolokace k běžným okruhům. Doplňování a kvíz.",
                "Collocations for common topics. Gaps and a quiz.",
                [
                    S("1 / 2   •   Okruhy", "Ne izolovaná slovíčka",
                      "Tip: ústní zkouška z němčiny bere osobu, rodinu, školu, volný čas, město, zdraví, práci a média.",
                      [
                          "eine Meinung haben, ein Beispiel nennen.",
                          "zu Fuß gehen, mit der Bahn fahren, im Stau stehen.",
                          "sich gesund ernähren, Sport treiben.",
                          "einen Nebenjob haben, Geld verdienen.",
                      ]),
                    S("2 / 2   •   Věta navíc",
                      "Obecné a osobní",
                      "Tip: Viele Jugendliche … Ich persönlich …",
                      [
                          "Einerseits … andererseits …",
                          "Das finde ich gut, weil …",
                          "Zum Beispiel …",
                          "Zusammenfassend …",
                      ]),
                ],
                gaps=[
                    G("Ich treibe dreimal pro Woche ___.", "Sport", "Sport treiben."),
                    G("Viele Leute stehen morgens im ___.", "Stau", "im Stau stehen."),
                    G("Sie hat einen ___ im Café.", "Nebenjob|Job", "brigáda, práce."),
                    G("Ich persönlich ___ die Bahn besser.", "finde", "finden."),
                    G("___ Beispiel lese ich im Zug.", "Zum", "Zum Beispiel."),
                ],
                quiz=[
                    Q("einerseits … andererseits",
                      ["nejdřív a nakonec bez kontrastu", "na jedné straně … na druhé straně", "protože … když", "včera … zítra"],
                      1, "kontrast."),
                    Q("sich gesund ernähren",
                      ["zdravě jíst", "být v zácpě", "psát e-mail", "zmeškat vlak"],
                      0, "stravovat se."),
                    Q("Was passt zu einer mündlichen Prüfung?",
                      ["Nur einzelne Wörter", "Eine allgemeine Aussage und ein persönliches Beispiel", "Ein Formular ohne Verben", "Nur Ja"],
                      1, "obecné plus příklad."),
                    Q("im Stau stehen",
                      ["stát ve frontě v bance jen", "být v dopravní zácpě", "stát na balkóně", "stát v čele třídy"],
                      1, "zácpa."),
                ],
            ),
            L(
                "Cvičná zkouška",
                "A practice paper",
                "Čtení, poslech, gramatika a krátký text.",
                "Reading, listening, grammar and a short text.",
                [
                    S("1 / 2   •   Celek", "Tři dovednosti",
                      "Tip: nejdřív si přečtěte otázky. U weil a wenn kontrolujte konec věty.",
                      [
                          "Čtení: význam, ne stejné slovo.",
                          "Poslech: čísla a nein.",
                          "Konjunktiv: hätte, wäre, würde.",
                          "Psaní: sloveso na druhém místě v hlavní větě.",
                      ]),
                    S("2 / 2   •   Text",
                      "Kamarádovi",
                      "Tip: tady není formální mail. Hallo stačí, ale věty ať jsou celé.",
                      [
                          "Napište proč píšete.",
                          "Jedna věta s weil.",
                          "Jedna věta s wenn nebo würde.",
                          "Otázka na konci.",
                      ]),
                ],
                reading=(
                    "In unserer Bibliothek kann man jetzt nicht nur Bücher, sondern "
                    "auch Werkzeug ausleihen. Die Idee kommt von einer Bibliothekarin, "
                    "die sagte, dass viele Leute Dinge kaufen, die sie nur einmal "
                    "brauchen. Im März haben Jugendliche zwanzig Wasserkocher repariert. "
                    "Wenn das Projekt weitergeht, machen andere Bibliotheken mit. "
                    "Man muss die Sachen sauber zurückgeben."
                ),
                readq=[
                    Q("Was kann man außer Büchern ausleihen?",
                      ["Autos", "Werkzeug", "Wohnungen", "Flugtickets"],
                      1, "Werkzeug."),
                    Q("Warum entstand die Idee?",
                      ["Weil Leute Dinge nur einmal brauchen und trotzdem kaufen", "Weil es keine Bücher gibt", "Weil der Zug zu spät war", "Weil niemand liest"],
                      0, "nur einmal brauchen."),
                    Q("Wie viele Wasserkocher haben sie im März repariert?",
                      ["Zwei", "Zwölf", "Zwanzig", "Zweihundert"],
                      2, "zwanzig."),
                    Q("Was muss man machen?",
                      ["Die Sachen sauber zurückgeben", "Einen neuen Kocher kaufen", "Nach Berlin fahren", "Nichts"],
                      0, "sauber zurückgeben."),
                ],
                listening=(
                    "Der Nachtzug nach Berlin hat vierzig Minuten Verspätung, weil "
                    "ein Baum auf den Gleisen liegt. Fahrgäste mit einem Platz können "
                    "in Wagen vier warten. Wenn Sie noch nicht eingecheckt haben, "
                    "machen Sie das bitte am Handy. Wir wären froh, wenn der Gang frei "
                    "bliebe. Um zehn wissen wir mehr."
                ),
                listenq=[
                    Q("Wie lang ist die Verspätung?",
                      ["Vier Minuten", "Vierzig Minuten", "Vier Stunden", "Der Zug ist pünktlich"],
                      1, "vierzig Minuten."),
                    Q("Warum?",
                      ["Schnee", "Ein Baum auf den Gleisen", "Kein Personal", "Ein Fest"],
                      1, "ein Baum."),
                    Q("Wo können Fahrgäste mit Platz warten?",
                      ["In Wagen vier", "Am Flughafen", "Zu Hause", "Im Gang"],
                      0, "Wagen vier."),
                    Q("Was sollen Leute ohne Check-in machen?",
                      ["Den Baum wegtragen", "Am Handy einchecken", "Nach Prag laufen", "Nichts sagen"],
                      1, "am Handy."),
                ],
                quiz=[
                    Q("Wenn ich Zeit ___, würde ich kommen.",
                      ["hätte", "habe würde", "bin", "hatte nur Perfekt"],
                      0, "hätte."),
                    Q("Sie sagt, dass der Zug Verspätung ___.",
                      ["hat", "hat er", "ist hat", "haben"],
                      0, "hat na konci."),
                    Q("Man muss die Sachen sauber ___.",
                      ["zurückgeben", "zurückgibt", "geben zurück sofort als einziges", "zu zurück"],
                      0, "zurückgeben na konci po muss."),
                    Q("Ich wäre froh, wenn du ___.",
                      ["kommst sofort nur so", "kommen würdest", "bist kommen", "kommst du"],
                      1, "würdest na konci je v pořádku. Alternativa wenn du kämst je těžší."),
                ],
                writing=W(
                    "Schreib mindestens 40 Wörter an einen Freund. Lad ihn ein, Werkzeug in der Bibliothek auszuleihen. Benutze weil.",
                    "Hallo Adam, in der Bibliothek kann man jetzt Werkzeug ausleihen, und ich glaube, das ist etwas für dich, weil du oft nur eine Bohrmaschine für einen Nachmittag brauchst. Wenn du am Samstag Zeit hast, komme ich mit. Wir müssen die Sachen danach sauber zurückgeben. Hast du Lust?",
                    "weil|wenn",
                    40,
                ),
            ),
        ],
    },
]


def main() -> None:
    if len(YEARS) != 4 or any(len(y["lessons"]) != 6 for y in YEARS):
        raise SystemExit("expected 4×6 lessons")
    body = ["/* Generated by scripts/gen-german.py. Do not edit by hand. */", ""]
    table = ["static const EnLesson de_lessons[DE_YEARS][DE_N] = {"]
    i18n = [
        '    {"de_home_sub", "Učebnice a čtyři ročníky s poslechem, čtením a psaním.", "The textbook plus four years of listening, reading and writing."},',
        '    {"de_book_title", "Učebnice", "Textbook"},',
        '    {"de_book_sub", "Neue Freunde, Aus aller Welt a Bei uns zu Hause.", "Neue Freunde, Aus aller Welt and Bei uns zu Hause."},',
        '    {"de_years_title", "Ročníky", "School years"},',
    ]
    for yi, year in enumerate(YEARS, start=1):
        i18n.append(
            f'    {{"de_y{yi}_sub", {c_str(year["sub_cs"])}, {c_str(year["sub_en"])}}},'
        )
        i18n.append(
            f'    {{"de_year{yi}", "{yi}. ročník", "Year {yi}"}},'
        )
        row = []
        for li, lesson in enumerate(year["lessons"], start=1):
            i18n.append(
                f'    {{"de_y{yi}_u{li}", {c_str(lesson["title_cs"])}, {c_str(lesson["title_en"])}}},'
            )
            i18n.append(
                f'    {{"de_y{yi}_u{li}_sub", {c_str(lesson["sub_cs"])}, {c_str(lesson["sub_en"])}}},'
            )
            inits = []
            emit_lesson(yi, li, lesson, body, inits, prefix="de")
            row.append(inits[0])
        table.append("    {\n" + ",\n".join(row) + "\n    },")
    table.append("};")
    table.append("")
    body.extend(table)
    (ROOT / "src" / "german_lessons.inc").write_text("\n".join(body) + "\n", encoding="utf-8")
    (ROOT / "src" / "german_i18n.inc").write_text("\n".join(i18n) + "\n", encoding="utf-8")
    print("wrote", 24, "german lessons")


if __name__ == "__main__":
    main()
