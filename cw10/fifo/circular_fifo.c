//
// Created by Arkadiusz Placha on 12.05.2018.
//

#include <assert.h>
#include <pthread.h>
#include "circular_fifo.h"

pthread_mutex_t mutex;
pthread_cond_t empty;
pthread_cond_t full;


void fifo_initialize(CircularFifo_t *fifo, size_t queueSize) {
    fifo->qMaxSize = queueSize;
    fifo->qSize = 0;
    fifo->readItr = 0;
}

int fifo_push(CircularFifo_t *fifo, Client_t *elem) {
    if (fifo_full(fifo)) { return -1; }

    size_t writeItr = (fifo->readItr + fifo->qSize) % fifo->qMaxSize;
    fifo->queue[writeItr] = elem;
    fifo->qSize++;

    return 0;
}

Client_t *fifo_pop(CircularFifo_t *fifo) {
    Client_t *elem = fifo_front(fifo);

    if (elem != NULL) {
        fifo->readItr = (fifo->readItr + 1) % fifo->qMaxSize;
        fifo->qSize--;
    }

    return elem;
}

Client_t *fifo_front(CircularFifo_t *fifo) {
    if (fifo_empty(fifo)) { return NULL; }

    Client_t *elem = fifo->queue[fifo->readItr];
    return elem;
}

int fifo_empty(CircularFifo_t *fifo) {
    return fifo_size(fifo) == 0;
}

int fifo_full(CircularFifo_t *fifo) {
    return fifo_size(fifo) == fifo->qMaxSize;
}

size_t fifo_size(CircularFifo_t *fifo) {
    return fifo->qSize;
}