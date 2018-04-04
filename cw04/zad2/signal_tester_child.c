//
// Created by farald on 04.04.18.
//

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "signal_tester.h"

volatile int canProceed = 0;

void proceed_handler(int signal) {
    canProceed = 1;
}

void child_work() {
    unsigned int sleepTime = (unsigned int) rand() % 11;
    int parentPID = getppid();
    sleep(sleepTime);
    kill(parentPID, SIGUSR1);
    while (!canProceed) {
        pause();
    }
    int signalNumber = rand() % (SIGRTMAX - SIGRTMIN + 1);
    kill(parentPID, SIGRTMIN + signalNumber);
    exit(sleepTime);
}

void set_child_handlers() {
    signal(SIGUSR2, proceed_handler);
    signal(SIGINT, SIG_DFL);
    signal(SIGUSR1, SIG_DFL);
}