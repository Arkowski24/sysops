//
// Created by Arkadiusz Placha on 23.05.2018.
//

#include "filter_prog.h"
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

void *allocate_memory(unsigned int count, size_t size) {
    void *mem = calloc(count, size);
    if (mem == NULL) {
        perror("Allocation error.");
        exit(EXIT_FAILURE);
    }
    return mem;
}

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
    pgmaImg->img = allocate_memory(imgSize, sizeof(unsigned int));

    read_pgma_args(pgmaImg->img, imgSize, file);
}

void read_pgma_image(char *path, pgma_img_t *pgma_img) {
    assert(path != NULL);
    assert(pgma_img != NULL);

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("Base image file error");
        exit(EXIT_FAILURE);
    }

    read_pgma_header(pgma_img, file);
    read_pgma_content(pgma_img, file);
}

void write_pgma_image(char *path, pgma_img_t *pgma_img) {
    assert(path != NULL);
    assert(pgma_img != NULL);

    FILE *file = fopen(path, "w");
    if (file == NULL) {
        perror("Cannot create output file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "P2\n");
    fprintf(file, "%u %u\n", pgma_img->w, pgma_img->h);
    fprintf(file, "%u\n", pgma_img->max_g);

    for (int i = 0; i < pgma_img->h; ++i) {
        fprintf(file, "%u", pgma_img->img[i * pgma_img->w]);
        for (int j = 1; j < pgma_img->w; ++j) {
            fprintf(file, " %u", pgma_img->img[i * pgma_img->w + j]);
        }
        fprintf(file, "\n");
    }

    fflush(file);
}

void read_filter_header(unsigned int *c, FILE *file) {
    char line[100];
    while (1) {
        int status = fgets_delm(line, 100, file);
        if (status == 0) {
            *c = (unsigned int) strtoul(line, NULL, 0);
            return;
        } else if (status == -2) {
            printf("File is not in PGMA format.\n");
            exit(EXIT_FAILURE);
        }
    }
}

void read_filter_args(double argV[], unsigned int argC, FILE *file) {
    char line[100];
    unsigned int itr = 0;

    while (itr < argC) {
        int status = fgets_delm(line, 100, file);
        if (status == 0) {
            argV[itr] = strtod(line, 0);
            itr++;
        } else if (status == -2) {
            printf("File is not in PGMA format.\n");
            exit(EXIT_FAILURE);
        }
    }
}

void read_filter_image(char *path, filter_img_t *filter) {
    assert(path != NULL);
    assert(filter != NULL);

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("Filter file error");
        exit(EXIT_FAILURE);
    }

    read_filter_header(&filter->c, file);
    unsigned int flrSize = filter->c * filter->c;

    filter->flr = allocate_memory(flrSize, sizeof(double));

    read_filter_args(filter->flr, flrSize, file);
}