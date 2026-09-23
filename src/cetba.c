#include "maturita.h"

/* ------------------------------------------------------------------ */
/* Maturitní četba: George Orwell – 1984                              */
/* ------------------------------------------------------------------ */

typedef struct {
    const char *q;
    const char *opts[4];
    int correct;
    const char *expl;
} LitQ;

static const LitQ lit_qs[] = {
    {
        "Kdo je hlavní hrdina románu a co dělá?",
        {
            "Winston Smith – přepisuje historii na Ministerstvu pravdy",
            "O'Brien – řídí Ministerstvo lásky",
            "Emmanuel Goldstein – vede Bratrstvo",
            "Winston Smith – velí Policii myšlení",
        },
        0,
        "Winston Smith je vnější člen strany, který na Ministerstvu pravdy "
        "upravuje staré záznamy a novinové články.",
    },
    {
        "Ve kterém fiktivním státě se děj odehrává?",
        {"V Eurasii", "V Oceánii", "Ve Východasii", "V Angsocu"},
        1,
        "Příběh se odehrává v Oceánii, v provincii Letopočet 1 (bývalá "
        "Anglie), jejímž hlavním městem je Londýn.",
    },
    {
        "Co znamená slogan „VÁLKA JE MÍR, SVOBODA JE OTROCTVÍ, "
        "NEVĚDOMOST JE SÍLA“?",
        {
            "Jsou to tři zásady Bratrstva",
            "Jsou to názvy ministerstev",
            "Jde o hesla prolety",
            "Jsou to paradoxní hesla, kterými strana ospravedlňuje útlak",
        },
        3,
        "Strana používá dvojité myšlení: hesla znějí vznešeně, ale ve "
        "skutečnosti obhajují válku, otroctví a nevědomost.",
    },
    {
        "Kdo je Velký bratr a je reálnou osobou?",
        {
            "Skutečný vůdce Bratrstva",
            "Winstonův nadřízený na ministerstvu",
            "Symbol všemocné moci a kultu osobnosti; možná vůbec neexistuje",
            "O'Brienův pseudonym",
        },
        2,
        "Velký bratr je tvář režimu. Není jasné, zda jde o živou osobu, "
        "nebo jen o symbol, který drží strach a poslušnost.",
    },
    {
        "Co je Newspeak (novořeč) a proč ho strana zavádí?",
        {
            "Tajná řeč Bratrstva",
            "Zjednodušená angličtina omezující slovní zásobu, aby nešlo "
            "vyslovit zakázané myšlenky",
            "Úřední jazyk prolety",
            "Šifra Policie myšlení",
        },
        1,
        "Newspeak neustále ubírá slova. Když pro svobodu nebo odpor "
        "neexistuje slovo, nelze o nich ani myslet.",
    },
    {
        "Co je teleobrazovka (telescreen) a jak funguje?",
        {
            "Televize určená jen pro zábavu",
            "Zařízení, které vysílá i sleduje; nelze ji úplně vypnout",
            "Rozhlasová stanice",
            "Počítač Ministerstva pravdy",
        },
        1,
        "Telescreen funguje obousměrně – neustále sleduje občany. Vypnout "
        "lze jen zvuk, obraz běží dál.",
    },
    {
        "Jak se jmenují tři supervelmoci světa 1984?",
        {
            "Oceánie, Eurasie a Východasie",
            "Oceánie, Evropa a Asie",
            "Anglie, Eurasie a Ostasie",
            "Oceánie, Východasie a Afrika",
        },
        0,
        "Svět je rozdělen na tři státy ve věčném, ale nerozhodném "
        "konfliktu: Oceánii, Eurasii a Východasii.",
    },
    {
        "Čím se zabývá Ministerstvo pravdy?",
        {
            "Vede válku",
            "Dohlíží na morálku a sňatky",
            "Upravuje a přepisuje historii, noviny a archivy",
            "Rozděluje potraviny",
        },
        2,
        "Ministerstvo pravdy vyrábí „pravdu“ – Winston v něm přepisuje "
        "staré záznamy tak, aby vždy odpovídaly současné linii strany.",
    },
    {
        "K čemu slouží Ministerstvo lásky?",
        {
            "Zajišťuje sňatky a lásku",
            "Je to budova bez oken, kde se mučí a likvidují nepřátelé strany",
            "Vyrábí zbraně",
            "Školí děti",
        },
        1,
        "Ministerstvo lásky je opakem svého názvu – sídlí tam mučírny a "
        "nachází se v něm i obávaná místnost 101.",
    },
    {
        "V čem je paradox Ministerstva hojnosti?",
        {
            "Má na starosti hospodářství, ale ve skutečnosti drží lidi "
            "v nedostatku",
            "Rozdává jídlo a zboží zdarma",
            "Zajišťuje dostatek pro všechny",
            "Starají se o něj prolety",
        },
        0,
        "Ministerstvo hojnosti plánuje výrobu, ale ekonomika je záměrně "
        "nastavena tak, aby lidé žili v trvalém nedostatku.",
    },
    {
        "Co je Ministerstvo míru a čím se zabývá?",
        {
            "Uzavírá mírové smlouvy",
            "Dojednává příměří",
            "Ve skutečnosti vede válku",
            "Velí Policii myšlení",
        },
        2,
        "Název je přesně opačný: Ministerstvo míru řídí válku, protože "
        "trvalý válečný stav pomáhá udržet moc strany.",
    },
    {
        "Kdo je Julie a jaký vztah s ní Winston naváže?",
        {
            "Winstonova žena",
            "Mladá členka strany, s níž Winston prožije tajnou milostnou "
            "aféru",
            "O'Brienova asistentka",
            "Proletka a obchodnice",
        },
        1,
        "Julie pracuje v oddělení beletrie a naoko je vzornou členkou "
        "strany. S Winstonem vytvoří tajný milostný vztah.",
    },
    {
        "Proč je vztah Winstona a Julie zakázaný?",
        {
            "Strana zakazuje lásku a sexualitu jako zločin; vztah je navíc "
            "tajný",
            "Julie je vdaná za člena vnitřní strany",
            "Winston je jejím nadřízeným",
            "Julie patří k proletům",
        },
        0,
        "Strana potlačuje citové a sexuální vazby, protože oddanost má "
        "patřit jen straně. Tajný vztah je proto dvojnásob nebezpečný.",
    },
    {
        "Kdo je O'Brien a jakou roli hraje v příběhu?",
        {
            "Skutečný vůdce Bratrstva",
            "Vysoce postavený člen vnitřní strany, který Winstona nakonec "
            "mučí a „vyléčí“",
            "Winstonův přítel z dětství",
            "Proletářský básník",
        },
        1,
        "Winston věří, že O'Brien je odbojář. Ve skutečnosti je to věrný "
        "člen vnitřní strany, který ho dostane do místnosti 101.",
    },
    {
        "Co je Bratrstvo a existuje vůbec?",
        {
            "Skutečná armáda Oceánie",
            "Náboženská organizace",
            "Údajná tajná organizace odporu; není jisté, zda existuje",
            "Odbory prolety",
        },
        2,
        "Bratrstvo zmiňuje Goldstein i O'Brien, ale kniha nikdy nepotvrdí, "
        "že doopravdy existuje – může jít o past strany.",
    },
    {
        "Kdo je Emmanuel Goldstein?",
        {
            "Ministr pravdy",
            "Oficiální nepřítel státu a údajný vůdce Bratrstva",
            "Winstonův kolega",
            "Velký bratrův dvojník",
        },
        1,
        "Goldstein je terčem dvouminutovky nenávisti. Má ztělesňovat "
        "nepřítele, proti kterému strana sjednocuje občany.",
    },
    {
        "Co je dvojité myšlení (doublethink)?",
        {
            "Schopnost myslet ve dvou jazycích",
            "Zdvojení osobnosti",
            "Schopnost věřit dvěma protikladným tvrzením zároveň",
            "Tajné zapisování myšlenek",
        },
        2,
        "Dvojité myšlení je jádro stranické ideologie – člověk přijme i "
        "zjevné rozpory, jako „válka je mír“.",
    },
    {
        "Proč strana neustále přepisuje minulost?",
        {
            "Aby opravila chyby v archivech",
            "Aby měla vždy pravdu a nepřítel jí nemohl dokázat opak",
            "Aby ušetřila papír",
            "Aby potěšila prolety",
        },
        1,
        "Kdo ovládá minulost, ovládá i budoucnost. Přepisováním historie "
        "strana udržuje absolutní kontrolu nad pravdou.",
    },
    {
        "Co je zločin myšlení (thoughtcrime)?",
        {
            "Pouhá myšlenka nebo názor, který se nelíbí straně",
            "Krádež státního majetku",
            "Nelegální obchod",
            "Opomenutí dvouminutovky",
        },
        0,
        "Strana trestá už samotné myšlenky. Winston se proviní tím, že si "
        "do deníku napíše „Dolů s Velkým bratrem“.",
    },
    {
        "Kdo je pan Charrington a jaké je jeho skutečné postavení?",
        {
            "Winstonův přítel a majitel hospody",
            "Proletářský básník",
            "Zdánlivě starý majitel obchodu, ve skutečnosti agent Policie "
            "myšlení",
            "Člen Bratrstva",
        },
        2,
        "Charrington Winstonovi pronajme pokoj bez teleobrazovky, ale "
        "právě on nakonec oba milence udá.",
    },
    {
        "Co je místnost 101?",
        {
            "Winstonova cela na policii",
            "Skladiště knih",
            "Místnost, ve které každého čeká jeho největší strach",
            "Zasedací místnost vnitřní strany",
        },
        2,
        "Místnost 101 je vrcholný mučicí nástroj. Winstonovi v ní hrozí "
        "krysy – jeho největší fobie.",
    },
    {
        "Jak skončí vztah Winstona a Julie?",
        {
            "Utečou spolu do Eurasie",
            "Vezmou se a žijí svobodně",
            "Winston se stane členem Bratrstva",
            "Zradí se a nakonec se přestanou milovat",
        },
        3,
        "Oba jsou po mučení zlomení a vzájemně se zradí. Na konci už "
        "k sobě nic necítí.",
    },
    {
        "Co znamená závěrečná věta „Miloval Velkého bratra“?",
        {
            "Winstonův vzdor se změnil v pokornou lásku k režimu",
            "Winston se stal novým vůdcem",
            "Byl to jen Winstonův sen",
            "Julie mu to řekla",
        },
        0,
        "Winston byl vnitřně zlomen. Přestal být sám sebou a nakonec "
        "režim opravdu přijal za svůj.",
    },
    {
        "Které tvrzení NENÍ jedním z hlavních témat románu?",
        {
            "Moc a absolutní kontrola",
            "Pravda a přepisování historie",
            "Svoboda a identita",
            "Idylický venkovský život",
        },
        3,
        "1984 je temná antiutopie. Hlavními tématy jsou moc, kontrola, "
        "manipulace s pravdou a ztráta svobody, ne venkovská idyla.",
    },
    {
        "V jakém roce byl román 1984 poprvé vydán?",
        {"1936", "1949", "1954", "1984"},
        1,
        "Román vyšel v roce 1949; letopočet 1984 označuje blízkou "
        "budoucnost, do níž Orwell příběh zasadil.",
    },
    {
        "Které další Orwellovo dílo se dá s 1984 srovnat?",
        {"Farma zvířat", "Velký Gatsby", "Zločin a trest", "Babička"},
        0,
        "Obě Orwellova díla varují před totalitou. Farma zvířat ji líčí "
        "alegoricky pomocí zvířat, 1984 očima jedince.",
    },
    {
        "Co je „dvouminutovka nenávisti“?",
        {
            "Krátká každodenní akce, při níž občané společně nenávidí "
            "Goldsteina a nepřítele",
            "Pravidelná modlitba ke Velkému bratrovi",
            "Sportovní přestávka ve škole",
            "Porada vedení ministerstva",
        },
        0,
        "Slouží k uvolnění a přesměrování agrese občanů na nepřítele "
        "státu, čímž strana posiluje svou moc.",
    },
    {
        "Proč je podle knihy válka pro systém nezbytná?",
        {
            "Přináší mír a blahobyt",
            "Udržuje strach, spotřebovává výrobu a zachovává chudobu "
            "i hierarchii",
            "Rozvíjí vědu a techniku",
            "Sjednocuje vládu s prolety",
        },
        1,
        "Trvalá válka pohlcuje nadbytečnou produkci a udržuje lidi ve "
        "strachu i chudobě, takže se nemohou vzbouřit.",
    },
    {
        "Jaký je rozdíl mezi vnějšími členy strany a prolety?",
        {
            "Proleti mají více práv než členové strany",
            "Vnější členové jsou dozíráni, kdežto proleti žijí v chudobě "
            "a strana o ně nestojí",
            "Není mezi nimi žádný rozdíl",
            "Proleti tvoří vedení strany",
        },
        1,
        "Proleti tvoří asi 85 % obyvatel. Žijí v bídě a jsou považováni "
        "za neškodné, dokud si neuvědomí svou sílu.",
    },
    {
        "Jak strana kontroluje sexualitu a osobní vztahy?",
        {
            "Podporuje volnou lásku a nezávislost",
            "Potlačuje lásku a sexualitu, děti vede k oddanosti straně",
            "Zakazuje veškerá manželství",
            "Povoluje vztahy jen s prolety",
        },
        1,
        "Strana chce, aby veškerá oddanost a energie patřily jí. Proto "
        "podporuje zdrženlivost a vede děti v Antisexuální lize mládeže.",
    },
    {
        "Co symbolizuje papírové těžítko, které si Winston koupí?",
        {
            "Jeho nenávist ke Goldsteinovi",
            "Kousek krásy a minulosti, kterou už nelze vrátit",
            "Moc Velkého bratra",
            "Lásku k Julii",
        },
        1,
        "Těžítko je drobný, krásný předmět z minulosti. Pro Winstona "
        "ztělesňuje svět, který strana zničila.",
    },
    {
        "Jaké poselství chtěl Orwell románem nejspíš předat?",
        {
            "Varování před totalitou a ztrátou svobody i pravdy",
            "Oslavu technického pokroku",
            "Návod, jak založit stát",
            "Kritiku sportu a zábavy",
        },
        0,
        "1984 je varování: když lidé přestanou bránit pravdu a svobodu, "
        "mohou o ně nenávratně přijít.",
    },
};

