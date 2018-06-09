//
// Created by farald on 09.06.18.
//

#include <pthread.h>
#include <assert.h>
#include "math_server.h"
#include "../../fifo/circular_fifo.h"

CircularFifo_t *fifo;
pthread_mutex_t fifoMutex;
pthread_cond_t fifoEmpty;

void insert_client(int type, int index) {
    Client_t *client = calloc(1, sizeof(Client_t));
    client->type = type;
    client->index = index;

    fifo_push(fifo, client);
}

void delete_client(int type, int index) {
    unsigned int searchedCount = 0;
    while (searchedCount < fifo_size(fifo)) {
        Client_t *client = fifo_pop(fifo);

        if (client->type == type && client->index == index) {
            free(client);
            return;
        } else {
            fifo_push(fifo, client);
            searchedCount++;
        }
    }
}

Client_t *fetchWorker() {
    pthread_mutex_lock(&fifoMutex);

    while (fifo_empty(fifo)) {
        pthread_cond_wait(&fifoEmpty, &fifoMutex);
    }
    Client_t *client = fifo_pop(fifo);

    pthread_mutex_unlock(&fifoMutex);
    return client;
}

void reinsertWorker(Client_t *worker) {
    assert(worker != NULL);

    pthread_mutex_lock(&fifoMutex);

    if (worker->type == TYPE_LOCAL) {
        if (is_present_local(worker->index)) {
            fifo_push(fifo, worker);
        }
    } else if (worker->type == TYPE_NETWORK) {
        if (is_present_network(worker->index)) {
            fifo_push(fifo, worker);
        }
    }
    pthread_cond_broadcast(&fifoEmpty);

    pthread_mutex_unlock(&fifoMutex);
}

void *routine(void *operation) {
    BinaryOperation_t *binaryOperation = (BinaryOperation_t *) operation;

    Client_t *worker = fetchWorker();
    if(worker->type == TYPE_LOCAL) {
        
    } else if(worker->type == TYPE_NETWORK) {

    }

    reinsertWorker(worker);
}
