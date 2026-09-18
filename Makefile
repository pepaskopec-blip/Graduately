CC = gcc

# Make sure pkg-config is found even when make runs with a minimal PATH
# (e.g. launched from an IDE such as CLion on macOS).
PKG_CONFIG ?= $(firstword $(wildcard \
	$(addsuffix /pkg-config,$(subst :, ,$(PATH)) /opt/homebrew/bin /usr/local/bin)))
ifeq ($(PKG_CONFIG),)
PKG_CONFIG := pkg-config
endif

GTK_CFLAGS := $(shell $(PKG_CONFIG) --cflags gtk4)
GTK_LIBS   := $(shell $(PKG_CONFIG) --libs gtk4)

SRC_DIR   := src
DATA_DIR  := data
BUILD_DIR := build

CFLAGS = -Wall -Wextra -Wno-deprecated-declarations -I$(SRC_DIR) \
	-MMD -MP $(GTK_CFLAGS)
LIBS = $(GTK_LIBS) -lm

# Published builds pass the commit they were made from (the CI workflow
# exports it), which the updater compares against the build in the repository.
# Without it the build keeps the "dev" default and never offers an update.
COMMIT ?=
ifneq ($(COMMIT),)
CFLAGS += -DAPP_COMMIT='"$(COMMIT)"'
endif

ifeq ($(OS),Windows_NT)
TARGET = maturita.exe
LIBS += -mwindows
WINDRES ?= windres
RC_SRC = $(DATA_DIR)/windows/maturita.rc
RC_OBJ = $(BUILD_DIR)/maturita_rc.o
UNAME_S :=
else
TARGET = maturita
RC_SRC =
RC_OBJ =
UNAME_S := $(shell uname -s 2>/dev/null)
ifeq ($(UNAME_S),Darwin)
LIBS += -framework AppKit -framework Foundation
endif
endif

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
DEP = $(OBJ:.o=.d)

ICON_DST = $(DATA_DIR)/share/icons/hicolor/512x512/apps/maturita.png
ICON_ICO = assets/app-icon.ico

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

ifeq ($(OS),Windows_NT)
$(RC_OBJ): $(RC_SRC) $(ICON_ICO) | $(BUILD_DIR)
	$(WINDRES) -I. -o $@ $<

$(TARGET): $(OBJ) $(RC_OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(RC_OBJ) $(LIBS)
else
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)
endif

clean:
	rm -f $(TARGET) maturita.exe
	rm -rf $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

ifeq ($(OS),Windows_NT)
bundle: $(TARGET)
	@rm -rf dist
	@mkdir -p dist
	@cp $(TARGET) dist/
	@cp $(DATA_DIR)/style.css dist/
	@mkdir -p dist/icons/hicolor/512x512/apps
	@cp "$(ICON_DST)" dist/icons/hicolor/512x512/apps/
	@cp -R $(DATA_DIR)/share dist/
	@ldd $(TARGET) | grep -Ei '/(ucrt64|mingw64)/bin/' | awk '{print $$3}' \
		| xargs -r -I{} cp -f {} dist/
	@GTK_PREFIX="$$($(PKG_CONFIG) --variable=prefix gtk4)"; \
	  if [ -d "$$GTK_PREFIX/share/glib-2.0" ]; then \
	    mkdir -p dist/share && cp -R "$$GTK_PREFIX/share/glib-2.0" dist/share/; \
	  fi; \
	  if [ -d "$$GTK_PREFIX/lib/gdk-pixbuf-2.0" ]; then \
	    mkdir -p dist/lib && cp -R "$$GTK_PREFIX/lib/gdk-pixbuf-2.0" dist/lib/; \
	  fi
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

# Browsers render .command/.sh/.cmd as text/plain. Rebuild these zips after
# changing a script so the README download links stay in sync. The macOS zip
# is a double-clickable .app — no Terminal, no chmod.
installer-zips:
	chmod +x installers/pack-installer-zips.sh
	./installers/pack-installer-zips.sh

-include $(DEP)

android:
	python3 scripts/extract-android-content.py
	cd android && ./gradlew :app:assembleRelease

.PHONY: all clean run bundle installer-zips android
