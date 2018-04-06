CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O2

all: date_writer

date_writer: date_writer.o
	$(CC) date_writer.o -o date_writer

date_writer.o: date_writer.c date_writer.h
	$(CC) $(CFLAGS) date_writer.c -o date_writer.o

.PHONY : clean
clean:
	rm *.o date_writer
