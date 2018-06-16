CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O2

all: math_server math_client

math_client: math_client.o
	$(CC) math_client.o -o math_client -pthread -lm

math_client.o: client/math_client.c client/math_client.h
	$(CC) $(CFLAGS) client/math_client.c -o math_client.o

math_server: circular_fifo.o math_server.o
	$(CC) circular_fifo.o math_server.o -o math_server -pthread -lm

math_server.o: server/math_server.c server/math_server.h
	$(CC) $(CFLAGS) server/math_server.c -o math_server.o

circular_fifo.o: server/fifo/circular_fifo.c server/fifo/circular_fifo.h
	$(CC) $(CFLAGS) server/fifo/circular_fifo.c -o circular_fifo.o

.PHONY: clean
clean:
	rm *.o math_server math_client
