//
// Created by Arkadiusz Placha on 12.05.2018.
//

#ifndef SYSOPS_CIRCLE_FIFO_H
#define SYSOPS_CIRCLE_FIFO_H

#include <stdlib.h>
#include <limits.h>

typedef struct {
    size_t qMaxSize;
    size_t qSize;
    size_t readItr;
    char *queue[];
} CircularFifo_t;

void fifo_initialize(CircularFifo_t *fifo, size_t queueSize);

int fifo_push(CircularFifo_t *fifo, char *elem);

char *fifo_pop(CircularFifo_t *fifo);

char *fifo_front(CircularFifo_t *fifo);

int fifo_empty(CircularFifo_t *fifo);

int fifo_full(CircularFifo_t *fifo);

size_t fifo_size(CircularFifo_t *fifo);

#endif //SYSOPS_CIRCLE_FIFO_H
