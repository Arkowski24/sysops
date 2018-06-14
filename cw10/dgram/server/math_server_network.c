//
// Created by farald on 09.06.18.
//

#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../fifo/circular_fifo.h"
#include "math_server.h"
#include "math_server_worker.h"
#include "math_server_network.h"

int networkInSocketDesc;
pthread_rwlock_t networkSocketLock;

unsigned int networkClientsCount = 0;
extern pthread_mutex_t fifoMutex;
extern pthread_cond_t fifoEmpty;
ClientNetwork_t *networkClients[MAX_NETWORK_CLIENTS];

//PRIVATE

int in_equal(struct sockaddr_in op1, struct sockaddr_in op2) {
    return op1.sin_family == op2.sin_family && op1.sin_port == op2.sin_port &&
           op1.sin_addr.s_addr == op2.sin_addr.s_addr;
}

ClientNetwork_t *retrieve_network(int index) {
    return networkClients[index];
}

int register_network_client(ClientNetwork_t *client) {
    assert(client != NULL);

    pthread_mutex_lock(&fifoMutex);

    if (networkClientsCount == MAX_NETWORK_CLIENTS || !is_unique_name(client->name)) {
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

    networkClients[index] = client;
    insert_worker(TYPE_NETWORK, index);
    networkClientsCount++;

    pthread_cond_broadcast(&fifoEmpty);
    pthread_mutex_unlock(&fifoMutex);
    return 0;
}

int deregister_network_client_a(struct sockaddr_in address) {
    pthread_mutex_lock(&fifoMutex);

    if (networkClientsCount == 0) {
        pthread_mutex_unlock(&fifoMutex);
        return -1;
    }

    int index = -1;
    for (int i = 0; i < MAX_NETWORK_CLIENTS; ++i) {
        if (in_equal(networkClients[i]->address, address)) {
            index = i;
            break;
        }
    }

    networkClients[index] = NULL;
    delete_worker(TYPE_NETWORK, index);
    networkClientsCount--;

    pthread_mutex_unlock(&fifoMutex);
    return 0;
}

void process_network_register_message(struct tlv_msg *msg, struct sockaddr_in *address) {
    ClientNetwork_t *client = malloc(sizeof(client));
    client->name = get_required_name(msg);
    client->address = *address;
    if (register_network_client(client) == -1) {
        free(client->name);
        free(client);
    }
}

//PUBLIC

void network_socket_init(uint16_t port) {
    networkInSocketDesc = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sockaddrIn;
    sockaddrIn.sin_family = AF_INET;
    sockaddrIn.sin_port = htobe16(port);
    sockaddrIn.sin_addr.s_addr = INADDR_ANY;
    bind(networkInSocketDesc, (struct sockaddr *) &sockaddrIn, sizeof(sockaddrIn));
    listen(networkInSocketDesc, MAX_NETWORK_CLIENTS);

    pthread_rwlock_init(&networkSocketLock, NULL);
}

int is_present_network(int index) {
    if (index >= MAX_NETWORK_CLIENTS) { return 0; }

    return networkClients[index] == NULL;
}

int is_unique_name_network(char *name) {
    int uniq = 1;
    for (int i = 0; i < MAX_NETWORK_CLIENTS && uniq; ++i) {
        if (networkClients[i] != NULL) {
            uniq = uniq && strcmp(networkClients[i]->name, name) == 0;
        }
    }
    return uniq;
}

void deregister_network_client_i(int index) {
    pthread_mutex_lock(&fifoMutex);

    networkClients[index] = NULL;
    delete_worker(TYPE_NETWORK, index);
    networkClientsCount--;

    pthread_mutex_unlock(&fifoMutex);
}

void process_network_messages() {
    pthread_rwlock_wrlock(&networkSocketLock);

    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    struct tlv_msg *msg;
    msg = read_response(networkInSocketDesc, (struct sockaddr *) address, sizeof(struct sockaddr_in));
    if (msg == NULL) {
        return;
    }

    switch (msg->type) {
        case MESSAGE_TYPE_REGISTER:
            process_network_register_message(msg, address);
            break;
        case MESSAGE_TYPE_DEREGISTER:
            deregister_network_client_a(*address);
            break;
        default:
            break;
    }
    pthread_rwlock_unlock(&networkSocketLock);
}

void *network_task_routine(int index, BinaryOperation_t *operation) {
    ClientNetwork_t *worker = retrieve_network(index);

    if (worker == NULL) {
        return NULL;
    }

    size_t msgSize;
    struct tlv_msg *msg = create_task(operation, &msgSize);

    pthread_rwlock_rdlock(&networkSocketLock);
    struct tlv_msg *res = send_message_and_read_response(networkInSocketDesc, msg, msgSize,
                                                         (struct sockaddr *) &worker->address,
                                                         MAX_NETWORK_TRIES, NETWORK_USLEEP_VAL);
    pthread_rwlock_unlock(&networkSocketLock);
    free(msg);

    return res;
}

void *network_ping_routine(int index, uint8_t msgType) {
    ClientNetwork_t *worker = retrieve_network(index);

    if (worker == NULL) {
        return NULL;
    }

    size_t msgSize;
    struct tlv_msg *msg = create_type_message(&msgSize, msgType);

    pthread_rwlock_rdlock(&networkSocketLock);
    struct tlv_msg *res = send_message_and_read_response(networkInSocketDesc, msg, msgSize,
                                                         (struct sockaddr *) &worker->address,
                                                         MAX_NETWORK_TRIES, NETWORK_USLEEP_VAL);
    pthread_rwlock_unlock(&networkSocketLock);
    free(msg);

    return res;
}