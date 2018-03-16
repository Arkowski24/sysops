CC=gcc
CFLAGS=-c -Wall -std=c99
SHRLIB=-Wall -std=c99 -fPIC -shared

all: blockarray.a libblockarray.so.1.0.1

libblockarray.so.1.0.1: blockarray-static-alloc-pic.o blockarray-dynamic-alloc-pic.o
	$(CC) $(SHRLIB) -Wl,-soname,libblockarray.so.1 -o libblockarray.so.1.0.1 blockarray-static-alloc-pic.o blockarray-dynamic-alloc-pic.o -lc

blockarray.a: blockarray-static-alloc.o blockarray-dynamic-alloc.o
	ar rcs blockarray.a blockarray-static-alloc.o blockarray-dynamic-alloc.o

blockarray-static-alloc-pic.o: blockarray-static-alloc.c
	$(CC) $(CFLAGS) -fPIC blockarray-static-alloc.c -o blockarray-static-alloc-pic.o

blockarray-dynamic-alloc-pic.o: blockarray-dynamic-alloc.c
	$(CC) $(CFLAGS) -fPIC blockarray-dynamic-alloc.c -o blockarray-dynamic-alloc-pic.o

blockarray-static-alloc.o: blockarray-static-alloc.c
	$(CC) $(CFLAGS) blockarray-static-alloc.c

blockarray-dynamic-alloc.o: blockarray-dynamic-alloc.c
	$(CC) $(CFLAGS) blockarray-dynamic-alloc.c

clean:
	rm *.o *.a libblockarray.so.1.0.1