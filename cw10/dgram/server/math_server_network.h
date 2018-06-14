//
// Created by farald on 10.06.18.
//

#ifndef SYSOPS_MATH_SERVER_NETWORK_H
#define SYSOPS_MATH_SERVER_NETWORK_H

#include "math_server.h"
#include <netinet/in.h>

#define MAX_NETWORK_CLIENTS 32

#define NETWORK_TIMEOUT 100
#define MAX_NETWORK_TRIES 10
#define NETWORK_USLEEP_VAL 1000

struct {
    char *name;
    struct sockaddr_in address;
} typedef ClientNetwork_t;

void network_socket_init(uint16_t port);

int is_present_network(int index);

int is_unique_name_network(char *name);

void deregister_network_client_i(int index);

void process_network_messages();

void *network_task_routine(int index, BinaryOperation_t *operation);

void *network_ping_routine(int index, uint8_t msgType);

#endif //SYSOPS_MATH_SERVER_NETWORK_H
