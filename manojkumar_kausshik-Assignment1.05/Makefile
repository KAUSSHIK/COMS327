CC = gcc
CFLAGS = -Wall -Werror  -lncurses
TARGET = poke327
SOURCE = poke327.c heap.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(SOURCE) -o $(TARGET) $(CFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)