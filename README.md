# maturita.c

An educational GTK 4 app written in C. / Vzdělávací GTK 4 aplikace napsaná v C.

**[English](#english)** · **[Česky](#česky)**

---

## English

### Description

maturita.c is an educational program for high-school and gymnasium students
preparing for their maturita exam. The UI is available in Czech and English and
is built from a set of small C modules (see
[Project structure](#project-structure)) that render a modern, CSS-styled
GTK 4 interface.

### Screenshots

| Welcome screen                          | Subjects ("Předměty")                    | German learning roadmap                 |
| --------------------------------------- | ---------------------------------------- | --------------------------------------- |
| ![Welcome screen](assets/welcomescreen.png) | ![Subjects screen](assets/subjectsscreen.png) | ![German roadmap](assets/germanlection.png) |

### Features

- **Welcome screen** – branded greeting with a description of the program and a
  continue button to get started
- **Subjects ("Předměty")** – adaptive serpentine subject map with 13 subjects
  - **Deutsch** is open (round bubble with the German flag) and opens the
    German learning path
  - **Správa počítačových sítí / Computer Network Administration** is open too
    (shown with a Wi-Fi icon) and leads to a 30-unit computer-networks path;
    **units 1–13 are playable**, units 14–30 are locked placeholders
  - **Technické vybavení / Computer Hardware** is open (chip icon) and leads
    to a 10-unit hardware path; **units 1–3 are playable**, units 4–10 are
    locked placeholders
  - the remaining subjects are dimmed/locked placeholders until their content
    is added
  - layout folds into multiple rows when the window is narrow
- **Learning roadmap** – adaptive serpentine learning path with 10 units
  - each unit numbered and labeled with its topic (e.g. "Neue Freunde")
  - units 1–3 are open; completed units shown in green with a checkmark
  - remaining units dimmed with a lock icon
  - finish node at the end of the path
  - layout folds into multiple rows when the window is narrow
- **Back navigation** – themed circular back button (top-left) returns to the
  previous screen
- **Unit 1 exercises** – "Neue Freunde" opens an interactive bubble map with 13
  exercises (dialogs, sentence building, multiple choice, free answers,
  numbers, verb conjugation, greeting sorting and more), each with German
  prompts and an instant-check button
- **Unit 2 exercises** – "Aus aller Welt" is playable from the start (no need
  to finish unit 1 first) and offers a bubble map with 19 exercises: verb
  conjugation, prepositions `aus`/`in`, question words, nationalities and
  countries, spelling/look-and-find tasks, a matching exercise, a number/word
  game, free-answer "Steckbrief" tasks and a Hangman game with 5 professions
- **Unit 3 exercises** – "Bei uns zu Hause" covers family vocabulary,
  possessives, accusative and related grammar across 15 exercises (drop-down
  sentence fill-ins, word building, "Was siehst du?", sentence ordering and
  missing-letter sentences)
- **Hints & model answers** – in exercises where you write or complete a German
  sentence the Czech meaning is shown right away as a hint, and clicking the
  check button also reveals the correct German sentence so you can compare
- **Computer networks path** – thirteen unlocked units (theory slides + practice):
  1. **Výpočet IP adres / IP Address Calculation** – 4-slide VLSM walkthrough
     and four interactive subnetting tasks (prefix, network, broadcast, host
     range) with check and “show solution”
  2. Základní pojmy a rozdělení sítí – slides + multiple-choice quiz
  3. Sítě Klient–Server a Cloud – slides + quiz
  4. Přenos dat a jeho zabezpečení – slides + quiz
  5. Signály a modulace – slides + quiz
  6. Cesta, kanál a okruh – slides + quiz
  7. Multiplex a směry přenosu – slides + quiz
  8. Přepojování – slides + quiz
  9. Vrstevnaté modely a protokoly – slides + quiz
  10. Model ISO/OSI a TCP/IP – slides + quiz
  11. Podrobný rozbor 7 vrstev ISO/OSI – slides + quiz
  12. Základy architektury TCP/IP – slides + quiz
  13. Topologie sítě – slides + quiz  
  Progress for networks is saved to `progress/net.conf` (one flag per unit);
  completed network units show a checkmark on the path and count in Statistics
- **Computer hardware path** – three unlocked units (theory slides + quiz):
  1. **Architektura počítače / Computer architecture** – von Neumann, the
     von Neumann model (ALU, control unit, memory, I/O), then a quiz
  2. **Historie počítačů / History of computers** – generations 0–4 (vacuum
     tubes, transistors, ICs, microprocessors and milestones), then a quiz
  3. **Zobrazování dat / Data representation** – bit and byte, then a quiz  
  Progress for hardware is saved to `progress/hw.conf`; completed units show
  a checkmark on the path and count in Statistics
- **Progress tracking** – completed exercises are marked green; progress is
  saved per unit to `progress/unit1.conf`, `progress/unit2.conf` and
  `progress/unit3.conf` and restored on startup
- **Statistics** – chart icon in the header opens a per-subject progress
  overview: summary totals (exercises done, percent, finished units) at the
  top, then every subject with its own progress bar (subjects without tracked
  content yet are shown as locked; **networks** and **hardware** use lesson
  completion from `progress/net.conf` / `progress/hw.conf`), and a per-unit
  breakdown with progress bars for Deutsch; numbers update as soon as you
  finish an exercise
- **Settings** – gear icon in the top-right header opens a panel where you can
  choose:
  - **mode**: dark or light, as a segmented control
  - **theme**: 10 palettes picked from a grid of color swatches
  - **language**: Czech or English, applied instantly without a restart
  - all three preferences are stored in `progress/settings.conf`
- **Keyboard quit shortcuts** – `Super/Cmd+Q` or `Alt+F4` closes the app
- Modular C codebase split across several files, cross-platform (Linux, macOS and Windows)

### Installation

#### Prerequisites

##### 1. GTK 4 development libraries

macOS:

```bash
brew install gtk4
```

Linux (Debian / Ubuntu):

```bash
sudo apt install libgtk-4-dev
```

Linux (Fedora):

```bash
sudo dnf install gtk4-devel
```

Linux (Arch):

```bash
sudo pacman -S gtk4
```

Windows – there is no official GTK 4 installer. The recommended path is
**MSYS2 + MinGW UCRT64** (no Visual Studio). See
[Windows install guide (GTK4)](#windows-install-guide-gtk4) below.

##### 2. Build tools (compiler, Make, pkg-config)

macOS – install these explicitly; they are **not** preinstalled:

```bash
brew install gcc make pkg-config
# or Apple's Clang toolchain:
# xcode-select --install
```

Linux – on most distributions `gcc` is already available
(`build-essential` / `base-devel`):

```bash
# Debian / Ubuntu
sudo apt install build-essential pkg-config
```

```bash
# Fedora
sudo dnf groupinstall "Development Tools"
sudo dnf install pkgconf-pkg-config
```

```bash
# Arch
sudo pacman -S base-devel pkgconf
```

Windows – `gcc`, GTK 4 and `pkg-config` are installed in step 1 of the
Windows guide below. Visual Studio is not required.

#### Building the application

1. Clone or download this repository
2. Navigate to the project directory
3. Build with the Makefile:

```bash
make
```

Or manually:

```bash
# Linux / macOS
gcc -o maturita *.c $(pkg-config --cflags --libs gtk4) -lm

# Windows (MSYS2 UCRT64)
gcc -o maturita.exe *.c $(pkg-config --cflags --libs gtk4) -lm -mwindows
```

##### Building with CMake (optional, Linux / macOS)

```bash
cmake -S . -B build
cmake --build build
```

CMake finds GTK 4 through pkg-config. On Windows prefer the MSYS2 flow below.

#### Running the application

```bash
make run
```

or

```bash
# Linux / macOS
./maturita

# Windows (from the same MSYS2 UCRT64 shell used to build)
./maturita.exe
```

#### Windows install guide (GTK4)

Follow these steps to compile and run on Windows.
**Visual Studio is not required** – MSYS2 and MinGW UCRT64 are enough.

1. **Install MSYS2 and GTK4**

   Open Command Prompt (CMD) or PowerShell as Administrator and run:

   ```cmd
   winget install --id MSYS2.MSYS2 -e --source winget
   C:\msys64\ucrt64.exe pacman -S --noconfirm mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-gtk4 mingw-w64-ucrt-x86_64-pkg-config
   ```

   Wait until both commands finish. If `winget` says MSYS2 is already
   installed, continue with the second command.
   (The second command opens an MSYS2 window, downloads packages, then exits –
   that is expected.)

2. **Get the project**

   Clone or download this repository and note the path, e.g.
   `C:\Users\Josef\maturita.c`.

3. **Open the MSYS2 UCRT64 terminal**

   In the Start menu search for **MSYS2 UCRT64** and open it.
   Do not use regular CMD or PowerShell – `gcc` and `pkg-config` work in this
   shell.

4. **Enter the project folder and compile**

   In MSYS2, paths look like `/c/...` instead of `C:\...`. Adjust the path and
   run:

   ```bash
   cd /c/Users/Josef/maturita.c
   gcc -o maturita.exe *.c $(pkg-config --cflags --libs gtk4) -lm -mwindows
   ```

   Path conversion examples:

   - `C:\Users\Josef\maturita.c` → `/c/Users/Josef/maturita.c`
   - `C:\Users\Josef\Desktop\maturita.c` → `/c/Users/Josef/Desktop/maturita.c`

   If compilation succeeds, `maturita.exe` appears in the project folder.

5. **Run the app**

   From the same UCRT64 terminal:

   ```bash
   ./maturita.exe
   ```

6. **(Optional) Double-click from File Explorer**

   Running `maturita.exe` outside the MSYS2 terminal usually fails with missing
   DLLs. From the UCRT64 terminal (after installing `make`) you can bundle
   everything into `dist/`:

   ```bash
   pacman -S --noconfirm make
   make bundle
   ```

   Then double-click `dist/maturita.exe`.

### Usage

When launched, the application shows the welcome screen. From there:

1. Click **"Pokračuj" / "Continue"** to open the **Předměty / Subjects** map.
2. Click the **Deutsch** bubble (German flag) for the German path, the **Wi-Fi**
   bubble ("Správa počítačových sítí") for networks, or the **chip** bubble
   ("Technické vybavení") for hardware. Other subjects are still locked.
3. Browse the units along the path; the roadmap reflows into rows and scrolls
   if the window is too narrow.
4. Click the **"Neue Freunde"** node to open unit 1, **"Aus aller Welt"** for
   unit 2 or **"Bei uns zu Hause"** for unit 3, then pick any bubble to start
   an exercise. Units 2 and 3 are available even if the earlier units are not
   finished yet.
5. On the networks path, units **1–13** are open. Unit 1 (**Výpočet IP adres**)
   walks through four VLSM slides and then four interactive subnetting tasks
   with check / “show solution”. Units 2–13 each have theory slides followed by
   a multiple-choice quiz (basic concepts, client–server/cloud, transmission
   security, signals, path/channel/circuit, multiplexing, switching, layered
   models, ISO/OSI & TCP/IP, the seven ISO/OSI layers, TCP/IP architecture,
   and network topologies). Units 14–30 stay locked placeholders.
6. On the hardware path, units **1–3** are open (architecture / von Neumann,
   computer history generations, bit & byte), each with slides and a quiz.
   Units 4–10 stay locked.
7. Use the **back button** in the top-left corner to return to the previous
   screen.
8. Open **Statistiky / Statistics** (chart icon, top-right) to see progress
   grouped per subject (overall totals plus, for Deutsch, a per-unit
   breakdown). Network and hardware lesson completion is included in the
   subject totals.
9. Open **Nastavení / Settings** (gear icon, top-right) to switch dark/light
   mode, pick a color theme, or change the interface language.

German units 1–3, network units 1–10 and hardware units 1–3 are currently
implemented. Locked nodes do nothing – they are placeholders until lessons are
added. Completed Deutsch exercises stay green and are saved per unit to
`progress/unit1.conf` / `progress/unit2.conf` / `progress/unit3.conf`.
Completed network units are saved to `progress/net.conf`; hardware units to
`progress/hw.conf`.

#### Keyboard shortcuts

| Shortcut            | Action           |
| ------------------- | ---------------- |
| `Super/Cmd` + `Q`   | Quit application |
| `Alt` + `F4`        | Quit application |

### Project structure

```
maturita.h              shared types, macros, globals and prototypes
globals.c               shared global state (window, stack, units, theme, …)
theme.c                 theme palettes, stylesheet injection and settings file
i18n.c                  Czech/English translation tables and language switching
icons.c                 Cairo-drawn vector icons (check, lock, wifi, chip, …)
util.c                  small helpers (answer normalisation, feedback, umlaut note)
progress.c              per-unit exercise progress (load/save/completion)
stats.c                 statistics page and progress aggregation
navigation.c            back button, top bar, welcome page, key shortcuts
roadmap.c               German learning-path roadmap
subjects.c              subjects map and subject catalogue
unitmap.c               per-unit exercise bubble map
exercise_common.c       shared exercise shell + combo/assembly/choice widgets
exercise_u1.c           unit 1 ("Neue Freunde") exercises
exercise_u2.c           unit 2 ("Aus aller Welt") exercises
exercise_u3.c           unit 3 ("Bei uns zu Hause") exercises
settings.c              settings popover (mode, theme, language)
net.c                   computer-networks path (30 nodes), units 1–10 slides
                        and exercises (unit 1: VLSM practice; 2–10: MC quizzes)
hw.c                    computer-hardware path (10 nodes), units 1–3 slides
                        and quizzes (architecture, history, bit/byte)
main.c                  application entry point and page wiring
style.css               GTK stylesheet (palette colors are injected at runtime)
Makefile                build & run targets (Linux, macOS, MSYS2); `make bundle` on Windows
CMakeLists.txt          optional CMake build (Linux / macOS)
build-windows.bat       legacy MSVC build (requires Visual Studio + gvsbuild GTK)
run-windows.bat         legacy launcher for the MSVC build with GTK DLLs on PATH
README.md               this file
LICENSE                 GPL-3.0 license
assets/                 README screenshots (welcome, subjects, German roadmap)
progress/               created at runtime
  unit1.conf            unit 1 exercise completion state
  unit2.conf            unit 2 exercise completion state
  unit3.conf            unit 3 exercise completion state
  net.conf              computer-networks lesson completion
  hw.conf               computer-hardware lesson completion
  settings.conf         theme, dark/light mode and language preference
```

### Theming

The app ships with ten palettes – Catppuccin, Nord, Dracula, Rose Pine, Ocean,
Gruvbox, Solarized, Everforest, Monokai and One Dark – each available in dark
and light mode. Colors are defined as theme palettes in `theme.c`, injected
as `@define-color` bindings, and combined with the rules in the external
`style.css` stylesheet (plus matching Cairo colors for the roadmap rails and
icons).

Change the look from the in-app settings panel, or edit the palette tables in
`theme.c` / the rules in `style.css` and rebuild.

### Languages

The interface can be switched between Czech and English from the settings
panel; the choice takes effect immediately and is remembered between runs.
Translations live in two tables in `i18n.c`: `tr_ui` holds interface
strings under short keys, while `tr_content` translates the Czech meanings
shown next to the German exercise items and uses the Czech text itself as the
key. Widgets are registered with `i18n_bind()` so `apply_language()` can
retranslate them in place.

German prompts, unit names and exercise titles are learning material and stay
German in both languages.

### To-Do plans

- [ ] Wire unit nodes to actual German vocabulary/grammar exercises
  - [x] Unit 1 ("Neue Freunde") – 13 interactive exercises with progress saving
  - [x] Unit 2 ("Aus aller Welt") – 19 interactive exercises (incl. Hangman)
  - [x] Unit 3 ("Bei uns zu Hause") – 15 interactive exercises
- [x] Computer networks units 1–10 (IP/VLSM + nine theory units with quizzes)
- [x] Progress tracking / statistics for the networks path
- [x] Technické vybavení units 1–3 (architecture, history, bit/byte + quizzes)
- [ ] Lesson progression and unlock system for the remaining units
- [ ] Add audio pronunciation features
- [x] Add user settings panel (theme, dark/light mode, language)
- [ ] Implement spaced repetition system for vocabulary
- [ ] Add listening and reading comprehension exercises
- [ ] Add writing practice with feedback
- [x] Expand progress tracking with statistics
- [x] Move CSS styling to an external file
- [x] Split the UI code into multiple files
- [x] Add internationalization support (Czech and English)
- [ ] Add more interface languages
- [ ] Add unit testing framework

### License

This project is licensed under the GPL-3.0 License – see the LICENSE file for
details.

### Acknowledgments

- Created as a learning exercise in C and GTK programming
- Inspired by language learning applications
- Built with GTK 4 for modern GUI development

---

## Česky

### Popis

maturita.c je vzdělávací program pro středoškoláky a gymnazisty připravující
se na maturitu. Rozhraní je v češtině a angličtině a aplikace je rozdělena do
několika malých C modulů (viz [Struktura projektu](#struktura-projektu)) s
moderním GTK 4 rozhraním stylovaným přes CSS.

### Screenshoty

| Úvodní obrazovka                        | Předměty                                 | Německá učební cesta                    |
| --------------------------------------- | ---------------------------------------- | --------------------------------------- |
| ![Welcome screen](assets/welcomescreen.png) | ![Subjects screen](assets/subjectsscreen.png) | ![German roadmap](assets/germanlection.png) |

### Funkce

- **Úvodní obrazovka** – branding, popis programu a tlačítko pokračovat
- **Předměty** – přizpůsobivá „hadí“ mapa s 13 předměty
  - **Deutsch** je otevřený (bublina s německou vlajkou) a vede na německou
    učební cestu
  - **Správa počítačových sítí** je také otevřená (ikona Wi‑Fi) a vede na cestu
    s 30 jednotkami; **jednotky 1–13 jsou hratelné**, 14–30 jsou zamčené
    placeholdery
  - **Technické vybavení** je otevřené (ikona čipu) a vede na cestu
    s 10 jednotkami; **jednotky 1–3 jsou hratelné**, 4–10 jsou zamčené
    placeholdery
  - ostatní předměty jsou ztmavené / zamčené placeholdery
  - při úzkém okně se mapa zalomí do více řad
- **Učební cesta** – přizpůsobivá cesta s 10 jednotkami
  - každá jednotka má číslo a název (např. „Neue Freunde“)
  - jednotky 1–3 jsou otevřené; dokončené jsou zelené s fajfkou
  - zbývající jsou ztmavené se zámkem
  - na konci je cílová / finish uzel
  - při úzkém okně se cesta zalomí do více řad
- **Zpět** – kulaté tlačítko vlevo nahoře vrátí na předchozí obrazovku
- **Cvičení jednotky 1** – „Neue Freunde“: mapa s 13 cvičeními (dialogy,
  skládání vět, výběr z možností, volné odpovědi, čísla, časování sloves,
  řazení pozdravů…), německé zadání a okamžitá kontrola
- **Cvičení jednotky 2** – „Aus aller Welt“ je hratelná hned (není nutné
  dokončit jednotku 1): 19 cvičení – časování, předložky `aus`/`in`, tázací
  slova, národnosti a země, pravopis / hledej a najdi, párování, hra s čísly,
  volné „Steckbrief“ odpovědi a Hangman s 5 povoláními
- **Cvičení jednotky 3** – „Bei uns zu Hause“: rodina, přivlastňovací zájmena,
  akuzativ a další gramatika v 15 cvičeních (doplňování vět, skládání slov,
  „Was siehst du?“, řazení vět, chybějící písmena)
- **Nápovědy a vzorové odpovědi** – u psaných / doplňovaných vět je hned
  český význam; po kontrole se ukáže i správná německá věta
- **Cesta počítačových sítí** – třináct odemčených jednotek (snímky + cvičení):
  1. **Výpočet IP adres** – 4 snímky VLSM a čtyři interaktivní úlohy
     (prefix, síť, broadcast, rozsah uzlů) s kontrolou a „ukázat řešení“
  2. Základní pojmy a rozdělení sítí – snímky + kvíz
  3. Sítě Klient–Server a Cloud – snímky + kvíz
  4. Přenos dat a jeho zabezpečení – snímky + kvíz
  5. Signály a modulace – snímky + kvíz
  6. Cesta, kanál a okruh – snímky + kvíz
  7. Multiplex a směry přenosu – snímky + kvíz
  8. Přepojování – snímky + kvíz
  9. Vrstevnaté modely a protokoly – snímky + kvíz
  10. Model ISO/OSI a TCP/IP – snímky + kvíz
  11. Podrobný rozbor 7 vrstev ISO/OSI – snímky + kvíz
  12. Základy architektury TCP/IP – snímky + kvíz
  13. Topologie sítě – snímky + kvíz  
  Postup u sítí se ukládá do `progress/net.conf` (příznak na jednotku);
  dokončené jednotky mají fajfku na cestě a počítají se ve Statistikách
- **Cesta technického vybavení** – tři odemčené jednotky (snímky + kvíz):
  1. **Architektura počítače** – von Neumann, Von Neumannovo schéma (ALS,
     řadič, paměť, V/V) a kvíz
  2. **Historie počítačů** – generace 0–4 (elektronky, tranzistory, IO,
     mikroprocesory a milníky) a kvíz
  3. **Zobrazování dat v počítači** – bit a byte a kvíz  
  Postup se ukládá do `progress/hw.conf`; dokončené jednotky mají fajfku
  na cestě a počítají se ve Statistikách
- **Ukládání postupu** – hotová cvičení zezelenají; stav se ukládá do
  `progress/unit1.conf`, `progress/unit2.conf` a `progress/unit3.conf` a při
  startu se načte zpět
- **Statistiky** – ikona grafu v hlavičce ukáže postup po předmětech (součty,
  procenta, dokončené jednotky), progress bary u každého předmětu (předměty
  bez trackovaného obsahu jsou zamčené; **sítě** a **technické vybavení**
  berou dokončení lekcí z `progress/net.conf` / `progress/hw.conf`) a rozpad
  po jednotkách u Deutsch
- **Nastavení** – ozubené kolečko vpravo nahoře:
  - **režim**: tmavý / světlý
  - **téma**: 10 barevných palet
  - **jazyk**: čeština nebo angličtina, bez restartu
  - vše se ukládá do `progress/settings.conf`
- **Klávesové zkratky pro ukončení** – `Super/Cmd+Q` nebo `Alt+F4`
- Modulární C kód v několika souborech, multiplatformní (Linux, macOS i Windows)

### Instalace

#### Požadavky

##### 1. Vývojářské knihovny GTK 4

macOS:

```bash
brew install gtk4
```

Linux (Debian / Ubuntu):

```bash
sudo apt install libgtk-4-dev
```

Linux (Fedora):

```bash
sudo dnf install gtk4-devel
```

Linux (Arch):

```bash
sudo pacman -S gtk4
```

Windows – oficiální instalátor GTK 4 neexistuje. Doporučený postup je
**MSYS2 + MinGW UCRT64** (bez Visual Studia). Viz
[Instalační návod pro Windows (GTK4)](#instalační-návod-pro-windows-gtk4)
níže.

##### 2. Build nástroje (kompilátor, Make, pkg-config)

macOS – nainstalujte je ručně, **nejsou** předinstalované:

```bash
brew install gcc make pkg-config
# nebo Apple Clang:
# xcode-select --install
```

Linux – na většině distro je `gcc` už k dispozici
(`build-essential` / `base-devel`):

```bash
# Debian / Ubuntu
sudo apt install build-essential pkg-config
```

```bash
# Fedora
sudo dnf groupinstall "Development Tools"
sudo dnf install pkgconf-pkg-config
```

```bash
# Arch
sudo pacman -S base-devel pkgconf
```

Windows – `gcc`, GTK 4 i `pkg-config` se nainstalují v kroku 1 návodu níže.
Visual Studio není potřeba.

#### Sestavení aplikace

1. Naklonujte nebo stáhněte tento repozitář
2. Přejděte do složky projektu
3. Sestavte pomocí Makefile:

```bash
make
```

Nebo ručně:

```bash
# Linux / macOS
gcc -o maturita *.c $(pkg-config --cflags --libs gtk4) -lm

# Windows (MSYS2 UCRT64)
gcc -o maturita.exe *.c $(pkg-config --cflags --libs gtk4) -lm -mwindows
```

##### Sestavení přes CMake (volitelné, Linux / macOS)

```bash
cmake -S . -B build
cmake --build build
```

CMake najde GTK 4 přes pkg-config. Na Windows použijte MSYS2 návod níže.

#### Spuštění aplikace

```bash
make run
```

nebo

```bash
# Linux / macOS
./maturita

# Windows (ze stejného MSYS2 UCRT64 terminálu, ve kterém jste kompilovali)
./maturita.exe
```

#### Instalační návod pro Windows (GTK4)

Následujte tyto kroky pro rychlé zkompilování a spuštění na Windows.
**Visual Studio není potřeba** – stačí MSYS2 a MinGW UCRT64.

1. **Nainstalujte MSYS2 a GTK4**

   Otevřete Příkazový řádek (CMD) nebo PowerShell jako Správce a spusťte:

   ```cmd
   winget install --id MSYS2.MSYS2 -e --source winget
   C:\msys64\ucrt64.exe pacman -S --noconfirm mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-gtk4 mingw-w64-ucrt-x86_64-pkg-config
   ```

   Počkejte, až se obě instalace dokončí. Pokud `winget` hlásí, že MSYS2 už
   je nainstalované, klidně pokračujte druhým příkazem.
   (Druhý příkaz otevře okno MSYS2, stáhne balíčky a zase se ukončí – to je
   v pořádku.)

2. **Stáhněte projekt**

   Naklonujte nebo stáhněte tento repozitář a zapamatujte si cestu, např.
   `C:\Users\Josef\maturita.c`.

3. **Otevřete terminál MSYS2 UCRT64**

   Ve Start menu vyhledejte **MSYS2 UCRT64** a spusťte ho.
   Nepoužívejte běžný CMD ani PowerShell – `gcc` a `pkg-config` fungují
   právě v tomto okně.

4. **Přejděte do složky projektu a zkompilujte**

   V MSYS2 se cesty píší jako `/c/...` místo `C:\...`. Upravte cestu podle
   sebe a spusťte:

   ```bash
   cd /c/Users/Josef/maturita.c
   gcc -o maturita.exe *.c $(pkg-config --cflags --libs gtk4) -lm -mwindows
   ```

   Příklady převodu cesty:

   - `C:\Users\Josef\maturita.c` → `/c/Users/Josef/maturita.c`
   - `C:\Users\Josef\Desktop\maturita.c` → `/c/Users/Josef/Desktop/maturita.c`

   Pokud kompilace projde bez chyb, ve složce projektu vznikne `maturita.exe`.

5. **Spusťte aplikaci**

   Ze stejného terminálu UCRT64:

   ```bash
   ./maturita.exe
   ```

6. **(Volitelně) Spuštění dvojklikem z Průzkumníka**

   Samotný `maturita.exe` bez MSYS2 terminálu obvykle spadne na chybějící
   DLL. Z UCRT64 terminálu (po instalaci `make`) můžete vytvořit složku
   `dist/` se vším potřebným:

   ```bash
   pacman -S --noconfirm make
   make bundle
   ```

   Pak spusťte `dist/maturita.exe` dvojklikem.

### Použití

Po spuštění se zobrazí úvodní obrazovka. Odtud:

1. Klikněte na **"Pokračuj" / "Continue"** a otevře se mapa **Předměty /
   Subjects**.
2. Klikněte na bublinu **Deutsch** (německá vlajka) pro německou cestu, na
   bublinu **Wi‑Fi** („Správa počítačových sítí“) pro sítě, nebo na bublinu
   **čipu** („Technické vybavení“) pro hardware. Ostatní předměty jsou zatím
   zamčené.
3. Procházejte jednotky po cestě; při úzkém okně se cesta zalomí a scrolluje.
4. Otevřete **"Neue Freunde"** (jednotka 1), **"Aus aller Welt"** (2) nebo
   **"Bei uns zu Hause"** (3) a vyberte bublinu cvičení. Jednotky 2 a 3 jdou
   otevřít i bez dokončení předchozích.
5. Na síťové cestě jsou otevřené jednotky **1–13**. Jednotka 1 (**Výpočet IP
   adres**) má 4 snímky VLSM a pak 4 interaktivní úlohy s kontrolou /
   „ukázat řešení“. Jednotky 2–13 mají snímky teorie a kvíz s výběrem
   (základní pojmy, klient–server/cloud, zabezpečení přenosu, signály, cesta/
   kanál/okruh, multiplex, přepojování, vrstevnaté modely, ISO/OSI a TCP/IP,
   rozbor 7 vrstev ISO/OSI, architektura TCP/IP, topologie sítě). Jednotky
   14–30 zůstávají zamčené.
6. Na cestě technického vybavení jsou otevřené jednotky **1–3** (architektura /
   von Neumann, historie generací, bit a byte), každá se snímky a kvízem.
   Jednotky 4–10 zůstávají zamčené.
7. Tlačítkem **zpět** vlevo nahoře se vrátíte na předchozí obrazovku.
8. **Statistiky** (ikona grafu vpravo nahoře) ukazují postup po předmětech
   (u Deutsch i po jednotkách). Dokončené síťové a hardwarové lekce se
   započítávají do součů předmětu.
9. **Nastavení** (ozubené kolečko) – tmavý/světlý režim, barevné téma nebo
   jazyk rozhraní.

Hotové jsou německé jednotky 1–3, síťové jednotky 1–10 a hardwarové jednotky
1–3. Zamčené uzly nic nedělají – jsou to placeholdery. Dokončená německá
cvičení zůstanou zelená a ukládají se do `progress/unit1.conf` /
`progress/unit2.conf` / `progress/unit3.conf`. Dokončené síťové jednotky se
ukládají do `progress/net.conf`, hardwarové do `progress/hw.conf`.

#### Klávesové zkratky

| Zkratka             | Akce              |
| ------------------- | ----------------- |
| `Super/Cmd` + `Q`   | Ukončit aplikaci  |
| `Alt` + `F4`        | Ukončit aplikaci  |

### Struktura projektu

```
maturita.h              sdílené typy, makra, globální proměnné a prototypy
globals.c               sdílený globální stav (okno, stack, jednotky, téma, …)
theme.c                 palety témat, vkládání stylu a soubor nastavení
i18n.c                  české/anglické překlady a přepínání jazyka
icons.c                 vektorové ikony kreslené Cairem (fajfka, zámek, wifi, čip, …)
util.c                  pomocné funkce (normalizace odpovědí, feedback, poznámka)
progress.c              postup cvičení jednotek (načtení/uložení/dokončení)
stats.c                 stránka statistik a agregace pokroku
navigation.c            tlačítko zpět, horní lišta, úvodní stránka, zkratky
roadmap.c               německá učební cesta
subjects.c              mapa předmětů a katalog předmětů
unitmap.c               mapa cvičení jednotlivých jednotek
exercise_common.c       společné jádro cvičení + combo/skládání/výběr
exercise_u1.c           cvičení jednotky 1 („Neue Freunde“)
exercise_u2.c           cvičení jednotky 2 („Aus aller Welt“)
exercise_u3.c           cvičení jednotky 3 („Bei uns zu Hause“)
settings.c              panel nastavení (režim, téma, jazyk)
net.c                   cesta počítačových sítí (30 uzlů), jednotky 1–13
                        (1: VLSM cvičení; 2–13: kvízy s výběrem)
hw.c                    cesta technického vybavení (10 uzlů), jednotky 1–3
                        (architektura, historie, bit/byte + kvízy)
main.c                  vstupní bod aplikace a zapojení stránek
style.css               GTK styl (barvy palety se vkládají za běhu)
Makefile                build & run (Linux, macOS, MSYS2); `make bundle` na Windows
CMakeLists.txt          volitelný CMake build (Linux / macOS)
build-windows.bat       legacy MSVC build (vyžaduje Visual Studio + gvsbuild GTK)
run-windows.bat         legacy spouštění MSVC buildu s GTK DLL na PATH
README.md               tento soubor
LICENSE                 licence GPL-3.0
assets/                 screenshoty do README (úvod, předměty, německá cesta)
progress/               vzniká za běhu
  unit1.conf            stav cvičení jednotky 1
  unit2.conf            stav cvičení jednotky 2
  unit3.conf            stav cvičení jednotky 3
  net.conf              dokončení síťových lekcí
  hw.conf               dokončení hardwarových lekcí
  settings.conf         téma, režim a jazyk
```

### Témata

Aplikace má deset palet – Catppuccin, Nord, Dracula, Rose Pine, Ocean, Gruvbox,
Solarized, Everforest, Monokai a One Dark – každou ve světlém i tmavém režimu.
Barvy jsou v `theme.c`, vkládají se jako `@define-color` a spojují se s
pravidly v externím souboru `style.css` (a Cairo barvy pro koleje a ikony na
mapě).

Vzhled změníte v nastavení, nebo upravte tabulky palet v `theme.c` / pravidla
v `style.css` a znovu zkompilujte.

### Jazyky

Rozhraní přepnete mezi češtinou a angličtinou v nastavení; změna platí hned a
zapamatuje se. Překlady jsou ve dvou tabulkách v `i18n.c`: `tr_ui` drží
texty rozhraní pod krátkými klíči, `tr_content` překládá české významy u
německých cvičení (klíčem je samotný český text). Widgety se registrují přes
`i18n_bind()`, aby je `apply_language()` přeložila na místě.

Německá zadání, názvy jednotek a cvičení zůstávají německá v obou jazycích
rozhraní.

### Plány do budoucna

- [ ] Napojit zbývající jednotky na skutečná cvičení slovní zásoby / gramatiky
  - [x] Jednotka 1 („Neue Freunde“) – 13 interaktivních cvičení s ukládáním
  - [x] Jednotka 2 („Aus aller Welt“) – 19 cvičení (včetně Hangmana)
  - [x] Jednotka 3 („Bei uns zu Hause“) – 15 cvičení
- [x] Počítačové sítě: jednotky 1–10 (IP/VLSM + 9 teoretických s kvízy)
- [x] Ukládání postupu / statistiky pro síťovou cestu
- [x] Technické vybavení: jednotky 1–3 (architektura, historie, bit/byte + kvízy)
- [ ] Postupné odemykání zbývajících jednotek
- [ ] Audio / výslovnost
- [x] Panel nastavení (téma, režim, jazyk)
- [ ] Spaced repetition pro slovní zásobu
- [ ] Poslech a čtení s porozuměním
- [ ] Psaní s feedbackem
- [x] Statistiky postupu
- [x] Přesunout CSS do externího souboru
- [x] Rozdělit UI kód do více souborů
- [x] Internacionalizace (čeština a angličtina)
- [ ] Další jazyky rozhraní
- [ ] Unit testy

### Licence

Projekt je pod licencí GPL-3.0 – detaily viz soubor LICENSE.

### Poděkování

- Vzniklo jako cvičení v C a GTK
- Inspirováno jazykovými učebními aplikacemi
- Postaveno na GTK 4
