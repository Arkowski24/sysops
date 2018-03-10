CC=gcc
CFLAGS=-c -Wall

all: blockarray

blockarray: blockarray.o
	ar rcs blockarray_library.a blockarray.o

blockarray.o: blockarray.c
	$(CC) $(CFLAGS) blockarray.c

clean:
	rm *o blockarray