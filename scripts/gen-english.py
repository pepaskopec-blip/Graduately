#!/usr/bin/env python3
"""Generate src/english_lessons.inc and src/english_i18n.inc.

Curriculum: four-year gymnasium English (RVP G, cizí jazyk, aiming at
maturita B1 and the CERMAT catalogue). Year 1 is A2, year 2 B1,
year 3 B1+, year 4 B2 exam skills. Texts are original.
"""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def norm(s: str) -> str:
    out = []
    space = False
    for ch in s.lower():
        if ch.isalnum():
            out.append(ch)
            space = False
        elif ch.isspace():
            if out and not space:
                out.append(" ")
            space = True
    return "".join(out).strip()


def has_word(text: str, word: str) -> bool:
    n = f" {norm(text)} "
    w = norm(word)
    return f" {w} " in n


def words(text: str) -> int:
    n = norm(text)
    return len(n.split()) if n else 0


def c_str(s: str) -> str:
    esc = (
        s.replace("\\", "\\\\")
        .replace('"', '\\"')
        .replace("\n", "\\n")
        .replace("\t", "\\t")
    )
    return f'"{esc}"'


def Q(prompt, options, correct, hint):
    if len(options) != 4:
        raise SystemExit(f"need 4 options: {prompt}")
    if not 0 <= correct < 4:
        raise SystemExit(f"bad correct: {prompt}")
    return (prompt, options, correct, hint)


def G(prompt, answers, meaning):
    return (prompt, answers, meaning)


def W(prompt, model, keys, min_words):
    for key in keys.split("|"):
        if not has_word(model, key):
            raise SystemExit(f"model misses keyword {key!r}: {prompt[:40]}")
    if words(model) < min_words:
        raise SystemExit(f"model shorter than {min_words}: {prompt[:40]}")
    return (prompt, model, keys, min_words)


def S(kicker, title, tip, lines):
    return (kicker, title, tip, lines)


def L(title_cs, title_en, sub_cs, sub_en, slides, **parts):
    return {
        "title_cs": title_cs,
        "title_en": title_en,
        "sub_cs": sub_cs,
        "sub_en": sub_en,
        "slides": slides,
        **parts,
    }


