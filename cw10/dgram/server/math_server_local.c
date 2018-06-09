//
// Created by farald on 09.06.18.
//

#include <pthread.h>
#include <assert.h>
#include "math_server.h"
#include "../../fifo/circular_fifo.h"

int localSocketDesc;

unsigned int localClientsCount = 0;
extern pthread_mutex_t fifoMutex;
extern pthread_cond_t fifoEmpty;
ClientLocal_t *localClients[MAX_LOCAL_CLIENTS];

int is_present_local(int index) {
    assert(index < MAX_LOCAL_CLIENTS);

    return localClients[index] == NULL;
}

ClientLocal_t *retrieve_local(int index) {
    pthread_mutex_lock(&fifoMutex);
    ClientLocal_t *clientLocal = localClients[index];
    pthread_mutex_unlock(&fifoMutex);

    return clientLocal;
}

int register_local_client(ClientLocal_t *client) {
    assert(client != NULL);

    pthread_mutex_lock(&fifoMutex);

    if (localClientsCount == MAX_LOCAL_CLIENTS) {
        pthread_mutex_unlock(&fifoMutex);
        return -1;
    }

    int index = -1;
    for (int i = 0; i < MAX_LOCAL_CLIENTS; ++i) {
        if (localClients[i] == NULL) {
            index = i;
            break;
        }
    }

    localClients[index] = client;
    insert_client(TYPE_LOCAL, index);
    localClientsCount++;

    pthread_cond_broadcast(&fifoEmpty);
    pthread_mutex_unlock(&fifoMutex);
    return 0;
}

int deregister_local_client(struct sockaddr_un address) {
    pthread_mutex_lock(&fifoMutex);

    if (localClientsCount == 0) {
        pthread_mutex_unlock(&fifoMutex);
        return -1;
    }

    int index = -1;
    for (int i = 0; i < MAX_LOCAL_CLIENTS; ++i) {
        if (localClients[i]->address == address) {
            index = i;
            break;
        }
    }

    localClients[index] = NULL;
    delete_client(TYPE_LOCAL, index);
    localClientsCount--;

    pthread_mutex_unlock(&fifoMutex);
    return 0;
}

int local_routine(int index, BinaryOperation_t *operation) {

}