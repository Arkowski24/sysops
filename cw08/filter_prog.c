//
// Created by Arkadiusz Placha on 22.05.2018.
//

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include "filter_prog.h"

pgma_img_t inputImg;
filter_img_t filterImg;
pgma_img_t outputImg;

unsigned int u_max(unsigned int a, unsigned int b) {
    return (a > b) ? a : b;
}

void init_output_img() {
    outputImg = inputImg;
    unsigned int flrSize = inputImg.w * inputImg.h;
    outputImg.img = allocate_memory(flrSize, sizeof(unsigned int));
}

void thread_routine(unsigned int sIndex, unsigned int count) {
    for (int k = sIndex; k < sIndex + count; ++k) {
        unsigned int x = k % inputImg.w;
        unsigned int y = k - x * inputImg.w;

        double s = 0;
        for (int i = 0; i < filterImg.c; ++i) {
            for (int j = 0; j < filterImg.c; ++j) {
                unsigned int halfC = (filterImg.c + 1) / 2;
                unsigned int inputIndex = u_max(0, x - halfC + i + 1) * inputImg.w + u_max(0, y - halfC + j + 1);

                s += inputImg.img[inputIndex] * filterImg.flr[i * filterImg.c + j];
            }
        }
        outputImg.img[k] = (unsigned int) round(s);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    read_pgma_image(argv[2], &inputImg);
    read_filter_image(argv[3], &filterImg);
    init_output_img();

    return 0;
}