//
// Created by Arkadiusz Placha on 25.04.2018.
//

#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "../msg_service.h"
#include "msg_client.h"

mqd_t serverQueueID = -1;
mqd_t queueID = -1;

void print_error_and_exit(int errorNumber) {
    fprintf(stderr, "%s \n", strerror(errorNumber));
    exit(EXIT_FAILURE);
}

void send_msg(long type, char *text) {
    struct cmd_msg msg;
    msg.mpid = getpid();
    msg.mtype = type;
    strncpy(msg.mtext, text, STR_LENGTH - 1);
    msg.mtext[STR_LENGTH - 1] = '\0';

    mq_send(serverQueueID, (char *) &msg, MSG_LENGTH, MSG_PRIORITY);
}

void open_public_queue() {
    char *name = PUBLIC_QUEUE_NAME;

    serverQueueID = mq_open(name, O_WRONLY | O_NONBLOCK);
    if (serverQueueID == -1) { print_error_and_exit(errno); }
}

void create_name(char *str) {
    pid_t pid = getpid();
    snprintf(str, STR_LENGTH, "/client.%i", pid);
}

void open_queue() {
    char name[STR_LENGTH];
    create_name(name);

    struct mq_attr attr;
    attr.mq_msgsize = MSG_LENGTH;
    attr.mq_maxmsg = MAX_MSG_IN_QUEUE;

    queueID = mq_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRWXU, &attr);
    if (queueID == -1) { print_error_and_exit(errno); }

    send_msg(MSG_CONNECT, name);

    fetch_response();
}

void close_queue() {
    if (queueID == -1) { return; }

    char name[STR_LENGTH];
    create_name(name);
    mq_close(queueID);
    mq_unlink(name);
    send_msg(MSG_STOP, "PROCESS ENDS.");
}

int get_type(char *text) {
    if (strncmp(text, "MIRROR", 6) == 0) {
        return MSG_MIRROR;
    } else if (strncmp(text, "CALC", 4) == 0) {
        return MSG_CALC;
    } else if (strncmp(text, "TIME", 4) == 0) {
        return MSG_TIME;
    } else if (strncmp(text, "END", 3) == 0) {
        return MSG_END;
    } else {
        return MSG_UNKNOWN;
    }
}

void fetch_response() {
    struct cmd_msg msg;
    memset(&msg, 0, sizeof(struct cmd_msg));
    if (mq_receive(queueID, (char *) &msg, MSG_LENGTH, NULL) == -1) { print_error_and_exit(errno); }

    printf("%s\n", msg.mtext);
}

void process_input() {
    char *text = NULL;
    size_t size = 0;
    if (getline(&text, &size, stdin) <= 0) { exit(EXIT_SUCCESS); }
    int type = get_type(text);

    switch (type) {
        case MSG_MIRROR:
            send_msg(MSG_MIRROR, text + 7);
            break;
        case MSG_CALC:
            send_msg(MSG_CALC, text + 5);
            break;
        case MSG_TIME:
            send_msg(MSG_TIME, text + 5);
            break;
        case MSG_END:
            send_msg(MSG_END, text + 4);
            break;
        default:
            printf("ERROR: Unknown command.\n");
            break;
    }

    if (type != MSG_UNKNOWN && type != MSG_END) {
        fetch_response();
    }
}

void exit_normally(int sig) {
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, exit_normally);
    atexit(close_queue);
    open_public_queue();
    open_queue();

    while (1) {
        process_input();
    }
}