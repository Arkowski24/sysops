//
// Created by farald on 07.06.18.
//


#include <sys/types.h>
#include <stddef.h>
#include <pthread.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <stdio.h>
#include <signal.h>
#include "fifo/circular_fifo.h"
#include "math_server.h"

CircularFifo_t *fifo;
pthread_mutex_t fifoMutex;
pthread_cond_t fifoEmpty;

int cont = 1;
int publicLocalSocket;
int publicNetworkSocket;

void initialize_queue(unsigned int qSize) {
    size_t memSize = offsetof(CircularFifo_t, queue) + sizeof(char *) * qSize;
    fifo = malloc(memSize);
    fifo_initialize(fifo, qSize);
}

void initialize_mutex_and_cond() {
    pthread_mutex_init(&fifoMutex, NULL);

    pthread_cond_init(&fifoEmpty, NULL);
}

void local_socket_init(char *socketPath) {
    publicLocalSocket = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un sockaddrUn;
    sockaddrUn.sun_family = AF_UNIX;
    strcpy(sockaddrUn.sun_path, socketPath);
    bind(publicLocalSocket, (struct sockaddr *) &sockaddrUn, sizeof(sockaddrUn));

    listen(publicLocalSocket, LOCAL_CLIENT_PENDING_MAX);
}

void network_socket_init(uint16_t port) {
    publicNetworkSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sockaddrIn;
    sockaddrIn.sin_family = AF_INET;
    sockaddrIn.sin_port = htons(port);
    sockaddrIn.sin_addr.s_addr = INADDR_ANY;
    bind(publicNetworkSocket, (struct sockaddr *) &sockaddrIn, sizeof(sockaddrIn));

    listen(publicNetworkSocket, NETWORK_CLIENT_PENDING_MAX);
}

tlvMsg_t *create_tlv_msg(uint8_t type, void *val, size_t len, size_t *fSize) {
    size_t memSize = offsetof(tlvMsg_t, value) + len;
    if (fSize != NULL) { *fSize = memSize; }
    tlvMsg_t *msg = malloc(memSize);

    msg->type = type;
    msg->length = len;
    if (val != NULL) { memcpy(&msg->value, val, len); }

    return msg;
}

void send_status_message(int desc, uint8_t errorCode) {
    size_t errSize;
    tlvMsg_t *errMsg = create_tlv_msg(MESSAGE_TYPE_STATUS, &errorCode, sizeof(uint8_t), &errSize);
    send(desc, errMsg, errSize, MSG_NOSIGNAL);
    free(errMsg);
}

int ping_client(int desc) {
    int ok = 1;

    size_t pingSize;
    tlvMsg_t *pingMsg = create_tlv_msg(MESSAGE_TYPE_PING, NULL, 0, &pingSize);
    size_t pongSize;
    tlvMsg_t *pongMsg = create_tlv_msg(MESSAGE_TYPE_PONG, NULL, 0, &pongSize);

    ok = ok && (send(desc, pingMsg, pingSize, MSG_NOSIGNAL) > 0);

    struct pollfd fdPoll;
    fdPoll.fd = desc;
    fdPoll.events = POLLIN;
    poll(&fdPoll, 1, PING_POLL_TIMEOUT);

    ok = ok && fdPoll.revents & POLLIN;
    ok = ok && (recv(desc, pongMsg, pongSize, MSG_NOSIGNAL | MSG_WAITALL | MSG_DONTWAIT) > 0);
    ok = ok && pongMsg->type == MESSAGE_TYPE_PONG;

    free(pingMsg);
    free(pongMsg);

    return ok;
}

void discard_worker(Worker_t *worker) {
    shutdown(worker->sDesc, SHUT_RDWR);
    close(worker->sDesc);
    free(worker);
}

