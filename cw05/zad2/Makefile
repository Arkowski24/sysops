CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O2

all: fifo_master fifo_slave

fifo_master: fifo_master.o
	$(CC) fifo_master.o -o fifo_master

fifo_master.o: master/fifo_master.c master/fifo_master.h
	$(CC) $(CFLAGS) master/fifo_master.c -o fifo_master.o

fifo_slave: fifo_slave.o
	$(CC) fifo_slave.o -o fifo_slave

fifo_slave.o: slave/fifo_slave.c slave/fifo_slave.h
	$(CC) $(CFLAGS) slave/fifo_slave.c -o fifo_slave.o


.PHONY : clean
clean:
	rm *.o fifo_master fifo_slave
