//
// Created by Arkadiusz Placha on 31.05.18.
//

#include "prod_cons.h"
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include "fifo/circular_fifo.h"

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

void parse_arguments(char *argv[]) {
    producersCount = (unsigned int) strtoul(argv[1], NULL, 0);
    consumersCount = (unsigned int) strtoul(argv[2], NULL, 0);
    searched_length = (unsigned int) strtoul(argv[5], NULL, 0);
    compare_mode = (int) strtol(argv[6], NULL, 0);
    verbose = (unsigned int) strtoul(argv[7], NULL, 0);
    timeout = (unsigned int) strtoul(argv[8], NULL, 0);
}

FILE *open_file(char *filename) {
    return fopen(filename, "r");
}

int main(int argc, char *argv[]) {
    parse_arguments(argv);
    file = open_file(argv[4]);
    unsigned int queueSize = (unsigned int) strtoul(argv[3], NULL, 0);
    initialize_queue(queueSize);
    initialize_mutex();

    dispatch_tasks();

    return 0;
}