//
// Created by Arkadiusz Placha on 14.05.2018.
//

#include "client.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <stddef.h>
#include <unistd.h>
#include <limits.h>
#include <wait.h>
#include <time.h>
#include "../fifo/circular_fifo.h"

#define BARBER_QUEUE_NAME "/barber"
#define CLIENT_READY_NAME "/client_ready"
#define ACCESS_WR_NAME "/access_wr"
#define BARBER_READY_NAME "/barber_ready"
#define TIMESTAMP_SIZE 256

CircularFifo_t *fifo;
size_t memSize;

sem_t *clientReady;
sem_t *accessWaitingRoom;
sem_t *barberReady;

sem_t *personalSem;
ClientInfo_t clientInfo;

char timeStamp[TIMESTAMP_SIZE];

void load_fifo() {
    int fd = shm_open(BARBER_QUEUE_NAME, O_RDWR, 0);
    if (fd == -1) {
        perror("SHARED MEMORY ERROR");
        exit(EXIT_FAILURE);
    }

    size_t firstElemSize = offsetof(CircularFifo_t, queue);
    fifo = mmap(NULL, firstElemSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fifo == MAP_FAILED) {
        perror("MEMORY MAP ERROR");
        exit(EXIT_FAILURE);
    }

    memSize = firstElemSize + sizeof(ClientInfo_t) * fifo->qMaxSize;
    munmap(fifo, firstElemSize);

    fifo = mmap(NULL, memSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fifo == MAP_FAILED) {
        perror("MEMORY MAP ERROR");
        exit(EXIT_FAILURE);
    }
}

void open_common_semaphores() {
    clientReady = sem_open(CLIENT_READY_NAME, O_RDWR);
    if (clientReady == SEM_FAILED) {
        perror("SEMAPHORE ERROR (clientReady)");
        exit(EXIT_FAILURE);
    }
    accessWaitingRoom = sem_open(ACCESS_WR_NAME, O_RDWR);
    if (accessWaitingRoom == SEM_FAILED) {
        perror("SEMAPHORE ERROR (accessWaitingRoom)");
        exit(EXIT_FAILURE);
    }
    barberReady = sem_open(BARBER_READY_NAME, O_RDWR);
    if (barberReady == SEM_FAILED) {
        perror("SEMAPHORE ERROR (barberReady)");
        exit(EXIT_FAILURE);
    }
}

void initialize_resources() {
    load_fifo();
    open_common_semaphores();
}

void create_personal_semaphore() {
    snprintf(clientInfo.sName, NAME_MAX, "/client.%d", getpid());
    personalSem = sem_open(clientInfo.sName, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG | S_IRWXO, 0);
    if (personalSem == SEM_FAILED) {
        perror("SEMAPHORE ERROR (personalSem)");
        exit(EXIT_FAILURE);
    }
}

void delete_personal_semaphore() {
    sem_close(personalSem);
    sem_unlink(clientInfo.sName);
}

void free_resources() {
    delete_personal_semaphore();

    munmap(fifo, memSize);
    sem_close(clientReady);
    sem_close(accessWaitingRoom);
    sem_close(barberReady);
}

char *get_timestamp() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    snprintf(timeStamp, TIMESTAMP_SIZE, "%ld %ld", ts.tv_sec, ts.tv_nsec);
    return timeStamp;
}

void main_task() {
    sem_wait(accessWaitingRoom);

    if (fifo_size(fifo) < fifo->qMaxSize || fifo->barberSleeping == 1) {
        if (fifo->barberSleeping == 1) {
            fifo->barberSleeping = 0;
            fifo->chair = clientInfo;

            printf("%s|PID %d: Waking up the barber.\n", get_timestamp(), clientInfo.sPid);
        } else {
            fifo_push(fifo, clientInfo);

            printf("%s|PID %d: Sitting in the waiting room.\n", get_timestamp(), clientInfo.sPid);
        }

        sem_post(accessWaitingRoom);
        sem_post(clientReady);

        sem_wait(personalSem);

        printf("%s|PID %d: Sitting in the chair.\n", get_timestamp(), clientInfo.sPid);

        sem_wait(barberReady);
        printf("%s|PID %d: Leaving after having hair cut.\n", get_timestamp(), clientInfo.sPid);
    } else {
        sem_post(accessWaitingRoom);
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