#define LIT_NQ ((int)G_N_ELEMENTS(lit_qs))

static const LitQ fuks_qs[] = {
    {
        "Kdo je hlavní postava románu a jaké je jeho povolání?",
        {
            "Pan Kopfrkingl – zaměstnanec krematoria",
            "Willi Reinke – nacistický důstojník",
            "Dr. Bettelheim – psychiatr",
            "Pan Kopfrkingl – ředitel školy",
        },
        0,
        "Hlavní postavou je pan Kopfrkingl, spořádaný zaměstnanec krematoria, "
        "který svou práci vnímá jako poslání.",
    },
    {
        "V jakém prostředí pan Kopfrkingl pracuje?",
        {"V nemocnici", "V krematoriu", "V divadle", "Ve škole"},
        1,
        "Kopfrkingl pracuje v krematoriu, kde má na starosti zpopelňování "
        "zemřelých.",
    },
    {
        "V jaké historické době se děj románu odehrává?",
        {
            "Za první republiky",
            "Za protektorátu Čechy a Morava (před a během 2. světové války)",
            "Po roce 1989",
            "V 19. století",
        },
        1,
        "Román se odehrává v době nastupujícího nacismu a protektorátu, "
        "který umožňuje Kopfrkinglovu proměnu.",
    },
    {
        "Jak se proměňuje hlavní hrdina v průběhu příběhu?",
        {
            "Z laskavého spořádaného muže v chladnokrevného vraha a fanatika",
            "Z vraha v kajícníka",
            "Z chudáka v boháče",
            "Vůbec se nemění",
        },
        0,
        "Kopfrkingl se postupně mění v člověka, který vraždí své blízké a "
        "ospravedlňuje to vyšším dobrem.",
    },
    {
        "Jaký je vztah pana Kopfrkingla k jeho rodině na začátku knihy?",
        {
            "Nenávidí je",
            "Je starostlivý, zdvořilý a především spořádaný",
            "S rodinou se nezná",
            "Týrá je",
        },
        1,
        "Na začátku působí jako vzorný manžel a otec, posedlý řádem a "
        "zdvořilostí.",
    },
    {
        "Jak se jmenuje manželka pana Kopfrkingla a jaký je jejich vztah?",
        {
            "Marie – milují se",
            "Lakmé – formálně zdvořilý, chladný vztah",
            "Zina – nenávidí se",
            "Julie – jsou rozvedení",
        },
        1,
        "Manželka se jmenuje Lakmé. Jejich vztah je spíš zdvořilá forma než "
        "opravdová láska.",
    },
    {
        "Jaký je vztah pana Kopfrkingla k dětem, Zině a Mílovi?",
        {
            "Miluje je víc než sebe",
            "Chová se k nim odtažitě, jako k součásti řádu",
            "Nevidí je",
            "Jsou adoptované",
        },
        1,
        "K dětem je odměřený; vnímá je spíš jako součást svého spořádaného "
        "světa než jako milované bytosti.",
    },
    {
        "Kdo je Willi Reinke a jakou roli hraje v proměně hlavního hrdiny?",
        {
            "Soused z krematoria",
            "Německý úředník a bývalý voják, který Kopfrkingla ovlivní",
            "Kopfrkinglův bratr",
            "Lékař v nemocnici",
        },
        1,
        "Reinke je Němec, který v Kopfrkinglovi probudí touhu po árijském "
        "původu a kariéře.",
    },
    {
        "Jak Willi Reinke ovlivňuje Kopfrkinglovo smýšlení?",
        {
            "Přesvědčí ho, aby se přidal k nacistům a byl „lepší“",
            "Nabádá ho k odchodu do zahraničí",
            "Učí ho tibetštinu",
            "Nijak ho neovlivní",
        },
        0,
        "Reinke v něm posiluje antisemitismus a touhu po moci a uznání "
        "v novém režimu.",
    },
    {
        "Jaký je Kopfrkinglův vztah ke krematoriu a smrti obecně?",
        {
            "Bojí se jich",
            "Má je rád, kremaci vnímá jako krásný obřad osvobození",
            "Nechce tam pracovat",
            "Je mu to lhostejné",
        },
        1,
        "Smrt a kremace jsou pro něj estetickým a filozofickým rituálem, ne "
        "něčím hrůzným.",
    },
    {
        "Jak pan Kopfrkingl vnímá a popisuje proces kremace?",
        {
            "Jako nutné zlo",
            "Jako vznešený, očistný obřad, o kterém rád mluví",
            "Jako hroznou povinnost",
            "Nikdy o něm nemluví",
        },
        1,
        "Kopfrkingl o kremaci hovoří básnivě a s nadšením, což ostře "
        "kontrastuje s hrůzou jeho činů.",
    },
    {
        "Jaká je symbolika krematoria v celém románu?",
        {
            "Symbol naděje",
            "Smrt, odlidštění a zvrácená ideologie, která z lidí dělá jen těla",
            "Symbol lásky",
            "Symbol mládí",
        },
        1,
        "Krematorium je ústředním symbolem – odhaluje chladnou mašinerii "
        "smrti, do níž se ideologie snaží proměnit člověka.",
    },
    {
        "Co symbolizuje Kopfrkinglova záliba v tibetské Knize mrtvých?",
        {
            "Jeho touhu po vědění a zvrácenou víru ve vlastní výjimečnost",
            "Historický zájem o Asii",
            "Strach ze smrti",
            "Lásku k manželce",
        },
        0,
        "Kopfrkingl si z ní vybírá jen to, co se hodí k ospravedlnění jeho "
        "činů a jeho pocitu vyvolenosti.",
    },
    {
        "Jak se v knize projevuje motiv buddhismu a reinkarnace?",
        {
            "Kopfrkingl jimi ospravedlňuje smrt jako „osvobození duše“",
            "Postavy se modlí k Bohu",
            "Nikde se neobjevuje",
            "Je to hlavní téma manželky",
        },
        0,
        "Reinkarnace a „osvobození od utrpení“ se v jeho mysli zvrhávají "
        "v právo zabíjet.",
    },
    {
        "Jak Kopfrkingl zneužívá filozofické a náboženské myšlenky?",
        {
            "Ospravedlňuje jimi vlastní vraždy jako službu vyššímu dobru",
            "Pomáhá jimi druhým",
            "Odmítá je",
            "Používá je jen v práci",
        },
        0,
        "Z vznešených idejí si staví masku, za kterou skrývá krutost a touhu "
        "po moci.",
    },
    {
        "Jak se postupně mění Kopfrkinglova mluva a způsob vyjadřování?",
        {
            "Začíná mluvit vulgárně",
            "Mění se v patetickou, ráznou řeč plnou frází o řádu a osvobození",
            "Přestane mluvit",
            "Od začátku mluví jen německy",
        },
        1,
        "Jeho zdvořilá mluvnost se postupně mění v ideologické fráze a "
        "bezcitný patos.",
    },
    {
        "Jaké opakující se fráze nebo návyky pana Kopfrkingla si všímáš?",
        {
            "Neustále se ptá na počasí",
            "Opakuje „to je takové“, „vidíte“ a lpí na zdvořilosti",
            "Zpívá si",
            "Mluví jen o jídle",
        },
        1,
        "Opakující se fráze a zdvořilostní obraty vytvářejí rytmus, který je "
        "čím dál tím děsivější.",
    },
    {
        "Jaký je vypravěčský styl románu (ich-forma, er-forma)?",
        {
            "Er-forma bez vnitřního světa",
            "Ich-forma – vypráví sám Kopfrkingl",
            "Deníková forma",
            "Forma dopisů",
        },
        1,
        "Ich-forma je klíčová: čtenář sleduje svět zkreslený Kopfrkinglovou "
        "myslí a sám postupně propadá jeho logice.",
    },
    {
        "Jak se v knize projevuje černý humor a groteska?",
        {
            "Vážné scény jsou líčeny s odstupem a absurditou",
            "Kniha je čistě vážná",
            "Humor je jediný žánr",
            "V knize se neprojevuje",
        },
        0,
        "Groteskní kontrast mezi slavnostním vyprávěním a hrůzou vytváří "
        "mrazivý černý humor.",
    },
    {
        "Jak kontrastuje banalita každodenního života s hrůzností činů?",
        {
            "Vůbec nesouvisí",
            "Všední detaily a zdvořilost činí hrůzu ještě děsivější",
            "Hrůza je potlačena",
            "Den je popsán jen jednou",
        },
        1,
        "Fuks zesiluje otřes tím, že vraždění je popisováno stejně klidně "
        "jako ranní káva nebo kremace.",
    },
    {
        "Jak se v knize projevuje antisemitismus?",
        {
            "Od začátku otevřeně",
            "Postupně, nejdřív v náznacích a vlivem Reinkeho",
            "Nikdy",
            "Jen mimochodem v závěru",
        },
        1,
        "Kopfrkinglův antisemitismus se rodí pozvolna a je živen dobovou "
        "atmosférou i Reinkem.",
    },
    {
        "Jak nacistická ideologie postupně prostupuje Kopfrkinglovo myšlení?",
        {
            "Odmítá ji",
            "Přijímá ji jako vyšší řád a ospravedlnění vlastní výjimečnosti",
            "Nerozumí jí",
            "Zajímá ho jen hudba",
        },
        1,
        "Ideologie se mu stává nástrojem kariéry i vnitřním ospravedlněním "
        "vražd.",
    },
    {
        "Jak se mění Kopfrkinglův vztah k manželce kvůli jejímu původu?",
        {
            "Začne ji milovat víc",
            "Odcizí se jí, protože je židovského původu",
            "Nijak se nemění",
            "Odstěhují se od sebe",
        },
        1,
        "Kvůli jejímu původu ji přestane vnímat jako rovnocennou a nakonec "
        "ji zabije.",
    },
    {
        "Co se stane s Kopfrkinglovou rodinou ve druhé polovině románu?",
        {
            "Odstěhují se",
            "Kopfrkingl svou ženu i děti zabije",
            "Všichni přežijí",
            "Zachrání je Reinke",
        },
        1,
        "Pod vlivem ideologie Kopfrkingl vyvraždí vlastní rodinu a považuje "
        "to za „osvobození“.",
    },
    {
        "Jak Kopfrkingl zdůvodňuje vraždu vlastní ženy?",
        {
            "Zlobila se na něj",
            "Že je židovského původu a překáží jeho kariéře",
            "Že ho podváděla",
            "Nijak ji nezdůvodňuje",
        },
        1,
        "Svou ženu zabije mimo jiné proto, že je židovského původu, tedy "
        "„méněcenná“ pro nový režim.",
    },
    {
        "Jakým způsobem Kopfrkingl vraždí své blízké?",
        {
            "Ve zlosti a afektu",
            "Chladně a s pocitem, že je „osvobozuje od utrpení“",
            "Omylem",
            "Na příkaz úřadů",
        },
        1,
        "Vraždí klidně, jako by vykonával bohulibý úkon, a svou krutost si "
        "vykládá jako soucit.",
    },
    {
        "Jak souvisí přesvědčení o „osvobození“ s jeho vražednými činy?",
        {
            "Dává mu zdání vyššího smyslu, kterým si vraždy ospravedlňuje",
            "Nemá s nimi nic společného",
            "Brání mu v zabíjení",
            "Je to jen náhodná fráze",
        },
        0,
        "Zvrácená víra v osvobození od utrpení mu slouží jako mravní alibi "
        "pro vraždy.",
    },
    {
        "Jaký je vztah pana Kopfrkingla k postavě Dr. Bettelheima?",
        {
            "Obdivuje ho a rád s ním mluví",
            "Nenávidí ho",
            "Nikdy se nepotkají",
            "Je to jeho nadřízený",
        },
        0,
        "S doktorem Bettelheimem vede Kopfrkingl své „filozofické“ rozhovory "
        "o smrti a lidskosti.",
    },
    {
        "Jak se mění společenské postavení pana Kopfrkingla v době okupace?",
        {
            "Upadá",
            "Stoupá – díky režimu a touze po árijském původu",
            "Nemění se",
            "Odchází do penze",
        },
        1,
        "Spolu s nacistickou mocí roste i Kopfrkinglova kariéra a "
        "sebevědomí.",
    },
    {
        "Jaký je symbolický význam jména Kopfrkingl?",
        {
            "Zdánlivě úřednické, ale zní hrozivě – naznačuje zvrácenost",
            "Je to běžné české jméno",
            "Znamená „laskavý“",
            "Je to pseudonym",
        },
        0,
        "Zvukomalebné jméno působí úředně a zároveň hrozivě, což podtrhuje "
        "dvojí tvář postavy.",
    },
    {
        "Jak funguje motiv masek, divadla a přetvářky?",
        {
            "Postavy nosí masky dobroty, pod nimiž se skrývá krutost",
            "V knize jsou jen skutečné divadelní masky",
            "Motiv se neobjevuje",
            "Masky symbolizují lásku",
        },
        0,
        "Přetvářka a předstíraná spořádanost jsou ústřední – zlo se tváří "
        "jako slušnost.",
    },
    {
        "Jaký je vztah mezi soukromým a kolektivním šílenstvím doby?",
        {
            "Kopfrkinglovo osobní šílenství roste spolu s ideologií doby",
            "Nijak nesouvisí",
            "Doba je klidná a šťastná",
            "Lidé režim jednomyslně odmítají",
        },
        0,
        "Fuks ukazuje, jak se individuální porucha napojí na kolektivní "
        "ideologii a čerpá z ní sílu.",
    },
    {
        "Jak končí román a co se stane s panem Kopfrkinglem na konci?",
        {
            "Uteče do zahraničí",
            "Je zatčen a zničen",
            "Stane se hrdinou",
            "Věří, že je vtělením Dalajlámy, a zůstává zaslepený",
        },
        3,
        "Kopfrkinglova zaslepenost vrcholí přesvědčením o vlastní božskosti; "
        "jeho zvrácená víra ho neopouští.",
    },
    {
        "Jak je Spalovač mrtvol adaptován filmově?",
        {
            "Film Juraje Herze je vizuálně expresivní a patří k vrcholům "
            "české kinematografie",
            "Byl zfilmován jen v zahraničí",
            "Film neexistuje",
            "Vznikl jako animovaný seriál",
        },
        0,
        "Známá filmová verze je od režiséra Juraje Herze z roku 1968.",
    },
    {
        "Které další dílo Ladislava Fukse se dá srovnat s touto knihou?",
        {
            "Pan Theodor Mundstock nebo Myši Natálie Mooshabrové",
            "Farma zvířat",
            "R.U.R.",
            "Babička",
        },
        0,
        "Fuks se opakovaně zabýval osudem židovských postav a atmosférou "
        "strachu; srovnat lze např. Pana Theodora Mundstocka.",
    },
    {
        "Jaká jsou hlavní témata románu?",
        {
            "Moc, manipulace, ideologie, násilí a přetvářka",
            "Láska a dobrodružství",
            "Sport a přátelství",
            "Příroda a vesmír",
        },
        0,
        "Kniha zkoumá, jak se obyčejný člověk přizpůsobí zlu a stane se jeho "
        "nástrojem.",
    },
    {
        "Jaké poselství podle tebe chtěl Ladislav Fuks knihou předat?",
        {
            "Varování před manipulací a tím, jak se zlo rodí z banality a "
            "lhostejnosti",
            "Oslavu války",
            "Návod k podnikání",
            "Kritiku školství",
        },
        0,
        "Fuks ukazuje, že hrůza může vzniknout z poslušnosti a touhy být "
        "„lepší“ než ostatní.",
    },
};

