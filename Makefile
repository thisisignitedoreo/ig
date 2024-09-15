# Variables
TARGET = demo
SRC = src/demo.c
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Default target
all: linux windows

# Compile the target
linux: $(SRC)
	$(CC) $(CFLAGS) -I./ext/raylib/include -L./ext/raylib/lib -o $(TARGET) $^ -l:libraylib.a -lm -ldl

windows: $(SRC)
	x86_64-w64-mingw32-$(CC) -mwindows $(CFLAGS) -I./ext/raylib-win/include -L./ext/raylib-win/lib -o $(TARGET).exe $^ -l:libraylib.a -lwinmm -lgdi32

windows-console: $(SRC)
	x86_64-w64-mingw32-$(CC) $(CFLAGS) -I./ext/raylib-win/include -L./ext/raylib-win/lib -o $(TARGET).exe $^ -l:libraylib.a -lwinmm -lgdi32 -ggdb

# Clean up build artifacts
clean:
	rm -f $(TARGET) $(TARGET).exe

.PHONY: all clean linux windows windows-console

