//
// Created by Arkadiusz Placha on 22.05.2018.
//

#ifndef SYSOPS_FILTER_PROG_H
#define SYSOPS_FILTER_PROG_H

#include <stdlib.h>

struct {
    unsigned int w;
    unsigned int h;
    unsigned int max_g;
    unsigned int *img;
} typedef pgma_img_t;

struct {
    unsigned int c;
    double *flr;
} typedef filter_img_t;

void *allocate_memory(unsigned int count, size_t size);

void read_pgma_image(char *path, pgma_img_t *pgma_img);

void write_pgma_image(char *path, pgma_img_t *pgma_img);

void read_filter_image(char *path, filter_img_t *filter);

#endif //SYSOPS_FILTER_PROG_H