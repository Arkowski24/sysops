//
// Created by Arkadiusz Placha on 30.03.2018.
//

#ifndef SYSOPS_BATCH_EXECUTER_H
#define SYSOPS_BATCH_EXECUTER_H

#endif //SYSOPS_BATCH_EXECUTER_H

#include <stdio.h>

int execute_command(char *program, char *args[]);

char **fetch_command(FILE *file);

int main(int argc, char *argv[]);
