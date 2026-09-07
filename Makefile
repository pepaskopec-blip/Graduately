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
TARGET = maturita
SRC = maturita.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
