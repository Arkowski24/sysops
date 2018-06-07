//
// Created by farald on 07.06.18.
//

#ifndef SYSOPS_MATH_SERVER_H
#define SYSOPS_MATH_SERVER_H

#include <stdint.h>

#define MSG_TYPE_REGISTER 0x01
#define MSG_TYPE_ERROR 0x02
#define MSG_TYPE_REQUEST 0x03

#define ERROR_REG_FIRST 0x01

struct math_operation {
    char operator;
    int32_t op1;
    int32_t op2;
};

struct tlv_msg {
    u_int8_t type;
    size_t length;
};

struct reg_request {
    int16_t length;
    char name[];
};

#endif //SYSOPS_MATH_SERVER_H
