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

# Windows (MSYS2 / MinGW): .exe suffix, no console window, embed .ico.
ifeq ($(OS),Windows_NT)
TARGET = maturita.exe
LIBS += -mwindows
WINDRES ?= windres
RC_OBJ = maturita_rc.o
else
TARGET = maturita
RC_OBJ =
# macOS Dock icon is set via AppKit (see macos_dock.c).
ifeq ($(shell uname -s 2>/dev/null),Darwin)
LIBS += -framework AppKit -framework Foundation
endif
endif

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

ICON_SRC = assets/app-icon.png
ICON_DST = assets/icons/hicolor/512x512/apps/maturita.png
ICON_ICO = assets/app-icon.ico

all: $(TARGET)

$(ICON_DST): $(ICON_SRC)
	mkdir -p $(dir $@)
	cp "$(ICON_SRC)" "$@"

ifeq ($(OS),Windows_NT)
$(RC_OBJ): maturita.rc $(ICON_ICO)
	$(WINDRES) maturita.rc -o $(RC_OBJ)

$(TARGET): $(SRC) $(ICON_DST) $(RC_OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(RC_OBJ) $(LIBS)
else
$(TARGET): $(SRC) $(ICON_DST)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)
endif

clean:
	rm -f $(TARGET) maturita.exe $(OBJ) $(RC_OBJ) maturita_rc.o

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
	@cp style.css dist/
	@mkdir -p dist/icons/hicolor/512x512/apps
	@cp "$(ICON_DST)" dist/icons/hicolor/512x512/apps/
	@cp -R share dist/
	@ldd $(TARGET) | grep -Ei '/(ucrt64|mingw64)/bin/' | awk '{print $$3}' \
		| xargs -r -I{} cp -f {} dist/
	@echo "Bundled into dist/ - run dist/$(TARGET)"
else
bundle:
	@echo "make bundle is only needed on Windows (MSYS2/MinGW)."
endif

.PHONY: all clean run bundle
