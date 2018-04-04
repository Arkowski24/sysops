//
// Created by Arkadiusz Placha on 03.04.2018.
//

#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <printf.h>
#include <stdio.h>
#include <time.h>
#include <wait.h>
#include "signal_tester.h"

size_t childrenCount = 0;
pid_t *childrenPID;

unsigned long requiredRequests = 0;
unsigned long receivedRequests = 0;
unsigned long terminatedChildren = 0;
pid_t *waitingChildren;

void kill_children(){
    for (int i = 0; i < childrenCount; ++i) {
        kill(childrenPID[i], SIGINT);
    }
}

void children_terminates(int sig){
    int result;
    wait(&result);
    printf("%i\n", result);

    terminatedChildren++;
    if(terminatedChildren == childrenCount){
        exit(EXIT_SUCCESS);
    }
}

void proceed_request_handler(int sig, siginfo_t *info, void *uncontext){
    requiredRequests++;
    if(receivedRequests > requiredRequests){
        kill(info->si_pid, SIGUSR2);
    } else if (receivedRequests == requiredRequests){
        for (int i = 0; i < receivedRequests - 1; ++i) {
            kill(waitingChildren[i], SIGUSR2);
        }
        kill(info->si_pid, SIGUSR2);
    } else {
        waitingChildren[receivedRequests - 1] = info->si_pid;
    }
}

void interrupt_handler(int sig){
    kill_children();
    exit(EXIT_SUCCESS);
}

void rts_handle(int sig){
    printf("WoWe.\n");
}

void set_parent_handlers(){
    struct sigaction proceed_request = {0};
    proceed_request.sa_sigaction = proceed_request_handler;
    proceed_request.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigaction(SIGUSR1, &proceed_request, NULL);

    signal(SIGINT, interrupt_handler);
    for (int i = SIGRTMIN; i <= SIGRTMAX; ++i) {
        signal(i, rts_handle);
    }
    signal(SIGCHLD, children_terminates);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    if(argc < 3) {
        exit(EXIT_FAILURE);
    }

    childrenCount = strtoul(argv[1], NULL, 0);
    requiredRequests = strtoul(argv[2], NULL, 0);
    childrenPID = calloc(childrenCount, sizeof(pid_t));
    waitingChildren = calloc(childrenCount, sizeof(pid_t));

    set_parent_handlers();
    for (int i = 0; i < childrenCount; ++i) {
        childrenPID[i] = fork();
        if(childrenPID[i] == 0){
            set_child_handlers();
            child_work();
        } else {
            while (1) {
                pause();
            }
        }
    }

}