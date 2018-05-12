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

#define BARBER_QUEUE_NAME "/barber"
#define CLIENT_READY_NAME "/client_ready"
#define ACCESS_WR_NAME "/access_wr"
#define BARBER_READY_NAME "/barber_ready"

void *shaMemAddr;
size_t memSize;
int continueWork = 1;

sem_t *clientReady;
sem_t *accessWaitingRoom;
sem_t *barberReady;

void sigterm_handle(int sig) {
    continueWork = 0;
}

void initialize_resources(int queueLength) {
    int fd = shm_open(BARBER_QUEUE_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG);
    memSize = queueLength * sizeof(ClientInfo_t);

    ftruncate(fd, memSize);
    shaMemAddr = mmap(NULL, memSize, PROT_READ, MAP_SHARED, fd, 0);

    clientReady = sem_open(CLIENT_READY_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG);
    accessWaitingRoom = sem_open(ACCESS_WR_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG);
    barberReady = sem_open(BARBER_READY_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG);
}

void free_resources() {
    munmap(shaMemAddr, memSize);
    shm_unlink(BARBER_QUEUE_NAME);

    sem_close(clientReady);
    sem_close(accessWaitingRoom);
    sem_close(barberReady);

    sem_unlink(CLIENT_READY_NAME);
    sem_unlink(ACCESS_WR_NAME);
    sem_unlink(BARBER_READY_NAME);
}

void acquire_client() {
    if (sem_trywait(clientReady) == -1) {
        printf("Barber is going to sleep.\n");
        sem_wait(clientReady);
        printf("Barber is waking up.\n");
    }
}

pid_t get_client() {
    sem_wait(accessWaitingRoom);
    //getClient
    sem_post(accessWaitingRoom);
}

int main(int argc, char *argv[]) {
    atexit(free_resources);
    signal(SIGTERM, sigterm_handle);

    int qSize = atoi(argv[1]);
    initialize_resources(qSize);

    while (continueWork) {
        acquire_client();
        pid_t client = get_client();

        sem_post(barberReady);
        printf("Cutting hair of %d.\n", client);
    }

    return 0;
}