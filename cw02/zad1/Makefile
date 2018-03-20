CC=gcc
CFLAGS=-c -Wall -std=c99 -O2

all: record_handler

test: record_handler timing_script.sh
	bash timing_script.sh &> results.txt

record_handler: record_handler.o record_handler_lib.o record_handler_sys.o
	$(CC) record_handler.o record_handler_lib.o record_handler_sys.o -o record_handler

record_handler.o: record_handler.c record_handler.h
	$(CC) $(CFLAGS) record_handler.c

record_handler_lib.o: record_handler_lib.c record_handler.h
	$(CC) $(CFLAGS) record_handler_lib.c -o record_handler_lib.o

record_handler_sys.o: record_handler_sys.c record_handler.h
	$(CC) $(CFLAGS) record_handler_sys.c -o record_handler_sys.o

.PHONY : clean
clean:
	rm *.o record_handler
