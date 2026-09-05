# Sprechen.c

A German learning application written in C using GTK 4.

## Description

Sprechen.c is an educational program designed for practicing German language
skills, aimed at high school and gymnasium students. The UI is written in Czech
and is built as a single C file (`sprechen.c`) that renders a modern,
CSS-styled GTK 4 interface with a Catppuccin Mocha dark theme.

## Features

- **Welcome screen** – branded greeting with a description of the program and a
  "Pokračuj" button to get started
- **Learning roadmap** – a wave-shaped "Učební plán" path with 10 units that
  scrolls horizontally
  - each unit numbered and labeled with its topic (e.g. "Neue Freunde")
  - current unit highlighted with a mauve→blue gradient and glow
  - completed units shown in green with a checkmark
  - locked units dimmed with a lock icon
  - finish flag at the end of the path
- **Back navigation** – themed circular back button (top-left) returns to the
  welcome screen
- **Unit 1 exercises** – "Neue Freunde" opens an interactive bubble map with 13
  exercises (dialogs, sentence building, numbers, verb conjugation, greeting
  sorting and more), each with German prompts and a "Zkontrolovat" check
- **Progress tracking** – completed exercises are marked green; progress is
  saved to `progress/unit1.conf` and restored on startup
- **Catppuccin Mocha theme** – gradient background, rounded cards, glowing
  buttons and nodes, all driven by an embedded CSS stylesheet
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

1. Click **"Pokračuj"** to open the learning roadmap.
2. Browse the units along the path; the roadmap scrolls horizontally if the
   window is too narrow.
3. Click the **"Neue Freunde"** node to open the unit's exercise map, then pick
   any bubble to start an exercise.
4. Use the **back button** in the top-left corner to return to the previous
   screen.

Only the first unit is currently implemented. Locked units do nothing – they
are placeholders until lessons are added. Completed exercises stay green and
are saved to `progress/unit1.conf` (created next to the app on first finish).

### Keyboard shortcuts

| Shortcut            | Action          |
| ------------------- | --------------- |
| `Super/Cmd` + `Q`   | Quit application |
| `Alt` + `F4`        | Quit application |

## Project structure

```
sprechen.c          entire application (UI, navigation, path drawing, styling)
Makefile            build & run targets
README.md           this file
LICENSE             MIT license
```

## Theming

Colors follow the [Catppuccin Mocha](https://github.com/catppuccin/catppuccin)
palette and are applied through a CSS string embedded at the top of
`sprechen.c`. To tweak the look (colors, node glow, gradients, spacing), edit
that stylesheet and rebuild.

## To-Do Plans

Future enhancements planned for Sprechen.c:

- [ ] Wire unit nodes to actual German vocabulary/grammar exercises
  - [x] Unit 1 ("Neue Freunde") – 13 interactive exercises with progress saving
- [ ] Lesson progression and unlock system for the remaining units
- [ ] Add audio pronunciation features
- [ ] Add user settings panel
- [ ] Implement spaced repetition system for vocabulary
- [ ] Add listening and reading comprehension exercises
- [ ] Add writing practice with feedback
- [ ] Implement progress tracking and statistics
- [ ] Move CSS styling to an external file
- [ ] Split the UI code into multiple files
- [ ] Add internationalization support (gettext)
- [ ] Add unit testing framework

## License

This project is licensed under the MIT License - see the LICENSE file for
details.

## Acknowledgments

- Created as a learning exercise in C and GTK programming
- Inspired by language learning applications
- Built with GTK 4 for modern GUI development
