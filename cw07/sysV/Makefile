CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O2

all: client_prog barber_prog

barber_prog: barber.o circular_fifo.o
	$(CC) barber.o circular_fifo.o -o barber_prog -lrt -pthread

client_prog: client.o circular_fifo.o
	$(CC) client.o circular_fifo.o -o client_prog -lrt -pthread

barber.o: barber/barber.h barber/barber.c
	$(CC) $(CFLAGS) barber/barber.c -o barber.o

client.o: client/client.h client/client.c
	$(CC) $(CFLAGS) client/client.c -o client.o

circular_fifo.o: fifo/circular_fifo.h fifo/circular_fifo.c
	$(CC) $(CFLAGS) fifo/circular_fifo.c -o circular_fifo.o

.PHONY: clean
clean:
	rm *.o client_prog barber_prog
