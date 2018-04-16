//
// Created by Arkadiusz Placha on 30.03.2018.
//

#ifndef SYSOPS_BATCH_EXECUTER_H
#define SYSOPS_BATCH_EXECUTER_H

#include <stdio.h>

int execute_line_of_commands(char *args[]);

char **fetch_commands(FILE *file);

int main(int argc, char *argv[]);

#endif //SYSOPS_BATCH_EXECUTER_H
