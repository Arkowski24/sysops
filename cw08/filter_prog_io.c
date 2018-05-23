//
// Created by Arkadiusz Placha on 23.05.2018.
//

#include "filter_prog.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int fgets_delm(char *arr, unsigned int size, FILE *file) {
    unsigned int readChars = 0;

    for (int i = 0; i < size; ++i) {
        int c = fgetc(file);

        if (c == EOF) {
            if (readChars == 0) { return -2; }
            else { break; }
        } else if (c == '#') {
            do { c = fgetc(file); } while (c != '\n' && c != EOF);
            if (readChars == 0) { return -1; }
            else { break; }
        } else if (isspace(c)) {
            if (readChars == 0) { return -1; }
            else { break; }
        } else {
            arr[readChars] = (char) c;
            readChars++;
        }
    }

    arr[readChars] = '\0';
    return 0;
}

void read_pgma_args(unsigned int argV[], unsigned int argC, FILE *file) {
    char line[100];
    unsigned int itr = 0;

    while (itr < argC) {
        int status = fgets_delm(line, 100, file);
        if (status == 0) {
            argV[itr] = (unsigned int) strtoul(line, NULL, 0);
            itr++;
        } else if (status == -2) {
            printf("File is not in PGMA format.\n");
            exit(EXIT_FAILURE);
        }
    }
}

void read_pgma_header(pgma_img_t *pgmaImg, FILE *file) {
    char line[100];
    unsigned int args[3];

    fgets_delm(line, 100, file);
    if (line[0] != 'P' || line[1] != '2') {
        printf("File is not in PGMA format.\n");
    }

    read_pgma_args(args, 3, file);
    pgmaImg->w = args[0];
    pgmaImg->h = args[1];
    pgmaImg->max_g = args[2];
}

void read_pgma_content(pgma_img_t *pgmaImg, FILE *file) {
    unsigned int imgSize = pgmaImg->w * pgmaImg->h;

    pgmaImg->img = calloc(imgSize, sizeof(unsigned int));
    if (pgmaImg->img == NULL) {
        perror("Allocation error.");
        exit(EXIT_FAILURE);
    }

    read_pgma_args(pgmaImg->img, imgSize, file);
}

void read_pgma_image(char *path, pgma_img_t *pgma_img) {
    FILE *file = fopen(path, "r");
    read_pgma_header(pgma_img, file);
    read_pgma_content(pgma_img, file);
}
