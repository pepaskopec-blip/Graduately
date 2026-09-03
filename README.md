# Sprechen.c

A German learning application written in C using GTK 4.

## Description

Sprechen.c is an educational program designed for practicing German language skills, targeted at high school and gymnasium students. The application features a beautiful graphical user interface with a gradient background and displays welcome information about the program.

## Features

- Beautiful gradient background with modern styling
- Centered welcome message with information about the program
- Resizable window (standard GTK behavior)
- Written in C using GTK 4 for cross-platform compatibility
- CSS-based styling for easy customization

## Installation

### Prerequisites

To build and run Sprechen.c, you need:

1. **GTK 4 Development Libraries**
   - macOS: `brew install gtk4`
   - Linux (Debian/Ubuntu): `sudo apt install libgtk-4-dev`
   - Linux (Fedora): `sudo dnf install gtk4-devel`
   - Linux (Arch): `sudo pacman -S gtk4`

2. **Build Tools**
   - GCC compiler (or Clang)
   - Make utility
   - pkg-config (usually comes with GTK installation)

### Building the Application

1. Clone or download this repository
2. Navigate to the project directory
3. Build using the provided Makefile:
   ```bash
   make
   ```
   Or manually:
   ```bash
   gcc -o sprechen sprechen.c `pkg-config --cflags --libs gtk4`
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

Once launched, the application will display a window with:
- Title: "Sprechen.c"
- Welcome message in Czech (as currently implemented)
- Beautiful dark bluish gradient background
- Styled text box with rounded corners and subtle shadow

The window is fully resizable like any standard application window.

## To-Do Plans

Future enhancements planned for Sprechen.c:

### Short-term Goals
- [ ] Add interactive German vocabulary exercises
- [ ] Implement basic grammar practice modules
- [ ] Add audio pronunciation features
- [ ] Create lesson progression system
- [ ] Add user settings panel

### Medium-term Goals
- [ ] Implement spaced repetition system for vocabulary
- [ ] Add listening comprehension exercises
- [ ] Include reading comprehension texts
- [ ] Add writing practice with feedback
- [ ] Implement progress tracking and statistics

### Long-term Goals
- [ ] Add multiple difficulty levels (A1-C2)
- [ ] Include cultural information about German-speaking countries
- [ ] Implement offline mode with downloadable lessons
- [ ] Add teacher/classroom management features
- [ ] Implement achievement system and rewards

### Technical Improvements
- [ ] Separate UI code into multiple files (ui.c, ui.h)
- [ ] Move CSS styling to external file
- [ ] Implement lesson data loading from JSON/XML
- [ ] Add internationalization support (gettext)
- [ ] Improve accessibility features
- [ ] Add unit testing framework

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for:
- Bug fixes
- Feature suggestions
- Translation improvements
- Documentation enhancements

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Created as a learning exercise in C and GTK programming
- Inspired by language learning applications
- Built with GTK 4 for modern GUI development