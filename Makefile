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
UNAME_S :=
else
TARGET = maturita
RC_OBJ =
UNAME_S := $(shell uname -s 2>/dev/null)
# macOS Dock icon is set via AppKit (see macos_dock.c).
ifeq ($(UNAME_S),Darwin)
LIBS += -framework AppKit -framework Foundation
endif
endif

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

ICON_SRC = assets/app-icon.png
# Always use the real 512×512 asset — copying app-icon.png (1024×1024) breaks
# linuxdeploy, which rejects non-standard icon resolutions.
ICON_DST = share/icons/hicolor/512x512/apps/maturita.png
ICON_RUNTIME = assets/icons/hicolor/512x512/apps/maturita.png
ICON_ICO = assets/app-icon.ico

all: $(TARGET)

$(ICON_RUNTIME): $(ICON_DST)
	mkdir -p $(dir $@)
	cp "$(ICON_DST)" "$@"

ifeq ($(OS),Windows_NT)
$(RC_OBJ): maturita.rc $(ICON_ICO)
	$(WINDRES) maturita.rc -o $(RC_OBJ)

$(TARGET): $(SRC) $(ICON_RUNTIME) $(RC_OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(RC_OBJ) $(LIBS)
else
$(TARGET): $(SRC) $(ICON_RUNTIME)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)
endif

clean:
	rm -f $(TARGET) maturita.exe $(OBJ) $(RC_OBJ) maturita_rc.o

run: $(TARGET)
	./$(TARGET)

# Self-contained package for the current platform into dist/.
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
else ifeq ($(UNAME_S),Darwin)
bundle:
	@chmod +x scripts/bundle-macos.sh
	@./scripts/bundle-macos.sh
else
bundle:
	@chmod +x scripts/bundle-linux.sh
	@./scripts/bundle-linux.sh
endif

.PHONY: all clean run bundle
