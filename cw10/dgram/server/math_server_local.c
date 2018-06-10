//
// Created by farald on 09.06.18.
//

#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/un.h>
#include "math_server.h"
#include "../fifo/circular_fifo.h"

#define UNIX_PATH_MAX 108

int localSocketDesc;

unsigned int localClientsCount = 0;
extern pthread_mutex_t fifoMutex;
extern pthread_cond_t fifoEmpty;
ClientLocal_t *localClients[MAX_LOCAL_CLIENTS];

void local_socket_init(char *socketPath) {
    if (strlen(socketPath) >= UNIX_PATH_MAX) {
        exit(EXIT_FAILURE);
    }
    localSocketDesc = socket(AF_UNIX, SOCK_DGRAM, 0);

    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, socketPath);
    bind(localSocketDesc, (struct sockaddr *) &address, sizeof(struct sockaddr_un));
}

void process_local_messages() {
    struct tlv_msg *msg = read_response(localSocketDesc, NULL, NULL, MSG_PEEK);

    if(msg == NULL) {
        return;
    }

    if(msg->type == MESSAGE_TYPE_RESULT) {

    }
}

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

int un_equal(struct sockaddr_un op1, struct sockaddr_un op2) {
    return op1.sun_family == op2.sun_family && strcmp(op1.sun_path, op2.sun_path) == 0;
}

void deregister_local_client_i(int index) {
    pthread_mutex_lock(&fifoMutex);

    localClients[index] = NULL;
    delete_client(TYPE_LOCAL, index);
    localClientsCount--;

    pthread_mutex_unlock(&fifoMutex);
}

int deregister_local_client_a(struct sockaddr_un address) {
    pthread_mutex_lock(&fifoMutex);

    if (localClientsCount == 0) {
        pthread_mutex_unlock(&fifoMutex);
        return -1;
    }

    int index = -1;
    for (int i = 0; i < MAX_LOCAL_CLIENTS; ++i) {
        if (un_equal(localClients[i]->address, address)) {
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


void *local_task_routine(int index, BinaryOperation_t *operation) {
    ClientLocal_t *worker = retrieve_local(index);

    if (worker == NULL) {
        return NULL;
    }

    size_t msgSize;
    struct tlv_msg *msg = create_task(operation, &msgSize);
    struct tlv_msg *res = NULL;

    unsigned int tries = 0;

    while (tries < MAX_LOCAL_TRIES) {
        sendto(localSocketDesc, msg, msgSize, 0, (struct sockaddr *) &worker->address, sizeof(struct sockaddr_un));
        res = read_response(localSocketDesc, (struct sockaddr *) &worker->address, sizeof(struct sockaddr_un), 0);
        if (res->type == MESSAGE_TYPE_RESULT) {
            break;
        } else if (res->type == MESSAGE_TYPE_DEREGISTER) {
            break;
        }

        free(res);
        tries++;
    }
    free(msg);

    return res;
}