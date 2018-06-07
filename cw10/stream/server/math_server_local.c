//
// Created by farald on 07.06.18.
//
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "math_server.h"

#define UNIX_PATH_MAX 108
#define LOCAL_CLIENT_PENDING_MAX 32
#define LOCAL_CLIENT_CONNECTED_MAX 32

struct local_client {
    char *name;
    int socket;
    struct sockaddr_un address;
};

int localSocketDesc;

pthread_mutex_t mutex;
struct local_client *localClients[LOCAL_CLIENT_CONNECTED_MAX];
unsigned int localClientsCount = 0;

void local_socket_init(char *socketPath) {
    if (strlen(socketPath) >= UNIX_PATH_MAX) {
        exit(EXIT_FAILURE);
    }
    localSocketDesc = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);

    struct sockaddr_un sockaddrUn;
    sockaddrUn.sun_family = AF_UNIX;
    strcpy(sockaddrUn.sun_path, socketPath);
    bind(localSocketDesc, (struct sockaddr *) &sockaddrUn, sizeof(sockaddrUn));
    listen(localSocketDesc, LOCAL_CLIENT_PENDING_MAX);
}

void delete_socket(int socketDesc) {
    shutdown(socketDesc, SHUT_RDWR);
    close(socketDesc);
}

char *get_error_message(u_int8_t error) {
    switch (error) {
        case ERROR_REG_FIRST:
            return "You need to register first.";
        default:
            return "Unknown error.";
    }
}

void send_error_msg(int socketDes, u_int8_t error) {
    char *errorMsg = get_error_message(error);
    struct tlv_msg response;
    response.type = MSG_TYPE_ERROR;
    response.length = (strlen(errorMsg) + 1) * sizeof(char);

    send(socketDes, &response, sizeof(response), MSG_NOSIGNAL);
    send(socketDes, &errorMsg, response.length, MSG_NOSIGNAL);
}

void *register_procedure(void *arg) {
    pthread_detach(pthread_self());

    int *socketDesc = (int *) arg;
    struct tlv_msg msg;
    ssize_t readBytes = recv(*socketDesc, &msg, sizeof(msg), MSG_WAITALL | MSG_NOSIGNAL);

    if (readBytes <= 0) {
        delete_socket(*socketDesc);
        free(socketDesc);
        pthread_exit(NULL);
    }

    if (msg.type != MSG_TYPE_REGISTER) {
        send_error_msg(*socketDesc, ERROR_REG_FIRST);
        delete_socket(*socketDesc);
        free(socketDesc);
        pthread_exit(NULL);
    }

    char *clientName = calloc(msg.length, sizeof(u_int8_t));
    if (recv(*socketDesc, &clientName, msg.length, MSG_WAITALL | MSG_NOSIGNAL) <= 0) {
        send_error_msg(*socketDesc, ERROR_REG_FIRST);
        delete_socket(*socketDesc);
        free(socketDesc);
        pthread_exit(NULL);
    }

    return NULL;
}

void local_client_reg() {
    struct sockaddr_un address;
    socklen_t addressLength = sizeof(struct sockaddr_un);
    int *socketDesc = malloc(sizeof(int));

    *socketDesc = accept(localSocketDesc, (struct sockaddr *) &address, &addressLength);
    while (*socketDesc != -1) {
        pthread_t threadID;
        pthread_create(&threadID, NULL, register_procedure, socketDesc);
        socketDesc = malloc(sizeof(int));
    }
    free(socketDesc);
}