//
// Created by Arkadiusz Placha on 03.04.2018.
//

#include <stdlib.h>
#include <zconf.h>
#include <signal.h>

volatile int can_proceed = 0;

void child_proceed(int signal) {
    can_proceed = 1;
}

void child_work() {
    unsigned int sleepTime = (unsigned int) rand() % 11;
    int parentPID = getppid();
    sleep(sleepTime);
    kill(parentPID, SIGUSR1);
    while (!can_proceed) {
        pause();
    }
    kill(parentPID, SIGUSR1);
    exit(sleepTime);
}

int main(int argc, char *argv[]) {

}