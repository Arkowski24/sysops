CC=gcc
CFLAGS=-c -Wall -std=c99 -fPIC
SHRLIB=-Wall -std=c99 -fPIC -shared
TASK1=../../../zad1
TASK2=../../../zad2

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
	$(CC) arrtest1.o arrtestdyn1.o arrteststa1.o arrtestutil1.o -L$(PWD) -l blockarray1 -Wl,-rpath=$(PWD) -o arrtest1

arrtest1.o: $(TASK2)/arrtest.c
	$(CC) $(CFLAGS) $(OPT1) $(TASK2)/arrtest.c -o arrtest1.o

arrtestdyn1.o: $(TASK2)/arrtestdyn.c
	$(CC) $(CFLAGS) $(OPT1) $(TASK2)/arrtestdyn.c -o arrtestdyn1.o

arrteststa1.o: $(TASK2)/arrteststa.c
	$(CC) $(CFLAGS) $(OPT1) $(TASK2)/arrteststa.c -o arrteststa1.o

arrtestutil1.o: $(TASK2)/arrtestutil.c
	$(CC) $(CFLAGS) $(OPT1) $(TASK2)/arrtestutil.c -o arrtestutil1.o

libblockarray1.so.1.0.1: blockarray-static-alloc1.o blockarray-dynamic-alloc1.o
	$(CC) $(SHRLIB) -Wl,-soname,libblockarray1.so.1 -o libblockarray1.so.1.0.1 blockarray-static-alloc1.o blockarray-dynamic-alloc1.o -lc
	ln -s libblockarray1.so.1.0.1 libblockarray1.so.1
	ln -s libblockarray1.so.1 libblockarray1.so

blockarray-static-alloc1.o: $(TASK1)/blockarray-static-alloc.c
	$(CC) $(CFLAGS) $(OPT1) $(TASK1)/blockarray-static-alloc.c -o blockarray-static-alloc1.o

blockarray-dynamic-alloc1.o: $(TASK1)/blockarray-dynamic-alloc.c
	$(CC) $(CFLAGS) $(OPT1) $(TASK1)/blockarray-dynamic-alloc.c -o blockarray-dynamic-alloc1.o
#---------------------------

arrtest2: arrtest2.o arrtestdyn2.o arrteststa2.o arrtestutil2.o libblockarray2.so.1.0.1
	$(CC) arrtest2.o arrtestdyn2.o arrteststa2.o arrtestutil2.o -L$(PWD) -l blockarray2 -Wl,-rpath=$(PWD) -o arrtest2

arrtest2.o: $(TASK2)/arrtest.c
	$(CC) $(CFLAGS) $(OPT2) $(TASK2)/arrtest.c -o arrtest2.o

arrtestdyn2.o: $(TASK2)/arrtestdyn.c
	$(CC) $(CFLAGS) $(OPT2) $(TASK2)/arrtestdyn.c -o arrtestdyn2.o

arrteststa2.o: $(TASK2)/arrteststa.c
	$(CC) $(CFLAGS) $(OPT2) $(TASK2)/arrteststa.c -o arrteststa2.o

arrtestutil2.o: $(TASK2)/arrtestutil.c
	$(CC) $(CFLAGS) $(OPT2) $(TASK2)/arrtestutil.c -o arrtestutil2.o

libblockarray2.so.1.0.1: blockarray-static-alloc2.o blockarray-dynamic-alloc2.o
	$(CC) $(SHRLIB) -Wl,-soname,libblockarray2.so.1 -o libblockarray2.so.1.0.1 blockarray-static-alloc2.o blockarray-dynamic-alloc2.o -lc
	ln -s libblockarray2.so.1.0.1 libblockarray2.so.1
	ln -s libblockarray2.so.1 libblockarray2.so

blockarray-static-alloc2.o: $(TASK1)/blockarray-static-alloc.c
	$(CC) $(CFLAGS) $(OPT2) $(TASK1)/blockarray-static-alloc.c -o blockarray-static-alloc2.o

blockarray-dynamic-alloc2.o: $(TASK1)/blockarray-dynamic-alloc.c
	$(CC) $(CFLAGS) $(OPT2) $(TASK1)/blockarray-dynamic-alloc.c -o blockarray-dynamic-alloc2.o
#---------------------------------

arrtest3: arrtest3.o arrtestdyn3.o arrteststa3.o arrtestutil3.o libblockarray3.so.1.0.1
	$(CC) arrtest3.o arrtestdyn3.o arrteststa3.o arrtestutil3.o -L$(PWD) -l blockarray3 -Wl,-rpath=$(PWD) -o arrtest3

arrtest3.o: $(TASK2)/arrtest.c
	$(CC) $(CFLAGS) $(OPT3) $(TASK2)/arrtest.c -o arrtest3.o

arrtestdyn3.o: $(TASK2)/arrtestdyn.c
	$(CC) $(CFLAGS) $(OPT3) $(TASK2)/arrtestdyn.c -o arrtestdyn3.o

arrteststa3.o: $(TASK2)/arrteststa.c
	$(CC) $(CFLAGS) $(OPT3) $(TASK2)/arrteststa.c -o arrteststa3.o

arrtestutil3.o: $(TASK2)/arrtestutil.c
	$(CC) $(CFLAGS) $(OPT3) $(TASK2)/arrtestutil.c -o arrtestutil2.o

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