#define FUKS_NQ ((int)G_N_ELEMENTS(fuks_qs))

typedef struct {
    const LitQ *qs;
    int n;
    int idx;
    int score;
    int answered;
    int finished;
    GtkWidget *progress;
    GtkWidget *score_lbl;
    GtkWidget *prompt;
    GtkWidget *grid;
    GtkWidget *opts[4];
    GtkWidget *opt_lbl[4];
    GtkWidget *expl;
    GtkWidget *next;
} LitQuiz;

static void lit_quiz_show(LitQuiz *z) {
    const LitQ *q = &z->qs[z->idx];
    char buf[128];

    g_snprintf(buf, sizeof(buf), tr("lit_question_fmt"), z->idx + 1, z->n);
    gtk_label_set_text(GTK_LABEL(z->progress), buf);
    g_snprintf(buf, sizeof(buf), tr("lit_score_fmt"), z->score, z->n);
    gtk_label_set_text(GTK_LABEL(z->score_lbl), buf);

    gtk_label_set_text(GTK_LABEL(z->prompt), q->q);
    for (int i = 0; i < 4; i++) {
        gtk_label_set_text(GTK_LABEL(z->opt_lbl[i]), q->opts[i]);
        gtk_widget_remove_css_class(z->opts[i], "ok");
        gtk_widget_remove_css_class(z->opts[i], "wrong");
        gtk_widget_set_sensitive(z->opts[i], TRUE);
    }
    gtk_widget_set_visible(z->grid, TRUE);
    gtk_widget_set_visible(z->expl, FALSE);
    gtk_widget_set_visible(z->next, FALSE);
    gtk_button_set_label(GTK_BUTTON(z->next), tr("lit_next"));
    z->answered = 0;
    z->finished = 0;
}

