CC = gcc

# Make sure pkg-config is found even when make runs with a minimal PATH
# (e.g. launched from an IDE such as CLion on macOS).
PKG_CONFIG ?= $(firstword $(wildcard \
	$(addsuffix /pkg-config,$(subst :, ,$(PATH)) /opt/homebrew/bin /usr/local/bin)))
ifeq ($(PKG_CONFIG),)
PKG_CONFIG := pkg-config
endif

# Use $(shell ...) instead of backticks so the flags are expanded by make
# itself; IDEs that import the Makefile via a dry run then see the real
# include paths.
GTK_CFLAGS := $(shell $(PKG_CONFIG) --cflags gtk4)
GTK_LIBS   := $(shell $(PKG_CONFIG) --libs gtk4)

CFLAGS = -Wall -Wextra -Wno-deprecated-declarations $(GTK_CFLAGS)
LIBS = $(GTK_LIBS) -lm

# Windows (MSYS2 / MinGW): .exe suffix and no console window for the GUI.
ifeq ($(OS),Windows_NT)
TARGET = maturita.exe
LIBS += -mwindows
else
TARGET = maturita
endif

SRC = maturita.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

clean:
	rm -f $(TARGET) maturita.exe

run: $(TARGET)
	./$(TARGET)

# Windows only: copy the executable and every non-system (MinGW/GTK) DLL it
# needs into dist/ so the app can be started by double-clicking, without an
# MSYS2 shell on PATH.
ifeq ($(OS),Windows_NT)
bundle: $(TARGET)
	@rm -rf dist
	@mkdir -p dist
	@cp $(TARGET) dist/
	@ldd $(TARGET) | grep -Ei '/(ucrt64|mingw64)/bin/' | awk '{print $$3}' \
		| xargs -r -I{} cp -f {} dist/
	@echo "Bundled into dist/ - run dist/$(TARGET)"
else
bundle:
	@echo "make bundle is only needed on Windows (MSYS2/MinGW)."
endif

.PHONY: all clean run bundle
