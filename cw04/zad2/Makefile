CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O2

all: signal_tester

signal_tester: signal_tester.o signal_tester_child.o
	$(CC) signal_tester.o signal_tester_child.o -o signal_tester

signal_tester.o: signal_tester.h signal_tester.c
	$(CC) $(CFLAGS) signal_tester.c -o signal_tester.o


signal_tester_child.o: signal_tester.h signal_tester_child.c
	$(CC) $(CFLAGS) signal_tester_child.c -o signal_tester_child.o

.PHONY : clean
clean:
	rm *.o signal_tester
