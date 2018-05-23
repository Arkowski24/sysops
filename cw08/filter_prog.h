//
// Created by Arkadiusz Placha on 22.05.2018.
//

#ifndef SYSOPS_FILTER_PROG_H
#define SYSOPS_FILTER_PROG_H

#endif //SYSOPS_FILTER_PROG_H

struct {
    unsigned int w;
    unsigned int h;
    unsigned int max_g;
    unsigned int *img;
} typedef pgma_img_t;

void read_pgma_image(char *path, pgma_img_t *pgma_img);