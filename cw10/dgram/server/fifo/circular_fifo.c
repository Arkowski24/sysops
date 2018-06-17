//
// Created by Arkadiusz Placha on 12.05.2018.
//

#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include "circular_fifo.h"

void fifo_initialize(CircularFifo_t *fifo, size_t queueSize) {
    fifo->qMaxSize = queueSize;
    fifo->qSize = 0;
    fifo->readItr = 0;
}

int fifo_push(CircularFifo_t *fifo, void *elem) {
    if (fifo_full(fifo)) { return -1; }

    size_t writeItr = (fifo->readItr + fifo->qSize) % fifo->qMaxSize;
    fifo->queue[writeItr] = elem;
    fifo->qSize++;

    return 0;
}

void *fifo_pop(CircularFifo_t *fifo) {
    Worker_t *elem = fifo_front(fifo);

    if (elem != NULL) {
        fifo->readItr = (fifo->readItr + 1) % fifo->qMaxSize;
        fifo->qSize--;
    }

    return elem;
}

void *fifo_front(CircularFifo_t *fifo) {
    if (fifo_empty(fifo)) { return NULL; }

    Worker_t *elem = fifo->queue[fifo->readItr];
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

int fifo_unique(CircularFifo_t *fifo, Worker_t *elem) {
    int isUnique = 1;

    int i = 0;
    while (i < fifo->qSize) {
        size_t index = (fifo->readItr + i) % fifo->qMaxSize;
        Worker_t *checked = fifo->queue[index];

        if (checked != elem) { isUnique = isUnique && !strcmp(checked->name, elem->name); }
        if (!isUnique) { break; }

        i++;
    }

    return isUnique;
}

int fifo_unique_name(CircularFifo_t *fifo, char *name) {
    int isUnique = 1;

    int i = 0;
    while (i < fifo_size(fifo)) {
        size_t index = (fifo->readItr + i) % fifo->qMaxSize;
        Worker_t *checked = fifo->queue[index];

        isUnique = isUnique && strcmp(checked->name, name) != 0;
        if (!isUnique) { break; }

        i++;
    }

    return isUnique;
}


int compare_sockaddr(struct sockaddr *address1, struct sockaddr *address2) {
    if (address1->sa_family != address2->sa_family) {
        return 0;
    } else {
        if (address1->sa_family == AF_UNIX) {
            return (memcmp(address1, address2, sizeof(struct sockaddr_un)) == 0);
        } else {
            return (memcmp(address1, address2, sizeof(struct sockaddr_in)) == 0);
        }
    }
}

Worker_t *fifo_find(CircularFifo_t *fifo, struct sockaddr *address) {
    size_t toCheck = fifo_size(fifo);

    while (toCheck > 0) {
        Worker_t *worker = fifo_pop(fifo);

        if (compare_sockaddr(worker->address, address)) {
            return worker;
        } else {
            fifo_push(fifo, worker);
            toCheck--;
        }
    }
    return NULL;
}