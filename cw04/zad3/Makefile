CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O2

all: comm_tester

comm_tester: comm_tester.o comm_tester_child.o
	$(CC) comm_tester.o comm_tester_child.o -o comm_tester

comm_tester.o: comm_tester.c comm_tester.h
	$(CC) $(CFLAGS) comm_tester.c -o comm_tester.o

comm_tester_child.o: comm_tester_child.c comm_tester.h
	$(CC) $(CFLAGS) comm_tester_child.c -o comm_tester_child.o


.PHONY : clean
clean:
	rm *.o signal_tester
