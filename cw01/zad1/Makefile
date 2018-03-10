CC=gcc
CFLAGS=-c -Wall -std=c99

all: blockarray

blockarray: blockarray_static.o blockarray_dynamic.o
	ar rcs blockarray_library.a blockarray_static.o blockarray_dynamic.o

blockarray_static.o: blockarray-static-alloc.c
	$(CC) $(CFLAGS) blockarray-static-alloc.c

blockarray_dynamic.o: blockarray-dynamic-alloc.c
	$(CC) $(CFLAGS) blockarray-dynamic-alloc.c

clean:
	rm *o blockarray