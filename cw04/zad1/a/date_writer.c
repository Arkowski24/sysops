//
// Created by Arkadiusz Placha on 02.04.2018.
//

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "date_writer.h"
#include <sys/time.h>
#include <time.h>

int continueExecution = 1;

void stop_signal_handler(int signal) {
    if (continueExecution) {
        fprintf(stdout, "Oczekuje na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu.\n");
        continueExecution = 0;
    } else {
        continueExecution = 1;
    }
}

void interrupt_signal_handler(int signal) {
    fprintf(stdout, "Odebrano sygnal SIGINT.\n");
    exit(EXIT_SUCCESS);
}

void print_date() {
    time_t rawTime;
    time(&rawTime);
    printf("%s", ctime(&rawTime));
}

void set_interrupt_handler() {
    signal(SIGINT, interrupt_signal_handler);
}

void set_stop_handler() {
    struct sigaction signalAction;
    sigset_t newMask;
    sigemptyset(&newMask);

    signalAction.sa_handler = stop_signal_handler;
    signalAction.sa_flags = 0;
    signalAction.sa_mask = newMask;
    sigaction(SIGTSTP, &signalAction, NULL);
}

int main(int argc, char *argv[]) {
    set_interrupt_handler();
    set_stop_handler();
    while (1) {
        if(continueExecution){
            print_date();
            sleep(1);
        }
    }

    return 0;
}