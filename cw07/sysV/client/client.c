//
// Created by Arkadiusz Placha on 14.05.2018.
//

#include "client.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <wait.h>
#include <time.h>
#include "../fifo/circular_fifo.h"

#define FIFO_KEY_ID 10
#define BARBER_KEY_ID 20
#define ACCESS_READY_KEY_ID 30
#define CLIENT_READY_KEY_ID 40

#define TIMESTAMP_SIZE 256

CircularFifo_t *fifo;
int fifoID;
int clientReadyID;
int accessWaitingRoomID;
int barberReadyID;
int personalSemID;

ClientInfo_t clientInfo;

char timeStamp[TIMESTAMP_SIZE];

void load_fifo() {
    char *home = getenv("HOME");
    key_t fifoKey = ftok(home, FIFO_KEY_ID);

    fifoID = shmget(fifoKey, 0, 0);
    if (fifoID == -1) {
        perror("SHARED MEMORY ERROR");
        exit(EXIT_FAILURE);
    }

    fifo = shmat(fifoID, NULL, 0);
    if (fifo == (void *) -1) {
        perror("MEMORY MAP ERROR");
        exit(EXIT_FAILURE);
    }
}

void open_common_semaphores() {
    char *home = getenv("HOME");
    key_t clientReadyKey = ftok(home, CLIENT_READY_KEY_ID);
    key_t accessWaitingRoomKey = ftok(home, ACCESS_READY_KEY_ID);
    key_t barberReadyKey = ftok(home, BARBER_KEY_ID);

    clientReadyID = semget(clientReadyKey, 1, 0);
    if (clientReadyID == -1) {
        perror("SEMAPHORE ERROR (clientReady)");
        exit(EXIT_FAILURE);
    }
    accessWaitingRoomID = semget(accessWaitingRoomKey, 1, 0);
    if (accessWaitingRoomID == -1) {
        perror("SEMAPHORE ERROR (accessWaitingRoom)");
        exit(EXIT_FAILURE);
    }
    barberReadyID = semget(barberReadyKey, 1, 0);
    if (barberReadyID == -1) {
        perror("SEMAPHORE ERROR (barberReady)");
        exit(EXIT_FAILURE);
    }
}

void initialize_resources() {
    load_fifo();
    open_common_semaphores();
}

void create_personal_semaphore() {
    char *pwd = getenv("PWD");
    clientInfo.sKey = ftok(pwd, getpid());

    personalSemID = semget(clientInfo.sKey, 1, IPC_CREAT | IPC_EXCL | S_IRWXU);
    if (personalSemID == -1) {
        perror("SEMAPHORE ERROR (personalSem)");
        exit(EXIT_FAILURE);
    }

    union mySemun semNum;
    semNum.val = 0;
    semctl(personalSemID, 0, SETVAL, semNum);
}

void free_resources() {
    semctl(personalSemID, 0, IPC_RMID);
    shmdt(fifo);
}

void sem_wait(int semID) {
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = -1;
    buf.sem_flg = 0;

    semop(semID, &buf, 1);
}

void sem_post(int semID) {
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = 1;
    buf.sem_flg = 0;

    semop(semID, &buf, 1);
}

char *get_timestamp() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    snprintf(timeStamp, TIMESTAMP_SIZE, "%ld %ld", ts.tv_sec, ts.tv_nsec);
    return timeStamp;
}

void main_task() {
    sem_wait(accessWaitingRoomID);

    if (fifo_size(fifo) < fifo->qMaxSize || fifo->barberSleeping == 1) {
        if (fifo->barberSleeping == 1) {
            fifo->barberSleeping = 0;
            fifo->chair = clientInfo;

            printf("%s|PID %d: Waking up the barber.\n", get_timestamp(), clientInfo.sPid);
        } else {
            fifo_push(fifo, clientInfo);

            printf("%s|PID %d: Sitting in the waiting room.\n", get_timestamp(), clientInfo.sPid);
        }

        sem_post(accessWaitingRoomID);
        sem_post(clientReadyID);

        sem_wait(personalSemID);

        printf("%s|PID %d: Sitting in the chair.\n", get_timestamp(), clientInfo.sPid);

        sem_wait(barberReadyID);
        printf("%s|PID %d: Leaving after having hair cut.\n", get_timestamp(), clientInfo.sPid);
    } else {
        sem_post(accessWaitingRoomID);
        printf("%s|PID %d: Leaving because waiting room is full.\n", get_timestamp(), clientInfo.sPid);
    }
}

void execute_tasks(int times) {
    clientInfo.sPid = getpid();
    create_personal_semaphore();
    for (int i = 0; i < times; ++i) {
        main_task();
    }
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: client_prog clientsNumber triesNumber\n");
    }

    atexit(free_resources);
    initialize_resources();

    int clients = atoi(argv[1]);
    int tries = atoi(argv[2]);

    if (clients < 0) {
        printf("Wrong clients number.\n");
    }
    if (tries < 0) {
        printf("Wrong tries number.\n");
    }

    for (int i = 0; i < clients; ++i) {
        if (fork() == 0) {
            execute_tasks(tries);
        }
    }
    for (int j = 0; j < clients; ++j) {
        wait(NULL);
    }

    return 0;
}