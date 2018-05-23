CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O2

all: filter_prog

filter_prog: filter_prog.o filter_prog_io.o
	$(CC) filter_prog.o filter_prog_io.o -o filter_prog -pthread

filter_prog_io.o: filter_prog_io.c filter_prog.h
	$(CC) $(CFLAGS) filter_prog_io.c -o filter_prog_io.o

filter_prog.o: filter_prog.c filter_prog.h
	$(CC) $(CFLAGS) filter_prog.c -o filter_prog.o

.PHONY: clean
clean:
	rm *.o filter_prog
