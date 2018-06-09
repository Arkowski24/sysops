//
// Created by farald on 09.06.18.
//

#include <pthread.h>
#include <assert.h>
#include "math_server.h"
#include "../../fifo/circular_fifo.h"

int networkSocketDesc;

unsigned int networkClientsCount = 0;
extern pthread_mutex_t fifoMutex;
extern pthread_cond_t fifoEmpty;
ClientNetwork_t *networkClients[MAX_NETWORK_CLIENTS];

int is_present_network(int index) {
    assert(index < MAX_LOCAL_CLIENTS);

    return networkClients[index] == NULL;
}

int register_network_client(ClientLocal_t *clientLocal) {
    assert(clientLocal != NULL);

    pthread_mutex_lock(&fifoMutex);

    if (networkClientsCount == MAX_NETWORK_CLIENTS) {
        pthread_mutex_unlock(&fifoMutex);
        return -1;
    }

    int index = -1;
    for (int i = 0; i < MAX_NETWORK_CLIENTS; ++i) {
        if (networkClients[i] == NULL) {
            index = i;
            break;
        }
    }

    insert_client(TYPE_NETWORK, index);
    networkClientsCount++;

    pthread_cond_broadcast(&fifoEmpty);
    pthread_mutex_unlock(&fifoMutex);
    return 0;
}

int deregister_network_client(struct sockaddr_un address) {
    pthread_mutex_lock(&fifoMutex);

    if (networkClientsCount == 0) {
        pthread_mutex_unlock(&fifoMutex);
        return -1;
    }

    int index = -1;
    for (int i = 0; i < MAX_LOCAL_CLIENTS; ++i) {
        if (networkClients[i]->address == address) {
            index = i;
            break;
        }
    }

    delete_client(TYPE_LOCAL, index);
    networkClientsCount--;

    pthread_mutex_unlock(&fifoMutex);
    return 0;
}

int network_routine(int index, BinaryOperation_t *operation) {

}