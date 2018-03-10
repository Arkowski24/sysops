CC=gcc
CFLAGS=-c -Wall -std=c99

all: blockarray_static.a libblockarray.so.1

libblockarray.so.1: blockarray_static_alloc_pic.o blockarray_dynamic_alloc_pic.o
	$(CC) $(CFLAGS) -shared -W1,-soname,libblockarray.so.1 -o  libblockarray.so.1.0.1 blockarray_static_alloc_pic.o blockarray_dynamic_alloc_pic.o -lc

blockarray_static.a: blockarray_static_alloc.o blockarray_dynamic_alloc.o
	ar rcs blockarray_static.a blockarray_static_alloc.o blockarray_dynamic_alloc.o

blockarray_static_alloc_pic.o: blockarray-static-alloc.c
	$(CC) $(CFLAGS) -fPIC blockarray-static-alloc.c

blockarray_dynamic_alloc_pic.o: blockarray-dynamic-alloc.c
	$(CC) $(CFLAGS) -fPIC blockarray-dynamic-alloc.c

blockarray_static_alloc.o: blockarray-static-alloc.c
	$(CC) $(CFLAGS) blockarray-static-alloc.c

blockarray_dynamic_alloc.o: blockarray-dynamic-alloc.c
	$(CC) $(CFLAGS) blockarray-dynamic-alloc.c

clean:
	rm *.o blockarray_static