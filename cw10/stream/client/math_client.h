//
// Created by farald on 16.06.18.
//

#ifndef SYSOPS_MATH_CLIENT_H
#define SYSOPS_MATH_CLIENT_H

#include <stdint.h>

#define MESSAGE_TYPE_REGISTER 0x01
#define MESSAGE_TYPE_DEREGISTER 0x02
#define MESSAGE_TYPE_STATUS 0x03
#define MESSAGE_TYPE_REQUEST 0x04
#define MESSAGE_TYPE_RESPONSE 0x05
#define MESSAGE_TYPE_PING 0x06
#define MESSAGE_TYPE_PONG 0x07


#define STATUS_OK 0x00
#define ERROR_REG_FIRST 0x01
#define ERROR_NAME_TAKEN 0x02
#define ERROR_UNK_OPERATION 0x03

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


#endif //SYSOPS_MATH_CLIENT_H
