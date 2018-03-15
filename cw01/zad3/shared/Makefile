CC=gcc
CFLAGS=-c -Wall -std=c99 -fPIC
SHRLIB=-Wall -std=c99 -fPIC -shared

all: arrtest

arrtest: arrtest.o arrtestdyn.o arrteststa.o arrtestutil.o libblockarray.so.1.0.1
	$(CC) arrtest.o arrtestdyn.o arrteststa.o arrtestutil.o libblockarray.so.1.0.1 -o arrtest
	ln -s libblockarray.so.1.0.1 libblockarray.so.1
	ln -s libblockarray.so.1 libblockarray.so

arrtest.o: ../../zad2/arrtest.c
	$(CC) $(CFLAGS) ../../zad2/arrtest.c

arrtestdyn.o: ../../zad2/arrtestdyn.c
	$(CC) $(CFLAGS) ../../zad2/arrtestdyn.c

arrteststa.o: ../../zad2/arrteststa.c
	$(CC) $(CFLAGS) ../../zad2/arrteststa.c

arrtestutil.o: ../../zad2/arrtestutil.c
	$(CC) $(CFLAGS) ../../zad2/arrtestutil.c

libblockarray.so.1.0.1: blockarray-static-alloc.o blockarray-dynamic-alloc.o
	$(CC) $(SHRLIB) -Wl,-soname,libblockarray.so.1 -o libblockarray.so.1.0.1 blockarray-static-alloc.o blockarray-dynamic-alloc.o -lc

blockarray-static-alloc.o: ../../zad1/blockarray-static-alloc.c
	$(CC) $(CFLAGS) ../../zad1/blockarray-static-alloc.c

blockarray-dynamic-alloc.o: ../../zad1/blockarray-dynamic-alloc.c
	$(CC) $(CFLAGS) ../../zad1/blockarray-dynamic-alloc.c

.PHONY : clean
clean:
	rm *.o arrtest *.a libblockarray.so.1.0.1