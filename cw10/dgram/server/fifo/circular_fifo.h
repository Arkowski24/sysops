//
// Created by Arkadiusz Placha on 12.05.2018.
//

#ifndef SYSOPS_CIRCLE_FIFO_H
#define SYSOPS_CIRCLE_FIFO_H

#include <stdlib.h>
#include <limits.h>
#include <sys/socket.h>
#include "../math_server.h"

typedef struct {
    char *name;
    struct sockaddr *address;
} Worker_t;

typedef struct {
    tlvMsg_t *msg;
    struct sockaddr *address;
} OtherMsg_t;

typedef struct {
    size_t qMaxSize;
    size_t qSize;
    size_t readItr;
    void *queue[];
} CircularFifo_t;

void fifo_initialize(CircularFifo_t *fifo, size_t queueSize);

int fifo_push(CircularFifo_t *fifo, void *elem);

void *fifo_pop(CircularFifo_t *fifo);

void *fifo_front(CircularFifo_t *fifo);

int fifo_empty(CircularFifo_t *fifo);

int fifo_full(CircularFifo_t *fifo);

size_t fifo_size(CircularFifo_t *fifo);

int fifo_unique(CircularFifo_t *fifo, Worker_t *elem);

int fifo_unique_name(CircularFifo_t *fifo, char *name);

Worker_t *fifo_find(CircularFifo_t *fifo, struct sockaddr *address);

int compare_sockaddr(struct sockaddr *address1, struct sockaddr *address2);

#endif //SYSOPS_CIRCLE_FIFO_H
