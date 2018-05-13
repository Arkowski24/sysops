//
// Created by Arkadiusz Placha on 12.05.2018.
//

#ifndef SYSOPS_CIRCLE_FIFO_H
#define SYSOPS_CIRCLE_FIFO_H

#include <stdlib.h>
#include <limits.h>

typedef struct {
    pid_t PID;
    char *sName[NAME_MAX];
} ClientInfo_t;

typedef struct {
    size_t qMaxSize;
    size_t readItr;
    size_t writeItr;
    ClientInfo_t clients[];
} CircleFifo_t;

void fifo_initalize(CircleFifo_t *fifo, size_t queueSize);

int fifo_push(CircleFifo_t *fifo, ClientInfo_t elem);

ClientInfo_t *fifo_pop(CircleFifo_t *fifo);

ClientInfo_t *fifo_front(CircleFifo_t *fifo);

int fifo_empty(CircleFifo_t *fifo);

size_t fifo_size(CircleFifo_t *fifo);

#endif //SYSOPS_CIRCLE_FIFO_H
