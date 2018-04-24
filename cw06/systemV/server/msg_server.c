//
// Created by Arkadiusz Placha on 21.04.2018.
//
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "../msg_service.h"
#include "msg_server.h"

int publicQueueID;
struct client *clients[CLIENTS_LIMIT] = {0};

void print_error_and_exit(int errorNumber) {
    fprintf(stderr, "%s \n", strerror(errorNumber));
    exit(EXIT_FAILURE);
}

void create_queue() {
    char *home = getenv("HOME");
    key_t key = ftok(home, PUBLIC_QUEUE_ID);

    publicQueueID = msgget(key, IPC_CREAT | IPC_EXCL | 0400);
    if (publicQueueID < 0) { print_error_and_exit(errno); }
}

void fetch_command() {
    struct cmd_msg msg;
    memset(&msg, 0, sizeof(struct cmd_msg));
    if (msgrcv(publicQueueID, &msg, MSG_LENGTH, 0, 0) == -1) { print_error_and_exit(errno); }

    switch (msg.mtype) {
        case MSG_CONNECT:
            service_connect(msg.mtext);
            pid_t
            break;
        case MSG_MIRROR:
            service_connect();
            break;
        case MSG_CALC:
            service_connect();
            break;
        case MSG_TIME:
            service_connect();
            break;
        case MSG_END:
            service_connect();
            break;
        case MSG_STOP:
            service_connect();
            break;
        default:
            break;
    }
}

int main(int argc, char *argv[]) {

}