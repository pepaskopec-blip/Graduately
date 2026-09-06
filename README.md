# Sprechen.c

A German learning application written in C using GTK 4.

## Description

Sprechen.c is an educational program designed for practicing German language
skills, aimed at high school and gymnasium students. The UI is available in
Czech and English and is built as a single C file (`sprechen.c`) that renders a
modern, CSS-styled GTK 4 interface.

## Features

- **Welcome screen** – branded greeting with a description of the program and a
  continue button to get started
- **Learning roadmap** – adaptive serpentine learning path with 10 units
  - each unit numbered and labeled with its topic (e.g. "Neue Freunde")
  - current unit highlighted with an accent gradient and glow
  - completed units shown in green with a checkmark
  - locked units dimmed with a lock icon
  - finish node at the end of the path
  - layout folds into multiple rows when the window is narrow
- **Back navigation** – themed circular back button (top-left) returns to the
  previous screen
- **Unit 1 exercises** – "Neue Freunde" opens an interactive bubble map with 13
  exercises (dialogs, sentence building, multiple choice, free answers,
  numbers, verb conjugation, greeting sorting and more), each with German
  prompts and a check button that reveals the translations
- **Progress tracking** – completed exercises are marked green; progress is
  saved to `progress/unit1.conf` and restored on startup
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
gcc -o sprechen sprechen.c `pkg-config --cflags --libs gtk4` -lm
```

### Running the Application

```bash
make run
```

or

```bash
./sprechen
```

## Usage

When launched, the application shows the welcome screen. From there:

1. Click **"Pokračuj" / "Continue"** to open the learning roadmap.
2. Browse the units along the path; the roadmap reflows into rows and scrolls
   if the window is too narrow.
3. Click the **"Neue Freunde"** node to open the unit's exercise map, then pick
   any bubble to start an exercise.
4. Use the **back button** in the top-left corner to return to the previous
   screen.
5. Open **Nastavení / Settings** (gear icon, top-right) to switch dark/light
   mode, pick a color theme, or change the interface language.

Only the first unit is currently implemented. Locked units do nothing – they
are placeholders until lessons are added. Completed exercises stay green and
are saved to `progress/unit1.conf` (created next to the app on first finish).

### Keyboard shortcuts

| Shortcut            | Action           |
| ------------------- | ---------------- |
| `Super/Cmd` + `Q`   | Quit application |
| `Alt` + `F4`        | Quit application |

## Project structure

```
sprechen.c              entire application (UI, navigation, themes, exercises)
Makefile                build & run targets
README.md               this file
LICENSE                 MIT license
progress/               created at runtime
  unit1.conf            exercise completion state
  settings.conf         theme, dark/light mode and language preference
```

## Theming

The app ships with ten palettes – Catppuccin, Nord, Dracula, Rose Pine, Ocean,
Gruvbox, Solarized, Everforest, Monokai and One Dark – each available in dark
and light mode. Colors are defined as theme palettes in `sprechen.c` and
applied through a generated CSS stylesheet (plus matching Cairo colors for the
roadmap rails and icons).

Change the look from the in-app settings panel, or edit the palette tables /
`build_theme_css()` in `sprechen.c` and rebuild.

## Languages

The interface can be switched between Czech and English from the settings
panel; the choice takes effect immediately and is remembered between runs.
Translations live in two tables in `sprechen.c`: `tr_ui` holds interface
strings under short keys, while `tr_content` translates the Czech meanings
shown next to the German exercise items and uses the Czech text itself as the
key. Widgets are registered with `i18n_bind()` so `apply_language()` can
retranslate them in place.

German prompts, unit names and exercise titles are learning material and stay
German in both languages.

## To-Do Plans

Future enhancements planned for Sprechen.c:

- [ ] Wire unit nodes to actual German vocabulary/grammar exercises
  - [x] Unit 1 ("Neue Freunde") – 13 interactive exercises with progress saving
- [ ] Lesson progression and unlock system for the remaining units
- [ ] Add audio pronunciation features
- [x] Add user settings panel (theme, dark/light mode, language)
- [ ] Implement spaced repetition system for vocabulary
- [ ] Add listening and reading comprehension exercises
- [ ] Add writing practice with feedback
- [ ] Expand progress tracking with statistics
- [ ] Move CSS styling to an external file
- [ ] Split the UI code into multiple files
- [x] Add internationalization support (Czech and English)
- [ ] Add more interface languages
- [ ] Add unit testing framework

## License

This project is licensed under the MIT License - see the LICENSE file for
details.

## Acknowledgments

- Created as a learning exercise in C and GTK programming
- Inspired by language learning applications
- Built with GTK 4 for modern GUI development
