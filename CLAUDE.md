# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Development Commands

- **Build**: Compile the GTK application using the Makefile:
  ```sh
  make
  ```
  Or manually with gcc:
  ```sh
  gcc -o sprechen sprechen.c `pkg-config --cflags --libs gtk4`
  ```

- **Run**: Execute the built binary:
  ```sh
  make run
  ```
  or
  ```sh
  ./sprechen
  ```

- **Clean**: Remove compiled binaries:
  ```sh
  make clean
  ```
  or
  ```sh
  rm -f sprechen
  ```

- **Format**: If using clang-format, run:
  ```sh
  clang-format -i *.c *.h
  ```

- **Lint**: Use `clang-tidy` or `cppcheck` for static analysis:
  ```sh
  clang-tidy sprechen.c -- `pkg-config --cflags gtk4`
  ```
  or
  ```sh
  cppcheck --enable=all sprechen.c
  ```

- **Test**: Currently no test framework is integrated. To add unit tests, consider using a lightweight framework like `check` or `cmocka`. Tests can be compiled and run similarly.

## Project Structure

- **Root**: Contains the CLAUDE.md, README.md, .gitignore, LICENSE, Makefile, and source files.
- **Source**: The main application resides in a single C file (`sprechen.c`) that uses GTK 4 for the graphical interface.
- **Build Artifacts**: Compiled binaries are placed in the project root by default; consider using a `build/` directory for out-of-source builds.
- **Dependencies**: Requires GTK 4 development libraries. On macOS, install via Homebrew: `brew install gtk4`. On Linux, use your package manager (e.g., `sudo apt install libgtk-4-dev`).

## Architecture Overview

The application is a simple GUI program written in C using GTK 4. It features:
- A main window with a title "Sprechen.c".
- A vertical box layout containing a centered label with welcome text.
- Custom CSS styling for a gradient background and styled label box.
- The GTK application lifecycle is managed via `GtkApplication`, with activation callback setting up the UI.

Future extensions may include:
- Additional widgets for language lessons (buttons, text entry, progress bars).
- Separation of UI code into multiple files (e.g., `ui.c`, `ui.h`).
- Lesson data structures and loading from external files (JSON or plain text).
- Internationalization support using gettext.

## Notes

- Ensure GTK 4 is installed and accessible via pkg-config.
- The code adheres to GTK 4 conventions and uses CSS for styling.
- When modifying the UI, consider keeping the CSS in a separate file for maintainability, though currently it is embedded for simplicity.
