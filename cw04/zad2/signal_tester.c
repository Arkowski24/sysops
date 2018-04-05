//
// Created by Arkadiusz Placha on 03.04.2018.
//

#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
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

void child_terminates_handler(int sig){
    int result;
    pid_t childPID;

    childPID = waitpid(-1, &result, WNOHANG);
    while (childPID > 0){
        terminatedChildren++;
        printf("Child %i: terminated with exit code %i.\n", childPID, WEXITSTATUS(result));
        childPID = waitpid(-1, &result, WNOHANG);
    }

    if(terminatedChildren == childrenCount){
        exit(EXIT_SUCCESS);
    }
}

void proceed_request_handler(int sig, siginfo_t *info, void *uncontext){
    receivedRequests++;
    printf("Child %i: received proceed request (SIGUSR1).\n", info->si_pid);

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
    printf("Received RTS signal: %i \n", sig);
}


void set_proceed_request_handler(){
    struct sigaction proceed_request;
    sigset_t newMask;
    sigemptyset(&newMask);
    sigaddset(&newMask, SIGUSR1);

    proceed_request.sa_sigaction = proceed_request_handler;
    proceed_request.sa_flags = SA_SIGINFO;
    proceed_request.sa_mask = newMask;
    sigaction(SIGUSR1, &proceed_request, NULL);
}

void set_child_terminates_handler(){
    struct sigaction proceed_request;
    sigset_t newMask;
    sigemptyset(&newMask);
    sigaddset(&newMask, SIGUSR1);

    proceed_request.sa_handler = child_terminates_handler;
    proceed_request.sa_flags = 0;
    proceed_request.sa_mask = newMask;
    sigaction(SIGCHLD, &proceed_request, NULL);
}


void set_parent_handlers(){
    set_proceed_request_handler();
    set_child_terminates_handler();

    signal(SIGINT, interrupt_handler);
    for (int i = SIGRTMIN; i <= SIGRTMAX; ++i) {
        signal(i, rts_handle);
    }
}

pid_t create_child(){
    pid_t childrenPID = fork();
    if(childrenPID == 0){
        child_work();
    }
    return childrenPID;
}

int main(int argc, char *argv[]) {
    if(argc < 3) {
        exit(EXIT_FAILURE);
    }

    childrenCount = strtoul(argv[1], NULL, 0);
    requiredRequests = strtoul(argv[2], NULL, 0);
    childrenPID = calloc(childrenCount, sizeof(pid_t));
    waitingChildren = calloc(childrenCount, sizeof(pid_t));

    set_parent_handlers();
    for (int i = 0; i < childrenCount; ++i) {
        childrenPID[i] = create_child();
        printf("Created child with PID %i.\n", childrenPID[i]);
    }
    while (1){
        pause();
    }
}