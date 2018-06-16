//
// Created by Arkadiusz Placha on 12.05.2018.
//

#ifndef SYSOPS_CIRCLE_FIFO_H
#define SYSOPS_CIRCLE_FIFO_H

#include <stdlib.h>
#include <limits.h>

typedef struct {
    char *name;
    int sDesc;
} Worker_t;

typedef struct {
    size_t qMaxSize;
    size_t qSize;
    size_t readItr;
    Worker_t *queue[];
} CircularFifo_t;

void fifo_initialize(CircularFifo_t *fifo, size_t queueSize);

int fifo_push(CircularFifo_t *fifo, Worker_t *elem);

Worker_t *fifo_pop(CircularFifo_t *fifo);

Worker_t *fifo_front(CircularFifo_t *fifo);

int fifo_empty(CircularFifo_t *fifo);

int fifo_full(CircularFifo_t *fifo);

size_t fifo_size(CircularFifo_t *fifo);

int fifo_unique(CircularFifo_t *fifo, Worker_t *elem);

int fifo_unique_name(CircularFifo_t *fifo, char *name);

int fifo_delete(CircularFifo_t* fifo, int sDesc);

#endif //SYSOPS_CIRCLE_FIFO_H
