CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O2

all: batch_executer simple_test

test: batch_executer simple_test
	./batch_executer test/test_file 2 2

simple_test: simple_test.o
	$(CC) simple_test.o -o simple_test

simple_test.o: test/simple_test.c
	$(CC) $(CFLAGS) test/simple_test.c -o simple_test.o

batch_executer: batch_executer.o
	$(CC) batch_executer.o -o batch_executer

batch_executer.o: batch_executer.c batch_executer.h
	$(CC) $(CFLAGS) batch_executer.c -o batch_executer.o

.PHONY : clean
clean:
	rm *.o batch_executer simple_test