void *operation_routine(void *arg) {
    BinaryOperation_t *operation = (BinaryOperation_t *) arg;

    size_t oSize;
    size_t rSize;
    tlvMsg_t *oMsg = create_tlv_msg(MESSAGE_TYPE_REQUEST, operation, sizeof(BinaryOperation_t), &oSize);
    tlvMsg_t *rMsg = create_tlv_msg(MESSAGE_TYPE_RESPONSE, NULL, sizeof(double), &rSize);
    double *result = malloc(sizeof(double));

    pthread_mutex_lock(&fifoMutex);
    while (1) {
        while (fifo_empty(fifo)) {
            pthread_cond_wait(&fifoEmpty, &fifoMutex);
        }
        Worker_t *worker = fifo_pop(fifo);

        int discard = 0;
        discard = (send(worker->sDesc, oMsg, oSize, MSG_NOSIGNAL) <= 0);
        discard = discard || (recv(worker->sDesc, rMsg, rSize, MSG_NOSIGNAL | MSG_WAITALL) <= 0);
        discard = discard || (rMsg->type != MESSAGE_TYPE_RESPONSE && rMsg->type != MESSAGE_TYPE_STATUS);

        if (discard) {
            discard_worker(worker);
        } else {
            if (rMsg->type == MESSAGE_TYPE_RESPONSE) {
                memcpy(result, &rMsg->value, sizeof(double));
            } else {
                free(result);
                result = NULL;
            }

            fifo_push(fifo, worker);
            break;
        }
    }
    pthread_mutex_unlock(&fifoMutex);

    free(arg);
    free(oMsg);
    free(rMsg);

    return result;
}

void close_communication(int desc) {
    shutdown(desc, SHUT_RDWR);
    close(desc);
}

Worker_t *create_worker(int sDesc, char *name) {
    Worker_t *worker = malloc(sizeof(Worker_t));
    size_t size = (strlen(name) + 1) * sizeof(char);

    worker->sDesc = sDesc;
    worker->name = malloc(size);
    strcpy(worker->name, name);

    return worker;
}

void *register_worker_routine(void *args) {
    pthread_detach(pthread_self());
    int sDesc = *(int *) args;
    free(args);

    pthread_mutex_lock(&fifoMutex);

    size_t pSize;
    tlvMsg_t *pMsg = create_tlv_msg(MESSAGE_TYPE_REGISTER, NULL, 0, &pSize);

    int nReg = fifo_full(fifo);
    nReg = nReg || (recv(sDesc, pMsg, pSize, MSG_NOSIGNAL | MSG_PEEK | MSG_WAITALL) <= 0);
    nReg = nReg || pMsg->type != MESSAGE_TYPE_REGISTER;

    size_t rSize;
    tlvMsg_t *rMsg = create_tlv_msg(MESSAGE_TYPE_REGISTER, NULL, pMsg->length, &rSize);
    free(pMsg);

    nReg = nReg || (recv(sDesc, rMsg, rSize, MSG_NOSIGNAL | MSG_WAITALL) <= 0);

    if (nReg) {
        close_communication(sDesc);
        free(rMsg);

        pthread_mutex_unlock(&fifoMutex);
        pthread_exit(NULL);
    }

    char *reqName = (char *) &rMsg->value;
    if (fifo_unique_name(fifo, reqName)) {
        Worker_t *worker = create_worker(sDesc, reqName);
        fifo_push(fifo, worker);
        send_status_message(sDesc, STATUS_OK);

        printf("Worker registered: %s\n", reqName);
        pthread_cond_broadcast(&fifoEmpty);
    } else {
        send_status_message(sDesc, ERROR_NAME_TAKEN);
        close_communication(sDesc);
    }
    pthread_mutex_unlock(&fifoMutex);

    free(rMsg);

    return NULL;
}

void *deregister_worker_routine(void *args) {
//    pthread_detach(pthread_self());
    int sDesc = *(int *) args;
    free(args);

    pthread_mutex_lock(&fifoMutex);

    size_t rSize;
    tlvMsg_t *rMsg = create_tlv_msg(MESSAGE_TYPE_DEREGISTER, NULL, 0, &rSize);

    int deReg;
    deReg = (recv(sDesc, rMsg, rSize, MSG_NOSIGNAL | MSG_WAITALL) <= 0);
    deReg = deReg || rMsg->type == MESSAGE_TYPE_DEREGISTER;

    if (deReg) {
        Worker_t *worker = fifo_find(fifo, sDesc);
        if (worker != NULL) {
            printf("Deregistered worker: %s\n", worker->name);
            free(worker);
        }

        send_status_message(sDesc, STATUS_OK);
        close_communication(sDesc);
    }
    pthread_mutex_unlock(&fifoMutex);

    free(rMsg);

    return NULL;
}

