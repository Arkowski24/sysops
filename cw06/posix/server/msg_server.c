//
// Created by Arkadiusz Placha on 21.04.2018.
//

#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include "../msg_service.h"
#include "msg_server.h"

mqd_t publicQueueID = -1;
int continueFetch = 1;
struct client *clients[CLIENTS_LIMIT] = {0};

void print_error_and_exit(int errorNumber) {
    fprintf(stderr, "%s \n", strerror(errorNumber));
    exit(EXIT_FAILURE);
}

void create_queue() {
    char *name = PUBLIC_QUEUE_NAME;
    struct mq_attr attr;
    attr.mq_msgsize = MSG_LENGTH;
    attr.mq_maxmsg = MAX_MSG_IN_QUEUE;

    publicQueueID = mq_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRWXU, &attr);
    if (publicQueueID == -1) { print_error_and_exit(errno); }
}

void fetch_command() {
    struct cmd_msg msg;
    memset(&msg, 0, sizeof(struct cmd_msg));
    if (mq_receive(publicQueueID, (char *) &msg, MSG_LENGTH, NULL) == -1) {
        int error = errno;
        if (error == ENOMSG) { return; }
        else { print_error_and_exit(error); }
    }

    printf("Recieved msg %s from %i.\n", msg.mtext, msg.mpid);

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

void close_queues_and_clients() {
    if (publicQueueID == -1) { return; }
    for (int i = 0; i < CLIENTS_LIMIT; ++i) {
        if (clients[i] != NULL) {
            mq_close(clients[i]->cQueue);
            kill(clients[i]->cPID, SIGINT);
        }
    }
    mq_close(publicQueueID);
    mq_unlink(PUBLIC_QUEUE_NAME);
}

void exit_normally(int sig) {
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, exit_normally);
    atexit(close_queues_and_clients);
    create_queue();
    while (continueFetch) {
        fetch_command();
    }
    return 0;
}