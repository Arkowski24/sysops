//
// Created by farald on 01.06.18.
//
#include "prod_cons.h"
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/times.h>
#include <semaphore.h>
#include "../fifo/circular_fifo.h"

#define MAX_STRING_LEN 256

extern CircularFifo_t *fifo;
extern FILE *file;

extern int compare_mode;
extern int searched_length;
extern unsigned int verbose;
extern unsigned int timeout;

pthread_mutex_t mutex;
sem_t emptyCount;
sem_t insertedCount;


void print_acquire_mutex_message() {
    printf("%ld: Thread %ld locked mutex.\n", times(NULL), pthread_self());
}

void print_released_mutex_message() {
    printf("%ld: Thread %ld unlocked mutex.\n", times(NULL), pthread_self());
}

void print_thread_finished_message() {
    printf("%ld: Thread %ld finished. \n", times(NULL), pthread_self());
}

void print_inserted_message() {
    printf("%ld: Thread %ld inserted element into buffer. \n", times(NULL), pthread_self());
}

void print_retrieved_message() {
    printf("%ld: Thread %ld retrieved element from buffer. \n", times(NULL), pthread_self());
}

void print_eof_message() {
    printf("%ld: Thread %ld reached end of file. \n", times(NULL), pthread_self());
}

void cleanup_producer_handler(void *arg) {
    pthread_mutex_unlock(&mutex);
    if (verbose) {
        print_released_mutex_message();
        print_thread_finished_message();
    }
    sem_post(&emptyCount);
}

void cleanup_consumer_handler(void *arg) {
    pthread_mutex_unlock(&mutex);
    if (verbose) {
        print_released_mutex_message();
        print_thread_finished_message();
    }
    sem_post(&insertedCount);
}

void initialize_queue(unsigned int qSize) {
    size_t memSize = offsetof(CircularFifo_t, queue) + sizeof(char *) * qSize;
    fifo = malloc(memSize);
    fifo_initialize(fifo, qSize);
}

void initialize_mutex(unsigned int qSize) {
    pthread_mutex_init(&mutex, NULL);
    sem_init(&emptyCount, 0, qSize);
    sem_init(&insertedCount, 0, 0);
}

int my_strlen(const char *str) {
    int size = 0;
    while (*(str + size) != '\0' && *(str + size) != '\n') {
        size++;
    }
    return size;
}

void *producer_routine(void *arg) {
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    while (1) {
        sem_wait(&emptyCount);
        pthread_mutex_lock(&mutex);
        if (verbose) { print_acquire_mutex_message(); }
        pthread_cleanup_push(cleanup_producer_handler, NULL) ;

                pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
                pthread_testcancel();

                char *string = calloc(sizeof(char), MAX_STRING_LEN);
                if (fgets(string, MAX_STRING_LEN, file) == NULL) {
                    if (verbose && feof(file)) { print_eof_message(); }
                    free(string);

                    if (timeout == 0) { kill_other_threads(); }
                    pthread_exit(NULL);
                }

                fifo_push(fifo, string);
                if (verbose) { print_inserted_message(); }
                pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

        pthread_cleanup_pop(0);
        pthread_mutex_unlock(&mutex);
        if (verbose) { print_released_mutex_message(); }

        sem_post(&insertedCount);
    }
}

int is_valid_string(char *str) {
    int elemLength = my_strlen(str);
    int print;

    switch (compare_mode) {
        case -1:
            print = elemLength < searched_length;
            break;
        case 0:
            print = elemLength == searched_length;
            break;
        case 1:
            print = elemLength > searched_length;
            break;
        default:
            print = 0;
    }

    return print;
}

void print_valid_string(int index, char *str) {
    if (verbose) { printf("%ld: Thread %ld found valid string. ", times(NULL), pthread_self()); }
    printf("Index: %3d, String: %s", index, str);
}

void *consumer_routine(void *arg) {
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    int qIndex;
    char *elem;

    while (1) {
        sem_wait(&insertedCount);
        pthread_mutex_lock(&mutex);
        if (verbose) { print_acquire_mutex_message(); }
        pthread_cleanup_push(cleanup_consumer_handler, NULL) ;
                pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
                pthread_testcancel();

                qIndex = (int) fifo->readItr;
                elem = fifo_pop(fifo);
                if (verbose) { print_retrieved_message(); }

                pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        pthread_cleanup_pop(0);

        pthread_mutex_unlock(&mutex);
        if (verbose) { print_released_mutex_message(); }
        sem_post(&emptyCount);

        if (is_valid_string(elem)) {
            print_valid_string(qIndex, elem);
        }
        free(elem);
    }
}