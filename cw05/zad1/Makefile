CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O2

all: batch_executer

batch_executer: batch_executer.o
	$(CC) batch_executer.o -o batch_executer

batch_executer.o: batch_executer.c batch_executer.h
	$(CC) $(CFLAGS) batch_executer.c -o batch_executer.o

.PHONY : clean
clean:
	rm *.o batch_executer
