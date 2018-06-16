//
// Created by farald on 16.06.18.
//
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <stddef.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "math_client.h"

#define _BSD_SOURCE

volatile int cont = 1;
int clientSocket;

void cleanup_and_exit() {
    shutdown(clientSocket, SHUT_RDWR);
    close(clientSocket);
    exit(EXIT_FAILURE);
}

void local_socket_init(char *socketPath) {
    clientSocket = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un serverAddr;
    serverAddr.sun_family = AF_UNIX;
    strcpy(serverAddr.sun_path, socketPath);

    connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr_un));
}

void network_socket_init(char *address) {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddr;
    inet_pton(AF_INET, address, &serverAddr);

    connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr_in));
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

tlvMsg_t *read_msg(int sDesc) {
    size_t pSize;
    tlvMsg_t *pMsg = create_tlv_msg(0, NULL, 0, &pSize);

    int nReg = 0;
    nReg = nReg || (recv(sDesc, pMsg, pSize, MSG_NOSIGNAL | MSG_PEEK | MSG_WAITALL) <= 0);

    size_t rSize;
    tlvMsg_t *rMsg = create_tlv_msg(0, NULL, pMsg->length, &rSize);
    free(pMsg);

    nReg = nReg || (recv(sDesc, rMsg, rSize, MSG_NOSIGNAL | MSG_WAITALL) <= 0);

    if (nReg) {
        free(rMsg);
        return NULL;
    } else {
        return rMsg;
    }
}

void register_worker(char *name) {
    size_t nameLength = strlen(name) + 1;
    size_t fLength;
    tlvMsg_t *regMsg = create_tlv_msg(MESSAGE_TYPE_REGISTER, name, nameLength, &fLength);
    send(clientSocket, regMsg, fLength, MSG_NOSIGNAL);
    free(regMsg);

    tlvMsg_t *retMsg = read_msg(clientSocket);

    if(retMsg == NULL) {
        return;
    }else if (retMsg->type == STATUS_OK) {
        printf("Registered successfully.\n");

        free(retMsg);
    } else if (retMsg->type == ERROR_NAME_TAKEN) {
        printf("Error: Name is already taken.\n");

        free(retMsg);
        cleanup_and_exit();
    }
}

void deregister_worker() {
    size_t deRegLength;
    tlvMsg_t *deRegMsg = create_tlv_msg(MESSAGE_TYPE_DEREGISTER, NULL, 0, &deRegLength);
    send(clientSocket, deRegMsg, deRegLength, MSG_NOSIGNAL);
    free(deRegMsg);
}

double *eval_operation(BinaryOperation_t operation) {
    double *result = malloc(sizeof(double));

    switch (operation.operator) {
        case '+':
            *result = operation.op1 + operation.op2;
            break;
        case '-':
            *result = operation.op1 - operation.op2;
            break;
        case '*':
            *result = operation.op1 * operation.op2;
            break;
        case '/':
            *result = operation.op1 / operation.op2;
            break;
        default:
            free(result);
            result = NULL;
            break;
    }

    return result;
}

void process_request_message(tlvMsg_t *msg) {
    BinaryOperation_t operation;
    memcpy(&operation, &msg->value, sizeof(BinaryOperation_t));
    free(msg);
    printf("Received request message.\n");

    double *res = eval_operation(operation);

    if (res == NULL) {
        send_status_message(clientSocket, ERROR_UNK_OPERATION);
    } else {
        size_t resLength;
        tlvMsg_t *resMsg = create_tlv_msg(MESSAGE_TYPE_RESPONSE, res, sizeof(double), &resLength);
        send(clientSocket, resMsg, resLength, MSG_NOSIGNAL);
        free(resMsg);
    }
    free(res);
}

void process_ping_message(tlvMsg_t *msg) {
    free(msg);
    printf("Received ping message.\n");

    size_t pongSize;
    tlvMsg_t *pongMsg = create_tlv_msg(MESSAGE_TYPE_PONG, NULL, 0, &pongSize);
    send(clientSocket, pongMsg, pongSize, MSG_NOSIGNAL);
    free(pongMsg);
}

void process_message() {
    tlvMsg_t *msg = read_msg(clientSocket);
    if (msg == NULL) {
        printf("Server broke connection.\n");
        exit(EXIT_SUCCESS);
    }

    switch (msg->type) {
        case MESSAGE_TYPE_REQUEST:
            process_request_message(msg);
            break;
        case MESSAGE_TYPE_PING:
            process_ping_message(msg);
            break;
        default:
            free(msg);
            printf("Received message with unknown type.\n");
            break;
    }
}

void sigint_handler(int sig) {
    cont = 0;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        exit(EXIT_FAILURE);
    }
    signal(SIGINT, sigint_handler);

    char mode = argv[2][0];

    if (mode == 'n' || mode == 'N') {
        network_socket_init(argv[3]);
    } else {
        local_socket_init(argv[3]);
    }

    register_worker(argv[1]);

    while (cont) {
        process_message();
    }

    deregister_worker();

    shutdown(clientSocket, SHUT_RDWR);
    close(clientSocket);

    return 0;
}