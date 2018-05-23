//
// Created by Arkadiusz Placha on 22.05.2018.
//

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <assert.h>
#include "filter_prog.h"

pgma_img_t inputImg;
filter_img_t filterImg;
pgma_img_t outputImg;

thread_args_t *threadArgs;

unsigned int u_max(unsigned int a, unsigned int b) {
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
        unsigned int x = k % inputImg.w + 1;
        unsigned int y = k - x * inputImg.w + 1;

        double s = 0;
        for (int i = 0; i < filterImg.c; ++i) {
            for (int j = 0; j < filterImg.c; ++j) {
                unsigned int halfC = (filterImg.c + 1) / 2;
                unsigned int inputIndex = u_max(0, x - halfC + i) * inputImg.w + u_max(0, y - halfC + j);

                s += inputImg.img[inputIndex] * filterImg.flr[i * filterImg.c + j];
            }
        }
        outputImg.img[k] = (unsigned int) round(s);
    }

    return NULL;
}

void dispatch_tasks(unsigned threadsCount) {
    assert(threadsCount > 0);

    threadArgs = allocate_memory(threadsCount, sizeof(thread_args_t));
    unsigned int imgSize = inputImg.w * inputImg.h;
    unsigned int elems = imgSize / threadsCount;

    unsigned int itr = 0;

    for (int i = 0; i < threadsCount - 1; ++i) {
        threadArgs[i].sIndex = itr;
        threadArgs[i].count = elems;

        pthread_create(&threadArgs[i].pthreadID, NULL, thread_routine, threadArgs + i);
        itr += elems;
    }
    threadArgs[threadsCount - 1].sIndex = itr;
    threadArgs[threadsCount - 1].count = imgSize - itr;

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

    dispatch_tasks(tCount);
    write_pgma_image(argv[4], &outputImg);

    return 0;
}