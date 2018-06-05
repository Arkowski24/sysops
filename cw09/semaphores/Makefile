CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O2

all: prod_cons

prod_cons: prod_cons.o prod_cons_rout.o circular_fifo.o
	$(CC) prod_cons.o prod_cons_rout.o circular_fifo.o -o prod_cons -pthread -lm

circular_fifo.o: ../fifo/circular_fifo.c ../fifo/circular_fifo.h
	$(CC) $(CFLAGS) ../fifo/circular_fifo.c -o circular_fifo.o

prod_cons_rout.o: prod_cons_rout.c prod_cons.h
	$(CC) $(CFLAGS) prod_cons_rout.c -o prod_cons_rout.o

prod_cons.o: prod_cons.c prod_cons.h
	$(CC) $(CFLAGS) prod_cons.c -o prod_cons.o

.PHONY: clean
clean:
	rm *.o prod_cons
