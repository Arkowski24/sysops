//
// Created by Arkadiusz Placha on 18.03.2018.
//
#include <fcntl.h>
#include <sys/errno.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include <zconf.h>
#include <stdlib.h>
#include "record_handler.h"


void generate_sys(char *filePath, unsigned int recordLength, unsigned int recordsCount) {
    assert(filePath != NULL);
    assert(recordLength > 0);
    assert(recordsCount > 0);

    int fileDesc = open(filePath, O_CREAT | O_EXCL | O_TRUNC | O_WRONLY);
    int error = errno;
    if (fileDesc == -1 && error == EEXIST) {
        char res;
        printf("File already exists. Overwrite it? (y/n) ");
        scanf("%c", &res);
        if (res == 'y' || res == 'Y') {
            fileDesc = open(filePath, O_CREAT | O_TRUNC | O_WRONLY);
            error = errno;
        }
    }
    if (fileDesc == -1) {
        printf("File open error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }

    unsigned char *buf = malloc(recordLength);
    int bufSize = recordLength;
    if (buf == NULL) {
        buf = malloc(1);
        bufSize = 1;
        error = errno;
        if (buf == NULL) {
            printf("Memory allocation error: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }

    if (bufSize == 1) {
        for (unsigned int i = 0; i < recordsCount * recordLength; ++i) {
            *buf = (unsigned char) rand();
            error = errno;
            long long written = write(fileDesc, buf, 1);
            if (written == -1) {
                printf("Write to file error: %s\n", strerror(error));
                exit(EXIT_FAILURE);
            }

        }
    } else {
        for (unsigned int i = 0; i < recordsCount; ++i) {
            for (int j = 0; j < recordLength; ++j) {
                buf[j] = (unsigned char) rand();
            }
            long long written = write(fileDesc, buf, recordLength);
            if (written == -1) {
                printf("Write to file error: %s\n", strerror(error));
                exit(EXIT_FAILURE);
            }
        }
    }

}

void sort_sys(char *filePath, unsigned int recordLength, unsigned int recordsCount) {
    assert(filePath != NULL);
    assert(recordLength != 0);
    assert(recordsCount != 0);

    int fileDesc = open(filePath, O_RDWR);
    int error = errno;

    unsigned char *key = malloc(recordLength);
    unsigned char *mvd = malloc(recordLength);


    for (int i = 1; i < recordsCount; ++i) {
        lseek(fileDesc, (i - 1) * recordLength, SEEK_SET);
        read(fileDesc, mvd, recordLength);
        read(fileDesc, key, recordLength);

        int j = i - 1;
        while (mvd[0] > key[0]) {
            lseek(fileDesc, (j + 1) * recordLength, SEEK_SET);
            write(fileDesc, mvd, recordLength);
            j--;
            if (j < 0) { break; }
            lseek(fileDesc, j * recordLength, SEEK_SET);
            read(fileDesc, mvd, recordLength);
        }
        lseek(fileDesc, (j + 1) * recordLength, SEEK_SET);
        write(fileDesc, key, recordLength);
    }

}