static void lit_quiz_finish(LitQuiz *z) {
    char buf[128];

    z->finished = 1;
    gtk_label_set_text(GTK_LABEL(z->progress), tr("lit_finished"));
    gtk_label_set_text(GTK_LABEL(z->prompt), tr("lit_finished_text"));
    g_snprintf(buf, sizeof(buf), tr("lit_score_fmt"), z->score, z->n);
    gtk_label_set_text(GTK_LABEL(z->score_lbl), buf);
    gtk_widget_set_visible(z->grid, FALSE);
    gtk_widget_set_visible(z->expl, FALSE);
    gtk_widget_set_visible(z->next, TRUE);
    gtk_button_set_label(GTK_BUTTON(z->next), tr("lit_again"));
}

static void lit_opt_clicked(GtkButton *button, gpointer data) {
    LitQuiz *z = data;
    int chosen = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "idx"));
    const LitQ *q;
    char buf[128];

    if (z->answered)
        return;
    z->answered = 1;
    q = &z->qs[z->idx];

    for (int i = 0; i < 4; i++)
        gtk_widget_set_sensitive(z->opts[i], FALSE);

    gtk_widget_add_css_class(z->opts[q->correct], "ok");
    if (chosen == q->correct)
        z->score++;
    else
        gtk_widget_add_css_class(z->opts[chosen], "wrong");

    g_snprintf(buf, sizeof(buf), tr("lit_score_fmt"), z->score, z->n);
    gtk_label_set_text(GTK_LABEL(z->score_lbl), buf);

    gtk_label_set_text(GTK_LABEL(z->expl), q->expl);
    gtk_widget_set_visible(z->expl, TRUE);

    gtk_button_set_label(GTK_BUTTON(z->next),
                         z->idx + 1 >= z->n ? tr("lit_show_result")
                                            : tr("lit_next"));
    gtk_widget_set_visible(z->next, TRUE);
}

