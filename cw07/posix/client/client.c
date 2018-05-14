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
#include "../../fifo/circular_fifo.h"

#define BARBER_QUEUE_NAME "/barber"
#define CLIENT_READY_NAME "/client_ready"
#define ACCESS_WR_NAME "/access_wr"
#define BARBER_READY_NAME "/barber_ready"

CircularFifo_t *fifo;
size_t memSize;

sem_t *clientReady;
sem_t *accessWaitingRoom;
sem_t *barberReady;

sem_t *personalSem;
ClientInfo_t clientInfo;

void load_fifo() {
    int fd = shm_open(BARBER_QUEUE_NAME, O_RDWR);
    size_t firstElemSize = offsetof(CircularFifo_t, clients);
    fifo = mmap(NULL, firstElemSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    memSize = firstElemSize + sizeof(ClientInfo_t) * fifo->qMaxSize;
    munmap(fifo, firstElemSize);
    fifo = mmap(NULL, memSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}

void initialize_resources() {
    load_fifo();

    clientReady = sem_open(CLIENT_READY_NAME, O_RDWR);
    accessWaitingRoom = sem_open(ACCESS_WR_NAME, O_RDWR);
    barberReady = sem_open(BARBER_READY_NAME, O_RDWR);

    clientInfo.PID = getpid();
    create_personal_semaphore();
}

void create_personal_semaphore() {
    snprintf(clientInfo.sName, NAME_MAX, "/client.%d", getpid());
    personalSem = sem_open(clientInfo.sName, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, 0);
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

void main_task() {
    sem_wait(accessWaitingRoom);

    if (fifo_size(fifo) < fifo->qMaxSize) {
        if (fifo_empty(fifo)) {
            printf("Waking barber.\n");
        } else {
            printf("Siting in the queue.\n");
        }

        fifo_push(fifo, clientInfo);
        sem_post(clientReady);
        sem_wait(personalSem);

        printf("Being cut by barber.\n");
        sem_wait(barberReady);
        printf("Barber finished.\n");
    } else {
        printf("Leaving without cutting.\n");
    }

    sem_post(accessWaitingRoom);
}

void execute_tasks(int s) {
    for (int i = 0; i < s; ++i) {
        main_task();
    }
}

int main(int argc, char *argv[]) {
    atexit(free_resources);
    initialize_resources();

    int n = atoi(argv[1]);
    int s = atoi(argv[2]);

    for (int i = 0; i < n; ++i) {
        if (fork() == 0) {
            execute_tasks(s);
        }
    }

    return 0;
}