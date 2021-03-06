//
// Created by Arkadiusz Placha on 31.05.18.
//

#include "prod_cons.h"
#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/times.h>
#include <signal.h>
#include <semaphore.h>
#include "../fifo/circular_fifo.h"

#define MAX_STRING_LEN 256

CircularFifo_t *fifo;
FILE *file;

pthread_t *producersID;
pthread_t *consumersID;
unsigned int producersCount;
unsigned int consumersCount;

int compare_mode;
unsigned int searched_length;
unsigned int verbose;
unsigned int timeout;

extern sem_t emptyCount;
extern sem_t insertedCount;

void sigint_handler(int sig) {
    kill_other_threads();
}

void parse_arguments(char *argv[]) {
    producersCount = (unsigned int) strtoul(argv[1], NULL, 0);
    consumersCount = (unsigned int) strtoul(argv[2], NULL, 0);
    searched_length = (unsigned int) strtoul(argv[5], NULL, 0);

    if (producersCount == 0 || consumersCount == 0) {
        printf("Number of producers and consumers must be more than 0.\n");
        exit(EXIT_FAILURE);
    }

    compare_mode = (int) strtol(argv[6], NULL, 0);
    verbose = (unsigned int) strtoul(argv[7], NULL, 0);
    timeout = (unsigned int) strtoul(argv[8], NULL, 0);
}

FILE *open_file(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Open file error.");
        exit(EXIT_FAILURE);
    }
    return file;
}

void setup_resources(char *argv[]) {
    parse_arguments(argv);
    file = open_file(argv[4]);

    unsigned int queueSize = (unsigned int) strtoul(argv[3], NULL, 0);
    if (queueSize == 0) {
        printf("Queue size must be more than 0.\n");
        exit(EXIT_FAILURE);
    }
    initialize_queue(queueSize);
    initialize_mutex(queueSize);
}

void setup_sigint_handler() {
    if (timeout) {
        signal(SIGINT, SIG_IGN);
    } else {
        signal(SIGINT, sigint_handler);
    }
}

void start_thread(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg) {
    if (pthread_create(thread, attr, start_routine, arg)) {
        perror("Thread creation error.");
        exit(EXIT_FAILURE);
    }
}

void dispatch_tasks() {
    assert(producersCount > 0);
    assert(consumersCount > 0);

    producersID = calloc(producersCount, sizeof(pthread_t));
    consumersID = calloc(consumersCount, sizeof(pthread_t));

    for (int i = 0; i < producersCount; ++i) {
        start_thread(producersID + i, NULL, producer_routine, producersID + i);
    }
    for (int i = 0; i < consumersCount; ++i) {
        start_thread(consumersID + i, NULL, consumer_routine, consumersID + i);
    }
}

void print_kill_message() {
    printf("%ld: Thread %ld is cancelling other threads.\n", times(NULL), pthread_self());
}

void print_timeout_message() {
    printf("%ld: Maximum time reached. \n", times(NULL));
}

void kill_other_threads() {
    if (verbose) { print_kill_message(); }
    pthread_t caller = pthread_self();
    for (int i = 0; i < producersCount; ++i) {
        if (producersID[i] != caller)
            pthread_cancel(producersID[i]);
    }
    for (int i = 0; i < consumersCount; ++i) {
        if (consumersID[i] != caller)
            pthread_cancel(consumersID[i]);
    }
    for (int i = 0; i < producersCount; ++i) {
        sem_post(&emptyCount);
    }
    for (int i = 0; i < consumersCount; ++i) {
        sem_post(&insertedCount);
    }
}

void collect_threads() {
    for (int i = 0; i < producersCount; ++i) {
        pthread_join(producersID[i], NULL);
    }
    for (int i = 0; i < consumersCount; ++i) {
        pthread_join(consumersID[i], NULL);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 9) {
        printf("Not enough arguments.\n");
        exit(EXIT_FAILURE);
    }

    setup_resources(argv);
    setup_sigint_handler();

    dispatch_tasks();

    if (timeout > 0) {
        sleep(timeout);
        if (verbose) { print_timeout_message(); }
        kill_other_threads();
    }
    collect_threads();

    return 0;
}