CC=gcc
CFLAGS=-c -Wall -std=c99 -O2

all: dir_explorer

dir_explorer: dir_explorer.o
	$(CC) dir_explorer.o -o dir_explorer

dir_explorer.o: dir_explorer.c dir_explorer.h
	$(CC) $(CFLAGS) dir_explorer.c -o dir_explorer.o

.PHONY : clean
clean:
	rm *.o dir_explorer
