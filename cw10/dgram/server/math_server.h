//
// Created by farald on 09.06.18.
//

#ifndef SYSOPS_MATH_SERVER_H
#define SYSOPS_MATH_SERVER_H

#include <stdint-gcc.h>
#include <netinet/in.h>

#define TRACKER_SLEEP_SEC 60
#define POLL_TIMEOUT 1000

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

int is_unique_name(char *name);

struct tlv_msg *create_task(BinaryOperation_t *operation, size_t *msgSize);

struct tlv_msg *create_type_message(size_t *msgSize, uint8_t type);

struct tlv_msg *read_response(int socket, struct sockaddr *src_addr, socklen_t addrlen);

void *send_message_and_read_response(int desc, struct tlv_msg *msg, size_t msgSize, struct sockaddr *address,
                                     int maxTries, unsigned int sleepTime);

void *process_task_routine(void *operation);

void ping_available_workers();

#endif //SYSOPS_MATH_SERVER_H
