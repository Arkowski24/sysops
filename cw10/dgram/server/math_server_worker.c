//
// Created by farald on 10.06.18.
//

#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "../fifo/circular_fifo.h"
#include "math_server.h"
#include "math_server_local.h"
#include "math_server_network.h"

extern CircularFifo_t *fifo;
extern pthread_mutex_t fifoMutex;
extern pthread_cond_t fifoEmpty;

void insert_worker(int type, int index) {
    Client_t *client = calloc(1, sizeof(Client_t));
    client->type = type;
    client->index = index;

    fifo_push(fifo, client);
}

void delete_worker(int type, int index) {
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

Client_t *fetch_worker() {
    pthread_mutex_lock(&fifoMutex);

    while (fifo_empty(fifo)) {
        pthread_cond_wait(&fifoEmpty, &fifoMutex);
    }
    Client_t *client = fifo_pop(fifo);

    pthread_mutex_unlock(&fifoMutex);
    return client;
}

void reinsert_worker(Client_t *worker) {
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

void discard_worker(Client_t *worker) {
    if (worker->type == TYPE_LOCAL) {
        deregister_local_client_i(worker->index);
    } else if (worker->type == TYPE_NETWORK) {
        deregister_network_client_i(worker->index);
    }
    free(worker);
}
