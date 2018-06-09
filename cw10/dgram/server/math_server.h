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

#endif //SYSOPS_MATH_SERVER_H
