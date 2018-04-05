//
// Created by farald on 06.04.18.
//

#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "comm_tester.h"

#define RTS_SIGN 1
#define RTS_END 1

unsigned long type = 0;
unsigned long L = 0;

unsigned long sentSignals = 0;
unsigned long receivedSignals = 0;
volatile int canSendNext = 1;
pid_t childPID;

void par_sign_handler(int sig) {
    receivedSignals++;
    canSendNext = 1;
}

void par_end_handler(int sig) {
    if (type == 3) {
        kill(childPID, RTS_END);
    } else {
        kill(childPID, SIGUSR2);
    }
    wait(NULL);
    exit(EXIT_SUCCESS);
}

void par_send_msg() {
    assert(type < 4 && type != 0);

    if (type == 1) {
        if (kill(childPID, SIGUSR1) == 0) { sentSignals++; }
    } else if (type == 2) {
        if (kill(childPID, SIGUSR1) == 0) { sentSignals++; }
        canSendNext = 0;

        while (canSendNext == 0) {
            pause();
        }
    } else {
        if (kill(childPID, RTS_SIGN) == 0) { sentSignals++; }
    }
}

void par_send_end() {
    assert(type < 4 && type != 0);

    if (type == 3) {
        kill(childPID, RTS_END);
    } else {
        kill(childPID, SIGUSR2);
    }
}

void par_set_handlers() {
    sigset_t sigset;
    sigfillset(&sigset);
    sigdelset(&sigset, SIGUSR1);
    sigdelset(&sigset, RTS_SIGN);

    //sigprocmask(SIG_BLOCK, &sigset, NULL);
    if (type == 3) {
        signal(RTS_SIGN, par_sign_handler);
    } else {
        signal(SIGUSR1, par_sign_handler);
    }
    signal(SIGINT, par_end_handler);
}

void parent_work() {
    for (int i = 0; i < L; ++i) {
        par_send_msg();
    }
    par_send_end();

    printf("Parent sent %lu signals.\n", sentSignals);
    printf("Child sent back %lu signals.\n", receivedSignals);
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "%s\n", "Not enough arguments.");
        exit(EXIT_FAILURE);
    }

    L = strtoul(argv[1], NULL, 0);
    type = strtoul(argv[2], NULL, 0);
    par_set_handlers();

    childPID = fork();
    if (childPID == 0) {
        if (type == 3) {
            child_work(RTS_SIGN, RTS_END);
        } else {
            child_work(SIGUSR1, SIGUSR2);
        }
    } else {
        parent_work();
    }
    return 0;
}