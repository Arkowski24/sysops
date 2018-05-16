//
// Created by Arkadiusz Placha on 12.05.2018.
//

#include "barber.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <stddef.h>
#include <signal.h>
#include <time.h>
#include "../fifo/circular_fifo.h"

#define BARBER_QUEUE_NAME "/barber"
#define CLIENT_READY_NAME "/client_ready"
#define ACCESS_WR_NAME "/access_wr"
#define BARBER_READY_NAME "/barber_ready"

CircularFifo_t *fifo;
size_t memSize;
int continueWork = 1;

sem_t *clientReady;
sem_t *accessWaitingRoom;
sem_t *barberReady;

void sigterm_handle(int sig) {
    continueWork = 0;
}

void open_shared_memory(size_t queueLength) {
    int fd = shm_open(BARBER_QUEUE_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG);
    if (fd == -1) {
        perror("SHARED MEMORY ERROR");
        exit(EXIT_FAILURE);
    }
    memSize = offsetof(CircularFifo_t, queue) + sizeof(ClientInfo_t) * queueLength;
    if (ftruncate(fd, memSize) == -1) {
        perror("TRUNCATE ERROR");
        exit(EXIT_FAILURE);
    }
    fifo = mmap(NULL, memSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fifo == MAP_FAILED) {
        perror("MEMORY MAP ERROR");
        exit(EXIT_FAILURE);
    }
}

void open_common_semaphores() {
    clientReady = sem_open(CLIENT_READY_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, 0);
    if (clientReady == SEM_FAILED) {
        perror("SEMAPHORE ERROR (clientReady)");
        exit(EXIT_FAILURE);
    }
    accessWaitingRoom = sem_open(ACCESS_WR_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, 1);
    if (accessWaitingRoom == SEM_FAILED) {
        perror("SEMAPHORE ERROR (accessWaitingRoom)");
        exit(EXIT_FAILURE);
    }
    barberReady = sem_open(BARBER_READY_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, 0);
    if (barberReady == SEM_FAILED) {
        perror("SEMAPHORE ERROR (barberReady)");
        exit(EXIT_FAILURE);
    }
}

void initialize_resources(size_t queueLength) {
    open_shared_memory(queueLength);
    fifo_initialize(fifo, queueLength);

    open_common_semaphores();
}

void free_resources() {
    munmap(fifo, memSize);
    shm_unlink(BARBER_QUEUE_NAME);

    sem_close(clientReady);
    sem_close(accessWaitingRoom);
    sem_close(barberReady);

    sem_unlink(CLIENT_READY_NAME);
    sem_unlink(ACCESS_WR_NAME);
    sem_unlink(BARBER_READY_NAME);
}

long get_timestamp() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_nsec;
}

pid_t use_client_semaphore(ClientInfo_t *client) {
    sem_t *clientSem = sem_open(client->sName, O_WRONLY);
    if (clientSem != SEM_FAILED) {
        sem_post(clientSem);
        sem_close(clientSem);

        return client->sPid;
    }
    perror("SEMAPHORE ERROR (client)");
    return -1;
}

pid_t get_client() {
    ClientInfo_t *client;
    sem_wait(accessWaitingRoom);

    if (sem_trywait(clientReady) == -1) {
        fifo->barberSleeping = 1;
        printf("%ld|Barber: Going to sleep.\n", get_timestamp());
        sem_post(accessWaitingRoom);

        sem_wait(clientReady);
        sem_wait(accessWaitingRoom);
        fifo->barberSleeping = 0;
        printf("%ld|Barber: Waking up.\n", get_timestamp());

        client = &fifo->chair;
    } else {
        client = fifo_pop(fifo);
        printf("%ld|Barber: Inviting %d from waiting room.\n", get_timestamp(), client->sPid);
    }
    sem_post(accessWaitingRoom);

    return use_client_semaphore(client);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: barber_prog chairsNumber\n");
    }

    atexit(free_resources);
    signal(SIGTERM, sigterm_handle);

    size_t qSize = (size_t) atoi(argv[1]);
    initialize_resources(qSize);

    while (continueWork) {
        pid_t client = get_client();

        if (client != -1) {
            printf("%ld|Barber: Started cutting hair of %d.\n", get_timestamp(), client);
            sem_post(barberReady);
            printf("%ld|Barber: Finished cutting hair of %d.\n", get_timestamp(), client);
        }
    }

    return 0;
}