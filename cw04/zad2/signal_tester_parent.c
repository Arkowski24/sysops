//
// Created by Arkadiusz Placha on 05.04.18.
//

#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "signal_tester.h"

pid_t processGroupID;
unsigned long createdChildren = 0;
unsigned long terminatedChildren = 0;

pid_t *childrenQueue;
unsigned long requestsToProceed = 0;
unsigned long receivedRequests = 0;

void child_terminates_handler(int sig) {
    int result;
    pid_t childPID;

    childPID = waitpid(-1, &result, WNOHANG);
    while (childPID > 0) {
        terminatedChildren++;
        if (TEST_PRINT_CHILD_EXIT) {
            if (WIFEXITED(result)) {
                printf("Child PID %i: terminated normally with exit code %i.\n", childPID, WEXITSTATUS(result));
            } else {
                printf("Child PID %i: terminated unexpectedly with exit code %i.\n", childPID, WEXITSTATUS(result));
            }
        }
        childPID = waitpid(-1, &result, WNOHANG);
    }

    if (terminatedChildren == createdChildren) {
        exit(EXIT_SUCCESS);
    }
}

void give_approval(pid_t processID) {
    if (TEST_PRINT_APPROVAL) {
        printf("Child %i: approval granted.\n", processID);
    }
    kill(processID, SIGUSR1);
}

void proceed_request_handler(int sig, siginfo_t *info, void *uncontext) {
    receivedRequests++;
    if (TEST_PRINT_REQUEST) {
        printf("Child %i: received proceed request (SIGUSR1).\n", info->si_pid);
    }
    if (receivedRequests > requestsToProceed) {
        give_approval(info->si_pid);
    } else if (receivedRequests == requestsToProceed) {
        for (int i = 0; i < receivedRequests - 1; ++i) {
            give_approval(childrenQueue[i]);
        }
        give_approval(info->si_pid);
    } else {
        childrenQueue[receivedRequests - 1] = info->si_pid;
    }
}

void kill_children() {
    kill(-processGroupID, SIGINT);
}

void interrupt_handler(int sig) {
    kill_children();
    exit(EXIT_SUCCESS);
}

void rts_handle(int sig) {
    if (TEST_PRINT_RTS) {
        printf("Received RTS signal: %i \n", sig);
    }
}

void set_proceed_request_handler() {
    struct sigaction proceed_request;
    sigset_t newMask;
    sigemptyset(&newMask);

    proceed_request.sa_sigaction = proceed_request_handler;
    proceed_request.sa_flags = SA_SIGINFO;
    proceed_request.sa_mask = newMask;
    sigaction(SIGUSR1, &proceed_request, NULL);
}

void set_child_terminates_handler() {
    struct sigaction proceed_request;
    sigset_t newMask;
    sigemptyset(&newMask);
    sigaddset(&newMask, SIGUSR1);
    for (int i = SIGRTMIN; i <= SIGRTMAX; ++i) {
        sigaddset(&newMask, i);
    }

    proceed_request.sa_handler = child_terminates_handler;
    proceed_request.sa_flags = 0;
    proceed_request.sa_mask = newMask;
    sigaction(SIGCHLD, &proceed_request, NULL);
}

void set_RTS_handle() {
    struct sigaction rts_act;
    sigset_t newMask;
    sigemptyset(&newMask);
    sigaddset(&newMask, SIGUSR1);


    rts_act.sa_handler = rts_handle;
    rts_act.sa_flags = 0;
    rts_act.sa_mask = newMask;

    for (int i = SIGRTMIN; i <= SIGRTMAX; ++i) {
        sigaction(i, &rts_act, NULL);
    }
}

void set_parent_handlers() {
    set_proceed_request_handler();
    set_child_terminates_handler();
    signal(SIGINT, interrupt_handler);
    signal(SIGUSR2, SIG_IGN);

    set_RTS_handle();
}

void create_children_queue() {
    childrenQueue = calloc(createdChildren, sizeof(pid_t));
}

pid_t create_child() {
    pid_t childrenPID = fork();
    if (childrenPID == 0) {
        child_work();
    }
    return childrenPID;
}

void send_readiness_signal() {
    kill(-processGroupID, SIGUSR2);
}

void parent_work() {
    setpgid(0, 0);
    processGroupID = getpid();

    create_children_queue();
    set_parent_handlers();

    for (int i = 0; i < createdChildren; ++i) {
        pid_t childPID = create_child();
        if (TEST_PRINT_CREATION) {
            printf("Created child with PID %i.\n", childPID);
        }
    }

    while (1) {
        send_readiness_signal();
    }
}