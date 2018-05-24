CC=gcc
CFLAGS= -Wall -std=gnu99 -O2

all: filter_gen

filter_gen: filter_gen.c
	$(CC) $(CFLAGS) filter_gen.c -o filter_gen

.PHONY: clean
clean:
	rm filter_gen
