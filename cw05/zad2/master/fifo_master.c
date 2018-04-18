//
// Created by Arkadiusz Placha on 16.04.2018.
//

#include <stdio.h>
#include <stdlib.h>
#include "fifo_master.h"
#include <sys/stat.h>
#include <sys/errno.h>
#include <string.h>
#include <zconf.h>
#include <fcntl.h>

#define BUFFER_SIZE 100

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "%s\n", "File path required.");
        exit(EXIT_FAILURE);
    }

    if (mkfifo(argv[1], S_IRWXU) != 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int fileHandle = open(argv[1], O_RDONLY | O_ASYNC);

    char *buffer = calloc(BUFFER_SIZE + 1, sizeof(char));

    while (1) {
        int readBytes = read(fileHandle, buffer, BUFFER_SIZE);
        buffer[readBytes + 1] = '\0';
        if (readBytes == 0) {
            pause();
        } else {
            fprintf(stdout, "%s", buffer);
        }
    }
}