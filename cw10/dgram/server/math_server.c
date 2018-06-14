//
// Created by farald on 09.06.18.
//

#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "../fifo/circular_fifo.h"
#include "math_server.h"
#include "math_server_worker.h"
#include "math_server_local.h"
#include "math_server_network.h"

CircularFifo_t *fifo;
pthread_mutex_t fifoMutex;
pthread_cond_t fifoEmpty;

extern int localInSocketDesc;
extern int networkInSocketDesc;

extern pthread_rwlock_t localSocketLock;
extern pthread_rwlock_t networkSocketLock;

void initialize_queue(unsigned int qSize) {
    size_t memSize = offsetof(CircularFifo_t, queue) + sizeof(char *) * qSize;
    fifo = malloc(memSize);
    fifo_initialize(fifo, qSize);
}

void initialize_mutex() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setprotocol(&attr, PTHREAD_MUTEX_RECURSIVE);

    pthread_mutex_init(&fifoMutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

void print_task_result(void *result) {
    double *taskResult = (double *) result;
    printf("%lf\n", *taskResult);
}

char *get_required_name(const struct tlv_msg *msg) {
    char *name = malloc(msg->length);
    memcpy(name, msg->value, msg->length);
    return name;
}

int is_unique_name(char *name) {
    return is_unique_name_local(name) && is_unique_name_network(name);
}

struct tlv_msg *create_task(BinaryOperation_t *operation, size_t *msgSize) {
    *msgSize = offsetof(struct tlv_msg, value) + sizeof(BinaryOperation_t);

    struct tlv_msg *msg = malloc(*msgSize);
    msg->type = MESSAGE_TYPE_TASK;
    msg->length = sizeof(BinaryOperation_t);
    memcpy(msg->value, operation, sizeof(BinaryOperation_t));
    return msg;
}

struct tlv_msg *create_type_message(size_t *msgSize, uint8_t type) {
    *msgSize = offsetof(struct tlv_msg, value);

    struct tlv_msg *msg = malloc(*msgSize);
    msg->type = type;
    msg->length = 0;
    return msg;
}

struct tlv_msg *read_response(int socket, struct sockaddr *src_addr, socklen_t addrlen) {
    size_t resSize = offsetof(struct tlv_msg, value);
    struct tlv_msg *res = malloc(resSize);

    if (recvfrom(socket, res, resSize, MSG_PEEK, src_addr, &addrlen) == -1) {
        free(res);
        return NULL;
    }

    resSize += res->length;
    res = realloc(res, resSize);
    if (recvfrom(socket, res, resSize, 0, src_addr, &addrlen) == -1) {
        free(res);
        return NULL;
    }

    return res;
}

void *send_message_and_read_response(int desc, struct tlv_msg *msg, size_t msgSize, struct sockaddr *address,
                                     int maxTries, unsigned int sleepTime) {
    unsigned int tries = 0;
    struct tlv_msg *res = NULL;

    while (tries < maxTries) {
        sendto(desc, msg, msgSize, 0, address, sizeof(struct sockaddr_in));

        usleep(sleepTime);

        res = read_response(desc, address, sizeof(struct sockaddr_in));
        if (res->type == MESSAGE_TYPE_RESULT) {
            break;
        } else if (res->type == MESSAGE_TYPE_DEREGISTER) {
            break;
        }

        free(res);
        tries++;
    }
}

void *process_task_routine(void *operation) {
    pthread_detach(pthread_self());
    BinaryOperation_t *binaryOperation = (BinaryOperation_t *) operation;

    Client_t *worker;
    struct tlv_msg *res = NULL;
    int cont = 1;

    while (cont) {
        pthread_mutex_lock(&fifoMutex);
        worker = fetch_worker();
        if (worker->type == TYPE_LOCAL) {
            res = local_task_routine(worker->index, binaryOperation);
        } else if (worker->type == TYPE_NETWORK) {
            res = network_task_routine(worker->index, binaryOperation);
        }
        pthread_mutex_unlock(&fifoMutex);

        if (res == NULL) {
            reinsert_worker(worker);
            continue;
        }

        switch (res->type) {
            case MESSAGE_TYPE_RESULT:
                print_task_result(res->value);
                cont = 0;
                break;
            case MESSAGE_TYPE_DEREGISTER:
                discard_worker(worker);
                break;
            default:
                reinsert_worker(worker);
        }
    }

    free(operation);
    return NULL;
}

void *tracker_routine(void *arg) {
    pthread_detach(pthread_self());

    while (1) {
        ping_available_workers();

        sleep(TRACKER_SLEEP_SEC);
    }
}

void *reader_routine(void *arg) {
    pthread_detach(pthread_self());
    
    struct pollfd polls[2];
    polls[0].fd = localInSocketDesc;
    polls[1].fd = networkInSocketDesc;

    while (1) {
        poll(polls, 2, POLL_TIMEOUT);
        if (polls[0].events & POLLIN) { process_local_messages(); }
        if (polls[1].events & POLLIN) { process_network_messages(); }
    }
}

void *ping_worker(Client_t *client) {
    switch (client->type) {
        case TYPE_LOCAL:
            return local_ping_routine(client->index, MESSAGE_TYPE_PING);
        case TYPE_NETWORK:
            return network_ping_routine(client->index, MESSAGE_TYPE_PING);
        default:
            return NULL;
    }
}

void ping_available_workers() {
    pthread_mutex_lock(&fifoMutex);

    size_t toCheck = fifo_size(fifo);
    while (toCheck > 0) {
        void *worker = fetch_worker();

        struct tlv_msg *res = ping_worker(worker);
        if (res == NULL || res->type == MESSAGE_TYPE_DEREGISTER) {
            discard_worker(worker);
        } else {
            reinsert_worker(worker);
        }

        toCheck--;
    }
    pthread_mutex_lock(&fifoMutex);
}

int main(int argc, char *argv[]) {
    initialize_queue(MAX_LOCAL_CLIENTS + MAX_NETWORK_CLIENTS);
    initialize_mutex();

    local_socket_init(argv[1]);
    int port = atoi(argv[2]);
    network_socket_init(port);

    pthread_t trackerID;
    pthread_t readerID;

    pthread_create(&trackerID, NULL, tracker_routine, NULL);
    pthread_create(&readerID, NULL, reader_routine, NULL);

    int cont = 1;
    while (cont) {
        BinaryOperation_t *operation = malloc(sizeof(BinaryOperation_t));
        scanf("%c %lf %lf", &operation->operation, &operation->op1, &operation->op2);

        if (operation->operation == 'e') {
            free(operation);
            cont = 0;
        } else {
            pthread_t threadID;
            pthread_create(&threadID, NULL, process_task_routine, operation);
        }
    }

    return 0;
}