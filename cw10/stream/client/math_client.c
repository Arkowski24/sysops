//
// Created by farald on 16.06.18.
//

#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <stddef.h>
#include <sys/un.h>
#include <unistd.h>
#include "math_client.h"

int clientSocket;

void local_socket_init(char *socketPath) {
    clientSocket = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un serverAddr;
    serverAddr.sun_family = AF_UNIX;
    strcpy(serverAddr.sun_path, socketPath);

    connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr_un));
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

int main(int argc, char *argv[]) {
    if (argc < 4) {
        exit(EXIT_FAILURE);
    }

    int mode = atoi(argv[2]);
    local_socket_init(argv[3]);

    size_t nameLength = strlen(argv[1]) + 1;
    size_t fLength;
    tlvMsg_t *regMsg = create_tlv_msg(MESSAGE_TYPE_REGISTER, argv[1], nameLength, &fLength);
    send(clientSocket, regMsg, fLength, MSG_NOSIGNAL);
    free(regMsg);

    size_t reqLength;
    tlvMsg_t *reqMsg = create_tlv_msg(MESSAGE_TYPE_REQUEST, NULL, sizeof(BinaryOperation_t), &reqLength);
    recv(clientSocket, reqMsg, reqLength, MSG_NOSIGNAL | MSG_WAITALL);

    BinaryOperation_t operation;
    memcpy(&operation, &reqMsg->value, sizeof(BinaryOperation_t));
    free(reqMsg);

    double result = operation.op1 + operation.op2;
    size_t resLength;
    tlvMsg_t *resMsg = create_tlv_msg(MESSAGE_TYPE_RESPONSE, &result, sizeof(double), &resLength);
    send(clientSocket, resMsg, resLength, MSG_NOSIGNAL);
    free(resMsg);

    size_t deRegLength;
    tlvMsg_t *deRegMsg = create_tlv_msg(MESSAGE_TYPE_DEREGISTER, NULL, 0, &deRegLength);
    send(clientSocket, deRegMsg, deRegLength, MSG_NOSIGNAL);
    free(deRegMsg);

    shutdown(clientSocket, SHUT_RDWR);
    close(clientSocket);

    return 0;
}