//
// Created by Arkadiusz Placha on 25.04.2018.
//

#ifndef SYSOPS_MSG_CLIENT_H
#define SYSOPS_MSG_CLIENT_H

int main(int argc, char *argv[]);

void send_msg(long type, char *text);

void fetch_response();

#endif //SYSOPS_MSG_CLIENT_H
