CC=gcc
CFLAGS=-c -Wall -std=c99
SHRLIB=-Wall -std=c99 -fPIC -shared

all: arrtest

arrtest: arrtest.o arrtestdyn.o arrteststa.o arrtestutil.o blockarray.a
	$(CC) arrtest.o arrtestdyn.o arrteststa.o arrtestutil.o blockarray.a -o arrtest

arrtest.o: ../../zad2/arrtest.c
	$(CC) $(CFLAGS) ../../zad2/arrtest.c

arrtestdyn.o: ../../zad2/arrtestdyn.c
	$(CC) $(CFLAGS) ../../zad2/arrtestdyn.c

arrteststa.o: ../../zad2/arrteststa.c
	$(CC) $(CFLAGS) ../../zad2/arrteststa.c

arrtestutil.o: ../../zad2/arrtestutil.c
	$(CC) $(CFLAGS) ../../zad2/arrtestutil.c

blockarray.a: blockarray-static-alloc.o blockarray-dynamic-alloc.o
	ar rcs blockarray.a blockarray-static-alloc.o blockarray-dynamic-alloc.o

blockarray-static-alloc.o: ../../zad1/blockarray-static-alloc.c
	$(CC) $(CFLAGS) ../../zad1/blockarray-static-alloc.c

blockarray-dynamic-alloc.o: ../../zad1/blockarray-dynamic-alloc.c
	$(CC) $(CFLAGS) ../../zad1/blockarray-dynamic-alloc.c

.PHONY : clean
clean:
	rm *.o arrtest *.a