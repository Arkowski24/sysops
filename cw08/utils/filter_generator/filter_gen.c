//
// Created by Arkadiusz Placha on 24.05.2018.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    srand(time(NULL));
    if (argc < 3) {
        printf("Not enough arguments.\n");
        exit(EXIT_FAILURE);
    }
    FILE *file = fopen(argv[2], "w");
    if (file == NULL) {
        perror("Cannot create file.");
        exit(EXIT_FAILURE);
    }

    int c = atoi(argv[1]);
    if (c <= 0) {
        printf("Wrong c value.\n");
        exit(EXIT_FAILURE);
    }

    double *tab = calloc(c * c, sizeof(double));
    double sum = 0;

    for (int i = 0; i < c * c; ++i) {
        tab[i] = (double) (rand() + 1) / (double) RAND_MAX;
        sum += tab[i];
    }
    for (int i = 0; i < c * c; ++i) {
        tab[i] /= sum;
    }

    fprintf(file, "%d\n", c);
    for (int i = 0; i < c; ++i) {
        fprintf(file, "%lf", tab[i * c]);
        for (int j = 1; j < c; ++j) {
            fprintf(file, " %lf", tab[i * c + j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
    
    return 0;
}