YEARS = [
    {
        "sub_cs": "Osobní život, škola, domov a minulost. Úroveň A2.",
        "sub_en": "Personal life, school, home and the past. Level A2.",
        "lessons": [
            L(
                "Já a moje rodina",
                "Me and my family",
                "Sloveso be, have got a přivlastňovací zájmena. Doplňování a kvíz.",
                "The verb be, have got and possessives. Gaps and a quiz.",
                [
                    S(
                        "1 / 2   •   Be",
                        "Kdo jsem",
                        "Tip: věk se říká slovesem be. „She has 16 years“ je čechismus.",
                        [
                            "I am, you are, he/she/it is, we/you/they are.",
                            "Zápor a otázka: She isn't tall. Are you from Prague?",
                            "Věk, povolání i nálada: I am 16. My mum is a nurse. I am fine.",
                            "Přivlastňovací zájmena: my, your, his, her, its, our, their.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Have got",
                        "Co mám",
                        "Tip: have got je britská hovorová vazba pro vlastnictví, ne pro věk.",
                        [
                            "I've got a younger brother. She hasn't got a pet.",
                            "Have you got any cousins? — Yes, I have. / No, I haven't.",
                            "V americké angličtině stačí have: I have a brother.",
                            "Rodina: parents, siblings, only child, get on well, look like.",
                        ],
                    ),
                ],
                gaps=[
                    G("My sister ___ sixteen.", "is", "Věk: be, ne have."),
                    G("___ you got a bike?", "Have", "Otázka s have got."),
                    G("This is ___ grandmother. She lives with us.", "our", "Přivlastňovací zájmeno."),
                    G("They ___ not from Brno. They are from Olomouc.", "are", "They + are."),
                    G("He ___ got a dog, but he has got two cats.", "hasn't|has not", "Zápor have got."),
                ],
                quiz=[
                    Q(
                        "Which sentence is correct?",
                        ["She has 16 years.", "She is 16 years old.", "She is 16 years.", "She has 16 years old."],
                        1,
                        "Věk: be + years old.",
                    ),
                    Q(
                        "I ___ two cousins and one aunt.",
                        ["am", "have got", "has got", "is"],
                        1,
                        "Vlastnictví v 1. osobě: have got.",
                    ),
                    Q(
                        "Eva is my sister. ___ room is next to mine.",
                        ["She", "Her", "His", "Hers"],
                        1,
                        "Před podstatným jménem je her, ne she.",
                    ),
                    Q(
                        "___ your parents at home?",
                        ["Is", "Are", "Have", "Do"],
                        1,
                        "Parents je množné číslo: Are.",
                    ),
                ],
            ),
            L(
                "Školní den",
                "A school day",
                "Přítomný čas prostý. Čtení a doplňování.",
                "The present simple. Reading and gaps.",
                [
                    S(
                        "1 / 2   •   Present simple",
                        "Co dělám pravidelně",
                        "Tip: ve 3. osobě jednotného čísla přibývá -s/-es: she watches.",
                        [
                            "I get up at seven. He gets up at six.",
                            "Zápor: I don't like maths. She doesn't eat meat.",
                            "Otázka: Do you walk to school? Does she play the piano?",
                            "Příslovce četnosti stojí před slovesem: I usually walk. She is always late.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Škola",
                        "Rozvrh a přestávky",
                        "Tip: předmět ve škole je bez členu, když mluvíme obecně: I like biology.",
                        [
                            "lessons, break, canteen, homework, timetable, subject.",
                            "Čas: at eight, at half past twelve, in the morning, on Monday.",
                            "because vysvětluje důvod: I like biology because we do experiments.",
                            "never / sometimes / often / usually / always.",
                        ],
                    ),
                ],
                reading=(
                    "My school day starts early. I get up at half past six, have breakfast "
                    "and catch the tram. Lessons begin at eight. On Monday we have maths, "
                    "English and biology. I like biology because we do experiments, but I "
                    "don't like maths. We have lunch at half past twelve in the canteen. "
                    "After school I do my homework or meet friends. I never go to bed after "
                    "ten on a school night."
                ),
                readq=[
                    Q(
                        "What time do lessons begin?",
                        ["At half past six", "At eight", "At half past twelve", "At ten"],
                        1,
                        "Lessons begin at eight.",
                    ),
                    Q(
                        "Why does the writer like biology?",
                        [
                            "Because it is easy",
                            "Because they do experiments",
                            "Because the teacher is funny",
                            "Because there is no homework",
                        ],
                        1,
                        "because we do experiments.",
                    ),
                    Q(
                        "Where do they have lunch?",
                        ["At home", "On the tram", "In the canteen", "In the park"],
                        2,
                        "in the canteen.",
                    ),
                    Q(
                        "On a school night the writer goes to bed…",
                        ["after eleven", "at midnight", "never after ten", "before six"],
                        2,
                        "never go to bed after ten.",
                    ),
                ],
                gaps=[
                    G("She ___ (watch) a series every evening.", "watches", "3. osoba: watches."),
                    G("We ___ (not go) to school on Sunday.", "don't go|do not go", "Zápor v 1. osobě množného čísla."),
                    G("___ he play football on Fridays?", "Does", "Otázka ve 3. osobě."),
                    G("I usually ___ (get) up at seven.", "get", "Usually nemění tvar slovesa."),
                ],
            ),
            L(
                "Víkend ve městě",
                "A weekend in town",
                "Přítomný čas průběhový pro domluvené plány. Poslech.",
                "The present continuous for arrangements. Listening.",
                [
                    S(
                        "1 / 2   •   Continuous",
                        "Co právě probíhá a co je domluvené",
                        "Tip: I'm meeting Eva at ten je plán, ne jen „potkávám“.",
                        [
                            "Tvar: am/is/are + -ing. I'm waiting. She isn't coming.",
                            "Teď: Look, it is raining. Please be quiet, the baby is sleeping.",
                            "Blízký plán: We're having lunch on Saturday. I'm playing volleyball.",
                            "Stavová slovesa obvykle průběhový čas nemají: I know, I like, I want.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Poslech",
                        "Jak poslouchat vzkaz",
                        "Tip: nejdřív si přečtěte otázky, pak teprve pusťte nahrávku.",
                        [
                            "Chyťte jména, časy a místa. Ta se v otázkách opakují.",
                            "Nenechte se zastavit jedním slovem. Poslouchejte dál.",
                            "I'm meeting, we're going, I'm playing jsou v tomhle vzkazu klíčové.",
                            "Na konci bývá prosba: Call me back.",
                        ],
                    ),
                ],
                listening=(
                    "Hi Tom, it's Anna. Are you free on Saturday? I'm meeting Eva at the "
                    "station at ten. We're going to the market first, and then we're having "
                    "lunch in that new café by the river. In the afternoon I'm playing "
                    "volleyball with my class in the park. Do you want to come to the café? "
                    "Call me back tonight."
                ),
                listenq=[
                    Q("Who is Anna meeting?", ["Tom", "Eva", "Her class", "Her mum"], 1, "I'm meeting Eva."),
                    Q(
                        "Where are they meeting?",
                        ["At school", "At the station", "In the park", "By the river only"],
                        1,
                        "at the station at ten.",
                    ),
                    Q(
                        "What is Anna doing in the afternoon?",
                        ["Going to the market", "Having lunch", "Playing volleyball", "Sleeping"],
                        2,
                        "I'm playing volleyball.",
                    ),
                    Q(
                        "What should Tom do?",
                        ["Buy tickets", "Call her back", "Come to school", "Bring a map"],
                        1,
                        "Call me back tonight.",
                    ),
                ],
                gaps=[
                    G("Listen, the phone ___.", "is ringing", "Děje se to teď."),
                    G("We ___ lunch with Eva on Saturday.", "are having|'re having", "Domluvený plán."),
                    G("I ___ not coming to the match.", "am|'m", "Zápor: am not."),
                    G("She ___ volleyball this afternoon.", "is playing|'s playing", "Domluvená aktivita."),
                ],
            ),
            L(
                "Můj pokoj",
                "My room",
                "There is / there are a předložky místa. Krátký popis a kvíz.",
                "There is / there are and prepositions. A short description and a quiz.",
                [
                    S(
                        "1 / 2   •   There is",
                        "Co v pokoji je",
                        "Tip: there is pro jedno, there are pro více. There je formální podmět.",
                        [
                            "There is a desk under the window. There are two chairs.",
                            "Zápor: There isn't a TV. There aren't any posters.",
                            "Otázka: Is there a lamp? Are there any books?",
                            "Předložky: in, on, under, next to, between, behind, in front of.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Psaní",
                        "Popis místa",
                        "Tip: jděte od velkého k malému, od dveří k oknu.",
                        [
                            "Začněte velikostí a světlem: small but bright.",
                            "Každá věta ať má věc a místo: The bag is under the chair.",
                            "Střídejte there is a there are, ať text není seznam.",
                            "Na konci jedna osobní věta: I like it because it is quiet.",
                        ],
                    ),
                ],
                writing=W(
                    "Describe your room in at least 30 words. Say what is in it and where things are. Use on and a piece of furniture such as a bed, and mention a window.",
                    "My room is small but bright. There is a bed next to the window and a wooden desk under a shelf. My books are on the desk and my school bag is under the chair. There are two posters on the wall. I like the room because it is quiet in the evening.",
                    "bed|window|on",
                    30,
                ),
                quiz=[
                    Q(
                        "The lamp is ___ the desk.",
                        ["under", "on", "between", "at"],
                        1,
                        "Lampa stojí na desce: on.",
                    ),
                    Q(
                        "There ___ a big window.",
                        ["are", "is", "be", "have"],
                        1,
                        "Jedno okno: there is.",
                    ),
                    Q(
                        "There ___ two posters on the wall.",
                        ["is", "are", "has", "been"],
                        1,
                        "Dva plakáty: there are.",
                    ),
                    Q(
                        "The cat is sleeping ___ the bed.",
                        ["under", "next", "between to", "at to"],
                        0,
                        "Pod postelí: under the bed.",
                    ),
                ],
            ),
            L(
                "Na trhu",
                "At the market",
                "Počitatelnost, some a any, how much a how many. Čtení a doplňování.",
                "Countability, some and any, how much and how many. Reading and gaps.",
                [
                    S(
                        "1 / 2   •   Množství",
                        "Počitatelné a nepočitatelné",
                        "Tip: some v kladné větě, any v otázce a záporu. Nabídka je výjimka: Would you like some tea?",
                        [
                            "Počitatelné: an apple, some apples. Nepočitatelné: some bread, some milk.",
                            "How many apples? How much milk?",
                            "a little milk, a few rolls, a lot of cheese.",
                            "V obchodě: How much is it? It's four euros a kilo.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Dialog",
                        "Zdvořilá žádost",
                        "Tip: Can I have… a I'd like… zní v obchodě přirozeně.",
                        [
                            "Have you got any bread? — Sorry, we haven't got any today.",
                            "We've got some rolls instead.",
                            "How many would you like? — Six, please.",
                            "Anything else? — No, that's all. Thank you.",
                        ],
                    ),
                ],
                reading=(
                    "On Saturday morning the market by the river is busy. A woman asks, "
                    "\"How much is this cheese?\" The seller says, \"It's four euros a kilo.\" "
                    "Then she asks, \"Have you got any bread?\" He answers, \"Sorry, we "
                    "haven't got any bread today, but we've got some fresh rolls.\" "
                    "\"How many rolls do you want?\" \"Six, please, and a little milk.\" "
                    "She also buys a few apples. \"Anything else?\" \"No, that's all.\""
                ),
                readq=[
                    Q(
                        "How much is the cheese?",
                        ["Four euros a piece", "Four euros a kilo", "Six euros", "A little"],
                        1,
                        "four euros a kilo.",
                    ),
                    Q(
                        "What hasn't the seller got?",
                        ["Rolls", "Milk", "Bread", "Apples"],
                        2,
                        "haven't got any bread today.",
                    ),
                    Q(
                        "How many rolls does the woman want?",
                        ["Four", "A few", "Six", "A kilo"],
                        2,
                        "Six, please.",
                    ),
                    Q(
                        "Which phrase is an offer of more shopping?",
                        ["That's all.", "Anything else?", "How much is it?", "A little milk."],
                        1,
                        "Anything else? nabízí další nákup.",
                    ),
                ],
                gaps=[
                    G("We haven't got ___ bread today.", "any", "Zápor: any."),
                    G("There are ___ rolls in the bag.", "some", "Kladná věta: some."),
                    G("How ___ apples do you want?", "many", "Apples se dají počítat."),
                    G("How ___ milk is in the bottle?", "much", "Milk je nepočitatelné."),
                ],
            ),
            L(
                "Minulé léto",
                "Last summer",
                "Minulý čas prostý, pravidelná i nepravidelná slovesa. Poslech a kvíz.",
                "The past simple, regular and irregular verbs. Listening and a quiz.",
                [
                    S(
                        "1 / 2   •   Past simple",
                        "Co se stalo a je dokončené",
                        "Tip: zápor a otázka používají did, sloveso zůstává v základním tvaru.",
                        [
                            "Pravidelná: play → played, visit → visited, try → tried, stop → stopped.",
                            "Nepravidelná: go → went, take → took, swim → swam, buy → bought.",
                            "I didn't stay in a hotel. Did you visit the island?",
                            "Signály: yesterday, last July, ago, in 2019, when I was ten.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Vyprávění",
                        "Jedna událost za druhou",
                        "Tip: then, after that a one day drží příběh pohromadě.",
                        [
                            "Začněte kdy a kam: Last July we went to Croatia.",
                            "Pak doprava, počasí a jedna konkrétní příhoda.",
                            "Nepravidelná slovesa se neučí po jednom u testu, ale ve větě.",
                            "Na konci dojem: It was the best week of the year.",
                        ],
                    ),
                ],
                listening=(
                    "Last July we went to Croatia. We travelled by train and then by bus "
                    "because the flight was too expensive. The weather was hot every day. "
                    "I swam in the morning and my brother tried surfing. He fell off the "
                    "board a lot, but he loved it. One day we visited a small island. We "
                    "didn't stay in a hotel. We stayed with our uncle near the beach. I "
                    "took hundreds of photos."
                ),
                listenq=[
                    Q("Where did they go?", ["Italy", "Croatia", "Greece", "Spain"], 1, "went to Croatia."),
                    Q(
                        "How did they travel?",
                        ["By plane", "By train and bus", "By car", "By ferry only"],
                        1,
                        "by train and then by bus.",
                    ),
                    Q(
                        "What did the brother try?",
                        ["Swimming", "Surfing", "Cooking", "Driving"],
                        1,
                        "tried surfing.",
                    ),
                    Q(
                        "Where did they stay?",
                        ["In a hotel", "On the island", "With their uncle", "On the train"],
                        2,
                        "stayed with our uncle.",
                    ),
                ],
                quiz=[
                    Q("go → ___", ["goed", "went", "gone", "going"], 1, "go, went, gone."),
                    Q("I ___ a lot of photos.", ["taked", "took", "taken", "token"], 1, "take, took, taken."),
                    Q("She ___ in the sea every morning.", ["swimmed", "swam", "swum", "swims"], 1, "Minulý tvar swim je swam."),
                    Q("We ___ stay in a hotel.", ["don't", "didn't", "wasn't", "haven't"], 1, "Zápor minulého času: didn't + sloveso."),
                ],
            ),
            L(
                "Zpráva kamarádovi",
                "A message to a friend",
                "Will a going to. Krátká neformální zpráva a doplňování.",
                "Will and going to. A short informal message and gaps.",
                [
                    S(
                        "1 / 2   •   Budoucnost",
                        "Plán, nebo rozhodnutí teď",
                        "Tip: going to má důkaz nebo plán. Will je rozhodnutí v tu chvíli nebo odhad.",
                        [
                            "We've got tickets. We're going to leave at six. To je plán.",
                            "Look at those clouds. It's going to rain. To je důkaz.",
                            "The phone is ringing. I'll answer it. Rozhodnutí teď.",
                            "I think I'll stay at home. Will jako názor, ne jistý plán.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Zpráva",
                        "Neformální styl",
                        "Tip: Hi, kontraktované tvary a na konci otázka kamarádovi.",
                        [
                            "Začněte Hi a jménem, ne Dear Sir.",
                            "Napište kdy, co a proč. Jedna věta s because stačí.",
                            "I'm going to… je v takové zprávě přirozené.",
                            "Konec: Want to come? Write back.",
                        ],
                    ),
                ],
                writing=W(
                    "Write a short message to a friend about next weekend. Use at least 25 words. Say what you are going to do, mention the weekend, and give a reason with because.",
                    "Hi Lena, next weekend I am going to visit my cousins in Pilsen. We are going to the cinema on Saturday because a new film is on. On Sunday we are going to cook lunch together. Do you want to come with me? Write back soon.",
                    "going|weekend|because",
                    25,
                ),
                gaps=[
                    G("Look at those clouds. It ___ rain.", "is going to|'s going to", "Je vidět důkaz: going to."),
                    G("I think I ___ stay at home tonight.", "will|'ll", "Názor v tu chvíli: will."),
                    G("The tickets are in my bag. We ___ leave at six.", "are going to|'re going to", "Už je to domluvené."),
                    G("Don't worry. I ___ help you with the bags.", "will|'ll", "Rozhodnutí teď: will."),
                ],
            ),
            L(
                "Test 1. ročníku",
                "Year 1 test",
                "Čtení, poslech, gramatika a krátké psaní. Úroveň A2.",
                "Reading, listening, grammar and a short piece of writing. Level A2.",
                [
                    S(
                        "1 / 2   •   Co test bere",
                        "A2 v jedné sadě",
                        "Tip: nejdřív otázky, pak text nebo nahrávku. U psaní počítejte slova.",
                        [
                            "Čtení: najděte v textu čas, místo a důvod, nehádejte z jednoho slova.",
                            "Poslech: jména a čísla si poznamenejte hned.",
                            "Gramatika: be, přítomné časy, some/any, minulý čas, will a going to.",
                            "Psaní: krátký souvislý text, ne jen odrážky.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Jak opravovat",
                        "Nejčastější čechismy",
                        "Tip: po kontrole si přečtěte nápovědy, i když je věta správně.",
                        [
                            "I am 16, ne I have 16 years.",
                            "She likes, ne she like.",
                            "Did you went je špatně. Did you go.",
                            "There is a book, ne It is a book on the table, když věc uvádíte.",
                        ],
                    ),
                ],
                reading=(
                    "Klara lives in a flat with her parents and a small dog. On weekdays "
                    "she walks to school because it is only ten minutes away. She loves "
                    "art. Last Friday her class visited a gallery and she bought a poster. "
                    "This Saturday she is meeting her cousin at the station. They are going "
                    "to a concert in the evening."
                ),
                readq=[
                    Q(
                        "How does Klara get to school?",
                        ["By tram", "She walks", "Her dad drives", "By train"],
                        1,
                        "she walks to school.",
                    ),
                    Q(
                        "What did she buy at the gallery?",
                        ["A dog", "A ticket", "A poster", "A flat"],
                        2,
                        "she bought a poster.",
                    ),
                    Q(
                        "Who is she meeting on Saturday?",
                        ["Her parents", "Her art teacher", "Her cousin", "The singer"],
                        2,
                        "meeting her cousin.",
                    ),
                ],
                listening=(
                    "Hi, it's Marek. I missed you after school. I'm at the sports centre. "
                    "The match starts at four, not at three, so don't hurry. Can you bring "
                    "my blue jacket? It's on the chair in the kitchen. See you there."
                ),
                listenq=[
                    Q("Where is Marek?", ["At school", "At the sports centre", "In the kitchen", "On the tram"], 1, "at the sports centre."),
                    Q("What time does the match start?", ["At three", "At four", "At five", "At ten"], 1, "starts at four, not at three."),
                    Q("What should his friend bring?", ["A ticket", "A blue jacket", "Lunch", "A poster"], 1, "bring my blue jacket."),
                ],
                quiz=[
                    Q("She ___ coffee.", ["don't like", "doesn't like", "not like", "isn't like"], 1, "3. osoba: doesn't like."),
                    Q("There ___ any milk.", ["isn't", "aren't", "hasn't", "don't"], 0, "Milk je jednotné: there isn't."),
                    Q("Yesterday we ___ to the lake.", ["go", "went", "gone", "going"], 1, "Minulý tvar go je went."),
                    Q("I've got the tickets. We ___ leave at noon.", ["will", "are going to", "did", "are"], 1, "Plán s důkazem: going to."),
                ],
                writing=W(
                    "Write at least 30 words about your family. Use is or are, and the words family and because.",
                    "My family is not very big. I live with my parents and my younger sister. She is twelve and she is funny. We get on well because we both like films. My dad is a driver and my mum is a nurse. I am happy at home.",
                    "family|because",
                    30,
                ),
            ),
        ],
    },
    {
        "sub_cs": "Cestování, zdraví, práce, média a příroda. Úroveň B1.",
        "sub_en": "Travel, health, work, the media and nature. Level B1.",
        "lessons": [
            L(
                "Na cestách",
                "On the road",
                "Stupňování a minulý čas průběhový. Doplňování a kvíz.",
                "Comparison and the past continuous. Gaps and a quiz.",
                [
                    S(
                        "1 / 2   •   Stupňování",
                        "Porovnat dvě věci i celou skupinu",
                        "Tip: kratší přídavná jména berou -er/-est, delší more/most.",
                        [
                            "cheap → cheaper → the cheapest. Expensive → more expensive → the most expensive.",
                            "Nepravidelné: good → better → the best, bad → worse → the worst, far → further → the furthest.",
                            "as … as: The bus is as fast as the train today.",
                            "than stojí po komparativu: This hostel is cheaper than the hotel.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Past continuous",
                        "Děje, který probíhal, když se stalo něco jiného",
                        "Tip: průběhový čas je pozadí, prostý je krátká událost.",
                        [
                            "I was reading when the train stopped.",
                            "While we were waiting, it started to rain.",
                            "Zápor: We weren't sleeping. Otázka: Were you driving?",
                            "Nepleťte si ho s minulým prostým: I walked home popisuje celou cestu.",
                        ],
                    ),
                ],
                gaps=[
                    G("This ticket is ___ (cheap) than the night train.", "cheaper", "Krátké přídavné jméno: -er."),
                    G("It was the ___ (bad) hotel in the town.", "worst", "bad, worse, the worst."),
                    G("Prague is not as ___ as London.", "big", "as + základní tvar + as."),
                    G("I ___ (read) when the bus suddenly stopped.", "was reading", "Pozadí: past continuous."),
                    G("While we ___ (wait), a street musician started to play.", "were waiting", "While + průběhový děj."),
                ],
                quiz=[
                    Q("far → ___ → the furthest", ["farer", "further", "more far", "farthest only"], 1, "far, further, the furthest. Farthest je taky možné, ale further je v učebnici první."),
                    Q(
                        "Which sentence is correct?",
                        [
                            "This café is more cheap than that one.",
                            "This café is cheaper than that one.",
                            "This café is the cheaper than that one.",
                            "This café is cheap than that one.",
                        ],
                        1,
                        "cheap → cheaper than.",
                    ),
                    Q(
                        "She ___ a map when she missed the stop.",
                        ["looked", "was looking", "looks", "is looking"],
                        1,
                        "Dělo se to v okamžiku, kdy zmeškala zastávku.",
                    ),
                    Q(
                        "good → better → ___",
                        ["the goodest", "the better", "the best", "the most good"],
                        2,
                        "the best.",
                    ),
                ],
            ),
            L(
                "U lékaře",
                "At the clinic",
                "Should, must a have to. Čtení letáku a kvíz.",
                "Should, must and have to. A leaflet and a quiz.",
                [
                    S(
                        "1 / 2   •   Modální slovesa",
                        "Rada, povinnost, zákaz",
                        "Tip: must je silný pocit mluvčího, have to je pravidlo zvenku.",
                        [
                            "You should rest. You shouldn't go to school tomorrow. To je rada.",
                            "I must call the doctor. Mluvčí cítí, že to je nutné.",
                            "You have to show your insurance card. To chce ordinace.",
                            "Mustn't je zákaz. Don't have to znamená, že to není nutné.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Tělo a příznaky",
                        "Jak to říct u přepážky",
                        "Tip: I've got a sore throat zní přirozeněji než I have pain in throat.",
                        [
                            "a headache, a sore throat, a cough, a temperature, a rash.",
                            "It hurts when I swallow. I feel dizzy.",
                            "How long have you had it? — Since Monday. / For three days.",
                            "Take these tablets twice a day. Drink plenty of water.",
                        ],
                    ),
                ],
                reading=(
                    "The school clinic is open from seven to three. If you feel ill, you "
                    "should tell your teacher first. You have to bring your insurance card. "
                    "You must not come to school with a high temperature. Students with a "
                    "cough should wear a mask in the waiting room. You don't have to make "
                    "an appointment for a small cut, but you should come before noon. The "
                    "nurse can give you a painkiller. She can't give you antibiotics."
                ),
                readq=[
                    Q(
                        "What do you have to bring?",
                        ["A mask", "An insurance card", "Antibiotics", "An appointment"],
                        1,
                        "have to bring your insurance card.",
                    ),
                    Q(
                        "What is forbidden?",
                        [
                            "Coming with a high temperature",
                            "Telling the teacher",
                            "Wearing a mask",
                            "Coming before noon",
                        ],
                        0,
                        "must not come with a high temperature.",
                    ),
                    Q(
                        "You don't have to make an appointment if…",
                        [
                            "you need antibiotics",
                            "you have a small cut",
                            "you have a temperature",
                            "you arrive at four",
                        ],
                        1,
                        "for a small cut.",
                    ),
                    Q(
                        "What can't the nurse give you?",
                        ["Water", "A painkiller", "Antibiotics", "A mask"],
                        2,
                        "She can't give you antibiotics.",
                    ),
                ],
                quiz=[
                    Q(
                        "You ___ shout in the waiting room. It's a rule.",
                        ["don't have to", "mustn't", "shouldn't to", "haven't"],
                        1,
                        "Zákaz: mustn't.",
                    ),
                    Q(
                        "You ___ wear a tie. Nobody minds.",
                        ["mustn't", "don't have to", "must", "should to"],
                        1,
                        "Není to nutné: don't have to.",
                    ),
                    Q(
                        "I've got a headache. You ___ drink some water and rest.",
                        ["should", "must to", "have", "are"],
                        0,
                        "Rada: should.",
                    ),
                    Q(
                        "Visitors ___ show a card. The clinic asks for it.",
                        ["have to", "should to", "don't must", "are have"],
                        0,
                        "Pravidlo zvenku: have to.",
                    ),
                ],
            ),
            L(
                "První brigáda",
                "A first job",
                "Předpřítomný čas, for a since. Poslech pohovoru a doplňování.",
                "The present perfect, for and since. A job interview and gaps.",
                [
                    S(
                        "1 / 2   •   Present perfect",
                        "Zkušenost a děj až do teď",
                        "Tip: have/has + příčestí. Otázka na zkušenost začíná Have you ever…?",
                        [
                            "I have worked in a café. She has never had a paid job.",
                            "Ever v otázce, never v záporném významu: Have you ever used a till?",
                            "For three months = jak dlouho. Since May = od kdy.",
                            "Just, already, yet: I've just finished. I haven't finished yet.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Pohovor",
                        "Krátké odpovědi",
                        "Tip: odpovězte a přidejte jednu konkrétní větu, ať to není jen yes.",
                        [
                            "I'm reliable and I get on well with people.",
                            "I have looked after my neighbours' children.",
                            "I can work on Saturdays, but I can't work on Sunday mornings.",
                            "Could you tell me what the shift is?",
                        ],
                    ),
                ],
                listening=(
                    "Manager: Have you ever worked in a shop? Tereza: Not in a shop, but "
                    "I have helped in my uncle's bakery since May. Manager: What do you do "
                    "there? Tereza: I have served customers and I have learned to use the "
                    "till. I haven't baked bread yet. Manager: Can you start next Saturday? "
                    "Tereza: Yes. I have already asked my uncle, and he says it's fine. "
                    "I can't work on Sunday mornings because I have a match."
                ),
                listenq=[
                    Q(
                        "Where has Tereza helped?",
                        ["In a supermarket", "In her uncle's bakery", "In a hotel", "At school"],
                        1,
                        "in my uncle's bakery.",
                    ),
                    Q(
                        "Since when?",
                        ["Since March", "Since May", "Since Saturday", "For a year"],
                        1,
                        "since May.",
                    ),
                    Q(
                        "What hasn't she done yet?",
                        ["Served customers", "Used the till", "Baked bread", "Asked her uncle"],
                        2,
                        "I haven't baked bread yet.",
                    ),
                    Q(
                        "Why can't she work on Sunday mornings?",
                        ["The bakery is closed", "She has a match", "She is ill", "She has no till"],
                        1,
                        "because I have a match.",
                    ),
                ],
                gaps=[
                    G("She ___ (live) here since 2019.", "has lived|'s lived", "Since + předpřítomný čas."),
                    G("I have known him ___ three years.", "for", "For + délka."),
                    G("Have you ___ (be) to London?", "been", "Been to = mít zkušenost."),
                    G("We haven't finished ___.", "yet", "Yet v záporu na konci."),
                    G("He has ___ eaten, so he isn't hungry.", "already|just", "Already nebo just."),
                ],
            ),
            L(
                "Recenze filmu",
                "A film review",
                "Vztažné věty who, which a that. Krátká recenze a kvíz.",
                "Relative clauses with who, which and that. A short review and a quiz.",
                [
                    S(
                        "1 / 2   •   Vztažné věty",
                        "Která osoba, která věc",
                        "Tip: who pro lidi, which pro věci. That jde často pro obojí v definující větě.",
                        [
                            "The actor who plays the captain is brilliant.",
                            "A lighthouse which stands on a black rock.",
                            "V definující větě se čárka nepíše: The boy who lives next door.",
                            "Nedefinující věta čárku má a that se v ní nepoužívá: Prague, which I love, …",
                        ],
                    ),
                    S(
                        "2 / 2   •   Recenze",
                        "Názor, ne jen děj",
                        "Tip: neprozrazujte konec. Napište, pro koho film je.",
                        [
                            "Jedna věta o ději, jedna o postavě, jedna o tom, co se povedlo.",
                            "I would recommend it to anyone who likes quiet films.",
                            "The ending, which I will not describe, stays with you.",
                            "Hodnocení ať je konkrétní: the music is too loud, the script is sharp.",
                        ],
                    ),
                ],
                writing=W(
                    "Write a short review of a film or a book in at least 40 words. Use who once and which once, and say who you would recommend it to.",
                    "The Lighthouse Keeper is a quiet film which follows a woman on a small island. The actor who plays her hardly speaks, but her face tells the story. The sea, which is almost a character, is beautiful and dangerous. I would recommend it to anyone who likes slow films. The ending is sad but honest.",
                    "who|which|recommend",
                    40,
                ),
                quiz=[
                    Q(
                        "The nurse ___ helped me was very calm.",
                        ["which", "who", "where", "whose it"],
                        1,
                        "Člověk: who.",
                    ),
                    Q(
                        "The app ___ crashed is useless.",
                        ["who", "which", "where", "whom"],
                        1,
                        "Věc: which.",
                    ),
                    Q(
                        "Prague, ___ is my home city, is full of tourists in July.",
                        ["that", "which", "who", "what"],
                        1,
                        "S čárkou that nepatří. Which.",
                    ),
                    Q(
                        "That's the boy ___ dog ran into the road.",
                        ["who", "which", "whose", "that his"],
                        2,
                        "Čí: whose.",
                    ),
                ],
            ),
            L(
                "Počasí a výlet",
                "Weather and a trip",
                "Too a enough, will a going to v předpovědi. Čtení a doplňování.",
                "Too and enough, will and going to in a forecast. Reading and gaps.",
                [
                    S(
                        "1 / 2   •   Too / enough",
                        "Příliš a dost",
                        "Tip: too stojí před přídavným jménem, enough za ním.",
                        [
                            "It's too cold to swim. The water isn't warm enough.",
                            "Too many people, too much rain.",
                            "Enough + podstatné jméno: enough water. Nebo za přídavným jménem: old enough.",
                            "Not enough time je častější než too little time.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Předpověď",
                        "Co říct, když se díváte z okna",
                        "Tip: mraky, které vidíte, vedou k going to. Odhad bez důkazu k will.",
                        [
                            "Those clouds are black. It's going to storm.",
                            "I think it will be sunny on Sunday. To je jen odhad.",
                            "If it rains, we'll stay in the cabin.",
                            "Slovník: foggy, chilly, pouring, a heatwave, a breeze.",
                        ],
                    ),
                ],
                reading=(
                    "We wanted to cycle to the lake, but the morning was too foggy to "
                    "start. By ten the sun was out and the path was dry enough. There "
                    "were too many cyclists near the dam, so we took a quieter road. "
                    "We didn't have enough water, which was a mistake. At the lake a "
                    "guard said, \"Look at that cloud. It's going to pour. I think the "
                    "storm will last an hour.\" We were not fast enough. We got soaked."
                ),
                readq=[
                    Q(
                        "Why didn't they start early?",
                        ["The bikes were broken", "It was too foggy", "The lake was closed", "They had no map"],
                        1,
                        "too foggy to start.",
                    ),
                    Q(
                        "Why did they change the road?",
                        [
                            "There were too many cyclists",
                            "They were lost",
                            "It was pouring",
                            "The guard sent them back",
                        ],
                        0,
                        "too many cyclists near the dam.",
                    ),
                    Q(
                        "What mistake did they make?",
                        ["They forgot the bikes", "They didn't have enough water", "They started at night", "They swam"],
                        1,
                        "didn't have enough water.",
                    ),
                    Q(
                        "What did the guard predict from the cloud?",
                        ["Fog", "A heatwave", "Rain", "Snow"],
                        2,
                        "It's going to pour.",
                    ),
                ],
                gaps=[
                    G("The rucksack is ___ heavy to carry.", "too", "Too + přídavné jméno."),
                    G("She isn't tall ___ to reach the shelf.", "enough", "Enough za přídavným jménem."),
                    G("There is too ___ sugar in this tea.", "much", "Nepočitatelné: too much."),
                    G("Look at the sky. It ___ snow.", "is going to|'s going to", "Viditelný důkaz."),
                ],
            ),
            L(
                "Festival",
                "A festival",
                "Předpřítomný čas proti minulému. Poslech a kvíz.",
                "Present perfect versus past simple. Listening and a quiz.",
                [
                    S(
                        "1 / 2   •   Dva časy",
                        "Kdy je děj uzavřený",
                        "Tip: jakmile zazní yesterday, last year nebo in 2019, patří minulý čas.",
                        [
                            "I have been to the festival three times. Neříkám kdy.",
                            "I went in 2023. Tehdy pršelo. To už je uzavřené.",
                            "Have you ever slept in a tent? — Yes. I slept in one last June.",
                            "Been to znamená, že už je člověk zpátky. Gone to znamená, že je pořád pryč.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Reportáž",
                        "Co se děje na místě",
                        "Tip: reportér míchá „už se stalo“ a „bylo to včera večer“.",
                        [
                            "The gates have just opened.",
                            "Last night the main band played for two hours.",
                            "Have you seen the lanterns yet?",
                            "A local baker has made a cake in the shape of a violin.",
                        ],
                    ),
                ],
                listening=(
                    "This is Nora at the river festival. The gates have just opened and "
                    "hundreds of people have already come in. I have never seen so many "
                    "lanterns. Last night the main band played for two hours, but tonight's "
                    "singer hasn't arrived yet. A local baker has made a cake in the shape "
                    "of a violin. I tried a piece yesterday. It was delicious. Have you "
                    "ever been here? I came for the first time in 2022, and I have come "
                    "back every summer since then."
                ),
                listenq=[
                    Q(
                        "What has just happened?",
                        ["The band has left", "The gates have opened", "It has started to snow", "The baker has closed"],
                        1,
                        "The gates have just opened.",
                    ),
                    Q(
                        "When did the main band play?",
                        ["Just now", "Last night", "In 2022", "Every summer"],
                        1,
                        "Last night the main band played.",
                    ),
                    Q(
                        "What hasn't happened yet?",
                        ["People have come in", "Tonight's singer has arrived", "Nora tried the cake", "The gates opened"],
                        1,
                        "hasn't arrived yet.",
                    ),
                    Q(
                        "When did Nora come for the first time?",
                        ["Yesterday", "Last night", "In 2022", "This morning"],
                        2,
                        "for the first time in 2022.",
                    ),
                ],
                quiz=[
                    Q(
                        "I ___ this play in 2019.",
                        ["have seen", "saw", "seen", "have saw"],
                        1,
                        "Rok uzavírá děj: saw.",
                    ),
                    Q(
                        "She has ___ to Paris. She is still there.",
                        ["been", "gone", "went", "going"],
                        1,
                        "Pořád je pryč: gone.",
                    ),
                    Q(
                        "___ you ever slept in a tent?",
                        ["Did", "Have", "Do", "Were"],
                        1,
                        "Ever + present perfect: Have you ever…?",
                    ),
                    Q(
                        "We ___ the lanterns yesterday.",
                        ["have seen", "saw", "seen", "have watch"],
                        1,
                        "Yesterday → past simple.",
                    ),
                ],
            ),
            L(
                "Kdyby",
                "If I could",
                "První a druhá podmínková věta. Psaní a doplňování.",
                "First and second conditionals. Writing and gaps.",
                [
                    S(
                        "1 / 2   •   Podmínky",
                        "Reálná a hypotetická",
                        "Tip: v if větě není will. If it rains, we will stay in.",
                        [
                            "1. kondicionál: If I finish early, I will call you. Je to možné.",
                            "2. kondicionál: If I had a car, I would drive. Teď auto nemám.",
                            "V formální druhé podmínce je were: If I were you, I would apologise.",
                            "Unless = if not: We'll miss the bus unless we leave now.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Vlastní věta",
                        "Jedna reálná, jedna vysněná",
                        "Tip: would se v if větě druhé podmínky neopakuje na obou stranách špatně. If I would have je chyba.",
                        [
                            "Napište situaci, která se může stát tento týden.",
                            "Pak situaci, která teď pravda není.",
                            "Would like není druhá podmínka. To je jen přání: I'd like a tea.",
                            "Krátký text ať má oba typy, ať je rozdíl vidět.",
                        ],
                    ),
                ],
                writing=W(
                    "Write at least 35 words. Include one real plan with if and will, and one unreal dream with would. Use the word if twice.",
                    "If I finish my project tonight, I will go climbing with Eva on Saturday. We will take the early bus if the weather is fine. If I had more free time, I would learn the guitar. I would also travel by night train if tickets were cheaper. If is a small word with two very different lives.",
                    "if|will|would",
                    35,
                ),
                gaps=[
                    G("If it ___ (rain), we will stay in the tent.", "rains", "1. podmínka: přítomný čas v if."),
                    G("If I ___ (be) you, I would tell the truth.", "were|was", "Were je přesnější, was se taky slyší."),
                    G("She would travel more if she ___ (have) time.", "had", "2. podmínka: minulý tvar."),
                    G("We'll be late ___ we hurry.", "unless", "Unless = if not."),
                ],
            ),
            L(
                "Test 2. ročníku",
                "Year 2 test",
                "Čtení, poslech, gramatika B1 a krátké psaní.",
                "Reading, listening, B1 grammar and a short piece of writing.",
                [
                    S(
                        "1 / 2   •   B1",
                        "Co už má držet pohromadě",
                        "Tip: u předpřítomného času hledejte ever, yet, since. U minulého yesterday a rok.",
                        [
                            "Stupňování, past continuous, modální slovesa rady a povinnosti.",
                            "Present perfect proti past simple.",
                            "Who a which, too a enough, první a druhá podmínka.",
                            "Psaní: krátký souvislý názor, ne odrážkový seznam.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Strategie",
                        "Jedna věta navíc",
                        "Tip: v psaní ať je důvod. Because nebo so text zvedne.",
                        [
                            "U čtení škrtněte odpověď, která v textu vůbec není.",
                            "U poslechu si čísla zapište číslicí.",
                            "Mustn't není don't have to.",
                            "If I would have time je chyba. If I had time, I would…",
                        ],
                    ),
                ],
                reading=(
                    "The night bus to Brno is cheaper than the train, but it is also "
                    "slower. Last Friday I was sleeping when the driver stopped at a "
                    "dark petrol station. A woman who had missed her connection asked "
                    "if she could sit next to me. She has travelled this route since "
                    "March because her new job starts at six. If the bus is late, she "
                    "has to run. She says she would move closer if flats were cheaper."
                ),
                readq=[
                    Q(
                        "The bus is ___ than the train.",
                        ["faster and cheaper", "cheaper and slower", "the best", "as fast"],
                        1,
                        "cheaper, but slower.",
                    ),
                    Q(
                        "What was the writer doing when the bus stopped?",
                        ["Reading", "Sleeping", "Running", "Driving"],
                        1,
                        "I was sleeping when the driver stopped.",
                    ),
                    Q(
                        "Why has the woman travelled this route since March?",
                        ["She likes petrol stations", "Her job starts early", "Flats are cheap", "She missed a concert"],
                        1,
                        "her new job starts at six.",
                    ),
                ],
                listening=(
                    "Doctor: How long have you had this cough? Adam: Since Sunday. I have "
                    "also had a sore throat for two days. Doctor: You should rest and drink "
                    "water. You don't have to stay in bed all day, but you mustn't play "
                    "football this week. If you still feel ill on Friday, come back."
                ),
                listenq=[
                    Q("Since when has Adam had the cough?", ["Since Friday", "Since Sunday", "For a month", "Since March"], 1, "Since Sunday."),
                    Q("What mustn't he do?", ["Drink water", "Rest", "Play football", "Come back"], 2, "mustn't play football."),
                    Q("When should he come back?", ["If he is still ill on Friday", "Every day", "Never", "On Sunday morning"], 0, "If you still feel ill on Friday."),
                ],
                quiz=[
                    Q("This hostel is ___ than the hotel.", "cheaper / more cheap / the cheapest / cheap".split(" / "), 0, "cheap → cheaper."),
                    Q("I have lived here ___ 2020.", ["for", "since", "ago", "during"], 1, "Since + bod v čase."),
                    Q("You ___ wear a helmet. It's the law.", ["must to", "have to", "don't have", "should to"], 1, "Pravidlo: have to."),
                    Q("If I ___ richer, I would take the train.", ["am", "will be", "were", "would be"], 2, "2. podmínka: were."),
                ],
                writing=W(
                    "Write at least 35 words about a trip or a job you would like. Use would and because.",
                    "I would like to work in a small bookshop by the station because I love quiet mornings. I would talk to customers and I would also read in the breaks. If I had the job, I would cycle there. I have never worked in a shop, but I have helped at school events.",
                    "would|because",
                    35,
                ),
            ),
        ],
    },
]


def y3():
    forest = (
        "Last month our class recorded a podcast called The Last Beech. "
        "A ranger said that the trees were thirsty because the summers had "
        "become hotter. She told us that volunteers had planted two thousand "
        "young trees, but she added that many would die unless it rained. "
        "If the town banned cars from the forest road, the air would be cleaner. "
        "If people keep using it as a shortcut, the soil will wash away."
    )
    return {
        "sub_cs": "Společnost, prostředí, média a složitější gramatika. Úroveň B1 až B2.",
        "sub_en": "Society, the environment, the media and harder grammar. Level B1 to B2.",
        "lessons": [
            L(
                "Jak se to vyrábí",
                "How things are made",
                "Trpný rod v přítomnosti a minulosti. Doplňování a kvíz.",
                "The present and past passive. Gaps and a quiz.",
                [
                    S(
                        "1 / 2   •   Passive",
                        "Když je důležitější děj než původce",
                        "Tip: be + příčestí. Čas nese sloveso be, ne příčestí.",
                        [
                            "English is spoken here. The window was broken last night.",
                            "Původce s by, jen když na něm záleží: The book was written by a nurse.",
                            "Modální trpný rod: The form must be signed. It can be recycled.",
                            "Nelze trpně vyjádřit každé sloveso. Happen trpný rod nemá.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Popis procesu",
                        "Od suroviny k věci",
                        "Tip: first, then, after that, finally. Původce často chybí, protože je obecný.",
                        [
                            "The beans are picked by hand. They are dried in the sun.",
                            "The jars were labelled yesterday.",
                            "Waste paper is collected every Thursday.",
                            "A new bridge is being built. To už je průběhový trpný rod, zatím stačí ho poznat.",
                        ],
                    ),
                ],
                gaps=[
                    G("This bread ___ (bake) every morning.", "is baked", "Přítomný trpný rod."),
                    G("The letters ___ (send) yesterday.", "were sent", "Minulý trpný rod, množné číslo."),
                    G("The poem ___ (write) by a student.", "was written", "Write → written."),
                    G("These bottles can ___ (recycle).", "be recycled", "Modální sloveso + be + příčestí."),
                    G("The window ___ (not break) by the wind. Someone hit it.", "wasn't broken|was not broken", "Zápor minulého trpného rodu."),
                ],
                quiz=[
                    Q(
                        "Which sentence is passive?",
                        [
                            "Someone stole my bike.",
                            "My bike was stolen.",
                            "My bike stole.",
                            "Someone was steal my bike.",
                        ],
                        1,
                        "was + stolen.",
                    ),
                    Q(
                        "The news ___ at ten.",
                        ["is reading", "are read", "is read", "read"],
                        2,
                        "News je jednotné: is read.",
                    ),
                    Q(
                        "You ___ wear a badge. The factory requires it, in the passive of must.",
                        ["must be worn a badge", "A badge must be worn", "A badge must worn", "A badge is must wear"],
                        1,
                        "A badge must be worn.",
                    ),
                    Q(
                        "They built the school in 1998. → The school ___ in 1998.",
                        ["built", "was built", "was build", "has built"],
                        1,
                        "was built.",
                    ),
                ],
            ),
            L(
                "Škola, o které se mluví",
                "A school people talk about",
                "Nepřímá řeč oznamovací. Čtení a kvíz.",
                "Reported statements. Reading and a quiz.",
                [
                    S(
                        "1 / 2   •   Reported speech",
                        "Posun času",
                        "Tip: said that, told me that. Tell potřebuje osobu, say ne.",
                        [
                            "„I am tired.“ → She said she was tired.",
                            "„I will help.“ → He said he would help.",
                            "„I have finished.“ → She told me she had finished.",
                            "„I can't come.“ → He said he couldn't come.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Kdy čas neposouvat",
                        "Pořád to platí",
                        "Tip: obecné pravdy a situace, které trvají, často zůstávají v přítomném čase.",
                        [
                            "The teacher said that water boils at 100 degrees.",
                            "She said she lives in Kolín, a pořád tam bydlí.",
                            "Zájmena se mění podle smyslu: „my book“ → he said his book.",
                            "Today → that day, tomorrow → the next day, yesterday → the day before. V testu to bude jen u jasných vět.",
                        ],
                    ),
                ],
                reading=(
                    "Our head teacher told the local paper that the school was changing. "
                    "She said that students would start an hour later next term. One pupil "
                    "told the reporter that he was pleased because he hated early buses. "
                    "A parent said that she was worried. She said her daughter had joined "
                    "three clubs and had very little time. The cook said that lunch would "
                    "still be at noon. He added that the menu was going to include more vegetables."
                ),
                readq=[
                    Q(
                        "What did the head teacher say about the start of school?",
                        [
                            "It would start an hour later",
                            "It would start at night",
                            "It had already closed",
                            "Clubs would end",
                        ],
                        0,
                        "students would start an hour later.",
                    ),
                    Q(
                        "Why was the pupil pleased?",
                        ["He liked buses", "He hated early buses", "He wanted less lunch", "He had three clubs"],
                        1,
                        "because he hated early buses.",
                    ),
                    Q(
                        "What had the daughter done?",
                        ["She had left school", "She had joined three clubs", "She had become a cook", "She had bought a bus"],
                        1,
                        "had joined three clubs.",
                    ),
                    Q(
                        "Lunch, according to the cook, …",
                        ["would move to the evening", "would still be at noon", "had been cancelled", "was only vegetables"],
                        1,
                        "would still be at noon.",
                    ),
                ],
                quiz=[
                    Q(
                        "„I am hungry.“ → She said she ___ hungry.",
                        ["is", "was", "has", "were be"],
                        1,
                        "am → was.",
                    ),
                    Q(
                        "„I will call.“ → He said he ___ call.",
                        ["will", "would", "called", "has"],
                        1,
                        "will → would.",
                    ),
                    Q(
                        "Which verb needs a person?",
                        ["say", "tell", "speak that", "talk that"],
                        1,
                        "tell somebody.",
                    ),
                    Q(
                        "She said, „I can't swim.“ → She said she ___.",
                        ["can't swim", "couldn't swim", "doesn't swim", "hasn't swum"],
                        1,
                        "can → could. V nepřímé řeči o minulém rozhovoru.",
                    ),
                ],
            ),
            L(
                "Poslední buk",
                "The last beech",
                "Podmínky a slovník prostředí. Poslech podcastu a doplňování.",
                "Conditionals and environment vocabulary. A podcast and gaps.",
                [
                    S(
                        "1 / 2   •   Prostředí",
                        "Konkrétní slova, ne jen nature",
                        "Tip: drought, soil, volunteer, ban, shortcut. U maturitního tématu zabírají víc než slovo pollution samo.",
                        [
                            "A drought is a long time without rain. Soil is the earth plants grow in.",
                            "Unless it rains, the young trees will die.",
                            "If the town banned cars, the air would be cleaner. Teď je nezakázalo, proto would.",
                            "If people keep using the road, the soil will wash away. To se může stát.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Podcast",
                        "Kdo co řekl",
                        "Tip: u nepřímé řeči v poslechu si pamatujte sloveso řekl/řekla a pak obsah.",
                        [
                            "Ranger = strážce lesa.",
                            "Young trees were planted, but many may still die.",
                            "A shortcut šetří čas a někdy ničí místo.",
                            "Čísla si zapište: two thousand.",
                        ],
                    ),
                ],
                listening=forest,
                listenq=[
                    Q(
                        "What did the ranger say about the trees?",
                        ["They were thirsty", "They had been cut down", "They were a shortcut", "They banned cars"],
                        0,
                        "the trees were thirsty.",
                    ),
                    Q(
                        "How many young trees had volunteers planted?",
                        ["Two hundred", "Two thousand", "Twenty", "None"],
                        1,
                        "two thousand.",
                    ),
                    Q(
                        "The young trees will die unless…",
                        ["people drive there", "it rains", "the podcast ends", "the soil is sold"],
                        1,
                        "unless it rained / unless it rains. V textu: unless it rained v nepřímé řeči.",
                    ),
                    Q(
                        "What would make the air cleaner?",
                        [
                            "Banning cars from the forest road",
                            "Using the road as a shortcut",
                            "Cutting the beeches",
                            "Washing the soil",
                        ],
                        0,
                        "If the town banned cars.",
                    ),
                ],
                gaps=[
                    G("If we ___ (ban) cars here, the air would be cleaner.", "banned", "2. podmínka."),
                    G("The soil will wash away ___ people stop using the shortcut.", "unless", "Unless = if not. Pozor: věta chce if not ve významu dokud nepřestanou. Unless people stop = if they don't stop."),
                    G("Many young trees ___ (plant) last month.", "were planted", "Minulý trpný rod."),
                    G("She said the summers ___ (become) hotter.", "had become", "Posun předpřítomného času."),
                ],
            ),
            L(
                "Pro a proti",
                "For and against",
                "Stavba úvahy a spojovací výrazy. Esej a kvíz.",
                "Essay structure and linking words. An essay and a quiz.",
                [
                    S(
                        "1 / 2   •   Čtyři kroky",
                        "Úvod, dvě strany, závěr",
                        "Tip: v úvodu otázku neopakujte doslova. Převeďte ji vlastními slovy.",
                        [
                            "Úvod: o čem text je a že existují dva pohledy.",
                            "Jeden odstavec pro výhody, jeden pro nevýhody.",
                            "On the one hand … On the other hand …",
                            "Závěr: váš názor. In my opinion, I believe. Nepřidávejte úplně nový argument.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Spojky",
                        "However není because",
                        "Tip: however stojí mezi větami a odděluje se čárkou. Although spojuje věty v jednu.",
                        [
                            "School uniforms are cheaper. However, they limit personality.",
                            "Although uniforms save time, many students hate them.",
                            "Because dává důvod. So dává výsledek.",
                            "In conclusion shrnuje. Nepíše se In the conclusion.",
                        ],
                    ),
                ],
                writing=W(
                    "Write a for-and-against paragraph of at least 50 words about school uniforms. Use however and because, and finish with your opinion using believe.",
                    "School uniforms are a common argument. They are practical because students do not have to decide what to wear, and families spend less money. However, a uniform hides personality and can feel childish at eighteen. Although it makes mornings faster, it does not make lessons better. I believe schools should keep a simple dress code, but they should not force one jacket on everyone.",
                    "however|because|believe",
                    50,
                ),
                quiz=[
                    Q(
                        "Which linker shows contrast?",
                        ["because", "however", "so", "for example"],
                        1,
                        "However staví protiklad.",
                    ),
                    Q(
                        "___ uniforms save time, students still complain.",
                        ["However", "Although", "So", "Believe"],
                        1,
                        "Although spojuje dvě části jedné věty.",
                    ),
                    Q(
                        "Choose the best ending of an essay.",
                        [
                            "In conclusion, I have no idea.",
                            "In conclusion, I believe a simple rule is enough.",
                            "Because however so.",
                            "To begin with, the essay starts now.",
                        ],
                        1,
                        "Závěr má názor, ne nový zmatek.",
                    ),
                    Q(
                        "Because introduces…",
                        ["a result only", "a reason", "a list of names", "a question"],
                        1,
                        "Because = důvod.",
                    ),
                ],
            ),
            L(
                "Kdo to musel být",
                "It must have been",
                "Modální slovesa dedukce. Čtení záhady a kvíz.",
                "Modals of deduction. A short mystery and a quiz.",
                [
                    S(
                        "1 / 2   •   Dedukce",
                        "Jisté, možné, vyloučené",
                        "Tip: must je skoro jistota, might je možnost, can't je „to přece nejde“.",
                        [
                            "The lights are on. She must be at home.",
                            "He might be in the lab. I'm not sure.",
                            "That can't be Eva. Eva is in Vienna this week.",
                            "O minulosti: must have left, might have forgotten, can't have seen it.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Vynález",
                        "Čtěte důkazy, ne nadpis",
                        "Tip: u must have hledejte důkaz, který už nejde rozumně popřít.",
                        [
                            "A notebook with today's date is stronger than a guess.",
                            "Can't have znamená, že to odporuje faktu.",
                            "Might have nechává dvě možnosti otevřené.",
                            "Nepoužívejte can pro dedukci v kladné větě. Can umí, must musí být.",
                        ],
                    ),
                ],
                reading=(
                    "Someone left a strange machine in the physics lab. It was warm, so "
                    "it must have been switched on recently. The only key can't have been "
                    "used by the caretaker, because he is in hospital. Two students, Nora "
                    "and Adam, know the code. Nora might have come back after the club, "
                    "but her train ticket shows she was in Pardubice. Adam's notebook is "
                    "on the bench and the handwriting matches the label. He must have built it."
                ),
                readq=[
                    Q(
                        "Why must the machine have been on recently?",
                        ["It was noisy", "It was warm", "It was new", "It was labelled"],
                        1,
                        "It was warm.",
                    ),
                    Q(
                        "Why can't the caretaker have used the key?",
                        ["He lost it", "He is in hospital", "He hates physics", "He was in Pardubice"],
                        1,
                        "he is in hospital.",
                    ),
                    Q(
                        "Where was Nora?",
                        ["In the lab", "In Pardubice", "In hospital", "On the bench"],
                        1,
                        "her ticket shows Pardubice.",
                    ),
                    Q(
                        "Who must have built the machine?",
                        ["The caretaker", "Nora", "Adam", "Nobody"],
                        2,
                        "The notebook and the handwriting point to Adam.",
                    ),
                ],
                quiz=[
                    Q("The window is broken and his bag is gone. He ___ left in a hurry.", ["must have", "must", "can have", "should"], 0, "Minulá skoro jistota: must have left."),
                    Q("That ___ be Lea. Lea is abroad.", ["must", "can't", "should", "is"], 1, "Odporuje faktu: can't."),
                    Q("I'm not sure. They ___ have missed the bus.", ["must", "can't", "might", "are"], 2, "Možnost: might have."),
                    Q("Which sentence is a deduction, not a duty?", ["You must wear gloves.", "She must be tired.", "You must sign this.", "Visitors must wait."], 1, "She must be tired je úsudek."),
                ],
            ),
            L(
                "Zprávy dne",
                "The news desk",
                "Nepřímé otázky. Poslech rozhovoru a kvíz.",
                "Reported questions. An interview and a quiz.",
                [
                    S(
                        "1 / 2   •   Reported questions",
                        "Otázka se v nepřímé řeči narovná",
                        "Tip: žádný otazník a pořadí podmětu a slovesa jako v oznamovací větě.",
                        [
                            "„Where do you live?“ → She asked me where I lived.",
                            "„Did you see it?“ → He asked if I had seen it.",
                            "Whether a if jsou u zjišťovacích otázek obě možné.",
                            "Tázací výraz zůstává: what, where, why, how long.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Redakce",
                        "Novinář se ptá starosty",
                        "Tip: v poslechu odlište otázku reportéra od odpovědi starosty.",
                        [
                            "Asked if the bridge was safe.",
                            "Wondered how long the repairs would take.",
                            "The mayor said he didn't know yet.",
                            "Slovesa ask, wonder, want to know.",
                        ],
                    ),
                ],
                listening=(
                    "Reporter: Mayor, is the old bridge safe? Mayor: I can't promise that. "
                    "An engineer told me that two cables were weak. Reporter: How long will "
                    "the repairs take? Mayor: She asked the firm the same question yesterday. "
                    "They said they didn't know yet. Reporter: Did anyone complain? Mayor: "
                    "A shopkeeper asked me why the bus stop had moved. I asked her if she "
                    "could wait until Friday. Reporter: Will you close the bridge? Mayor: "
                    "We might. I have asked the police whether they can send officers tonight."
                ),
                listenq=[
                    Q(
                        "What did the engineer tell the mayor?",
                        ["The bridge was new", "Two cables were weak", "The shop was closed", "Friday was impossible"],
                        1,
                        "two cables were weak.",
                    ),
                    Q(
                        "What didn't the firm know?",
                        ["The mayor's name", "How long the repairs would take", "Where the bus stop was", "Who the police were"],
                        1,
                        "They said they didn't know yet. Šlo o délku oprav.",
                    ),
                    Q(
                        "What did the shopkeeper ask?",
                        [
                            "Why the bus stop had moved",
                            "If the mayor could swim",
                            "Where the cables were made",
                            "Whether Friday was a holiday",
                        ],
                        0,
                        "why the bus stop had moved.",
                    ),
                    Q(
                        "Who might come tonight?",
                        ["The firm", "The shopkeeper", "Police officers", "The engineer only"],
                        2,
                        "whether they can send officers tonight.",
                    ),
                ],
                quiz=[
                    Q(
                        "„Where do you live?“ → He asked me where I ___.",
                        ["did live", "lived", "do live", "living"],
                        1,
                        "Pořadí oznamovací věty: where I lived.",
                    ),
                    Q(
                        "„Are you ready?“ → She asked if I ___.",
                        ["was ready", "am I ready", "ready was", "did ready"],
                        0,
                        "if I was ready.",
                    ),
                    Q(
                        "Which sentence is wrong?",
                        [
                            "He asked where she was.",
                            "He asked where was she.",
                            "He asked if she was tired.",
                            "He wanted to know why she had left.",
                        ],
                        1,
                        "V nepřímé otázce není inverze.",
                    ),
                    Q(
                        "Whether means about the same as…",
                        ["because", "if", "however", "whose"],
                        1,
                        "Whether ≈ if v nepřímé zjišťovací otázce.",
                    ),
                ],
            ),
            L(
                "Frázová slovesa v příběhu",
                "Phrasal verbs in a story",
                "Běžná frázová slovesa. Doplňování a kvíz.",
                "Common phrasal verbs. Gaps and a quiz.",
                [
                    S(
                        "1 / 2   •   Částice mění význam",
                        "Look není look after",
                        "Tip: frázové sloveso se učte jako jednu jednotku, s českým protějškem ve větě.",
                        [
                            "look after = starat se, look for = hledat, look forward to = těšit se.",
                            "give up = vzdát se, find out = zjistit, run out of = dojít zásoba.",
                            "turn off = vypnout, take off = sundat nebo vzlétnout, get on = nastoupit i vycházet.",
                            "Look forward to je následované -ing: I look forward to seeing you.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Příběh",
                        "Jedno sloveso, jedna situace",
                        "Tip: když částici neznáte, představte si gesto. Off je pryč, up je do konce.",
                        [
                            "The bus took off není o oblečení. U letadla ano, u autobusu spíš pulled away.",
                            "Take off a coat = sundat. Take off = vzlétnout. Kontext rozhoduje.",
                            "Get on with somebody = vycházet s někým.",
                            "Run out of time je u testu častější než run out time.",
                        ],
                    ),
                ],
                gaps=[
                    G("Can you look ___ my dog this weekend?", "after", "Look after = postarat se."),
                    G("We are looking ___ to the trip.", "forward", "Look forward to."),
                    G("Don't give ___. The last chapter is the best.", "up", "Give up = vzdát to."),
                    G("I need to find ___ who left the note.", "out", "Find out = zjistit."),
                    G("We've run ___ of milk.", "out", "Run out of."),
                ],
                quiz=[
                    Q("Please turn ___ the lights.", ["off", "after", "forward", "out of"], 0, "Turn off = vypnout."),
                    Q("She gets ___ well with her neighbours.", ["on", "off", "up", "after"], 0, "Get on with = vycházet."),
                    Q("He took ___ his wet coat.", ["off", "up to", "after", "forward"], 0, "Sundat: take off."),
                    Q("I look forward to ___ you.", ["see", "seeing", "saw", "seen"], 1, "To je předložka, následuje -ing."),
                ],
            ),
            L(
                "Test 3. ročníku",
                "Year 3 test",
                "Čtení, poslech, gramatika B1+ a krátká úvaha.",
                "Reading, listening, B1+ grammar and a short argument.",
                [
                    S(
                        "1 / 2   •   B1+",
                        "Trpný rod, řeč, podmínky, dedukce",
                        "Tip: u trpného rodu hledejte be. U nepřímé otázky žádnou inverzi.",
                        [
                            "was written, must be signed, is spoken.",
                            "said she would, asked where I lived.",
                            "If they banned cars, the air would be cleaner.",
                            "must have, might have, can't have.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Úvaha ve zkratce",
                        "However a because v jednom textu",
                        "Tip: jedna výhoda, jeden háček, jedna věta s názorem.",
                        [
                            "Neopakujte zadání první větou.",
                            "Phrasal verbs nechte ve správné částici. Look after není look for.",
                            "Told me, ne said me.",
                            "In conclusion je až na konci, ne v úvodu.",
                        ],
                    ),
                ],
                reading=(
                    "A repair café opened in a former bakery. Broken radios are mended "
                    "there on Saturdays. The founder said that people would rather repair "
                    "than buy if they were shown how. She told a reporter that twenty "
                    "kettles had been saved in March. Visitors have to bring the broken "
                    "object, but they don't have to pay. If the café closed, a lot of "
                    "working parts would be thrown away."
                ),
                readq=[
                    Q("What happens to radios there?", ["They are sold", "They are mended", "They are banned", "They are thrown away at once"], 1, "are mended."),
                    Q("How many kettles had been saved in March?", ["Twelve", "Twenty", "Two thousand", "None"], 1, "twenty kettles."),
                    Q("What would happen if the café closed?", ["Parts would be thrown away", "Radios would be cheaper", "People would pay more", "The bakery would reopen as a shop"], 0, "working parts would be thrown away."),
                ],
                listening=(
                    "Editor: Did the mayor answer you? Journalist: I asked him whether the "
                    "bridge was safe. He said he didn't know. I also asked how long the "
                    "repairs would take. He told me that the firm hadn't decided. A resident "
                    "asked me why the paper hadn't printed a map. I said we would print one "
                    "tomorrow."
                ),
                listenq=[
                    Q("What did the journalist ask about safety?", ["Whether the bridge was safe", "If the paper was closed", "Where the mayor lived", "Why kettles break"], 0, "whether the bridge was safe."),
                    Q("What hadn't the firm done?", ["Decided", "Printed a map", "Closed the paper", "Asked the resident"], 0, "hadn't decided."),
                    Q("What did the journalist promise?", ["To close the bridge", "To print a map tomorrow", "To repair the cables", "To interview a baker"], 1, "we would print one tomorrow."),
                ],
                quiz=[
                    Q("The cake ___ by a student.", ["baked", "was baked", "was bake", "has bake"], 1, "was baked."),
                    Q("She asked me where I ___.", ["did live", "lived", "do live", "living"], 1, "where I lived."),
                    Q("He ___ be at school. I just saw him there.", ["can't", "must", "mustn't", "should to"], 1, "Skoro jistota: must."),
                    Q("Please look ___ the tickets. I can't find them.", ["for", "forward", "after", "up of"], 0, "Look for = hledat. Look after by bylo hlídat."),
                ],
                writing=W(
                    "In at least 40 words, give one reason for repairing things and one objection. Use because and however.",
                    "Repairing things matters because a kettle can work for years after a small fix. However, some people do not have the tools or the time, so a café where volunteers help is a fair idea. I believe every town should have one quiet room for broken objects. It is slower than shopping, but it is kinder.",
                    "because|however",
                    40,
                ),
            ),
        ],
    }


def y4():
    article = (
        "When the night train from Prague to Berlin returned last spring, "
        "stations filled with people who prefer a bed to a boarding gate. "
        "A single ticket is often more expensive than a flight, which surprises "
        "anyone who thinks trains are always the cheap choice. Still, passengers "
        "say they arrive calmer. They can read, talk and avoid the bright shops "
        "of an airport. Critics point out that a delay of two hours is common "
        "and that the cabins are small. The railway has promised that new carriages "
        "will be added if demand stays high. A student I spoke to had taken the "
        "train three times. She said she would fly only if she missed the evening "
        "departure. For her, the journey is part of the trip, not a problem to be "
        "deleted."
    )
    interview = (
        "Interviewer: Why did you apply to the summer camp in Wales? "
        "Hana: I have worked with children for two years, and I wanted a job "
        "where English is the only language. Interviewer: What would you do if "
        "a child refused to join an activity? Hana: I wouldn't force them. I "
        "would ask why, and I might offer a quieter task. Interviewer: Have you "
        "ever dealt with an emergency? Hana: Yes. Last August a boy cut his foot. "
        "I cleaned the cut and called the nurse. I didn't give him any medicine. "
        "Interviewer: The camp is rainy. Is that a problem? Hana: Not really. "
        "I have already bought boots. I look forward to the hills, although I "
        "know the work will be tiring."
    )
    return {
        "sub_cs": "Maturitní dovednosti: mluvení, formální psaní, čtení, poslech a gramatika. Úroveň B2.",
        "sub_en": "Exam skills: speaking, formal writing, reading, listening and grammar. Level B2.",
        "lessons": [
            L(
                "Maturitní kartička",
                "The speaking card",
                "Strategie ústní zkoušky a kolokace osobních témat. Kvíz a doplňování.",
                "Speaking-exam strategy and personal-topic collocations. A quiz and gaps.",
                [
                    S(
                        "1 / 2   •   Ústní zkouška",
                        "Čtyři minuty nejsou román",
                        "Tip: katalog CERMAT chce souvislý projev, popis obrázku, porovnání a reakci na otázky. Ne seznam slov.",
                        [
                            "Úvod jednou větou: I'd like to talk about my town, which is smaller than it looks.",
                            "Obecné plus osobní: Many people commute. I cycle, because the centre is close.",
                            "Když slovo chybí, opis: the thing you use to open a tin.",
                            "Na obrázku: In the foreground, in the background, it looks as if.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Kolokace",
                        "Make, do, have, take",
                        "Tip: české „dělat“ se rozpadá. Do homework, make a mistake, take a photo, have a rest.",
                        [
                            "Make a decision, make friends, make the bed.",
                            "Do the washing-up, do a course, do someone a favour.",
                            "Take part in, take care of, take an exam.",
                            "Have a look, have a word with, have trouble with.",
                        ],
                    ),
                ],
                gaps=[
                    G("I need to ___ a decision before Friday.", "make", "Make a decision."),
                    G("She ___ her homework on the train.", "does|did", "Do homework."),
                    G("Can I ___ a photo of the bridge?", "take", "Take a photo."),
                    G("Let's ___ a rest. We've walked for hours.", "have", "Have a rest."),
                    G("He ___ part in the school play every year.", "takes|took", "Take part in."),
                ],
                quiz=[
                    Q(
                        "You don't know the word kettle. What helps?",
                        [
                            "Stop the exam",
                            "Paraphrase it",
                            "Switch to Czech for the rest",
                            "Invent a Latin word and insist",
                        ],
                        1,
                        "Opis je lepší než ticho.",
                    ),
                    Q(
                        "In the foreground means…",
                        ["at the back", "at the front of the picture", "outside the frame", "in the past"],
                        1,
                        "Foreground = popředí.",
                    ),
                    Q(
                        "Which collocation is right?",
                        ["do a mistake", "make a mistake", "take a mistake", "have a mistake"],
                        1,
                        "Make a mistake.",
                    ),
                    Q(
                        "A strong speaking answer…",
                        [
                            "is a list of single words",
                            "adds a personal example",
                            "translates the question",
                            "avoids all verbs",
                        ],
                        1,
                        "Obecné plus vlastní příklad.",
                    ),
                ],
            ),
            L(
                "Formální e-mail",
                "A formal email",
                "Žádost a stížnost. Psaní a kvíz rejstříku.",
                "A request and a complaint. Writing and a register quiz.",
                [
                    S(
                        "1 / 2   •   Kostra",
                        "Komu, proč, co chcete, jak se loučíte",
                        "Tip: neznámé jméno → Dear Sir or Madam a Yours faithfully. Známé jméno → Dear Ms Novák a Yours sincerely.",
                        [
                            "I am writing to apply for… / to complain about…",
                            "I would be grateful if you could…",
                            "Kontrakce v čistě formálním textu omezte: I am, not I'm.",
                            "Na konci celé jméno. Žádné Cheers ani xoxo.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Stížnost",
                        "Fakta, dopad, žádost",
                        "Tip: zlost text oslabuje. Datum, číslo spoje a klidná žádost ho posílí.",
                        [
                            "The train on 3 May, which I had booked, arrived 90 minutes late.",
                            "As a result I missed the interview.",
                            "I would like a refund or an explanation.",
                            "I look forward to your reply.",
                        ],
                    ),
                ],
                writing=W(
                    "Write a formal email of at least 55 words to a railway company. Complain about a delay. Start with Dear, use would, and end with sincerely or faithfully.",
                    "Dear Sir or Madam, I am writing to complain about the night train to Berlin on 3 May. It arrived ninety minutes late, so I missed a morning interview. I would be grateful if you could explain the delay and offer a refund. I have attached my ticket. Please reply to this address. Yours faithfully, Tereza Malá.",
                    "dear|would|faithfully",
                    55,
                ),
                quiz=[
                    Q(
                        "You do not know the person's name. You end with…",
                        ["Yours faithfully", "Yours sincerely", "Cheers", "Love"],
                        0,
                        "Neznámé jméno: faithfully. Po Dear Sir or Madam.",
                    ),
                    Q(
                        "Dear Ms Novák, … ___",
                        ["Yours faithfully", "Yours sincerely", "Hi again", "Bye"],
                        1,
                        "Jméno znáte: sincerely.",
                    ),
                    Q(
                        "Which opening is formal?",
                        ["Hey!!!", "I am writing to apply for the post.", "I wanna the job.", "What's up"],
                        1,
                        "I am writing to…",
                    ),
                    Q(
                        "A formal request sounds like…",
                        [
                            "Give me a refund now.",
                            "I would be grateful if you could look into this.",
                            "You guys messed up.",
                            "Send money pls.",
                        ],
                        1,
                        "I would be grateful if you could…",
                    ),
                ],
            ),
            L(
                "Noční vlak",
                "The night train",
                "Delší čtení ve stylu didaktického testu. Čtyři otázky k článku.",
                "A longer reading in the style of the written paper. Four questions.",
                [
                    S(
                        "1 / 2   •   Čtení u maturity",
                        "Nečtěte každé slovo stejně pomalu",
                        "Tip: u výběru ze čtyř možností nejdřív zjistěte, na co se otázka ptá, a teprve pak hledejte odstavec.",
                        [
                            "Hlavní myšlenka bývá v úvodu nebo v závěru, ne v jednom příkladu.",
                            "Distraktor často opakuje slovo z textu, ale v jiném vztahu.",
                            "Which surprises… uvádí postoj, ne jen fakt o ceně.",
                            "Not given v tomhle cvičení není. Jedna možnost je opřená o text.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Slovník z kontextu",
                        "Boarding gate a carriage",
                        "Tip: neznámé slovo ohraničte větou před ním a za ním.",
                        [
                            "A boarding gate je letištní východ k letadlu. Text ho staví proti posteli ve vlaku.",
                            "A carriage je vůz vlaku.",
                            "Demand = poptávka. If demand stays high, přidají vozy.",
                            "Deleted journey by byl let, který člověk chce mít co nejkratší.",
                        ],
                    ),
                ],
                reading=article,
                readq=[
                    Q(
                        "What is the main point?",
                        [
                            "Flights have been cancelled",
                            "Some people choose the night train even when it costs more",
                            "Berlin has closed its station",
                            "Students may not travel",
                        ],
                        1,
                        "Lístek bývá dražší než let, a přesto lidé vlak volí.",
                    ),
                    Q(
                        "What surprises some readers?",
                        [
                            "That a train ticket can cost more than a flight",
                            "That stations are empty",
                            "That nobody reads",
                            "That the train is free",
                        ],
                        0,
                        "more expensive than a flight, which surprises…",
                    ),
                    Q(
                        "What do critics mention?",
                        ["Free meals", "Delays and small cabins", "Too many books", "A lack of stations"],
                        1,
                        "a delay of two hours, cabins are small.",
                    ),
                    Q(
                        "The student would fly only if…",
                        [
                            "the ticket was cheap",
                            "she missed the evening departure",
                            "the cabins were small",
                            "she hated reading",
                        ],
                        1,
                        "only if she missed the evening departure.",
                    ),
                    Q(
                        "New carriages will be added if…",
                        ["critics agree", "demand stays high", "flights are banned", "students complain"],
                        1,
                        "if demand stays high.",
                    ),
                ],
            ),
            L(
                "Tábor ve Walesu",
                "A camp in Wales",
                "Delší poslech: pohovor. Pět otázek.",
                "A longer listening: an interview. Five questions.",
                [
                    S(
                        "1 / 2   •   Poslech u maturity",
                        "Dvě přehrání stačí, když víte co",
                        "Tip: při prvním poslechu chyťte kostru, při druhém doplňte past, podmínku a zápor.",
                        [
                            "Otázky si přečtěte dřív. Podtrhněte tázací slovo.",
                            "Čísla a never/already mění význam celé věty.",
                            "Wouldn't force není couldn't.",
                            "I didn't give him any medicine je důležité právě proto, že je zápor.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Práce s dětmi",
                        "Slovník pohovoru",
                        "Tip: deal with an emergency, refuse, offer, look forward to.",
                        [
                            "For two years je délka, ne bod v čase.",
                            "A quieter task je alternativa, ne trest.",
                            "Although přiznává nevýhodu a názor nemění.",
                            "Boots jsou důkaz, že s deštěm počítá. Proto going to / already.",
                        ],
                    ),
                ],
                listening=interview,
                listenq=[
                    Q(
                        "Why does Hana want the job?",
                        [
                            "She has never met children",
                            "She wants a job where only English is spoken",
                            "She hates hills",
                            "She wants to sell boots",
                        ],
                        1,
                        "where English is the only language.",
                    ),
                    Q(
                        "How long has she worked with children?",
                        ["For two weeks", "For two years", "Since last August only", "She hasn't"],
                        1,
                        "for two years.",
                    ),
                    Q(
                        "What would she do if a child refused an activity?",
                        [
                            "Force them",
                            "Send them home",
                            "Ask why and maybe offer a quieter task",
                            "Give them medicine",
                        ],
                        2,
                        "I wouldn't force them. I might offer a quieter task.",
                    ),
                    Q(
                        "What didn't she do in the emergency?",
                        ["Clean the cut", "Call the nurse", "Give medicine", "Help the boy"],
                        2,
                        "I didn't give him any medicine.",
                    ),
                    Q(
                        "How does she feel about the rain?",
                        [
                            "She has already bought boots and still looks forward to the hills",
                            "She will refuse the job",
                            "She has never seen rain",
                            "She hates children",
                        ],
                        0,
                        "already bought boots, look forward to the hills.",
                    ),
                ],
            ),
            L(
                "Názorová esej",
                "An opinion essay",
                "Struktura názoru. Esej a kvíz o odstavcích.",
                "Opinion-essay structure. An essay and a quiz about paragraphs.",
                [
                    S(
                        "1 / 2   •   Názor",
                        "Nejdřív stanovisko, pak důvody",
                        "Tip: u názorové eseje čtenář nemá hádat, na čí straně jste. Řekněte to v úvodu.",
                        [
                            "I believe that… / In my opinion…",
                            "First reason, second reason, a short answer to the other side.",
                            "Although some people argue that…, I still think…",
                            "Závěr ať zní jako závěr, ne jako třetí nový důvod.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Rozsah",
                        "Radši čtyři hutné věty než deset prázdných",
                        "Tip: u písemné práce se počítají slova. Opakování zadání se nepočítá jako myšlenka.",
                        [
                            "Každý důvod ať má příklad z běžného života.",
                            "Nepoužívejte etc. Místo toho jedno konkrétní slovo.",
                            "Spojky: firstly, in addition, on the other hand, in conclusion.",
                            "Kontraktované tvary jsou v názorové eseji v pořádku, když text není stížnost úřadu.",
                        ],
                    ),
                ],
                writing=W(
                    "Write an opinion of at least 60 words: should students have a gap year before university? Use believe, although and in conclusion.",
                    "I believe a gap year can help, but only when it has a plan. Although some students waste the time, others work, travel or look after a relative and return calmer. A year in a job also shows whether a degree is really necessary. On the other hand, a long break can make it harder to study again. In conclusion, a gap year is useful if the person can explain what they will do with it.",
                    "believe|although|conclusion",
                    60,
                ),
                quiz=[
                    Q(
                        "Where should your opinion appear in this kind of essay?",
                        ["Only in a footnote", "In the introduction and again at the end", "Never", "Only in the title"],
                        1,
                        "Úvod i závěr.",
                    ),
                    Q(
                        "Although introduces…",
                        ["a concession", "a list of books", "a greeting", "a date"],
                        0,
                        "Ústupka: something true that does not win.",
                    ),
                    Q(
                        "In conclusion should…",
                        ["start a brand-new topic", "sum up", "replace the introduction", "ask the examiner a question"],
                        1,
                        "Shrnutí.",
                    ),
                    Q(
                        "Which is the best topic sentence?",
                        [
                            "Gap year gap year gap year.",
                            "A gap year is worth it when it has a clear plan.",
                            "Hello my name is and I will write.",
                            "Etc. etc. etc.",
                        ],
                        1,
                        "Věta, která už nese názor.",
                    ),
                ],
            ),
            L(
                "Slovní zásoba okruhů",
                "Topic vocabulary",
                "Kolokace k maturitním tématům. Doplňování a kvíz.",
                "Collocations for the exam topics. Gaps and a quiz.",
                [
                    S(
                        "1 / 2   •   Okruhy",
                        "Ne izolovaná slovíčka",
                        "Tip: ústní zkouška sahá na osobu, rodinu, domov, školu, volný čas, kulturu, sport, cestování, zdraví, jídlo, služby, práci, společnost a přírodu.",
                        [
                            "Heavy rain, strong coffee, fast food, junk food. Ne strong rain.",
                            "A balanced diet, processed food, a food allergy.",
                            "Public transport, rush hour, a traffic jam, a return ticket.",
                            "Renewable energy, a carbon footprint, a heatwave, a drought.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Společnost a práce",
                        "Fráze, které unesou odstavec",
                        "Tip: apply for a job, hand in a notice, earn a living, from home.",
                        [
                            "A gap year, an apprenticeship, a degree, tuition fees.",
                            "Freedom of speech, a stereotype, volunteer work.",
                            "Break the law, pay a fine, a witness.",
                            "Mental health, a check-up, be in pain. Ne have pain in the leg jako jediná možnost: My leg hurts.",
                        ],
                    ),
                ],
                gaps=[
                    G("There was heavy ___ all night.", "rain", "Heavy rain, ne strong rain."),
                    G("I'd like a ___ ticket to Brno.", "return", "Return ticket = zpáteční."),
                    G("She applied ___ the post of guide.", "for", "Apply for."),
                    G("Solar power is ___ energy.", "renewable", "Renewable energy."),
                    G("He has a nut ___, so he can't eat the cake.", "allergy", "A food allergy."),
                ],
                quiz=[
                    Q("Which pair is right?", ["strong rain / heavy coffee", "heavy rain / strong coffee", "fast coffee / junk rain", "a traffic diet"], 1, "Heavy rain, strong coffee."),
                    Q("Rush hour is…", ["a quiet morning", "the busiest time to travel", "a kind of ticket", "a school subject"], 1, "Špička."),
                    Q("An apprenticeship is…", ["a fine", "training for a job", "a heatwave", "a stereotype"], 1, "Praktická příprava na řemeslo nebo práci."),
                    Q("My leg ___.", ["hurts", "is pain", "has hurted", "does hurtness"], 0, "My leg hurts."),
                ],
            ),
            L(
                "Gramatická ambulance",
                "Grammar clinic",
                "Smíšené jevy k didaktickému testu. Kvíz a doplňování.",
                "Mixed points for the language paper. A quiz and gaps.",
                [
                    S(
                        "1 / 2   •   Články a časy",
                        "To, co se v testu sype",
                        "Tip: the night train, a night train, night trains. Člen podle toho, jestli čtenář ví který.",
                        [
                            "I have lived here since May. I lived there in 2018.",
                            "If I were you, I would wait. If it rains, we will wait.",
                            "The book was written in 1962. People speak English here → English is spoken here.",
                            "Used to: I used to walk. Zvyk, který už neplatí. Didn't use to.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Věta",
                        "Slovosled a shoda",
                        "Tip: never dávejte před sloveso, ale za am/is/are: She is never late. She never arrives late.",
                        [
                            "A little time, a few minutes. Much v záporu a otázce, a lot of v kladné větě je bezpečné.",
                            "Suggest + -ing nebo suggest that. Suggest me to go je chyba.",
                            "Enjoy, avoid, mind, can't help + -ing.",
                            "Want, decide, promise, hope + to.",
                        ],
                    ),
                ],
                gaps=[
                    G("I ___ (live) in this flat since May.", "have lived|'ve lived", "Since → present perfect."),
                    G("She suggested ___ (take) the night train.", "taking", "Suggest + -ing."),
                    G("If I ___ you, I would book earlier.", "were|was", "If I were you."),
                    G("These letters must ___ (sign) today.", "be signed", "Must be + příčestí."),
                    G("I ___ to get up at five. I don't anymore.", "used", "Used to."),
                ],
                quiz=[
                    Q("She enjoys ___.", ["to swim", "swimming", "swim", "swam"], 1, "Enjoy + -ing."),
                    Q("I have ___ finished.", ["yet", "just", "since", "ago"], 1, "Just before the participle. Yet belongs at the end in negatives."),
                    Q("English ___ all over the building.", ["speaks", "is spoken", "is spoke", "speak"], 1, "Trpný rod."),
                    Q("There isn't ___ time.", ["many", "much", "a few", "several"], 1, "Time je nepočitatelné: much."),
                    Q("He asked me where ___.", ["did I live", "I lived", "do I lived", "lived I"], 1, "Bez inverze."),
                    Q("You ___ shout. The baby is asleep. It is forbidden.", ["don't have to", "mustn't", "might", "used to"], 1, "Zákaz: mustn't."),
                ],
            ),
            L(
                "Cvičná maturita",
                "A practice paper",
                "Čtení, poslech, jazyková kompetence a krátká písemná práce.",
                "Reading, listening, language in use and a short piece of writing.",
                [
                    S(
                        "1 / 2   •   Celá zkouška",
                        "Tři části, jeden mozek",
                        "Tip: didaktický test míchá poslech, čtení a jazykovou kompetenci. Písemná práce je zvlášť. Tady je zmenšený model.",
                        [
                            "Nejdřív přečtěte zadání psaní, ať víte, kolik času si necháte.",
                            "U čtení nehledejte stejná slova. Hledejte stejný význam.",
                            "U poslechu zápor bývá ta správná odpověď.",
                            "Když váháte mezi dvěma, vyhoďte tu, která přidává informaci v textu neslyšenou.",
                        ],
                    ),
                    S(
                        "2 / 2   •   Písemná práce",
                        "Účel, čtenář, rozsah",
                        "Tip: e-mail kamarádovi a stížnost úřadu se nepíšou stejným jazykem. Tady jde o kamaráda.",
                        [
                            "Řekněte proč píšete hned v první větě.",
                            "Dejte tam čas a místo, ať text není mlha.",
                            "Jedna věta s because a jedna s if textu pomůžou.",
                            "Na konci otázka, která zní jako opravdový zájem.",
                        ],
                    ),
                ],
                reading=(
                    "The town library now lends tools as well as books. Hammers, "
                    "sewing machines and a cake tin can be borrowed for a week. "
                    "The idea came from a librarian who was tired of people buying "
                    "things they would use once. She said the workshop had been "
                    "visited by more teenagers than the reading room. Critics argue "
                    "that a library should stay quiet. She replies that a library "
                    "should be useful. If the experiment works, other branches will "
                    "copy it. Users have to return each object clean, and they must "
                    "not lend it to anyone else."
                ),
                readq=[
                    Q(
                        "What can people borrow besides books?",
                        ["Cars", "Tools", "Flats", "Tickets to Berlin"],
                        1,
                        "Hammers, sewing machines, a cake tin.",
                    ),
                    Q(
                        "Why did the idea start?",
                        [
                            "People were buying things they would use once",
                            "The reading room was too popular",
                            "Teenagers hated books",
                            "The tools were illegal",
                        ],
                        0,
                        "buying things they would use once.",
                    ),
                    Q(
                        "Who has visited the workshop more, according to her?",
                        ["Critics", "Teenagers, more than the reading room", "Other branches", "Nobody"],
                        1,
                        "more teenagers than the reading room.",
                    ),
                    Q(
                        "What must users not do?",
                        ["Return the object", "Clean it", "Lend it to someone else", "Visit the library"],
                        2,
                        "must not lend it to anyone else.",
                    ),
                ],
                listening=(
                    "Welcome to the night-train podcast. The 22:40 to Berlin has been "
                    "delayed by forty minutes because a tree is on the line near Ústí. "
                    "Passengers who already have a seat can wait in carriage four, where "
                    "tea is being served. If you have not checked in, please do it on "
                    "your phone. We would be grateful if you kept the corridor clear. "
                    "The delay might be shorter. We will know more at ten."
                ),
                listenq=[
                    Q("How long is the delay?", ["14 minutes", "40 minutes", "4 hours", "22 minutes"], 1, "forty minutes."),
                    Q("Why?", ["Snow", "A tree on the line", "A missing driver", "A festival"], 1, "a tree is on the line."),
                    Q("Where can passengers with a seat wait?", ["Carriage four", "The airport", "Ústí station only", "The corridor"], 0, "carriage four."),
                    Q("What should people without a check-in do?", ["Buy tea", "Check in on their phone", "Leave the train", "Call the mayor"], 1, "do it on your phone."),
                ],
                quiz=[
                    Q("Tea ___ in carriage four.", ["serves", "is being served", "is serving", "has serve"], 1, "Průběhový trpný rod."),
                    Q("We would be grateful if you ___ the corridor clear.", ["keep", "kept", "keeping", "to keep"], 1, "Would be grateful if + minulý tvar."),
                    Q("She said the workshop ___ by more teenagers.", ["visited", "had been visited", "has visit", "is visiting"], 1, "Had been visited."),
                    Q("Users ___ return the tools clean.", ["have to", "must to", "don't must", "are have"], 0, "Have to."),
                ],
                writing=W(
                    "Write at least 45 words to a friend. Invite them to borrow something from a library of things. Use because and if.",
                    "Hi Adam, the library now lends tools, and I think you would like it because you always need a drill for one afternoon. If you come on Saturday, I will show you how it works. You have to bring your card and return everything clean. We could borrow a cake tin and bake something after. Let me know if you are free.",
                    "because|if",
                    45,
                ),
            ),
        ],
    }


YEARS.append(y3())
YEARS.append(y4())


def emit_questions(name, items, lines):
    lines.append(f"static const ChoiceQ {name}[] = {{")
    for prompt, options, correct, _hint in items:
        opts = ", ".join(c_str(o) for o in options)
        lines.append(f"    {{{c_str(prompt)}, {{{opts}}}, 4, {correct}}},")
    lines.append("};")
    lines.append(f"static const char *{name}_hints[] = {{")
    for _p, _o, _c, hint in items:
        lines.append(f"    {c_str(hint)},")
    lines.append("};")
    lines.append("")


def emit_lesson(year, index, lesson, lines, inits, prefix="en"):
    p = f"{prefix}_y{year}_l{index}"
    lines.append(f"static const NetSlide {p}_slides[] = {{")
    for kicker, title, tip, bullets in lesson["slides"]:
        lines.append("    {")
        lines.append(f"        {c_str(kicker)}, {c_str(title)}, {c_str(tip)},")
        lines.append("        {")
        for bullet in bullets:
            lines.append(f"            {c_str(bullet)},")
        lines.append("            NULL,")
        lines.append("        },")
        lines.append("    },")
    lines.append("};")
    lines.append("")

    init = [
        f"        .slides = {p}_slides,",
        f"        .n_slides = (int)(sizeof {p}_slides / sizeof {p}_slides[0]),",
    ]

    if lesson.get("reading"):
        lines.append(f"static const char *{p}_read = {c_str(lesson['reading'])};")
        lines.append("")
        init.append(f"        .reading = {c_str(lesson['reading'])},")
    if lesson.get("readq"):
        emit_questions(f"{p}_readq", lesson["readq"], lines)
        init.append(f"        .readq = {p}_readq,")
        init.append(f"        .read_hints = {p}_readq_hints,")
        init.append(f"        .n_read = (int)(sizeof {p}_readq / sizeof {p}_readq[0]),")
    if lesson.get("listening"):
        lines.append(f"static const char *{p}_listen = {c_str(lesson['listening'])};")
        lines.append("")
        init.append(f"        .listening = {c_str(lesson['listening'])},")
    if lesson.get("listenq"):
        emit_questions(f"{p}_listenq", lesson["listenq"], lines)
        init.append(f"        .listenq = {p}_listenq,")
        init.append(f"        .listen_hints = {p}_listenq_hints,")
        init.append(f"        .n_listen = (int)(sizeof {p}_listenq / sizeof {p}_listenq[0]),")
    if lesson.get("gaps"):
        lines.append(f"static const TypedQ {p}_gaps[] = {{")
        for prompt, answers, meaning in lesson["gaps"]:
            lines.append(f"    {{{c_str(prompt)}, {c_str(answers)}, {c_str(meaning)}}},")
        lines.append("};")
        lines.append("")
        init.append(f"        .gaps = {p}_gaps,")
        init.append(f"        .n_gaps = (int)(sizeof {p}_gaps / sizeof {p}_gaps[0]),")
    if lesson.get("writing"):
        prompt, model, keys, min_words = lesson["writing"]
        lines.append(f"static const char *{p}_write[] = {{")
        lines.append(f"    {c_str(prompt)},")
        lines.append(f"    {c_str(model)},")
        lines.append(f"    {c_str(keys)},")
        lines.append(f"    {c_str(str(min_words))},")
        lines.append("};")
        lines.append("")
        init.append(f"        .write_prompt = {c_str(prompt)},")
        init.append(f"        .write_model = {c_str(model)},")
        init.append(f"        .write_keys = {c_str(keys)},")
        init.append(f"        .write_min = {min_words},")
    if lesson.get("quiz"):
        emit_questions(f"{p}_quiz", lesson["quiz"], lines)
        init.append(f"        .quiz = {p}_quiz,")
        init.append(f"        .quiz_hints = {p}_quiz_hints,")
        init.append(f"        .n_quiz = (int)(sizeof {p}_quiz / sizeof {p}_quiz[0]),")

    kinds = [k for k in ("reading", "listening", "writing", "gaps", "quiz", "readq", "listenq") if lesson.get(k)]
    if len(kinds) < 2 and "reading" not in kinds and "listening" not in kinds:
        raise SystemExit(f"thin lesson y{year} l{index}: {kinds}")
    inits.append("    {\n" + "\n".join(init) + "\n    }")


def main() -> None:
    if len(YEARS) != 4 or any(len(y["lessons"]) != 8 for y in YEARS):
        raise SystemExit("expected 4 years × 8 lessons")

    body = [
        "/* Generated by scripts/gen-english.py. Do not edit by hand. */",
        "",
    ]
    table = ["static const EnLesson en_lessons[EN_YEARS][EN_N] = {"]
    i18n = [
        '    {"en_years_sub", "Čtyři ročníky podle osnov gymnázia, od A2 k maturitě.", "Four years of the gymnasium syllabus, from A2 to the exam."},',
        '    {"en_year1", "1. ročník", "Year 1"},',
        '    {"en_year2", "2. ročník", "Year 2"},',
        '    {"en_year3", "3. ročník", "Year 3"},',
        '    {"en_year4", "4. ročník", "Year 4"},',
        '    {"en_sec_read", "Čtení", "Reading"},',
        '    {"en_sec_listen", "Poslech", "Listening"},',
        '    {"en_sec_write", "Psaní", "Writing"},',
        '    {"en_sec_gap", "Doplňte", "Complete"},',
        '    {"en_sec_quiz", "Kvíz", "Quiz"},',
        '    {"en_sec_test", "Jazyková část", "Language"},',
        '    {"en_play", "Přehrát", "Play"},',
        '    {"en_pause", "Pauza", "Pause"},',
        '    {"en_resume", "Pokračovat", "Resume"},',
        '    {"en_speed", "Rychlost", "Speed"},',
        '    {"en_volume", "Hlasitost", "Volume"},',
        '    {"en_listen_note", "Nejdřív si přečtěte otázky. Nahrávku přehraje hlas zařízení. Bez něj se věty ukážou postupně.", "Read the questions first. The device voice plays the recording. Without one, the sentences appear one by one."},',
        '    {"en_listen_first", "Nejdřív poslech přehrajte.", "Play the recording first."},',
        '    {"en_listen_fallback", "Hlas není dostupný, věty se ukazují postupně.", "No voice is available, so the sentences appear one by one."},',
        '    {"en_write_short", "Text je krátký nebo v něm chybí požadovaná slova.", "The text is short or it misses the required words."},',
        '    {"en_model", "Vzor", "Model"},',
        '    {"en_write_ph", "Pište anglicky…", "Write in English…"},',
    ]
    for yi, year in enumerate(YEARS, start=1):
        i18n.append(
            f'    {{"en_y{yi}_sub", {c_str(year["sub_cs"])}, {c_str(year["sub_en"])}}},'
        )
        row = []
        for li, lesson in enumerate(year["lessons"], start=1):
            i18n.append(
                f'    {{"en_y{yi}_u{li}", {c_str(lesson["title_cs"])}, {c_str(lesson["title_en"])}}},'
            )
            i18n.append(
                f'    {{"en_y{yi}_u{li}_sub", {c_str(lesson["sub_cs"])}, {c_str(lesson["sub_en"])}}},'
            )
            inits = []
            emit_lesson(yi, li, lesson, body, inits)
            row.append(inits[0])
        table.append("    {\n" + ",\n".join(row) + "\n    },")
    table.append("};")
    table.append("")
    body.extend(table)

    (ROOT / "src" / "english_lessons.inc").write_text("\n".join(body) + "\n", encoding="utf-8")
    (ROOT / "src" / "english_i18n.inc").write_text("\n".join(i18n) + "\n", encoding="utf-8")
    skills = []
    for yi, year in enumerate(YEARS, start=1):
        bag = set()
        for lesson in year["lessons"]:
            bag.update(k for k in ("reading", "listening", "writing", "gaps", "quiz") if lesson.get(k))
        skills.append(f"year {yi}: {', '.join(sorted(bag))}")
    print("wrote", len(YEARS) * 8, "lessons")
    print("\n".join(skills))


if __name__ == "__main__":
    main()
