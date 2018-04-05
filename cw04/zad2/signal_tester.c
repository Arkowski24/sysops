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
    if (argc < 3) {
        fprintf(stderr, "%s\n", "Not enough arguments.");
        exit(EXIT_FAILURE);
    }

    createdChildren = strtoul(argv[1], NULL, 0);
    requestsToProceed = strtoul(argv[2], NULL, 0);
    if (requestsToProceed > createdChildren) {
        fprintf(stderr, "%s\n", "Approvals required cannot be higher than number of created children.");
        exit(EXIT_FAILURE);
    }

    parent_work();
    return 0;
}