static void lit_next_clicked(GtkButton *button, gpointer data) {
    LitQuiz *z = data;

    (void)button;

    if (z->finished) {
        z->finished = 0;
        z->idx = 0;
        z->score = 0;
        lit_quiz_show(z);
        return;
    }
    if (z->idx + 1 >= z->n) {
        lit_quiz_finish(z);
        return;
    }
    z->idx++;
    lit_quiz_show(z);
}

static GtkWidget *lit_quiz_page(const LitQ *qs, int n, const char *back,
                                const char *title, const char *sub) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *box;
    GtkWidget *head;
    LitQuiz *z = g_new0(LitQuiz, 1);

    z->qs = qs;
    z->n = n;

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page), top_bar(back, title, sub));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_widget_set_margin_start(box, 6);
    gtk_widget_set_margin_end(box, 6);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), box);

    head = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append(GTK_BOX(box), head);

    z->progress = gtk_label_new("");
    gtk_widget_set_halign(z->progress, GTK_ALIGN_START);
    gtk_widget_set_hexpand(z->progress, TRUE);
    gtk_label_set_xalign(GTK_LABEL(z->progress), 0.0);
    gtk_widget_add_css_class(z->progress, "lit-progress");
    gtk_box_append(GTK_BOX(head), z->progress);

    z->score_lbl = gtk_label_new("");
    gtk_widget_set_halign(z->score_lbl, GTK_ALIGN_END);
    gtk_widget_add_css_class(z->score_lbl, "lit-score");
    gtk_box_append(GTK_BOX(head), z->score_lbl);

    z->prompt = gtk_label_new("");
    gtk_widget_set_halign(z->prompt, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(z->prompt), TRUE);
    gtk_widget_set_margin_top(z->prompt, 4);
    gtk_widget_add_css_class(z->prompt, "lit-prompt");
    gtk_box_append(GTK_BOX(box), z->prompt);

    z->grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(z->grid), 12);
    gtk_grid_set_column_spacing(GTK_GRID(z->grid), 12);
    gtk_grid_set_row_homogeneous(GTK_GRID(z->grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(z->grid), TRUE);
    gtk_widget_set_vexpand(z->grid, TRUE);
    gtk_box_append(GTK_BOX(box), z->grid);

    for (int i = 0; i < 4; i++) {
        GtkWidget *btn = gtk_button_new();
        GtkWidget *lbl;
        char cls[16];

        lbl = gtk_label_new("");
        gtk_label_set_wrap(GTK_LABEL(lbl), TRUE);
        gtk_label_set_justify(GTK_LABEL(lbl), GTK_JUSTIFY_CENTER);
        gtk_label_set_max_width_chars(GTK_LABEL(lbl), 26);

        g_snprintf(cls, sizeof(cls), "kahoot-%d", i);
        gtk_widget_add_css_class(btn, "kahoot-opt");
        gtk_widget_add_css_class(btn, cls);
        gtk_widget_set_hexpand(btn, TRUE);
        gtk_widget_set_vexpand(btn, TRUE);
        gtk_button_set_child(GTK_BUTTON(btn), lbl);
        g_object_set_data(G_OBJECT(btn), "idx", GINT_TO_POINTER(i));
        g_signal_connect(btn, "clicked", G_CALLBACK(lit_opt_clicked), z);

        z->opts[i] = btn;
        z->opt_lbl[i] = lbl;
        gtk_grid_attach(GTK_GRID(z->grid), btn, i % 2, i / 2, 1, 1);
    }

    z->expl = gtk_label_new("");
    gtk_widget_set_halign(z->expl, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(z->expl), TRUE);
    gtk_label_set_xalign(GTK_LABEL(z->expl), 0.0);
    gtk_widget_add_css_class(z->expl, "lit-expl");
    gtk_widget_set_visible(z->expl, FALSE);
    gtk_box_append(GTK_BOX(box), z->expl);

    z->next = gtk_button_new();
    i18n_bind(z->next, "lit_next", 1);
    gtk_widget_add_css_class(z->next, "btn-primary");
    gtk_widget_set_halign(z->next, GTK_ALIGN_END);
    gtk_widget_set_margin_top(z->next, 6);
    gtk_widget_set_visible(z->next, FALSE);
    gtk_box_append(GTK_BOX(box), z->next);
    g_signal_connect(z->next, "clicked", G_CALLBACK(lit_next_clicked), z);

    lit_quiz_show(z);

    g_object_set_data(G_OBJECT(page), "lit-quiz", z);
    return page;
}

GtkWidget *build_cetba1984_quiz_page(void) {
    return lit_quiz_page(lit_qs, LIT_NQ, "cetba1984",
                         "lit_quiz_title", "lit_quiz_sub");
}

GtkWidget *build_cetba_fuks_quiz_page(void) {
    return lit_quiz_page(fuks_qs, FUKS_NQ, "cetbaFuks",
                         "lit_fuks_quiz_title", "lit_fuks_quiz_sub");
}

/* ------------------------------------------------------------------ */
/* Sestavte děj (drag & drop)                                         */
/* ------------------------------------------------------------------ */

static const AssemblyItem lit_plot_items[] = {
    {
        "Přetáhněte části příběhu do správného pořadí:",
        {
            "Winston upravuje historii na Ministerstvu pravdy",
            "Seznámí se s Julií a začnou se tajně scházet",
            "Pronajme si pokoj nad obchodem pana Charringtona",
            "Přečte Goldsteinovu knihu u O'Briena",
            "Jsou zatčeni Policií myšlení",
            "Winston se zlomí v místnosti 101",
        },
        6,
    },
};

static const char *lit_plot_meaning[] = {
    "Správné pořadí: přepisování historie → seznámení s Julií → "
    "pronájem pokoje → četba Goldsteinovy knihy → zatčení → místnost 101.",
};

static UnitCtx lit_plot_unit;

GtkWidget *build_cetba1984_plot_page(void) {
    lit_plot_unit.page = "cetba1984";
    lit_plot_unit.progress_file = app_progress_cetba;
    return build_assembly(&lit_plot_unit, tr("lit_plot_title"),
                          "lit_plot_sub", 1, lit_plot_items,
                          lit_plot_meaning, 1);
}

static const AssemblyItem fuks_plot_items[] = {
    {
        "Přetáhněte části příběhu do správného pořadí:",
        {
            "Kopfrkingl je spořádaným zaměstnancem krematoria",
            "Seznámí se s Willim Reinkem",
            "Začne věřit v árijskou nadřazenost",
            "Postupně se odcizí své rodině",
            "Zabije ženu a děti jako „osvobození“",
            "Na konci se považuje za vtělení Dalajlámy",
        },
        6,
    },
};

static const char *fuks_plot_meaning[] = {
    "Správné pořadí: spořádaný úředník → setkání s Reinkem → árijská "
    "ideologie → odcizení rodině → vraždy → přesvědčení o vlastní "
    "božskosti.",
};

GtkWidget *build_cetba_fuks_plot_page(void) {
    lit_plot_unit.page = "cetbaFuks";
    lit_plot_unit.progress_file = app_progress_cetba;
    return build_assembly(&lit_plot_unit, tr("lit_fuks_plot_title"),
                          "lit_fuks_plot_sub", 1, fuks_plot_items,
                          fuks_plot_meaning, 1);
}

/* ------------------------------------------------------------------ */
/* Reading list and 1984 overview                                     */
/* ------------------------------------------------------------------ */

