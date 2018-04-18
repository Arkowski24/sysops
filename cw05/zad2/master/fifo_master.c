//
// Created by Arkadiusz Placha on 16.04.2018.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <string.h>
#include <zconf.h>
#include <fcntl.h>
#include "fifo_master.h"

#define BUFFER_SIZE 100

void print_error_and_exit(int error) {
    fprintf(stderr, "%s\n", strerror(error));
    exit(EXIT_FAILURE);
}

int open_fifo(char *filePath) {
    int fileHandle = open(filePath, O_RDONLY);
    if (fileHandle < 0) {
        print_error_and_exit(errno);
    }
    return fileHandle;
}

char *create_buffer() {
    char *buffer = calloc(BUFFER_SIZE + 1, sizeof(char));
    if (buffer == NULL) {
        print_error_and_exit(errno);
    }
    return buffer;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "%s\n", "Fifo path required.");
        exit(EXIT_FAILURE);
    }

    if (mkfifo(argv[1], S_IREAD | S_IWRITE) != 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int fileHandle = open_fifo(argv[1]);
    char *buffer = create_buffer();

    while (1) {
        ssize_t readBytes = read(fileHandle, buffer, BUFFER_SIZE * sizeof(char));
        int error = errno;

        buffer[readBytes + 1] = '\0';
        if (readBytes > 0) {
            fprintf(stdout, "%s", buffer);
        } else if (readBytes < 0) {
            unlink(argv[1]);
            print_error_and_exit(error);
        } else {
            unlink(argv[1]);
            exit(EXIT_SUCCESS);
        }
    }
}