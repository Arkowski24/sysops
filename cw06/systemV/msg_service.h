//
// Created by Arkadiusz Placha on 22.04.2018.
//
#include <unistd.h>

#ifndef SYSOPS_MSG_SERVICE_H
#define SYSOPS_MSG_SERVICE_H

#define MSG_CONNECT 0
#define MSG_MIRROR 1
#define MSG_CALC 2
#define MSG_TIME 3
#define MSG_END 4
#define MSG_STOP 5
#define MSG_RESPONSE 6

#define PUBLIC_QUEUE_ID 46
#define PRIVATE_QUEUE_ID_MIN 1
#define PRIVATE_QUEUE_ID_MAX 100
#define CLIENTS_LIMIT (PRIVATE_QUEUE_ID_MAX - PRIVATE_QUEUE_ID_MIN + 1)
#define STR_LENGTH 256
#define MSG_LENGTH (sizeof(pid_t) + STR_LENGTH * sizeof(char))

struct cmd_msg {
    long mtype;
    pid_t mpid;
    char mtext[STR_LENGTH]; //Last char must be /0
};

#endif //SYSOPS_MSG_SERVICE_H
