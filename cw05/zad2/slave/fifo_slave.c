//
// Created by Arkadiusz Placha on 16.04.2018.
//

#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <time.h>

#define BUFFER_SIZE 256
#define SLEEP_MIN 2
#define SLEEP_MAX 5

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "%s\n", "FIFO path and iterations number required.");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    FILE *fifo = fopen(argv[1], "w");
    int N = atoi(argv[2]);

    fprintf(stdout, "%i\n", getpid());
    char *buffer = calloc(BUFFER_SIZE + 1, sizeof(char));

    for (int i = 0; i < N; ++i) {
        fprintf(fifo, "%i ", getpid());
        FILE *dateFile = popen("date", "r");
        size_t readBites = fread(buffer, BUFFER_SIZE, sizeof(char), dateFile);

        while (readBites > 0) {
            fwrite(buffer, readBites, sizeof(char), fifo);
            readBites = fread(buffer, BUFFER_SIZE, sizeof(char), dateFile);
        }

        pclose(dateFile);
        sleep(rand() % (SLEEP_MAX - SLEEP_MIN + 1) + SLEEP_MIN);
    }
    fclose(fifo);
    return 0;
}