static GtkWidget *lit_link_card_ex(const char *target, const char *title,
                                   const char *sub,
                                   GtkDrawingAreaDrawFunc icon,
                                   GCallback clicked) {
    GtkWidget *btn = gtk_button_new();
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 14);
    GtkWidget *ic;
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    GtkWidget *t = gtk_label_new(NULL);
    GtkWidget *s = gtk_label_new(NULL);

    gtk_widget_add_css_class(btn, "lit-link");
    gtk_widget_set_hexpand(btn, TRUE);

    ic = icon_area_new(icon, 0, 0, 0, 34);
    gtk_widget_set_valign(ic, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(row), ic);

    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_halign(t, GTK_ALIGN_START);
    gtk_label_set_xalign(GTK_LABEL(t), 0.0);
    gtk_label_set_wrap(GTK_LABEL(t), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(t), 40);
    gtk_widget_add_css_class(t, "lit-link-title");
    i18n_bind(t, title, 0);
    gtk_box_append(GTK_BOX(box), t);

    gtk_widget_set_halign(s, GTK_ALIGN_START);
    gtk_label_set_xalign(GTK_LABEL(s), 0.0);
    gtk_label_set_wrap(GTK_LABEL(s), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(s), 46);
    gtk_widget_add_css_class(s, "lit-link-sub");
    i18n_bind(s, sub, 0);
    gtk_box_append(GTK_BOX(box), s);

    gtk_box_append(GTK_BOX(row), box);
    gtk_button_set_child(GTK_BUTTON(btn), row);
    if (target)
        g_object_set_data_full(G_OBJECT(btn), "target", g_strdup(target), g_free);
    g_signal_connect(btn, "clicked",
                     clicked ? clicked : G_CALLBACK(on_nav_clicked), NULL);
    return btn;
}

static GtkWidget *lit_link_card(const char *target, const char *title,
                                const char *sub, GtkDrawingAreaDrawFunc icon) {
    return lit_link_card_ex(target, title, sub, icon, NULL);
}

static void lit_note_card(GtkWidget *parent, const char *title,
                          GtkDrawingAreaDrawFunc icon,
                          const char *const *lines) {
    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *head;
    GtkWidget *ic;
    GtkWidget *t;

    gtk_widget_add_css_class(card, "notes-card");
    gtk_widget_set_margin_bottom(card, 14);
    gtk_box_append(GTK_BOX(parent), card);

    head = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append(GTK_BOX(card), head);

    ic = icon_area_new(icon, 0, 0, 0, 26);
    gtk_widget_set_valign(ic, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(head), ic);

    t = gtk_label_new(title);
    gtk_widget_set_halign(t, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(t), TRUE);
    gtk_widget_set_valign(t, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(t, "notes-title");
    gtk_box_append(GTK_BOX(head), t);

    for (int i = 0; lines[i]; i++) {
        GtkWidget *l = gtk_label_new(lines[i]);

        gtk_widget_set_halign(l, GTK_ALIGN_START);
        gtk_label_set_wrap(GTK_LABEL(l), TRUE);
        gtk_label_set_xalign(GTK_LABEL(l), 0.0);
        gtk_widget_add_css_class(l, "notes-body");
        gtk_box_append(GTK_BOX(card), l);
    }
}

/* ------------------------------------------------------------------ */
/* Reading roadmap (Maturitní četba)                                  */
/* ------------------------------------------------------------------ */

#include "cetba_catalog.inc"
#include "cetba_book_notes.inc"
#include "cetba_book_exercises.inc"

static int stub_book_idx = -1;
static GtkWidget *stub_title_lbl;
static GtkWidget *stub_links_box;
static GtkWidget *stub_notes_box;
static LitQuiz *stub_quiz;
static GtkWidget *stub_plot_host;
static UnitCtx stub_plot_unit;

static void cetba_prepare_stub_quiz(void) {
    const BookExPack *p;

    if (!stub_quiz || stub_book_idx < 0 || stub_book_idx >= BOOK_NODES)
        return;
    p = &book_ex_packs[stub_book_idx];
    if (!p->qs || p->nq <= 0)
        return;
    stub_quiz->qs = p->qs;
    stub_quiz->n = p->nq;
    stub_quiz->idx = 0;
    stub_quiz->score = 0;
    stub_quiz->answered = 0;
    stub_quiz->finished = 0;
    lit_quiz_show(stub_quiz);
}

static void cetba_prepare_stub_plot(void) {
    GtkWidget *child;
    const BookExPack *p;
    GtkWidget *inner;

    if (!stub_plot_host || stub_book_idx < 0 || stub_book_idx >= BOOK_NODES)
        return;

    while ((child = gtk_widget_get_first_child(stub_plot_host)))
        gtk_box_remove(GTK_BOX(stub_plot_host), child);

    p = &book_ex_packs[stub_book_idx];
    if (!p->plot || p->nplot <= 0)
        return;

    stub_plot_unit.page = "cetbastub";
    stub_plot_unit.progress_file = app_progress_cetba;
    inner = build_assembly(&stub_plot_unit, tr("lit_plot_title"),
                           "lit_plot_sub", 1, p->plot, p->plot_meaning,
                           p->nplot);
    gtk_widget_set_hexpand(inner, TRUE);
    gtk_widget_set_vexpand(inner, TRUE);
    gtk_box_append(GTK_BOX(stub_plot_host), inner);
}

static void on_stub_quiz_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    cetba_prepare_stub_quiz();
    if (main_stack)
        gtk_stack_set_visible_child_name(main_stack, "cetbastubquiz");
}

static void on_stub_plot_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    cetba_prepare_stub_plot();
    if (main_stack)
        gtk_stack_set_visible_child_name(main_stack, "cetbastubdej");
}

static GtkWidget *book_rail;
static GtkWidget *book_nodes[BOOK_NODES];
static GtkWidget *book_labels[BOOK_NODES];
static double book_cx[BOOK_NODES];
static double book_cy[BOOK_NODES];
static int book_cw = 540;
static int book_ch = 300;

static void book_point(double t, double *ox, double *oy) {
    int k = (int)t;
    double u = t - (double)k;
    double u2, u3;
    double p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y;

    if (k < 0) { k = 0; u = 0.0; }
    if (k >= BOOK_NODES - 1) { k = BOOK_NODES - 2; u = 1.0; }

    p1x = book_cx[k];     p1y = book_cy[k];
    p2x = book_cx[k + 1]; p2y = book_cy[k + 1];
    if (k - 1 >= 0) { p0x = book_cx[k - 1]; p0y = book_cy[k - 1]; }
    else            { p0x = p1x - (p2x - p1x); p0y = p1y - (p2y - p1y); }
    if (k + 2 < BOOK_NODES) { p3x = book_cx[k + 2]; p3y = book_cy[k + 2]; }
    else                    { p3x = p2x + (p2x - p1x); p3y = p2y + (p2y - p1y); }

    u2 = u * u;
    u3 = u2 * u;
    *ox = 0.5 * (2.0 * p1x + (-p0x + p2x) * u
                 + (2.0 * p0x - 5.0 * p1x + 4.0 * p2x - p3x) * u2
                 + (-p0x + 3.0 * p1x - 3.0 * p2x + p3x) * u3);
    *oy = 0.5 * (2.0 * p1y + (-p0y + p2y) * u
                 + (2.0 * p0y - 5.0 * p1y + 4.0 * p2y - p3y) * u2
                 + (-p0y + 3.0 * p1y - 3.0 * p2y + p3y) * u3);
}

static void book_path(cairo_t *cr, double t0, double t1) {
    const double steps_per_unit = 48.0;
    int n = (int)((t1 - t0) * steps_per_unit);
    double x, y;

    if (n < 1)
        n = 1;
    book_point(t0, &x, &y);
    cairo_move_to(cr, x, y);
    for (int s = 1; s <= n; s++) {
        book_point(t0 + (t1 - t0) * (double)s / (double)n, &x, &y);
        cairo_line_to(cr, x, y);
    }
}

static void book_geometry(double avail) {
    const double phase = 2.0 * G_PI * 1.7 / (double)(BOOK_NODES - 1);
    int rows;
    int r, c, i;

    for (rows = 1; rows <= BOOK_NODES; rows++) {
        int cols = (BOOK_NODES + rows - 1) / rows;
        if (2.0 * ROAD_MX + (cols - 1) * PATH_SPAC <= avail + 1.0)
            break;
    }
    if (rows > BOOK_NODES)
        rows = BOOK_NODES;
    int cols = (BOOK_NODES + rows - 1) / rows;
    if (cols < 1)
        cols = 1;
    book_cw = (int)(2.0 * ROAD_MX + (cols - 1) * PATH_SPAC);
    book_ch = (int)(2.0 * ROAD_MY + (rows - 1) * ROAD_GAP);

    for (r = 0; r < rows; r++) {
        int base = r * cols;
        int len = MIN(cols, BOOK_NODES - base);
        int fwd = (r % 2) == 0;
        for (c = 0; c < len; c++) {
            i = base + c;
            int cc = fwd ? c : (cols - 1 - c);
            book_cx[i] = ROAD_MX + cc * PATH_SPAC;
            book_cy[i] = ROAD_MY + r * ROAD_GAP
                         + ROAD_WAVE * sin((double)i * phase);
        }
    }
}

