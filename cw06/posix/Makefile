CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O2

all: msg_server msg_client

msg_client: msg_client.o
	$(CC) -lrt msg_client.o -o msg_client -lrt

msg_client.o: msg_service.h client/msg_client.h client/msg_client.c
	$(CC) $(CFLAGS) client/msg_client.c -o msg_client.o 

msg_server: msg_server.o msg_server_services.o
	$(CC) msg_server.o msg_server_services.o -o msg_server -lrt

msg_server_services.o: msg_service.h server/msg_server.h server/msg_server_services.c
	$(CC) $(CFLAGS) server/msg_server_services.c -o msg_server_services.o

msg_server.o: msg_service.h server/msg_server.h server/msg_server.c
	$(CC) $(CFLAGS) server/msg_server.c -o msg_server.o

.PHONY : clean
clean:
	rm *.o msg_client msg_server
