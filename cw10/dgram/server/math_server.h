//
// Created by farald on 07.06.18.
//

#ifndef SYSOPS_MATH_SERVER_H
#define SYSOPS_MATH_SERVER_H

#include <stdint.h>

#define MESSAGE_TYPE_REGISTER 0x01
#define MESSAGE_TYPE_DEREGISTER 0x02
#define MESSAGE_TYPE_STATUS 0x03
#define MESSAGE_TYPE_REQUEST 0x04
#define MESSAGE_TYPE_RESPONSE 0x05
#define MESSAGE_TYPE_PING 0x06
#define MESSAGE_TYPE_PONG 0x07

#define STATUS_OK 0x00
#define ERROR_NAME_TAKEN 0x02
#define ERROR_UNK_OPERATION 0x03

#define POLL_TIMEOUT 2000
#define PING_THREAD_SLEEP 120

#define PING_TIMEOUT 600
#define OPERATION_TIMEOUT 600
#define ALL_CLIENTS_MAX 64

#define MAX_OTHER_MESSAGES 256
#define MAX_MESSAGES_IN_BATCH 10
#define PROCESS_MSG_SLEEP 100

struct {
    char operator;
    double op1;
    double op2;
} typedef BinaryOperation_t;

struct {
    u_int8_t type;
    size_t length;
    uint8_t value[];
} typedef tlvMsg_t;

#endif //SYSOPS_MATH_SERVER_H
