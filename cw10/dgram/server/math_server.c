//
// Created by farald on 09.06.18.
//

#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "math_server.h"
#include "math_server_worker.h"
#include "../fifo/circular_fifo.h"

CircularFifo_t *fifo;
pthread_mutex_t fifoMutex;
pthread_cond_t fifoEmpty;

void print_task_result(void *result) {
    double *taskResult = (double *) result;
    printf("%lf\n", *taskResult);
}

struct tlv_msg *create_task(BinaryOperation_t *operation, size_t *msgSize) {
    *msgSize = offsetof(struct tlv_msg, value) + sizeof(BinaryOperation_t);

    struct tlv_msg *msg = malloc(*msgSize);
    msg->type = MESSAGE_TYPE_TASK;
    msg->length = sizeof(BinaryOperation_t);
    memcpy(msg->value, operation, sizeof(BinaryOperation_t));
    return msg;
}

struct tlv_msg *read_response(int socket, struct sockaddr *src_addr, socklen_t addrlen, int flags) {
    size_t resSize = offsetof(struct tlv_msg, value);
    struct tlv_msg *res = malloc(resSize);

    if (recvfrom(socket, res, resSize, MSG_PEEK | flags, src_addr, &addrlen) == -1) {
        free(res);
        return NULL;
    }

    resSize += res->length;
    realloc(res, resSize);
    if (recvfrom(socket, res, resSize, 0 | flags, src_addr, &addrlen) == -1) {
        free(res);
        return NULL;
    }

    return res;
}

void *process_task_routine(void *operation) {
    BinaryOperation_t *binaryOperation = (BinaryOperation_t *) operation;

    Client_t *worker;
    struct tlv_msg *res = NULL;

    while (1) {
        worker = fetch_worker();
        if (worker->type == TYPE_LOCAL) {
            res = local_task_routine(worker->index, binaryOperation);
        } else if (worker->type == TYPE_NETWORK) {
            res = network_task_routine(worker->index, binaryOperation);
        }

        if (res == NULL) {
            reinsert_worker(worker);
        } else if (res->type == MESSAGE_TYPE_RESULT) {
            print_task_result(res->value);
            break;
        } else if (res->type == MESSAGE_TYPE_DEREGISTER) {
            discard_worker(worker);
        } else {
            reinsert_worker(worker);
        }
    }

    return NULL;
}
