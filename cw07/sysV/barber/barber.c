//
// Created by Arkadiusz Placha on 12.05.2018.
//

#include "barber.h"
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <signal.h>
#include <time.h>
#include "../fifo/circular_fifo.h"

#define FIFO_KEY_ID 10
#define BARBER_KEY_ID 20
#define ACCESS_READY_KEY_ID 30
#define CLIENT_READY_KEY_ID 40

#define CLIENT_READY_NAME "/client_ready"
#define ACCESS_WR_NAME "/access_wr"
#define BARBER_READY_NAME "/barber_ready"
#define TIMESTAMP_SIZE 256

CircularFifo_t *fifo;
size_t memSize;
int continueWork = 1;

int fifoID;
int clientReadyID;
int accessWaitingRoomID;
int barberReadyID;

char timeStamp[TIMESTAMP_SIZE];

void sigterm_handle(int sig) {
    continueWork = 0;
    if (fifo == NULL || fifo->barberSleeping) {
        exit(EXIT_SUCCESS);
    }
}

void open_shared_memory(size_t queueLength) {
    char *home = getenv("HOME");
    key_t fifoKey = ftok(home, FIFO_KEY_ID);

    memSize = offsetof(CircularFifo_t, queue) + sizeof(ClientInfo_t) * queueLength;
    fifoID = shmget(fifoKey, memSize, IPC_CREAT | IPC_EXCL | S_IRWXU);
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

    clientReadyID = semget(clientReadyKey, 1, IPC_CREAT | IPC_EXCL | S_IRWXU);
    if (clientReadyID == -1) {
        perror("SEMAPHORE ERROR (clientReady)");
        exit(EXIT_FAILURE);
    }
    accessWaitingRoomID = semget(accessWaitingRoomKey, 1, IPC_CREAT | IPC_EXCL | S_IRWXU);
    if (accessWaitingRoomID == -1) {
        perror("SEMAPHORE ERROR (accessWaitingRoom)");
        exit(EXIT_FAILURE);
    }
    barberReadyID = semget(barberReadyKey, 1, IPC_CREAT | IPC_EXCL | S_IRWXU);
    if (barberReadyID == -1) {
        perror("SEMAPHORE ERROR (barberReady)");
        exit(EXIT_FAILURE);
    }

    union semun num = 0;
    semctl(clientReadyID, 0, SETVAL, num);
    semctl(barberReadyID, 0, SETVAL, num);

    num = 1;
    semctl(accessWaitingRoomID, 0, SETVAL, num);
}

void initialize_resources(size_t queueLength) {
    open_shared_memory(queueLength);
    fifo_initialize(fifo, queueLength);

    open_common_semaphores();
}

void free_resources() {
    shmdt(fifo);
    shmctl(fifoID, IPC_RMID, NULL);

    semctl(clientReadyID, 0, IPC_RMID);
    semctl(accessWaitingRoomID, 0, IPC_RMID);
    semctl(barberReadyID, 0, IPC_RMID);
}

char *get_timestamp() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    snprintf(timeStamp, TIMESTAMP_SIZE, "%ld %ld", ts.tv_sec, ts.tv_nsec);
    return timeStamp;
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

int use_client_semaphore(ClientInfo_t *client) {
    int clientSemID = semget(client->sKey, 1, 0);
    if (clientSemID == -1) {
        perror("SEMAPHORE ERROR (client)");
        return -1;
    }
    sem_post(clientSemID);
    semctl(clientSemID, 0, IPC_RMID);
    return 0;
}

pid_t get_client() {
    ClientInfo_t *client;
    sem_wait(accessWaitingRoomID);

    if (fifo_empty(fifo)) {
        fifo->barberSleeping = 1;
        printf("%s|Barber: Going to sleep.\n", get_timestamp());
        sem_post(accessWaitingRoomID);

        sem_wait(clientReadyID);
        sem_wait(accessWaitingRoomID);
        fifo->barberSleeping = 0;
        printf("%s|Barber: Waking up.\n", get_timestamp());

        client = &fifo->chair;
    } else {
        sem_wait(clientReadyID);
        client = fifo_pop(fifo);

        printf("%s|Barber: Inviting %d from waiting room.\n", get_timestamp(), client->sPid);
    }
    pid_t clientPid = use_client_semaphore(client) ? -1 : client->sPid;

    sem_post(accessWaitingRoomID);
    return clientPid;
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
            printf("%s|Barber: Started cutting hair of %d.\n", get_timestamp(), client);
            sem_post(barberReadyID);
            printf("%s|Barber: Finished cutting hair of %d.\n", get_timestamp(), client);
        }
    }

    return 0;
}