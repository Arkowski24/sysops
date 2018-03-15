CC=gcc
CFLAGS=-c -Wall -std=c99
SHRLIB=-Wall -std=c99 -fPIC -shared
TASK1=../../../zad1
TASK2=../../../zad2

all: arrtest
	./arrtest s 10000 10000 init rmin 5000 armin 5000 fnd 200 > static_report.txt
    ./arrtest d 10000 10000 init rmin 5000 armin 5000 fnd 200 > dynamic_report.txt

arrtest: arrtest.o arrtestdyn.o arrteststa.o arrtestutil.o blockarray.a
	$(CC) arrtest.o arrtestdyn.o arrteststa.o arrtestutil.o blockarray.a -o arrtest

arrtest.o: $(TASK2)/arrtest.c
	$(CC) $(CFLAGS) $(TASK2)/arrtest.c

arrtestdyn.o: $(TASK2)/arrtestdyn.c
	$(CC) $(CFLAGS) $(TASK2)/arrtestdyn.c

arrteststa.o: $(TASK2)/arrteststa.c
	$(CC) $(CFLAGS) $(TASK2)/arrteststa.c

arrtestutil.o: $(TASK2)/arrtestutil.c
	$(CC) $(CFLAGS) $(TASK2)/arrtestutil.c

blockarray.a: blockarray-static-alloc.o blockarray-dynamic-alloc.o
	ar rcs blockarray.a blockarray-static-alloc.o blockarray-dynamic-alloc.o

blockarray-static-alloc.o: $(TASK1)/blockarray-static-alloc.c
	$(CC) $(CFLAGS) $(TASK1)/blockarray-static-alloc.c

blockarray-dynamic-alloc.o: $(TASK1)/blockarray-dynamic-alloc.c
	$(CC) $(CFLAGS) $(TASK1)/blockarray-dynamic-alloc.c

.PHONY : clean
clean:
	rm *.o arrtest *.a