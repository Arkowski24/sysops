//
// Created by Arkadiusz Placha on 22.04.2018.
//
#include <unistd.h>

#ifndef SYSOPS_MSG_SERVICE_H
#define SYSOPS_MSG_SERVICE_H

#define MSG_UNKNOWN 9
#define MSG_CONNECT 10
#define MSG_MIRROR 11
#define MSG_CALC 12
#define MSG_TIME 13
#define MSG_END 14
#define MSG_STOP 15
#define MSG_RESPONSE 16

#define PRIVATE_QUEUE_ID_MIN 1
#define PRIVATE_QUEUE_ID_MAX 100
#define CLIENTS_LIMIT (PRIVATE_QUEUE_ID_MAX - PRIVATE_QUEUE_ID_MIN + 1)
#define STR_LENGTH 256
#define MSG_LENGTH (sizeof(long) + sizeof(pid_t) + STR_LENGTH * sizeof(char))
#define PUBLIC_QUEUE_NAME "/msg_server.public_queue"
#define MAX_MSG_IN_QUEUE 256
#define MSG_PRIORITY 2

struct cmd_msg {
    long mtype;
    pid_t mpid;
    char mtext[STR_LENGTH]; //Last char must be /0
};

#endif //SYSOPS_MSG_SERVICE_H
