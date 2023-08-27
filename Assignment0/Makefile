# Compiler to use
CC = gcc

# Compilation flags
CFLAGS = -Wall -Werror

# Linker flags
LDFLAGS = -lm

# Name of the executable
TARGET = imgproc

# Default rule
all: $(TARGET)

# Rule to build the executable
$(TARGET): imgproc.c
	$(CC) $(CFLAGS) imgproc.c -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

# Clean rule
clean:
	rm -f $(TARGET)
