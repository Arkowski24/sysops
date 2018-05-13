//
// Created by Arkadiusz Placha on 12.05.2018.
//

#include <assert.h>
#include "circular_fifo.h"

void fifo_initalize(CircleFifo_t *fifo, size_t queueSize) {
    assert(queueSize > 0);

    fifo->qMaxSize = queueSize;
    fifo->readItr = 0;
    fifo->writeItr = 0;
}

int fifo_push(CircleFifo_t *fifo, ClientInfo_t elem) {
    if (fifo_size(fifo) == fifo->qMaxSize) { return -1; }

    fifo->clients[fifo->writeItr] = elem;
    fifo->writeItr = (fifo->writeItr + 1) % fifo->qMaxSize;
    return 0;
}

ClientInfo_t *fifo_pop(CircleFifo_t *fifo) {
    ClientInfo_t *elem = fifo_front(fifo);

    if (elem != NULL) {
        fifo->readItr = (fifo->readItr + 1) % fifo->qMaxSize;
    }

    return elem;
}

ClientInfo_t *fifo_front(CircleFifo_t *fifo) {
    if (fifo_empty(fifo)) { return NULL; }

    ClientInfo_t *elem = fifo->clients + fifo->readItr;
    return elem;
}

int fifo_empty(CircleFifo_t *fifo) {
    return fifo_size(fifo) == 0;
}

size_t fifo_size(CircleFifo_t *fifo) {
    if (fifo->writeItr >= fifo->readItr) {
        return fifo->writeItr - fifo->readItr;
    } else {
        return fifo->qMaxSize - fifo->readItr + fifo->writeItr;
    }
}