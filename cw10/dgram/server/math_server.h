//
// Created by farald on 09.06.18.
//

#ifndef SYSOPS_MATH_SERVER_H
#define SYSOPS_MATH_SERVER_H

#include <stdint-gcc.h>
#include <netinet/in.h>

#define TYPE_LOCAL 1
#define TYPE_NETWORK 2

#define MESSAGE_TYPE_TASK 1
#define MESSAGE_TYPE_RESULT 2
#define MESSAGE_TYPE_REGISTER 3
#define MESSAGE_TYPE_DEREGISTER 4
#define MESSAGE_TYPE_PING 5
#define MESSAGE_TYPE_OK 6
#define MESSAGE_TYPE_ERROR 7

struct tlv_msg {
    uint8_t type;
    uint32_t length;
    uint8_t value[];
};

struct {
    uint8_t operation;
    double op1;
    double op2;
} typedef BinaryOperation_t;

char *get_required_name(const struct tlv_msg *msg);

struct tlv_msg *create_task(BinaryOperation_t *operation, size_t *msgSize);

struct tlv_msg *read_response(int socket, struct sockaddr *src_addr, socklen_t addrlen);

void *process_task_routine(void *operation);

#endif //SYSOPS_MATH_SERVER_H
