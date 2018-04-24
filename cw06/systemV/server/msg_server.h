//
// Created by Arkadiusz Placha on 22.04.2018.
//

#ifndef SYSOPS_MSG_SERVER_H
#define SYSOPS_MSG_SERVER_H

struct client {
    pid_t cPID;
    int cQueue;
};

void service_mirror(char *str, size_t length);

char *service_time();

char *service_calc(char *calculations);

char *service_end();

char *service_connect();

#endif //SYSOPS_MSG_SERVER_H
