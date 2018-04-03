//
// Created by Arkadiusz Placha on 02.04.2018.
//

#include <zconf.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int continueExecution = 0;
int executorID = 0;

void stop_signal_handler(int signal) {
    kill(executorID, SIGINT);
    if (continueExecution == 0) {
        fprintf(stdout, "Oczekuje na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu.\n");
        continueExecution = 1;
    } else {
        create_executor();
        continueExecution = 0;
    }
}

void interrupt_signal_handler(int signal) {
    kill(executorID, SIGINT);
    fprintf(stdout, "Odebrano sygnal SIGINT.\n");
    exit(EXIT_SUCCESS);
}

void print_date() {
    while (1) {
        int childID = fork();
        if (childID == 0) {
            execlp("date", "-R", NULL);
        } else {
            wait(NULL);
        }
        sleep(1);
    }
}

void create_executor() {
    executorID = fork();
    if (executorID == 0) {
        signal(SIGTSTP, SIG_DFL);
        signal(SIGINT, SIG_DFL);
        print_date();
    }
}

void set_interrupt_handler() {
    signal(SIGINT, interrupt_signal_handler);
}

void set_stop_handler() {
    struct sigaction signalAction;
    signalAction.sa_handler = stop_signal_handler;
    signalAction.sa_flags = 0;
    signalAction.sa_mask = 0;
    sigaction(SIGTSTP, &signalAction, NULL);
}

int main(int argc, char *argv[]) {
    create_executor();
    set_interrupt_handler();
    set_stop_handler();
    while (1) {
        pause();
    }
    return 0;
}