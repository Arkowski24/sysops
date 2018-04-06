//
// Created by farald on 06.04.18.
//

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "comm_tester.h"

int receivedSignals = 0;

void sign_handler(int sig) {
    ++receivedSignals;
    pid_t parentPID = getppid();
    kill(parentPID, sig);
}

void end_handler(int sig) {
    printf("Child received %i signals.\n", receivedSignals);
    exit(EXIT_SUCCESS);
}

void set_handlers(int signSignal, int endSignal) {
    sigset_t newMask;
    sigfillset(&newMask);
    sigdelset(&newMask, signSignal);
    sigdelset(&newMask, endSignal);
    sigprocmask(SIG_BLOCK, &newMask, NULL);

    signal(signSignal, sign_handler);
    signal(endSignal, end_handler);
}

void child_work(int signSignal, int endSignal) {
    set_handlers(signSignal, endSignal);

    pid_t parentPID = getppid();
    kill(parentPID, SIGUSR2);

    while (1) {
        pause();
    }
}