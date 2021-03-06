//
// Created by Arkadiusz Placha on 30.03.2018.
//

#ifndef SYSOPS_BATCH_EXECUTER_H
#define SYSOPS_BATCH_EXECUTER_H

#include <stdio.h>
#include <sys/resource.h>

int execute_command(char *program, char *args[], unsigned int taskNumber);

char **fetch_command(FILE *file);

void set_limit(int limitType, rlim_t resourceLimit);

int main(int argc, char *argv[]);

#endif //SYSOPS_BATCH_EXECUTER_H
