CC=gcc
CFLAGS=-c -Wall -std=c99
SHRLIB=-Wall -std=c99 -fPIC -shared
TASK1=../../../zad1
TASK2=../../../zad2

OPT1=-o0
OPT2=-os
OPT3=-o3

all: arrtest1 arrtest2 arrtest3
	./arrtest1 s 10000 10000 init fnd 1000 rmIn 10000 armIn 10000 > static_report.txt
	./arrtest2 s 10000 10000 init fnd 1000 rmIn 10000 armIn 10000 >> static_report.txt
	./arrtest2 s 10000 10000 init fnd 1000 rmIn 10000 armIn 10000 >> static_report.txt
    ./arrtest1 d 10000 10000 init fnd 1000 rmIn 10000 armIn 10000 > dynamic_report.txt
    ./arrtest2 d 10000 10000 init fnd 1000 rmIn 10000 armIn 10000 >> dynamic_report.txt
    ./arrtest2 d 10000 10000 init fnd 1000 rmIn 10000 armIn 10000 >> dynamic_report.txt

arrtest1: arrtest1.o arrtestdyn1.o arrteststa1.o arrtestutil1.o blockarray1.a
	$(CC) arrtest1.o arrtestdyn1.o arrteststa1.o arrtestutil1.o blockarray1.a -o arrtest1

arrtest1.o: $(TASK2)/arrtest.c
	$(CC) $(CFLAGS) $(OPT1) $(TASK2)/arrtest.c -o arrtest1.o

arrtestdyn1.o: $(TASK2)/arrtestdyn.c
	$(CC) $(CFLAGS) $(OPT1) $(TASK2)/arrtestdyn.c -o arrtestdyn1.o

arrteststa1.o: $(TASK2)/arrteststa.c
	$(CC) $(CFLAGS) $(OPT1) $(TASK2)/arrteststa.c -o arrteststa1.o

arrtestutil1.o: $(TASK2)/arrtestutil.c
	$(CC) $(CFLAGS) $(OPT1) $(TASK2)/arrtestutil.c -o arrtestutil1.o

blockarray1.a: blockarray-static-alloc1.o blockarray-dynamic-alloc1.o
	ar rcs blockarray1.a blockarray-static-alloc.o blockarray-dynamic-alloc.o

blockarray-static-alloc1.o: $(TASK1)/blockarray-static-alloc.c
	$(CC) $(CFLAGS) $(OPT1) $(TASK1)/blockarray-static-alloc.c -o blockarray-static-alloc1.o

blockarray-dynamic-alloc1.o: $(TASK1)/blockarray-dynamic-alloc.c
	$(CC) $(CFLAGS) $(OPT1) $(TASK1)/blockarray-dynamic-alloc.c -o blockarray-dynamic-alloc1.o

#-------------
arrtest2: arrtest2.o arrtestdyn2.o arrteststa2.o arrtestutil2.o blockarray2.a
	$(CC) arrtest2.o arrtestdyn2.o arrteststa2.o arrtestutil2.o blockarray2.a -o arrtest2

arrtest2.o: $(TASK2)/arrtest.c
	$(CC) $(CFLAGS) $(OPT2) $(TASK2)/arrtest.c -o arrtest2.o

arrtestdyn2.o: $(TASK2)/arrtestdyn.c
	$(CC) $(CFLAGS) $(OPT2) $(TASK2)/arrtestdyn.c -o arrtestdyn2.o

arrteststa2.o: $(TASK2)/arrteststa.c
	$(CC) $(CFLAGS) $(OPT2) $(TASK2)/arrteststa.c -o arrteststa2.o

arrtestutil2.o: $(TASK2)/arrtestutil.c
	$(CC) $(CFLAGS) $(OPT2) $(TASK2)/arrtestutil.c -o arrtestutil2.o

blockarray2.a: blockarray-static-alloc2.o blockarray-dynamic-alloc2.o
	ar rcs blockarray2.a blockarray-static-alloc.o blockarray-dynamic-alloc.o

blockarray-static-alloc2.o: $(TASK1)/blockarray-static-alloc.c
	$(CC) $(CFLAGS) $(OPT2) $(TASK1)/blockarray-static-alloc.c -o blockarray-static-alloc2.o

blockarray-dynamic-alloc2.o: $(TASK1)/blockarray-dynamic-alloc.c
	$(CC) $(CFLAGS) $(OPT2) $(TASK1)/blockarray-dynamic-alloc.c -o blockarray-dynamic-alloc2.o

#-------------
arrtest3: arrtest3.o arrtestdyn3.o arrteststa3.o arrtestutil3.o blockarray3.a
	$(CC) arrtest3.o arrtestdyn3.o arrteststa3.o arrtestutil3.o blockarray3.a -o arrtest3

arrtest3.o: $(TASK2)/arrtest.c
	$(CC) $(CFLAGS) $(OPT3) $(TASK2)/arrtest.c -o arrtest3.o

arrtestdyn3.o: $(TASK2)/arrtestdyn.c
	$(CC) $(CFLAGS) $(OPT3) $(TASK2)/arrtestdyn.c -o arrtestdyn3.o

arrteststa3.o: $(TASK2)/arrteststa.c
	$(CC) $(CFLAGS) $(OPT3) $(TASK2)/arrteststa.c -o arrteststa3.o

arrtestutil3.o: $(TASK2)/arrtestutil.c
	$(CC) $(CFLAGS) $(OPT3) $(TASK2)/arrtestutil.c -o arrtestutil3.o

blockarray3.a: blockarray-static-alloc3.o blockarray-dynamic-alloc3.o
	ar rcs blockarray3.a blockarray-static-alloc.o blockarray-dynamic-alloc.o

blockarray-static-alloc3.o: $(TASK1)/blockarray-static-alloc.c
	$(CC) $(CFLAGS) $(OPT3) $(TASK1)/blockarray-static-alloc.c -o blockarray-static-alloc3.o

blockarray-dynamic-alloc3.o: $(TASK1)/blockarray-dynamic-alloc.c
	$(CC) $(CFLAGS) $(OPT3) $(TASK1)/blockarray-dynamic-alloc.c -o blockarray-dynamic-alloc3.o

.PHONY : clean
clean:
	rm *.o arrtest *.a