void process_worker_messages() {
    pthread_mutex_lock(&fifoMutex);
    while (fifo_empty(fifo)) {
        pthread_mutex_unlock(&fifoMutex);
        return;
    }

    size_t toCheck = fifo_size(fifo);
    struct pollfd workerPoll[toCheck];
    pthread_t cleaners[toCheck];
    int toClean = 0;

    for (int i = 0; i < toCheck; ++i) {
        Worker_t *worker = fifo_pop(fifo);
        workerPoll[i].fd = worker->sDesc;
        workerPoll[i].events = POLLIN;
        fifo_push(fifo, worker);
    }

    poll(workerPoll, toCheck, POLL_TIMEOUT);

    for (int j = 0; j < toCheck; ++j) {
        if (workerPoll[j].revents & POLLIN || workerPoll[j].revents & POLLERR) {
            int *desc = malloc(sizeof(int));
            *desc = workerPoll[j].fd;

            pthread_create(cleaners + toClean, NULL, deregister_worker_routine, desc);
            toClean++;
        }
    }
    pthread_mutex_unlock(&fifoMutex);

    for (int k = 0; k < toClean; ++k) {
        pthread_join(cleaners[k], NULL);
    }
}

void process_public_messages(struct pollfd cPoll[], size_t cPollCount) {
    poll(cPoll, cPollCount, POLL_TIMEOUT);

    for (int i = 0; i < cPollCount; ++i) {
        if (cPoll[i].revents & POLLIN) {
            int *desc = malloc(sizeof(int));
            *desc = accept(publicLocalSocket, NULL, NULL);

            pthread_t threadID;
            pthread_create(&threadID, NULL, register_worker_routine, desc);
//            pthread_join(threadID, NULL);
        }
    }
}

void *process_messages_routine(void *arg) {
    struct pollfd publicPoll[2];
    publicPoll[0].fd = publicLocalSocket;
    publicPoll[0].events = POLLIN;
    publicPoll[1].fd = publicNetworkSocket;
    publicPoll[1].events = POLLIN;

    while (1) {
        pthread_testcancel();

        process_worker_messages();
        process_public_messages(publicPoll, 2);

        if (arg != NULL && *(int *) arg == 0) { break; }
    }

    return NULL;
}

void *periodic_ping_routine(void *arg) {
    pthread_detach(pthread_self());

    while (1) {
        pthread_testcancel();

        pthread_mutex_lock(&fifoMutex);
        size_t toCheck = fifo_size(fifo);
        for (int i = 0; i < toCheck; ++i) {
            Worker_t *worker = fifo_pop(fifo);

            if (ping_client(worker->sDesc)) {
                fifo_push(fifo, worker);
            } else {
                discard_worker(worker);
            }
        }
        pthread_mutex_unlock(&fifoMutex);

        if (arg != NULL && *(int *) arg == 0) { break; }
        sleep(PING_THREAD_SLEEP);
    }

    return NULL;
}

void shutdown_all_workers(pthread_t snifferID) {
    pthread_cancel(snifferID);

    pthread_mutex_lock(&fifoMutex);
    while (!fifo_empty(fifo)) {
        Worker_t *worker = fifo_pop(fifo);
        discard_worker(worker);
    }
    pthread_mutex_unlock(&fifoMutex);
}

void sigint_handler(int sig) {
    cont = 0;
    alarm(10);
    printf("Shutting down.\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, sigint_handler);
    initialize_queue(ALL_CLIENTS_MAX);
    initialize_mutex_and_cond();

    uint16_t port = (uint16_t) atoi(argv[2]);
    local_socket_init(argv[1]);
    network_socket_init(port);

    pthread_t snifferID;
    pthread_t pingerID;
    pthread_create(&snifferID, NULL, process_messages_routine, NULL);
    pthread_create(&pingerID, NULL, periodic_ping_routine, NULL);

    while (cont) {
        BinaryOperation_t *operation = malloc(sizeof(BinaryOperation_t));
        if (scanf("%lf %c %lf", &operation->op1, &operation->operator, &operation->op2) < 3) {
            break;
        }

        if (operation->operator == '0') {
            break;
        }

        pthread_t supervisorID;
        pthread_create(&supervisorID, NULL, operation_routine, operation);

        void *res;
        pthread_join(supervisorID, &res);
        if (res == NULL) {
            printf("Unknown operation.\n");
        } else {
            printf("%lf\n", *(double *) res);
            free(res);
        }
    }

    shutdown_all_workers(snifferID);
    shutdown(publicLocalSocket, SHUT_RDWR);
    shutdown(publicNetworkSocket, SHUT_RDWR);

    unlink(argv[1]);

    return 0;
}