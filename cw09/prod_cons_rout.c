//
// Created by farald on 01.06.18.
//
#include "prod_cons.h"
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include "fifo/circular_fifo.h"

#define MAX_STRING_LEN 256

CircularFifo_t *fifo;
extern FILE *file;

extern int compare_mode;
extern int searched_length;

pthread_mutex_t mutex;
pthread_cond_t empty;
pthread_cond_t full;

void initialize_queue(unsigned int qSize) {
    size_t memSize = offsetof(CircularFifo_t, queue) + sizeof(char *) * qSize;
    fifo = malloc(memSize);
    fifo_initialize(fifo, qSize);
}

void initialize_mutex() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&empty, NULL);
    pthread_cond_init(&full, NULL);
}

int my_strlen(const char *str) {
    int size = 0;
    while (*(str + size) != '\0' && *(str + size) != '\n') {
        size++;
    }
    return size;
}

void *producer_routine(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (fifo_full(fifo)) { pthread_cond_wait(&full, &mutex); }

        char *string = calloc(sizeof(char), MAX_STRING_LEN);
        if (fgets(string, MAX_STRING_LEN, file) == NULL) {
            free(string);
            pthread_exit(NULL);
        }

        fifo_push(fifo, string);
        pthread_cond_broadcast(&empty);
        pthread_mutex_unlock(&mutex);
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

void *consumer_routine(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (fifo_empty(fifo)) { pthread_cond_wait(&empty, &mutex); }

        int index = fifo->readItr;
        char *elem = fifo_pop(fifo);

        pthread_cond_broadcast(&full);
        pthread_mutex_unlock(&mutex);

        if (is_valid_string(elem)) {
            printf("%d %s\n", index, elem);
        }
        free(elem);
    }
}