static void draw_book_rail(GtkDrawingArea *area, cairo_t *cr,
                           int width, int height, gpointer data) {
    const double t_end = (double)(BOOK_NODES - 1);
    const Rgb rail = color_from_hex(app_theme.rail);
    const Rgb muted = color_from_hex(app_theme.subtext);

    (void)area;
    (void)width;
    (void)height;
    (void)data;

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

    cairo_new_path(cr);
    book_path(cr, 0.0, t_end);
    cairo_set_line_width(cr, 12);
    cairo_set_source_rgb(cr, rail.r, rail.g, rail.b);
    cairo_stroke(cr);

    cairo_set_source_rgba(cr, muted.r, muted.g, muted.b, 0.17);
    cairo_set_dash(cr, (double[]){2.0, 40.0}, 2, 0.0);
    cairo_new_path(cr);
    book_path(cr, 0.0, t_end);
    cairo_stroke(cr);
    cairo_set_dash(cr, NULL, 0, 0.0);
}

void book_rail_theme_reset(void) {
    if (book_rail)
        gtk_widget_queue_draw(book_rail);
}

static void rebuild_stub_about(int idx) {
    GtkWidget *child;
    const BookNotePack *pack;
    const BookExPack *ex;
    GtkDrawingAreaDrawFunc icons[4] = {
        draw_book_icon, draw_globe_icon, draw_people_icon, draw_bulb_icon
    };

    if (idx < 0 || idx >= BOOK_NODES)
        return;

    if (stub_links_box) {
        while ((child = gtk_widget_get_first_child(stub_links_box)))
            gtk_box_remove(GTK_BOX(stub_links_box), child);
        ex = &book_ex_packs[idx];
        if (ex->qs && ex->nq > 0)
            gtk_box_append(GTK_BOX(stub_links_box),
                           lit_link_card_ex(NULL, "book_quiz_heading",
                                           "lit_quiz_sub", draw_quiz_icon,
                                           G_CALLBACK(on_stub_quiz_clicked)));
        if (ex->plot && ex->nplot > 0)
            gtk_box_append(GTK_BOX(stub_links_box),
                           lit_link_card_ex(NULL, "lit_plot_title",
                                           "lit_plot_sub", draw_order_icon,
                                           G_CALLBACK(on_stub_plot_clicked)));
        gtk_widget_set_visible(stub_links_box,
                               gtk_widget_get_first_child(stub_links_box) != NULL);
    }

    if (!stub_notes_box)
        return;

    while ((child = gtk_widget_get_first_child(stub_notes_box)))
        gtk_box_remove(GTK_BOX(stub_notes_box), child);

    pack = &book_note_packs[idx];
    if (!pack->secs || pack->nsec <= 0)
        return;
    for (int i = 0; i < pack->nsec; i++) {
        GtkDrawingAreaDrawFunc icon = icons[i < 4 ? i : 0];
        lit_note_card(stub_notes_box, pack->secs[i].title, icon,
                      pack->secs[i].lines);
    }
}

static void on_book_node_clicked(GtkButton *button, gpointer user_data) {
    int idx = GPOINTER_TO_INT(user_data);
    const char *target = book_defs[idx].target;

    (void)button;
    if (!book_defs[idx].full) {
        stub_book_idx = idx;
        if (stub_title_lbl)
            gtk_label_set_text(GTK_LABEL(stub_title_lbl), book_defs[idx].title);
        rebuild_stub_about(idx);
    }
    if (main_stack && target)
        gtk_stack_set_visible_child_name(main_stack, target);
}

static GtkWidget *book_make_node(int i) {
    GtkWidget *btn = gtk_button_new();
    GtkWidget *icon = icon_area_new(draw_book_badge_icon, 0, 0, 0, 48);

    gtk_widget_add_css_class(btn, "unit-node");
    gtk_widget_add_css_class(btn, "current");
    gtk_widget_set_can_focus(btn, FALSE);
    gtk_widget_set_size_request(btn, (int)NODE_SIZE, (int)NODE_SIZE);

    gtk_button_set_child(GTK_BUTTON(btn), icon);

    g_signal_connect(btn, "clicked", G_CALLBACK(on_book_node_clicked),
                     GINT_TO_POINTER(i));
    return btn;
}

GtkWidget *build_readinglist_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *wrap;
    GtkWidget *fixed;
    GtkWidget *rail;
    int i;

    book_geometry(1000.0);

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("czechmap", "Maturitní četba", "reading_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);

    wrap = gtk_center_box_new();
    gtk_widget_set_vexpand(wrap, TRUE);
    gtk_widget_set_hexpand(wrap, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), wrap);

    fixed = gtk_fixed_new();
    gtk_widget_set_halign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(fixed, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(fixed, book_cw, book_ch);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(wrap), fixed);
    gtk_widget_set_margin_start(fixed, 24);
    gtk_widget_set_margin_end(fixed, 24);

    rail = gtk_drawing_area_new();
    gtk_widget_set_size_request(rail, book_cw, book_ch);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(rail), draw_book_rail,
                                   NULL, NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rail, 0, 0);
    book_rail = rail;

    for (i = 0; i < BOOK_NODES; i++) {
        GtkWidget *btn = book_make_node(i);
        GtkWidget *name = gtk_label_new(book_defs[i].title);

        gtk_widget_set_size_request(name, (int)(PATH_SPAC - 20.0), -1);
        gtk_widget_set_halign(name, GTK_ALIGN_CENTER);
        gtk_label_set_justify(GTK_LABEL(name), GTK_JUSTIFY_CENTER);
        gtk_label_set_wrap(GTK_LABEL(name), TRUE);
        gtk_widget_add_css_class(name, "unit-name");
        gtk_widget_add_css_class(name, "cz-label");

        gtk_fixed_put(GTK_FIXED(fixed), btn, 0, 0);
        gtk_fixed_put(GTK_FIXED(fixed), name, 0, 0);

        gtk_fixed_move(GTK_FIXED(fixed), btn,
                       (int)(book_cx[i] - NODE_SIZE / 2.0),
                       (int)(book_cy[i] - NODE_SIZE / 2.0));
        gtk_fixed_move(GTK_FIXED(fixed), name,
                       (int)(book_cx[i] - (PATH_SPAC - 20.0) / 2.0),
                       (int)(book_cy[i] + NODE_SIZE / 2.0 + 10.0));

        book_nodes[i] = btn;
        book_labels[i] = name;
    }

    return page;
}

