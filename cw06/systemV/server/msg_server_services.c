//
// Created by Arkadiusz Placha on 22.04.2018.
//

#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include "../msg_service.h"
#include "msg_server.h"

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

int find_desc(pid_t clientPID){
    for (int i = 0; i < CLIENTS_LIMIT; ++i) {
        if (clients[i] != NULL && clients[i]->cPID == clientPID) {
            return clients[i]->cQueue;
        }
    }
    return -1;
}

void send_pure_msg(pid_t clientPID, struct cmd_msg * msg){
    int desc = find_desc(clientPID);
    msgsnd(desc, msg, MSG_LENGTH, IPC_NOWAIT);
}

void send_msg(pid_t clientPID, long type, char* text) {
    struct cmd_msg msg;
    msg.mpid = getpid();
    msg.mtype = type;
    strncpy(msg.mtext, text, MSG_LENGTH - 1);
    msg.mtext[MSG_LENGTH - 1] = '\0';

    send_pure_msg(clientPID, &msg);
}

void service_connect(pid_t clientPID, char *str) {
    key_t clientKey = (key_t) strtol(str, NULL, 0);
    int desc = msgget(clientKey, IPC_R);
    if(desc != -1) {
        int id = add_new_client(clientKey, desc);
        if(id != -1) {

        }
    }

}

void service_mirror(char *str, size_t length) {
    for (int i = 0; i < length / 2; ++i) {
        char tmp = str[i];
        str[i] = str[length - i];
        str[length - i] = tmp;
    }
}

void service_time() {
    time_t timeNow = time(NULL);
    return ctime(&timeNow);
}

void service_calc(char *calculations) {
    size_t length = strlen(calculations);
    if (length < 7) { return "ERROR: Not enough arguments."; }

    double A;
    double B;
    double C;

    int parsed = sscanf(calculations + 4, "%lf %lf", &A, &B);
    if (parsed != 2) { return "ERROR: Bad arguments."; }

    calculations[3] = '\0';

    if (strcmp(calculations, "ADD") == 0) { C = A + B; }
    else if (strcmp(calculations, "MUL") == 0) { C = A * B; }
    else if (strcmp(calculations, "SUB") == 0) { C = A - B; }
    else if (strcmp(calculations, "DIV") == 0) { C = A / B; }
    else { return "ERROR: Unknown command."; }

    char *result = calloc(MAX_NUMBER_LENGTH + 1, sizeof(char));
    sprintf(result, "%lf", C);
    return result;
}

void service_end() {

}