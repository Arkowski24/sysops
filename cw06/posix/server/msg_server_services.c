//
// Created by Arkadiusz Placha on 22.04.2018.
//

#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mqueue.h>
#include <sys/types.h>
#include <fcntl.h>
#include "../msg_service.h"
#include "msg_server.h"

extern int continueFetch;
extern struct client *clients[CLIENTS_LIMIT];

int add_new_client(pid_t clientPID, int qDesc) {
    for (int i = 0; i < CLIENTS_LIMIT; ++i) {
        if (clients[i] == NULL) {
            clients[i] = calloc(1, sizeof(struct client));
            clients[i]->cPID = clientPID;
            clients[i]->cQueue = qDesc;
            return i;
        }
    }
    return -1;
}

int find_desc(pid_t clientPID) {
    for (int i = 0; i < CLIENTS_LIMIT; ++i) {
        if (clients[i] != NULL && clients[i]->cPID == clientPID) {
            return clients[i]->cQueue;
        }
    }
    return -1;
}

void send_pure_msg(pid_t clientPID, struct cmd_msg *msg) {
    int desc = find_desc(clientPID);
    printf("Sending msg to desc %i, PID %i.\n", desc, clientPID);
    mq_send(desc, (char *) msg, MSG_LENGTH, MSG_PRIORITY);
}

void send_msg(pid_t clientPID, long type, char *text) {
    struct cmd_msg msg;
    msg.mpid = getpid();
    msg.mtype = type;
    strncpy(msg.mtext, text, STR_LENGTH - 1);
    msg.mtext[STR_LENGTH - 1] = '\0';

    send_pure_msg(clientPID, &msg);
}

void service_connect(pid_t clientPID, char *str) {
    int desc = mq_open(str, O_WRONLY);
    if (desc != -1) {
        int id = add_new_client(clientPID, desc);
        char text[STR_LENGTH];

        if (id != -1) { snprintf(text, STR_LENGTH, "%d", id); }
        else { sprintf(text, "ERROR: TOO MANY CONNECTED CLIENTS."); }

        send_msg(clientPID, MSG_RESPONSE, text);
    }
}

void service_mirror(pid_t clientPID, char *str) {
    size_t length = strlen(str);
    for (int i = 0; i < length / 2; ++i) {
        char tmp = str[i];
        str[i] = str[length - i - 1];
        str[length - i - 1] = tmp;
    }

    send_msg(clientPID, MSG_RESPONSE, str);
}

void service_time(pid_t clientPID) {
    time_t timeNow = time(NULL);
    char *timeStr = ctime(&timeNow);
    char text[STR_LENGTH];
    snprintf(text, STR_LENGTH, "%s", timeStr);

    send_msg(clientPID, MSG_RESPONSE, text);
}

char *perform_calc(char *calculations) {
    char *text = calloc(STR_LENGTH, sizeof(char));
    size_t length = strlen(calculations);

    if (length < 7) {
        snprintf(text, STR_LENGTH, "ERROR: Not enough arguments.");
        return text;
    }

    double A;
    double B;
    double C;

    int parsed = sscanf(calculations + 4, "%lf %lf", &A, &B);
    if (parsed != 2) {
        snprintf(text, STR_LENGTH, "ERROR: Bad arguments.");
        return text;
    }

    calculations[3] = '\0';

    if (strncmp(calculations, "ADD", 3) == 0) { C = A + B; }
    else if (strncmp(calculations, "MUL", 3) == 0) { C = A * B; }
    else if (strncmp(calculations, "SUB", 3) == 0) { C = A - B; }
    else if (strncmp(calculations, "DIV", 3) == 0) { C = A / B; }
    else {
        snprintf(text, STR_LENGTH, "ERROR: Unknown command.");
        return text;
    }

    snprintf(text, STR_LENGTH, "%lf", C);
    return text;
}

void service_calc(pid_t clientPID, char *str) {
    char *text = perform_calc(str);
    send_msg(clientPID, MSG_RESPONSE, text);
    free(text);
}

void service_end() {
    continueFetch = 0;
}

void service_stop(pid_t clientPID) {
    for (int i = 0; i < CLIENTS_LIMIT; ++i) {
        if (clients[i] != NULL && clients[i]->cPID == clientPID) {
            mq_close(clients[i]->cQueue);
            free(clients[i]);
        }
    }
}