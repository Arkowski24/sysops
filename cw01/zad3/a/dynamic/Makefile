CC=gcc
CFLAGS=-c -Wall -std=c99 -fPIC
SHRLIB=-Wall -std=c99 -fPIC -shared
TASK1=../../../zad1

all: arrtest
	./arrtest s 10000 10000 init rmin 5000 armin 5000 fnd 200 > static_report.txt
	./arrtest d 10000 10000 init rmin 5000 armin 5000 fnd 200 > dynamic_report.txt

arrtest: arrtest.o arrtestdyn.o arrteststa.o arrtestutil.o libblockarray.so.1.0.1
	$(CC) arrtest.o arrtestdyn.o arrteststa.o arrtestutil.o -Wl,-rpath=$(PWD) -o arrtest -ldl

arrtest.o: src/arrtest.c
	$(CC) $(CFLAGS) src/arrtest.c

arrtestdyn.o: src/arrtestdyn.c
	$(CC) $(CFLAGS) src/arrtestdyn.c

arrteststa.o: src/arrteststa.c
	$(CC) $(CFLAGS) src/arrteststa.c

arrtestutil.o: src/arrtestutil.c
	$(CC) $(CFLAGS) src/arrtestutil.c

libblockarray.so.1.0.1: blockarray-static-alloc.o blockarray-dynamic-alloc.o
	$(CC) $(SHRLIB) -Wl,-soname,libblockarray.so.1 -o libblockarray.so.1.0.1 blockarray-static-alloc.o blockarray-dynamic-alloc.o -lc
	ln -s libblockarray.so.1.0.1 libblockarray.so.1
	ln -s libblockarray.so.1 libblockarray.so

blockarray-static-alloc.o: $(TASK1)/blockarray-static-alloc.c
	$(CC) $(CFLAGS) $(TASK1)/blockarray-static-alloc.c

blockarray-dynamic-alloc.o: $(TASK1)/blockarray-dynamic-alloc.c
	$(CC) $(CFLAGS) $(TASK1)/blockarray-dynamic-alloc.c

.PHONY : clean
clean:
	rm *.o libblockarray.* arrtest
