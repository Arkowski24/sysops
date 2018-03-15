CC=gcc
CFLAGS=-c -Wall -std=c99 -fPIC
SHRLIB=-Wall -std=c99 -fPIC -shared
TASK1=../../../zad1
DYNA=../../a/dynamic

OPT1=-o0
OPT2=-os
OPT3=-o3

all: arrtest1 arrtest2 arrtest3
	./arrtest1 s 10000 10000 init fnd 1000 rmIn 10000 armIn 10000 > static_report.txt
	./arrtest2 s 10000 10000 init fnd 1000 rmIn 10000 armIn 10000 >> static_report.txt
	./arrtest3 s 10000 10000 init fnd 1000 rmIn 10000 armIn 10000 >> static_report.txt
	./arrtest1 d 10000 10000 init fnd 1000 rmIn 10000 armIn 10000 > dynamic_report.txt
	./arrtest2 d 10000 10000 init fnd 1000 rmIn 10000 armIn 10000 >> dynamic_report.txt
	./arrtest3 d 10000 10000 init fnd 1000 rmIn 10000 armIn 10000 >> dynamic_report.txt

arrtest1: arrtest1.o arrtestdyn1.o arrteststa1.o arrtestutil1.o libblockarray1.so.1.0.1
	$(CC) arrtest1.o arrtestdyn1.o arrteststa1.o arrtestutil1.o -Wl,-rpath=$(PWD) -o arrtest1 -ldl

arrtest1.o: src/arrtest1.c
	$(CC) $(CFLAGS) $(OPT1) src/arrtest1.c

arrtestdyn1.o: $(DYNA)/src/arrtestdyn.c
	$(CC) $(CFLAGS) $(OPT1) $(DYNA)/src/arrtestdyn.c -o arrtestdyn1.o

arrteststa1.o: $(DYNA)/src/arrteststa.c
	$(CC) $(CFLAGS) $(OPT1) $(DYNA)/src/arrteststa.c -o arrteststa1.o

arrtestutil1.o: $(DYNA)/src/arrtestutil.c
	$(CC) $(CFLAGS) $(OPT1) $(DYNA)/src/arrtestutil.c -o arrtestutil1.o

libblockarray1.so.1.0.1: blockarray-static-alloc1.o blockarray-dynamic-alloc1.o
	$(CC) $(SHRLIB) -Wl,-soname,libblockarray1.so.1 -o libblockarray1.so.1.0.1 blockarray-static-alloc1.o blockarray-dynamic-alloc1.o -lc
	ln -s libblockarray1.so.1.0.1 libblockarray1.so.1
	ln -s libblockarray1.so.1 libblockarray1.so

blockarray-static-alloc1.o: $(TASK1)/blockarray-static-alloc.c
	$(CC) $(CFLAGS) $(OPT1) $(TASK1)/blockarray-static-alloc.c -o blockarray-static-alloc1.o

blockarray-dynamic-alloc1.o: $(TASK1)/blockarray-dynamic-alloc.c
	$(CC) $(CFLAGS) $(OPT1) $(TASK1)/blockarray-dynamic-alloc.c -o blockarray-dynamic-alloc1.o
#----------------------------------

arrtest2: arrtest2.o arrtestdyn2.o arrteststa2.o arrtestutil2.o libblockarray2.so.1.0.1
	$(CC) arrtest2.o arrtestdyn2.o arrteststa2.o arrtestutil2.o -Wl,-rpath=$(PWD) -o arrtest2 -ldl

arrtest2.o: src/arrtest2.c
	$(CC) $(CFLAGS) $(OPT2) src/arrtest2.c

arrtestdyn2.o: $(DYNA)/src/arrtestdyn.c
	$(CC) $(CFLAGS) $(OPT2) $(DYNA)/src/arrtestdyn.c -o arrtestdyn2.o

arrteststa2.o: $(DYNA)/src/arrteststa.c
	$(CC) $(CFLAGS) $(OPT2) $(DYNA)/src/arrteststa.c -o arrteststa2.o

arrtestutil2.o: $(DYNA)/src/arrtestutil.c
	$(CC) $(CFLAGS) $(OPT2) $(DYNA)/src/arrtestutil.c -o arrtestutil2.o

libblockarray2.so.1.0.1: blockarray-static-alloc2.o blockarray-dynamic-alloc2.o
	$(CC) $(SHRLIB) -Wl,-soname,libblockarray2.so.1 -o libblockarray2.so.1.0.1 blockarray-static-alloc2.o blockarray-dynamic-alloc2.o -lc
	ln -s libblockarray2.so.1.0.1 libblockarray2.so.1
	ln -s libblockarray2.so.1 libblockarray2.so

blockarray-static-alloc2.o: $(TASK1)/blockarray-static-alloc.c
	$(CC) $(CFLAGS) $(OPT2) $(TASK1)/blockarray-static-alloc.c -o blockarray-static-alloc2.o

blockarray-dynamic-alloc2.o: $(TASK1)/blockarray-dynamic-alloc.c
	$(CC) $(CFLAGS) $(OPT2) $(TASK1)/blockarray-dynamic-alloc.c -o blockarray-dynamic-alloc2.o
#--------------------------------------------------------

arrtest3: arrtest3.o arrtestdyn3.o arrteststa3.o arrtestutil3.o libblockarray3.so.1.0.1
	$(CC) arrtest3.o arrtestdyn3.o arrteststa3.o arrtestutil3.o -Wl,-rpath=$(PWD) -o arrtest3 -ldl

arrtest3.o: src/arrtest3.c
	$(CC) $(CFLAGS) $(OPT3) src/arrtest3.c

arrtestdyn3.o: $(DYNA)/src/arrtestdyn.c
	$(CC) $(CFLAGS) $(OPT3) $(DYNA)/src/arrtestdyn.c -o arrtestdyn3.o

arrteststa3.o: $(DYNA)/src/arrteststa.c
	$(CC) $(CFLAGS) $(OPT3) $(DYNA)/src/arrteststa.c -o arrteststa3.o

arrtestutil3.o: $(DYNA)/src/arrtestutil.c
	$(CC) $(CFLAGS) $(OPT3) $(DYNA)/src/arrtestutil.c -o arrtestutil3.o

libblockarray3.so.1.0.1: blockarray-static-alloc3.o blockarray-dynamic-alloc3.o
	$(CC) $(SHRLIB) -Wl,-soname,libblockarray3.so.1 -o libblockarray3.so.1.0.1 blockarray-static-alloc3.o blockarray-dynamic-alloc3.o -lc
	ln -s libblockarray3.so.1.0.1 libblockarray3.so.1
	ln -s libblockarray3.so.1 libblockarray3.so

blockarray-static-alloc3.o: $(TASK1)/blockarray-static-alloc.c
	$(CC) $(CFLAGS) $(OPT3) $(TASK1)/blockarray-static-alloc.c -o blockarray-static-alloc3.o

blockarray-dynamic-alloc3.o: $(TASK1)/blockarray-dynamic-alloc.c
	$(CC) $(CFLAGS) $(OPT3) $(TASK1)/blockarray-dynamic-alloc.c -o blockarray-dynamic-alloc3.o

.PHONY : clean
clean:
	rm *.o libblockarray1.* libblockarray2.* libblockarray3.* arrtest1 arrtest2 arrtest3
