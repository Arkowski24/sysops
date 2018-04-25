//
// Created by Arkadiusz Placha on 25.04.2018.
//

#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include "../msg_service.h"
#include "msg_client.h"

int serverQueueID = -1;
int queueID = -1;

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

    msgsnd(serverQueueID, &msg, MSG_LENGTH, IPC_NOWAIT);
}

void open_public_queue() {
    char *home = getenv("HOME");
    key_t key = ftok(home, PUBLIC_QUEUE_ID);
    if(key == -1) {print_error_and_exit(errno);}

    serverQueueID = msgget(key, S_IWUSR);
    if (serverQueueID < 0) { print_error_and_exit(errno); }
}

void open_queue() {
    char *home = getenv("HOME");
    pid_t pid = getpid();
    key_t queueKey = ftok(home, pid);

    queueID = msgget(queueKey, S_IRWXU | IPC_CREAT | IPC_EXCL);
    if (queueID < 0) { print_error_and_exit(errno); }

    char text[STR_LENGTH];
    snprintf(text, STR_LENGTH, "%i", queueKey);
    send_msg(MSG_CONNECT, text);

    fetch_response();
}

void close_queue() {
    if (queueID == -1) { return; }
    msgctl(queueID, IPC_RMID, NULL);
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
    if (msgrcv(queueID, &msg, MSG_LENGTH, 0, 0) == -1) { print_error_and_exit(errno); }

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
        case MSG_UNKNOWN:
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