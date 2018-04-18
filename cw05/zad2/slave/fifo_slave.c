//
// Created by Arkadiusz Placha on 16.04.2018.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "fifo_slave.h"

#define BUFFER_SIZE 256
#define SLEEP_MIN 2
#define SLEEP_MAX 5

void print_error_and_exit(int error) {
    fprintf(stderr, "%s\n", strerror(error));
    exit(EXIT_FAILURE);
}

FILE *open_fifo(char *filePath) {
    int fileHandler = open(filePath, O_WRONLY | O_NONBLOCK);
    if (fileHandler < 0) {
        print_error_and_exit(errno);
    }
    FILE *file = fdopen(fileHandler, "w");
    return file;
}

char *create_buffer() {
    char *buffer = calloc(BUFFER_SIZE + 1, sizeof(char));
    if (buffer == NULL) {
        print_error_and_exit(errno);
    }
    return buffer;
}

FILE *create_date_writer() {
    FILE *dateFile = popen("date", "r");
    if (dateFile == NULL) {
        print_error_and_exit(errno);
    }
    return dateFile;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "%s\n", "Fifo path and iterations number required.");
        exit(EXIT_FAILURE);
    }
    srand(time(NULL));

    FILE *fifo = open_fifo(argv[1]);
    int N = strtol(argv[2], NULL, 0);

    fprintf(stdout, "%i\n", getpid());
    char *buffer = create_buffer();

    for (int i = 0; i < N; ++i) {
        FILE *dateFile = create_date_writer();
        fread(buffer, BUFFER_SIZE, sizeof(char), dateFile);
        fprintf(fifo, "Slave PID: %i, Date: %s", getpid(), buffer);

        pclose(dateFile);
        fflush(fifo);
        sleep(rand() % (SLEEP_MAX - SLEEP_MIN + 1) + SLEEP_MIN);
    }
    return 0;
}