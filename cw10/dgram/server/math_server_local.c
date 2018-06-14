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
#include <unistd.h>
#include "../fifo/circular_fifo.h"
#include "math_server.h"
#include "math_server_worker.h"
#include "math_server_local.h"

#define UNIX_PATH_MAX 108

int localInSocketDesc;
pthread_rwlock_t localSocketLock;

unsigned int localClientsCount = 0;
extern pthread_mutex_t fifoMutex;
extern pthread_cond_t fifoEmpty;
ClientLocal_t *localClients[MAX_LOCAL_CLIENTS];

//PRIVATE

int un_equal(struct sockaddr_un op1, struct sockaddr_un op2) {
    return op1.sun_family == op2.sun_family && strcmp(op1.sun_path, op2.sun_path) == 0;
}

ClientLocal_t *retrieve_local(int index) {
    return localClients[index];;
}

int register_local_client(ClientLocal_t *client) {
    assert(client != NULL);

    pthread_mutex_lock(&fifoMutex);

    if (localClientsCount == MAX_LOCAL_CLIENTS || !is_unique_name(client->name)) {
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
    insert_worker(TYPE_LOCAL, index);
    localClientsCount++;

    pthread_cond_broadcast(&fifoEmpty);
    pthread_mutex_unlock(&fifoMutex);
    return 0;
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
    delete_worker(TYPE_LOCAL, index);
    localClientsCount--;

    pthread_mutex_unlock(&fifoMutex);
    return 0;
}

void process_local_register_message(struct tlv_msg *msg, struct sockaddr_un *address) {
    ClientLocal_t *client = malloc(sizeof(client));
    client->name = get_required_name(msg);
    client->address = *address;
    if (register_local_client(client) == -1) {
        free(client->name);
        free(client);
    }
}

//PUBLIC

void local_socket_init(char *socketPath) {
    if (strlen(socketPath) >= UNIX_PATH_MAX) {
        exit(EXIT_FAILURE);
    }
    localInSocketDesc = socket(AF_UNIX, SOCK_DGRAM, 0);

    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, socketPath);
    bind(localInSocketDesc, (struct sockaddr *) &address, sizeof(struct sockaddr_un));

    pthread_rwlock_init(&localSocketLock, NULL);
}

int is_present_local(int index) {
    if (index >= MAX_LOCAL_CLIENTS) { return 0; }

    return localClients[index] == NULL;
}

int is_unique_name_local(char *name) {
    int uniq = 1;
    for (int i = 0; i < MAX_LOCAL_CLIENTS && uniq; ++i) {
        if (localClients[i] != NULL) {
            uniq = uniq && strcmp(localClients[i]->name, name) == 0;
        }
    }
    return uniq;
}

void deregister_local_client_i(int index) {
    pthread_mutex_lock(&fifoMutex);

    localClients[index] = NULL;
    delete_worker(TYPE_LOCAL, index);
    localClientsCount--;

    pthread_mutex_unlock(&fifoMutex);
}

void process_local_messages() {
    pthread_rwlock_wrlock(&localSocketLock);

    struct sockaddr_un *address = malloc(sizeof(struct sockaddr_un));
    struct tlv_msg *msg;
    msg = read_response(localInSocketDesc, (struct sockaddr *) address, sizeof(struct sockaddr_un));
    if (msg == NULL) {
        return;
    }

    switch (msg->type) {
        case MESSAGE_TYPE_REGISTER:
            process_local_register_message(msg, address);
            break;
        case MESSAGE_TYPE_DEREGISTER:
            deregister_local_client_a(*address);
            break;
        default:
            break;
    }
    pthread_rwlock_unlock(&localSocketLock);
}

void *local_task_routine(int index, BinaryOperation_t *operation) {
    ClientLocal_t *worker = retrieve_local(index);

    if (worker == NULL) {
        return NULL;
    }

    size_t msgSize;
    struct tlv_msg *msg = create_task(operation, &msgSize);

    pthread_rwlock_rdlock(&localSocketLock);
    struct tlv_msg *res = send_message_and_read_response(localInSocketDesc, msg, msgSize,
                                                         (struct sockaddr *) &worker->address,
                                                         MAX_LOCAL_TRIES, LOCAL_USLEEP_VAL);
    pthread_rwlock_unlock(&localSocketLock);
    free(msg);

    return res;
}

void *local_ping_routine(int index, uint8_t msgType)  {
    ClientLocal_t *worker = retrieve_local(index);

    if (worker == NULL) {
        return NULL;
    }

    size_t msgSize;
    struct tlv_msg *msg = create_type_message(&msgSize, msgType);

    pthread_rwlock_rdlock(&localSocketLock);
    struct tlv_msg *res = send_message_and_read_response(localInSocketDesc, msg, msgSize,
                                                         (struct sockaddr *) &worker->address,
                                                         MAX_LOCAL_TRIES, LOCAL_USLEEP_VAL);
    pthread_rwlock_unlock(&localSocketLock);
    free(msg);

    return res;
}