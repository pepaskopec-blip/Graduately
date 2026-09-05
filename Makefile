CC = gcc
CFLAGS = -Wall -Wextra -Wno-deprecated-declarations `pkg-config --cflags gtk4`
LIBS = `pkg-config --libs gtk4` -lm
TARGET = sprechen
SRC = sprechen.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run