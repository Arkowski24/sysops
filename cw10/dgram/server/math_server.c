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

CircularFifo_t *workerFifo;
pthread_mutex_t workerFifoMutex;
pthread_cond_t workerFifoEmpty;

CircularFifo_t *othersFifo;
pthread_mutex_t othersFifoMutex;

int cont = 1;
char *local_socket_name;
int publicLocalSocket;
int publicNetworkSocket;
pthread_mutex_t socketsMutex;

void initialize_queues(unsigned int qSize, unsigned int oSize) {
    size_t memSize = offsetof(CircularFifo_t, queue) + sizeof(char *) * qSize;
    workerFifo = malloc(memSize);
    fifo_initialize(workerFifo, qSize);

    size_t othSize = offsetof(CircularFifo_t, queue) + sizeof(tlvMsg_t *) * oSize;
    othersFifo = malloc(othSize);
    fifo_initialize(othersFifo, oSize);
}

void initialize_mutex_and_cond() {
    pthread_mutex_init(&workerFifoMutex, NULL);
    pthread_cond_init(&workerFifoEmpty, NULL);

    pthread_mutex_init(&othersFifoMutex, NULL);

    pthread_mutex_init(&socketsMutex, NULL);
}

void local_socket_init(char *socketPath) {
    publicLocalSocket = socket(AF_UNIX, SOCK_DGRAM, 0);

    struct sockaddr_un sockaddrUn;
    sockaddrUn.sun_family = AF_UNIX;
    strcpy(sockaddrUn.sun_path, socketPath);
    bind(publicLocalSocket, (struct sockaddr *) &sockaddrUn, sizeof(sockaddrUn));
}

