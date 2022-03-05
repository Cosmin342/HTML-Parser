CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -g
SOURCES = fcoada.c style.c format.c delete.c aux.c functii.c interpret.c main.c
EXE = tema3

build:
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXE)
clean:
	rm -f $(EXE)