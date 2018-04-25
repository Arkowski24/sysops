//
// Created by Arkadiusz Placha on 22.04.2018.
//

#ifndef SYSOPS_MSG_SERVER_H
#define SYSOPS_MSG_SERVER_H

struct client {
    pid_t cPID;
    mqd_t cQueue;
};

void service_connect(pid_t clientPID, char *str);

void service_mirror(pid_t clientPID, char *str);

void service_time(pid_t clientPID);

void service_calc(pid_t clientPID, char *str);

void service_end();

void service_stop(pid_t clientPID);

#endif //SYSOPS_MSG_SERVER_H