GtkWidget *build_cetba1984_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *box;
    GtkWidget *links;
    static const char *about[] = {
        "Autor: George Orwell (vl. jm. Eric Arthur Blair)",
        "Žánr: antiutopický (dystopický) román",
        "Poprvé vydáno: 1949",
        "Děj: Londýn, provincie Letopočet 1, stát Oceánie",
        "Vyprávění: v er-formě, po deníkových zápiscích Winstona Smithe",
        NULL,
    };
    static const char *world[] = {
        "Svět je rozdělen na tři supervelmoci: Oceánii, Eurasii a Východasii, "
        "které spolu vedou trvalou válku.",
        "Vládne jediná strana (Angsoc) v čele s kultem Velkého bratra.",
        "Ministerstvo pravdy – přepisuje historii a vyrábí „pravdu“.",
        "Ministerstvo lásky – mučírny a likvidace nepohodlných.",
        "Ministerstvo hojnosti – plánuje hospodářství a drží lidi v "
        "nedostatku.",
        "Ministerstvo míru – vede válku.",
        NULL,
    };
    static const char *people[] = {
        "Winston Smith – hlavní hrdina, přepisuje historii, touží po svobodě.",
        "Julie – mladá členka strany, Winstonova tajná láska.",
        "O'Brien – člen vnitřní strany, předstírá odboj a Winstona zničí.",
        "Emmanuel Goldstein – nepřítel státu, údajný vůdce Bratrstva.",
        "Velký bratr – symbol všemocné moci strany.",
        "Pan Charrington – zdánlivý obchodník, ve skutečnosti agent.",
        NULL,
    };
    static const char *terms[] = {
        "Newspeak – uměle zjednodušovaný jazyk, který omezuje myšlení.",
        "Dvojité myšlení – ochota věřit dvěma protikladným pravdám.",
        "Zločin myšlení – už samotná zakázaná myšlenka.",
        "Policie myšlení – tajná policie, která zatýká a mučí.",
        "Bratrstvo – možná neexistující odbojová organizace.",
        "Místnost 101 – nejhlubší mučírna, pro každého jeho největší strach.",
        NULL,
    };

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("readinglist", "George Orwell – 1984",
                           "cetba1984_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_widget_set_margin_bottom(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), box);

    links = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_bottom(links, 14);
    gtk_box_append(GTK_BOX(box), links);
    gtk_box_append(GTK_BOX(links),
                   lit_link_card("cetba1984quiz", "lit_quiz_title",
                                 "lit_quiz_sub", draw_quiz_icon));
    gtk_box_append(GTK_BOX(links),
                   lit_link_card("cetba1984dej", "lit_plot_title",
                                 "lit_plot_sub", draw_order_icon));

    lit_note_card(box, "O knize", draw_book_icon, about);
    lit_note_card(box, "Svět a strana", draw_globe_icon, world);
    lit_note_card(box, "Postavy", draw_people_icon, people);
    lit_note_card(box, "Klíčové pojmy", draw_bulb_icon, terms);

    return page;
}

GtkWidget *build_cetba_fuks_page(void) {
    GtkWidget *page;
    GtkWidget *scroll;
    GtkWidget *box;
    GtkWidget *links;
    static const char *about[] = {
        "Autor: Ladislav Fuks (1923–1994)",
        "Žánr: psychologický román s prvky grotesky a hororu",
        "Poprvé vydáno: 1967",
        "Děj: Praha v době protektorátu (konec 30. let)",
        "Vyprávění: ich-forma – vypráví sám pan Kopfrkingl",
        NULL,
    };
    static const char *world[] = {
        "Děj se odehrává v protektorátu Čechy a Morava za nastupujícího "
        "nacismu.",
        "Atmosféru tvoří strach, přizpůsobování a touha zalíbit se moci.",
        "Krematorium je symbolem smrti a odlidštění.",
        "Antisemitismus a árijská ideologie postupně prostupují společnost.",
        "Obyčejný úředník se mění v nástroj zla.",
        NULL,
    };
    static const char *people[] = {
        "Pan Kopfrkingl – zaměstnanec krematoria, vypravěč a vrah své rodiny.",
        "Lakmé – jeho žena, židovského původu.",
        "Zina a Mil – jeho děti.",
        "Willi Reinke – Němec, který Kopfrkingla přivede k nacismu.",
        "Dr. Bettelheim – lékař, s nímž Kopfrkingl rozmlouvá o smrti.",
        NULL,
    };
    static const char *terms[] = {
        "Krematorium – symbol smrti a zvrácené „očisty“.",
        "Kniha mrtvých – tibetský text, který si Kopfrkingl překrucuje.",
        "Reinkarnace a „osvobození“ – záminka pro vraždy.",
        "Masky a přetvářka – zlo se tváří jako zdvořilost a řád.",
        "Groteska a černý humor – hrůza podaná klidným, slavnostním tónem.",
        "Voda a Vltava – motiv smrti a plynutí.",
        NULL,
    };

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page),
                   top_bar("readinglist", "Ladislav Fuks – Spalovač mrtvol",
                           "cetbaFuks_sub"));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_widget_set_margin_bottom(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), box);

    links = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_bottom(links, 14);
    gtk_box_append(GTK_BOX(box), links);
    gtk_box_append(GTK_BOX(links),
                   lit_link_card("cetbaFuksQuiz", "lit_fuks_quiz_title",
                                 "lit_fuks_quiz_sub", draw_quiz_icon));
    gtk_box_append(GTK_BOX(links),
                   lit_link_card("cetbaFuksDej", "lit_fuks_plot_title",
                                 "lit_fuks_plot_sub", draw_order_icon));

    lit_note_card(box, "O knize", draw_book_icon, about);
    lit_note_card(box, "Doba a svět", draw_globe_icon, world);
    lit_note_card(box, "Postavy", draw_people_icon, people);
    lit_note_card(box, "Klíčové motivy", draw_bulb_icon, terms);

    return page;
}

GtkWidget *build_cetba_stub_page(void) {
    GtkWidget *page;
    GtkWidget *top;
    GtkWidget *center;
    GtkWidget *scroll;
    GtkWidget *body;
    GtkWidget *sub;

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    top = gtk_center_box_new();
    gtk_widget_add_css_class(top, "page-top");
    gtk_widget_set_margin_top(top, 8);
    gtk_widget_set_margin_bottom(top, 18);
    gtk_center_box_set_start_widget(GTK_CENTER_BOX(top),
                                    make_back_button("readinglist"));

    center = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_valign(center, GTK_ALIGN_CENTER);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(top), center);

    stub_title_lbl = gtk_label_new("…");
    gtk_widget_set_halign(stub_title_lbl, GTK_ALIGN_CENTER);
    gtk_label_set_wrap(GTK_LABEL(stub_title_lbl), TRUE);
    gtk_widget_add_css_class(stub_title_lbl, "roadmap-title");
    gtk_box_append(GTK_BOX(center), stub_title_lbl);

    sub = gtk_label_new(NULL);
    i18n_bind(sub, "cetba1984_sub", 0);
    gtk_widget_set_halign(sub, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(sub, "roadmap-sub");
    gtk_label_set_wrap(GTK_LABEL(sub), TRUE);
    gtk_box_append(GTK_BOX(center), sub);

    gtk_box_append(GTK_BOX(page), top);

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_widget_set_margin_bottom(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);

    body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), body);

    stub_links_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_bottom(stub_links_box, 14);
    gtk_box_append(GTK_BOX(body), stub_links_box);

    stub_notes_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_append(GTK_BOX(body), stub_notes_box);

    return page;
}

GtkWidget *build_cetba_stub_quiz_page(void) {
    const LitQ *qs = book_ex_packs[0].qs;
    int n = book_ex_packs[0].nq > 0 ? book_ex_packs[0].nq : 1;
    GtkWidget *page;

    page = lit_quiz_page(qs, n, "cetbastub",
                         "book_quiz_heading", "lit_quiz_sub");
    stub_quiz = g_object_get_data(G_OBJECT(page), "lit-quiz");
    return page;
}

GtkWidget *build_cetba_stub_plot_page(void) {
    stub_plot_host = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(stub_plot_host, TRUE);
    gtk_widget_set_vexpand(stub_plot_host, TRUE);
    return stub_plot_host;
}

void cetba_open_stub(int idx) {
    if (idx < 0 || idx >= BOOK_NODES || book_defs[idx].full)
        return;
    stub_book_idx = idx;
    if (stub_title_lbl)
        gtk_label_set_text(GTK_LABEL(stub_title_lbl), book_defs[idx].title);
    rebuild_stub_about(idx);
    if (main_stack)
        gtk_stack_set_visible_child_name(main_stack, "cetbastub");
}

void cetba_register_search(void) {
    char target[32];

    for (int i = 0; i < BOOK_NODES; i++) {
        if (book_defs[i].full) {
            search_catalog_add(book_defs[i].title, tr("search_book"),
                               book_defs[i].target, FALSE, 2, book_defs[i].author);
            if (g_strcmp0(book_defs[i].target, "cetba1984") == 0)
                search_catalog_add(tr("lit_quiz_title"), tr("search_exercise"),
                                   "cetba1984quiz", FALSE, 3, "orwell kviz");
            else if (g_strcmp0(book_defs[i].target, "cetbaFuks") == 0)
                search_catalog_add(tr("lit_fuks_quiz_title"),
                                   tr("search_exercise"), "cetbaFuksQuiz",
                                   FALSE, 3, "fuks kviz");
        } else {
            g_snprintf(target, sizeof target, "cetbastub:%d", i);
            search_catalog_add(book_defs[i].title, tr("search_book"),
                               target, FALSE, 2, book_defs[i].author);
        }
    }
}
