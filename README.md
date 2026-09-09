# maturita.c

An educational application written in C using GTK 4.

## Description

maturita.c is an educational program for high-school and gymnasium students
preparing for their maturita exam. The UI is available in Czech and English and
is built as a single C file (`maturita.c`) that renders a modern, CSS-styled
GTK 4 interface.

## Screenshots

| Welcome screen                          | Subjects ("Předměty")                    | German learning roadmap                 |
| --------------------------------------- | ---------------------------------------- | --------------------------------------- |
| ![Welcome screen](assets/welcomescreen.png) | ![Subjects screen](assets/subjectsscreen.png) | ![German roadmap](assets/germanlection.png) |

## Features

- **Welcome screen** – branded greeting with a description of the program and a
  continue button to get started
- **Subjects ("Předměty")** – adaptive serpentine subject map with 13 subjects
  - only **Deutsch** is open (round bubble with the German flag); picking it
    opens the German learning path
  - the other subjects are dimmed/locked placeholders until their content is
    added
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
  prompts and a check button that reveals the translations
- **Unit 2 exercises** – "Aus aller Welt" is playable from the start (no need
  to finish unit 1 first) and offers a bubble map with 19 exercises: verb
  conjugation, prepositions `aus`/`in`, question words, nationalities and
  countries, spelling/look-and-find tasks, a matching exercise, a number/word
  game, free-answer "Steckbrief" tasks and a Hangman game with 5 professions
- **Unit 3 exercises** – "Bei uns zu Hause" covers family vocabulary,
  possessives, accusative and related grammar across 15 exercises
- **Progress tracking** – completed exercises are marked green; progress is
  saved per unit to `progress/unit1.conf`, `progress/unit2.conf` and
  `progress/unit3.conf` and restored on startup
- **Statistics** – chart icon in the header opens a progress overview with
  totals (exercises done, percent, finished units) and a per-unit breakdown
  with progress bars; numbers update as soon as you finish an exercise
- **Settings** – gear icon in the top-right header opens a panel where you can
  choose:
  - **mode**: dark or light, as a segmented control
  - **theme**: 10 palettes picked from a grid of color swatches
  - **language**: Czech or English, applied instantly without a restart
  - all three preferences are stored in `progress/settings.conf`
- **Keyboard quit shortcuts** – `Super/Cmd+Q` or `Alt+F4` closes the app
- Single-file C codebase, cross-platform (Linux and macOS supported)

## Installation

### Prerequisites

#### 1. GTK 4 development libraries

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

#### 2. Build tools (compiler, Make, pkg-config)

macOS – install all of these explicitly, they are **not** preinstalled:

```bash
# installs the GNU C compiler, Make, and pkg-config
brew install gcc make pkg-config
# if you prefer Apple's Clang toolchain instead, you can run:
# xcode-select --install
```

Linux – **no need to install gcc, it is already preinstalled** on most
distributions (it ships with `build-essential`/`base-devel`):

```bash
# Debian / Ubuntu: make sure you have the base toolchain and pkg-config
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

### Building the Application

1. Clone or download this repository
2. Navigate to the project directory
3. Build using the provided Makefile:

```bash
make
```

Or manually:

```bash
gcc -o maturita maturita.c `pkg-config --cflags --libs gtk4` -lm
```

### Running the Application

```bash
make run
```

or

```bash
./maturita
```

## Usage

When launched, the application shows the welcome screen. From there:

1. Click **"Pokračuj" / "Continue"** to open the **Předměty / Subjects** map.
2. Click the **Deutsch** bubble (German flag) to open the German learning
   roadmap. The other subjects are locked for now.
3. Browse the units along the path; the roadmap reflows into rows and scrolls
   if the window is too narrow.
4. Click the **"Neue Freunde"** node to open unit 1 or **"Aus aller Welt"** to
   open unit 2, then pick any bubble to start an exercise. Unit 2 is available
   even if unit 1 is not finished yet.
5. Use the **back button** in the top-left corner to return to the previous
   screen.
6. Open **Statistiky / Statistics** (chart icon, top-right) to see overall and
   per-unit progress.
7. Open **Nastavení / Settings** (gear icon, top-right) to switch dark/light
   mode, pick a color theme, or change the interface language.

Units 1–3 are currently implemented. Locked units do nothing – they are
placeholders until lessons are added. Completed exercises stay green and are
saved per unit to `progress/unit1.conf` / `progress/unit2.conf` /
`progress/unit3.conf` (created next to the app on first finish).

### Keyboard shortcuts

| Shortcut            | Action           |
| ------------------- | ---------------- |
| `Super/Cmd` + `Q`   | Quit application |
| `Alt` + `F4`        | Quit application |

## Project structure

```
maturita.c              entire application (UI, navigation, themes, exercises)
Makefile                build & run targets
README.md               this file
LICENSE                 GPL-3.0 license
progress/               created at runtime
  unit1.conf            unit 1 exercise completion state
  unit2.conf            unit 2 exercise completion state
  unit3.conf            unit 3 exercise completion state
  settings.conf         theme, dark/light mode and language preference
```

## Theming

The app ships with ten palettes – Catppuccin, Nord, Dracula, Rose Pine, Ocean,
Gruvbox, Solarized, Everforest, Monokai and One Dark – each available in dark
and light mode. Colors are defined as theme palettes in `maturita.c` and
applied through a generated CSS stylesheet (plus matching Cairo colors for the
roadmap rails and icons).

Change the look from the in-app settings panel, or edit the palette tables /
`build_theme_css()` in `maturita.c` and rebuild.

## Languages

The interface can be switched between Czech and English from the settings
panel; the choice takes effect immediately and is remembered between runs.
Translations live in two tables in `maturita.c`: `tr_ui` holds interface
strings under short keys, while `tr_content` translates the Czech meanings
shown next to the German exercise items and uses the Czech text itself as the
key. Widgets are registered with `i18n_bind()` so `apply_language()` can
retranslate them in place.

German prompts, unit names and exercise titles are learning material and stay
German in both languages.

## To-Do Plans

Future enhancements planned for maturita.c:

- [ ] Wire unit nodes to actual German vocabulary/grammar exercises
  - [x] Unit 1 ("Neue Freunde") – 13 interactive exercises with progress saving
  - [x] Unit 2 ("Aus aller Welt") – 19 interactive exercises (incl. Hangman)
  - [x] Unit 3 ("Bei uns zu Hause") – 15 interactive exercises
- [ ] Lesson progression and unlock system for the remaining units
- [ ] Add audio pronunciation features
- [x] Add user settings panel (theme, dark/light mode, language)
- [ ] Implement spaced repetition system for vocabulary
- [ ] Add listening and reading comprehension exercises
- [ ] Add writing practice with feedback
- [x] Expand progress tracking with statistics
- [ ] Move CSS styling to an external file
- [ ] Split the UI code into multiple files
- [x] Add internationalization support (Czech and English)
- [ ] Add more interface languages
- [ ] Add unit testing framework

## License

This project is licensed under the GPL3.0 License - see the LICENSE file for
details.

## Acknowledgments

- Created as a learning exercise in C and GTK programming
- Inspired by language learning applications
- Built with GTK 4 for modern GUI development
