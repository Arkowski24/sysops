//
// Created by Arkadiusz Placha on 02.04.2018.
//

#include <zconf.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int continueExecution = 0;
int childID = 0;

void stop_signal_handler(int signal) {
    if(continueExecution == 0){
        fprintf(stdout, "Oczekuje na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu.\n");
        continueExecution = 1;
    } else {
        continueExecution = 0;
    }
    return;
}

void interrupt_signal_handler(int signal) {
    fprintf(stdout, "Odebrano sygnal SIGINT.\n");
    kill(childID, SIGINT);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    signal(SIGTSTP, stop_signal_handler);
    signal(SIGINT, interrupt_signal_handler);

    while (1) {
        if(continueExecution == 1){
            pause();
        }

        childID = fork();
        if (childID == 0) {
            execlp("date", "-R", NULL);
        } else {
            wait(NULL);
        }
        sleep(1);
    }
}