//
// Created by Arkadiusz Placha on 12.05.2018.
//

#ifndef SYSOPS_BARBER_H
#define SYSOPS_BARBER_H

#include <unistd.h>
#include <limits.h>

void sigterm_handle(int sig);

void initialize_resources(size_t queueLength);

void free_resources();

pid_t get_client();

int main(int argc, char *argv[]);

#endif //SYSOPS_BARBER_H
