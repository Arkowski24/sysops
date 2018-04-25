//
// Created by Arkadiusz Placha on 21.04.2018.
//
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include "../msg_service.h"
#include "msg_server.h"

int publicQueueID = -1;
int continueFetch = 1;
struct client *clients[CLIENTS_LIMIT] = {0};

void print_error_and_exit(int errorNumber) {
    fprintf(stderr, "%s \n", strerror(errorNumber));
    exit(EXIT_FAILURE);
}

void create_queue() {
    char *home = getenv("HOME");
    key_t key = ftok(home, PUBLIC_QUEUE_ID);

    publicQueueID = msgget(key, S_IRWXU | IPC_CREAT | IPC_EXCL);
    if (publicQueueID == -1) { print_error_and_exit(errno); }
}

void fetch_command() {
    struct cmd_msg msg;
    memset(&msg, 0, sizeof(struct cmd_msg));
    if (continueFetch) {
        if (msgrcv(publicQueueID, &msg, MSG_LENGTH, 0, 0) == -1) { print_error_and_exit(errno); }
    } else {
        if (msgrcv(publicQueueID, &msg, MSG_LENGTH, 0, IPC_NOWAIT) == -1) {
            int error = errno;
            if (error == ENOMSG) { return; }
            else { print_error_and_exit(error); }
        }
    }

    printf("Recieved msg from %i\n.", msg.mpid);

    switch (msg.mtype) {
        case MSG_CONNECT:
            service_connect(msg.mpid, msg.mtext);
            break;
        case MSG_MIRROR:
            service_mirror(msg.mpid, msg.mtext);
            break;
        case MSG_CALC:
            service_calc(msg.mpid, msg.mtext);
            break;
        case MSG_TIME:
            service_time(msg.mpid);
            break;
        case MSG_END:
            service_end();
            break;
        case MSG_STOP:
            service_stop(msg.mpid);
            break;
        default:
            break;
    }
}

void close_queue() {
    if (publicQueueID == -1) { return; }
    msgctl(publicQueueID, IPC_RMID, NULL);
}

int main(int argc, char *argv[]) {
    atexit(close_queue);
    create_queue();
    while (continueFetch) {
        fetch_command();
    }
    return 0;
}