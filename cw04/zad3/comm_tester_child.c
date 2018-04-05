//
// Created by farald on 06.04.18.
//

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "comm_tester.h"

static int receivedSignals = 0;

void sign_handler(int sig) {
    receivedSignals++;
    int parentPID = getppid();
    kill(parentPID, sig);
}

void end_handler(int sig) {
    printf("Child received %i signals.\n", receivedSignals);
    exit(EXIT_SUCCESS);
}

void set_handlers(int signSignal, int endSignal) {
    sigset_t sigset;
    sigfillset(&sigset);
    sigdelset(&sigset, signSignal);
    sigdelset(&sigset, endSignal);

    //sigprocmask(SIG_BLOCK, &sigset, NULL);
    signal(signSignal, sign_handler);
    signal(endSignal, end_handler);
}

void child_work(int signSignal, int endSignal) {
    set_handlers(signSignal, endSignal);
    while (1) {
        pause();
    }
}