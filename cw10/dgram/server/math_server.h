//
// Created by farald on 09.06.18.
//

#ifndef SYSOPS_MATH_SERVER_H
#define SYSOPS_MATH_SERVER_H

#include <stdint-gcc.h>
#include <netinet/in.h>

#define MAX_LOCAL_CLIENTS 32
#define MAX_NETWORK_CLIENTS 32

#define TYPE_LOCAL 1
#define TYPE_NETWORK 2

#define MESSAGE_TYPE_TASK 1
#define MESSAGE_TYPE_RESULT 2
#define MESSAGE_TYPE_REGISTER 3
#define MESSAGE_TYPE_DEREGISTER 4

#define LOCAL_TIMEOUT 100
#define MAX_LOCAL_TRIES 10

struct tlv_msg {
    uint8_t type;
    uint32_t length;
    uint8_t value[];
};

struct {
    char *name;
    struct sockaddr_un address;
} typedef ClientLocal_t;

struct {
    char *name;
    struct sockaddr_in address;
} typedef ClientNetwork_t;

struct {
    uint8_t operation;
    double op1;
    double op2;
} typedef BinaryOperation_t;

void insert_client(int type, int index);

void delete_client(int type, int index);

int is_present_local(int index);

int is_present_network(int index);

void *local_task_routine(int index, BinaryOperation_t *operation);

void *network_task_routine(int index, BinaryOperation_t *operation);

void deregister_local_client_i(int index);

void deregister_network_client_i(int index);

struct tlv_msg *create_task(BinaryOperation_t *operation, size_t *msgSize);

struct tlv_msg *read_response(int socket, struct sockaddr *src_addr, socklen_t addrlen, int flags);

void *task_routine(void *operation);

#endif //SYSOPS_MATH_SERVER_H