void network_socket_init(uint16_t port) {
    publicNetworkSocket = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in sockaddrIn;
    sockaddrIn.sin_family = AF_INET;
    sockaddrIn.sin_port = htons(port);
    sockaddrIn.sin_addr.s_addr = INADDR_ANY;
    bind(publicNetworkSocket, (struct sockaddr *) &sockaddrIn, sizeof(sockaddrIn));
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

void send_to(tlvMsg_t *tlvMsg, size_t tlvMsgSize, struct sockaddr *address) {
    if (address->sa_family == AF_UNIX) {
        sendto(publicLocalSocket, tlvMsg, tlvMsgSize, MSG_NOSIGNAL, address, sizeof(struct sockaddr_un));
    } else {
        sendto(publicNetworkSocket, tlvMsg, tlvMsgSize, MSG_NOSIGNAL, address, sizeof(struct sockaddr_in));
    }
}

void send_status_message(struct sockaddr *address, uint8_t errorCode) {
    size_t errSize;
    tlvMsg_t *errMsg = create_tlv_msg(MESSAGE_TYPE_STATUS, &errorCode, sizeof(uint8_t), &errSize);

    send_to(errMsg, errSize, address);
    free(errMsg);
}

tlvMsg_t *receive(int socket, struct sockaddr *address, socklen_t *addrLen) {
    size_t pSize;
    tlvMsg_t *pMsg = create_tlv_msg(0, NULL, 0, &pSize);

    recvfrom(socket, pMsg, pSize, MSG_NOSIGNAL | MSG_PEEK, address, addrLen);
    size_t rSize;
    tlvMsg_t *rMsg = create_tlv_msg(0, NULL, pMsg->length, &rSize);
    free(pMsg);
    recvfrom(socket, rMsg, rSize, MSG_NOSIGNAL, address, addrLen);

    return rMsg;
}

void receive_from_all(int socket) {
    socklen_t addrLen;
    struct sockaddr *address;
    if (socket == publicLocalSocket) {
        addrLen = sizeof(struct sockaddr_un);
    } else {
        addrLen = sizeof(struct sockaddr_in);
    }
    address = malloc(addrLen);

    tlvMsg_t *rMsg = receive(socket, address, &addrLen);

    OtherMsg_t *otherMsg = malloc(sizeof(OtherMsg_t));
    otherMsg->msg = rMsg;
    otherMsg->address = address;

    pthread_mutex_lock(&othersFifoMutex);
    if (fifo_push(othersFifo, otherMsg) == -1) {
        free(otherMsg);
    }
    pthread_mutex_unlock(&othersFifoMutex);
}

tlvMsg_t *receive_from(struct sockaddr *sender, int timeout) {
    struct pollfd socketPoll;
    socketPoll.events = POLLIN;
    tlvMsg_t *rMsg;

    while (1) {
        socklen_t addrLen;
        struct sockaddr *address;
        if (sender->sa_family == AF_UNIX) {
            socketPoll.fd = publicLocalSocket;
            address = malloc(sizeof(struct sockaddr_un));
        } else {
            socketPoll.fd = publicNetworkSocket;
            address = malloc(sizeof(struct sockaddr_in));
        }

        if (poll(&socketPoll, 1, timeout) == 0) {
            free(address);
            return NULL;
        }


        rMsg = receive(socketPoll.fd, address, &addrLen);
        if (compare_sockaddr(sender, address)) {
            free(address);
            return rMsg;
        } else {
            OtherMsg_t *otherMsg = malloc(sizeof(OtherMsg_t));
            otherMsg->msg = rMsg;
            otherMsg->address = address;

            pthread_mutex_lock(&othersFifoMutex);
            if (fifo_push(othersFifo, otherMsg) == -1) { free(otherMsg); }
            pthread_mutex_unlock(&othersFifoMutex);
        }
    }
}

int ping_client(struct sockaddr *address) {
    size_t pingSize;
    tlvMsg_t *pingMsg = create_tlv_msg(MESSAGE_TYPE_PING, NULL, 0, &pingSize);
    tlvMsg_t *pongMsg;

    if (address->sa_family == AF_UNIX) {
        sendto(publicLocalSocket, pingMsg, pingSize, MSG_NOSIGNAL, address, sizeof(struct sockaddr_un));
        pongMsg = receive_from(address, PING_TIMEOUT);
    } else {
        sendto(publicNetworkSocket, pingMsg, pingSize, MSG_NOSIGNAL, address, sizeof(struct sockaddr_in));
        pongMsg = receive_from(address, PING_TIMEOUT);
    }

    int ok = pongMsg != NULL;
    ok = ok && (pongMsg->type == MESSAGE_TYPE_PONG);

    free(pingMsg);
    free(pongMsg);

    return ok;
}

void *operation_routine(void *arg) {
    pthread_mutex_lock(&socketsMutex);
    BinaryOperation_t *operation = (BinaryOperation_t *) arg;

    size_t oSize;
    tlvMsg_t *oMsg = create_tlv_msg(MESSAGE_TYPE_REQUEST, operation, sizeof(BinaryOperation_t), &oSize);
    tlvMsg_t *rMsg;
    double *result = malloc(sizeof(double));

    pthread_mutex_lock(&workerFifoMutex);
    while (1) {
        while (fifo_empty(workerFifo)) {
            pthread_mutex_unlock(&socketsMutex);
            pthread_cond_wait(&workerFifoEmpty, &workerFifoMutex);
            pthread_mutex_lock(&socketsMutex);
        }
        Worker_t *worker = fifo_pop(workerFifo);
        struct sockaddr *address = worker->address;

        send_to(oMsg, oSize, address);
        rMsg = receive_from(address, OPERATION_TIMEOUT);

        int discard = rMsg == NULL;
        discard = discard || (rMsg->type != MESSAGE_TYPE_RESPONSE && rMsg->type != MESSAGE_TYPE_STATUS);

        if (discard) {
            printf("Not received request response from: %s. Discarding worker.\n", worker->name);
            free(worker);
        } else {
            printf("Received request response from: %s\n", worker->name);
            if (rMsg->type == MESSAGE_TYPE_RESPONSE) {
                memcpy(result, &rMsg->value, sizeof(double));
            } else {
                free(result);
                result = NULL;
            }

            fifo_push(workerFifo, worker);
            break;
        }
    }
    pthread_mutex_unlock(&workerFifoMutex);

    free(arg);
    free(oMsg);
    free(rMsg);

    pthread_mutex_unlock(&socketsMutex);
    return result;
}

Worker_t *create_worker(struct sockaddr *address, char *name) {
    Worker_t *worker = malloc(sizeof(Worker_t));
    size_t size = (strlen(name) + 1) * sizeof(char);

    worker->address = address;
    worker->name = malloc(size);
    strcpy(worker->name, name);

    return worker;
}

void *register_worker_routine(void *arg) {
    pthread_detach(pthread_self());
    Worker_t *worker = (Worker_t *) arg;

    pthread_mutex_lock(&workerFifoMutex);
    char *reqName = worker->name;
    int ok = 1;
    ok = ok && fifo_unique_name(workerFifo, reqName);
    ok = ok && (fifo_push(workerFifo, worker) == 0);

    if (ok) {
        send_status_message(worker->address, STATUS_OK);

        printf("Worker registered: %s\n", reqName);
        pthread_cond_broadcast(&workerFifoEmpty);
    } else {
        send_status_message(worker->address, ERROR_NAME_TAKEN);
        free(arg);
    }
    pthread_mutex_unlock(&workerFifoMutex);

    return NULL;
}

void *deregister_worker_routine(void *arg) {
//    pthread_detach(pthread_self());
    struct sockaddr *address = (struct sockaddr *) arg;

    pthread_mutex_lock(&workerFifoMutex);
    Worker_t *worker = fifo_find(workerFifo, address);
    if (worker != NULL) {
        printf("Deregistered worker: %s\n", worker->name);
        free(worker);
    }
    send_status_message(address, STATUS_OK);

    pthread_mutex_unlock(&workerFifoMutex);

    return NULL;
}

void process_other_messages() {
    pthread_mutex_lock(&othersFifoMutex);
    if (fifo_empty(othersFifo)) {
        pthread_mutex_unlock(&othersFifoMutex);
        return;
    }

    int itr = 0;
    size_t toCheck = fifo_size(othersFifo);
    pthread_t slaves[toCheck];

    while (!fifo_empty(othersFifo)) {
        OtherMsg_t *otherMsg = fifo_pop(othersFifo);

        if (otherMsg->msg->type == MESSAGE_TYPE_REGISTER) {
            char *name = (char *) otherMsg->msg->value;
            Worker_t *worker = create_worker(otherMsg->address, name);

            free(otherMsg->msg);
            free(otherMsg);

            pthread_create(slaves + itr, NULL, register_worker_routine, worker);
            itr++;
        } else if (otherMsg->msg->type == MESSAGE_TYPE_REGISTER) {
            struct sockaddr *address = otherMsg->address;
            free(otherMsg->msg);
            free(otherMsg);

            pthread_create(slaves + itr, NULL, deregister_worker_routine, address);
            itr++;
        }
    }
    pthread_mutex_unlock(&othersFifoMutex);

    for (int i = 0; i < itr; ++i) {
        pthread_join(slaves[i], NULL);
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

        pthread_mutex_lock(&socketsMutex);
        for (int i = 0; i < MAX_MESSAGES_IN_BATCH; ++i) {
            poll(publicPoll, 2, POLL_TIMEOUT);

            if (publicPoll[0].revents & POLLIN) {
                receive_from_all(publicLocalSocket);
            }
            if (publicPoll[1].revents & POLLIN) {
                receive_from_all(publicNetworkSocket);
            }
            if (!(publicPoll[0].revents & publicPoll[1].revents & POLLIN)) {
                break;
            }
        }
        pthread_mutex_unlock(&socketsMutex);
        process_other_messages();

        if (arg != NULL && *(int *) arg == 0) { break; }
        usleep(PROCESS_MSG_SLEEP);
    }

    return NULL;
}

void *periodic_ping_routine(void *arg) {
    pthread_detach(pthread_self());

    while (1) {
        sleep(PING_THREAD_SLEEP);
        pthread_testcancel();
        pthread_mutex_lock(&socketsMutex);

        pthread_mutex_lock(&workerFifoMutex);
        printf("Pinging available workers.\n");
        size_t toCheck = fifo_size(workerFifo);
        for (int i = 0; i < toCheck; ++i) {
            Worker_t *worker = fifo_pop(workerFifo);

            if (ping_client(worker->address)) {
                printf("Worker %s still available.\n", worker->name);
                fifo_push(workerFifo, worker);
            } else {
                printf("Worker %s unavailable.\n", worker->name);
                free(worker->address);
                free(worker->name);
                free(worker);
            }
        }
        pthread_mutex_unlock(&workerFifoMutex);

        pthread_mutex_unlock(&socketsMutex);
        if (arg != NULL && *(int *) arg == 0) { break; }
    }

    return NULL;
}

void sigint_handler(int sig) {
    unlink(local_socket_name);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, sigint_handler);
    initialize_queues(ALL_CLIENTS_MAX, MAX_OTHER_MESSAGES);
    initialize_mutex_and_cond();

    local_socket_name = argv[1];
    uint16_t port = (uint16_t) atoi(argv[2]);
    local_socket_init(local_socket_name);
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

    unlink(argv[1]);

    return 0;
}