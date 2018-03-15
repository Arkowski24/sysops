CC=gcc
CFLAGS=-c -Wall -std=c99 -fPIC
SHRLIB=-Wall -std=c99 -fPIC -shared
TASK1=../../../zad1
TASK2=../../../zad2

all: arrtest
	./arrtest s 10000 10000 init rmin 5000 armin 5000 fnd 200 > static_report.txt
	./arrtest d 10000 10000 init rmin 5000 armin 5000 fnd 200 > dynamic_report.txt

arrtest: arrtest.o arrtestdyn.o arrteststa.o arrtestutil.o libblockarray.so.1.0.1
	$(CC) arrtest.o arrtestdyn.o arrteststa.o arrtestutil.o -L$(PWD) -l blockarray -Wl,-rpath=$(PWD) -o arrtest

arrtest.o: $(TASK2)/zad2/arrtest.c
	$(CC) $(CFLAGS) $(TASK2)/zad2/arrtest.c

arrtestdyn.o: $(TASK2)/zad2/arrtestdyn.c
	$(CC) $(CFLAGS) $(TASK2)/zad2/arrtestdyn.c

arrteststa.o: $(TASK2)/zad2/arrteststa.c
	$(CC) $(CFLAGS) $(TASK2)/zad2/arrteststa.c

arrtestutil.o: $(TASK2)/zad2/arrtestutil.c
	$(CC) $(CFLAGS) $(TASK2)/zad2/arrtestutil.c

libblockarray.so.1.0.1: blockarray-static-alloc.o blockarray-dynamic-alloc.o
	$(CC) $(SHRLIB) -Wl,-soname,libblockarray.so.1 -o libblockarray.so.1.0.1 blockarray-static-alloc.o blockarray-dynamic-alloc.o -lc
	ln -s libblockarray.so.1.0.1 libblockarray.so.1
	ln -s libblockarray.so.1 libblockarray.so

blockarray-static-alloc.o: $(TASK1)/zad1/blockarray-static-alloc.c
	$(CC) $(CFLAGS) $(TASK1)/zad1/blockarray-static-alloc.c

blockarray-dynamic-alloc.o: $(TASK1)/zad1/blockarray-dynamic-alloc.c
	$(CC) $(CFLAGS) $(TASK1)/zad1/blockarray-dynamic-alloc.c

.PHONY : clean
clean:
	rm *.o libblockarray.* arrtest
