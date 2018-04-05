//
// Created by Arkadiusz Placha on 04.04.18.
//

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include "signal_tester.h"
#include <stdio.h>

volatile int canProceed = 0;

void proceed_handler(int sig) {
    canProceed = 1;
}

void can_request_handler(int sig) {
    if (canProceed == 0) {
        int parentPID = getppid();
        kill(parentPID, SIGUSR1);
        pause();
    }
}

void unset_parent_handlers() {
    //Ignore user signals to prevent process from being woken up by parent
    signal(SIGINT, SIG_DFL);
    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);
}

void set_proceed_handler() {
    struct sigaction proceed;
    sigset_t newMask;
    sigemptyset(&newMask);
    sigaddset(&newMask, SIGUSR2);

    proceed.sa_handler = proceed_handler;
    proceed.sa_flags = 0;
    proceed.sa_mask = newMask;
    sigaction(SIGUSR1, &proceed, NULL);
}

void set_request_handler() {
    struct sigaction proceed;
    sigset_t newMask;
    sigemptyset(&newMask);

    proceed.sa_handler = can_request_handler;
    proceed.sa_flags = 0;
    proceed.sa_mask = newMask;
    sigaction(SIGUSR2, &proceed, NULL);
}

void set_child_handlers() {
    set_proceed_handler();
    set_request_handler();
}

void send_RTS_signal() {
    int parentPID = getppid();
    int signalNumber = rand() % (SIGRTMAX - SIGRTMIN + 1);
    kill(parentPID, SIGRTMIN + signalNumber);
}

void child_work() {
    unset_parent_handlers();
    srand((unsigned int) time(NULL) * getpid());

    unsigned int sleepTime = (unsigned int) rand() % 11;
    sleep(sleepTime);

    set_child_handlers();
    while (canProceed == 0) {
        pause();
    }
    send_RTS_signal();

    exit(sleepTime);
}
