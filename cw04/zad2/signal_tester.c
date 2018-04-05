//
// Created by Arkadiusz Placha on 03.04.2018.
//

#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include "signal_tester.h"

extern unsigned long createdChildren;
extern unsigned long requestsToProceed;

int main(int argc, char *argv[]) {
    if(argc < 3) {
        exit(EXIT_FAILURE);
    }

    createdChildren = strtoul(argv[1], NULL, 0);
    requestsToProceed = strtoul(argv[2], NULL, 0);

    parent_work();
}