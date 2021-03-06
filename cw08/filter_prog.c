//
// Created by Arkadiusz Placha on 22.05.2018.
//

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <assert.h>
#include <sys/times.h>
#include "filter_prog.h"

pgma_img_t inputImg;
filter_img_t filterImg;
pgma_img_t outputImg;

thread_args_t *threadArgs;

int i_max(int a, int b) {
    return (a > b) ? a : b;
}

void init_output_img() {
    outputImg = inputImg;
    unsigned int flrSize = inputImg.w * inputImg.h;
    outputImg.img = allocate_memory(flrSize, sizeof(unsigned int));
}

void *thread_routine(void *arg) {
    thread_args_t *args = (thread_args_t *) arg;

    for (int k = args->sIndex; k < args->sIndex + args->count; ++k) {
        int x = k / inputImg.w + 1;
        int y = k % inputImg.w + 1;

        double s = 0;
        for (int i = 0; i < filterImg.c; ++i) {
            for (int j = 0; j < filterImg.c; ++j) {
                int halfC = (filterImg.c + 1) / 2;
                int inputHeight = (i_max(0, x - halfC + i) % inputImg.h);
                int inputWidth = (i_max(0, y - halfC + j) % inputImg.w);
                int inputIndex = inputHeight * inputImg.w + inputWidth;

                s += inputImg.img[inputIndex] * filterImg.flr[i * filterImg.c + j];
            }
        }
        outputImg.img[k] = (unsigned int) round(s);
    }

    return NULL;
}

void start_thread(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg) {
    if (pthread_create(thread, attr, start_routine, arg)) {
        perror("Thread creation error.");
        exit(EXIT_FAILURE);
    }
}

void dispatch_tasks(unsigned int threadsCount) {
    assert(threadsCount > 0);

    threadArgs = allocate_memory(threadsCount, sizeof(thread_args_t));
    unsigned int imgSize = inputImg.w * inputImg.h;
    unsigned int elems = imgSize / threadsCount;

    unsigned int itr = 0;

    for (int i = 0; i < threadsCount - 1; ++i) {
        threadArgs[i].sIndex = itr;
        threadArgs[i].count = elems;

        start_thread(&threadArgs[i].pthreadID, NULL, thread_routine, threadArgs + i);
        itr += elems;
    }
    threadArgs[threadsCount - 1].sIndex = itr;
    threadArgs[threadsCount - 1].count = imgSize - itr;
    start_thread(&threadArgs[threadsCount - 1].pthreadID, NULL, thread_routine, threadArgs + threadsCount - 1);
}

void wait_for_threads(unsigned int threadsCount) {
    for (int i = 0; i < threadsCount; ++i) {
        pthread_join(threadArgs[i].pthreadID, NULL);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Not enough arguments.\n");
        exit(EXIT_FAILURE);
    }

    unsigned int tCount = (unsigned int) strtoul(argv[1], NULL, 0);
    if (tCount == 0) {
        fprintf(stderr, "Wrong thread count.\n");
        exit(EXIT_FAILURE);
    }

    read_pgma_image(argv[2], &inputImg);
    read_filter_image(argv[3], &filterImg);
    init_output_img();

    struct tms tm;
    clock_t beginTime = times(&tm);
    dispatch_tasks(tCount);
    wait_for_threads(tCount);
    clock_t endTime = times(&tm);
    printf("Elapsed time (in clock ticks): %lu\n", endTime - beginTime);

    write_pgma_image(argv[4], &outputImg);

    return 0;
}