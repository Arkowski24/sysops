//
// Created by farald on 10.06.18.
//

#ifndef SYSOPS_MATH_SERVER_LOCAL_H
#define SYSOPS_MATH_SERVER_LOCAL_H

#include "math_server.h"
#include <sys/un.h>

#define MAX_LOCAL_CLIENTS 32

#define LOCAL_TIMEOUT 100
#define MAX_LOCAL_TRIES 10
#define LOCAL_USLEEP_VAL 1000

struct {
    char *name;
    struct sockaddr_un address;
} typedef ClientLocal_t;

void local_socket_init(char *socketPath);

int is_present_local(int index);

int is_unique_name_local(char *name);

void deregister_local_client_i(int index);

void process_local_messages();

void *local_task_routine(int index, BinaryOperation_t *operation);

void *local_ping_routine(int index, uint8_t msgType);

#endif //SYSOPS_MATH_SERVER_LOCAL_H
