//
// Created by Arkadiusz Placha on 18.03.2018.
//
#include <fcntl.h>
#include <sys/errno.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include "record_handler.h"

int create_or_overwrite_file(char *filePath) {
    int fileDesc = open(filePath, O_CREAT | O_EXCL | O_TRUNC | O_WRONLY);
    int error = errno;
    if (fileDesc < 0 && error == EEXIST) {
        char res;
        printf("File already exists. Overwrite it? (y/n) ");
        scanf("%c", &res);
        if (res == 'y' || res == 'Y') {
            fileDesc = open(filePath, O_CREAT | O_TRUNC | O_WRONLY);
            error = errno;
        }
    }

    if (fileDesc < 0) {
        printf("File open error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
    return fileDesc;
}

int open_file(char *filePath, int flags) {
    int fileDesc = open(filePath, flags);
    int error = errno;
    if (fileDesc < 0) {
        printf("File open error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }

    return fileDesc;
}

unsigned char *create_buffer(unsigned int size) {
    unsigned char *buf = malloc(sizeof(unsigned char) * size);
    int error = errno;
    if (buf == NULL) {
        printf("Memory allocation error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
    return buf;
}

void write_to_file(int fileDescriptor, unsigned char *buffer, unsigned int bitsCount) {
    long long writtenBits = write(fileDescriptor, buffer, bitsCount);
    int error = errno;
    if (writtenBits != bitsCount) {
        printf("Write to file error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
}

void read_from_file(int fileDescriptor, unsigned char *buffer, unsigned int bitsCount) {
    long long readBits = read(fileDescriptor, buffer, bitsCount);
    int error = errno;
    if (readBits != bitsCount) {
        printf("Read from file error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
}

void sys_generate(char *filePath, unsigned int recordLength, unsigned int recordsCount) {
    assert(filePath != NULL);
    assert(recordLength > 0);
    assert(recordsCount > 0);

    unsigned char *buf = create_buffer(recordLength);
    int fileDesc = create_or_overwrite_file(filePath);

    for (unsigned int i = 0; i < recordsCount; ++i) {
        for (int j = 0; j < recordLength; ++j) {
            buf[j] = (unsigned char) rand();
        }
        write_to_file(fileDesc, buf, recordLength);
    }
    close(fileDesc);
}

void sys_sort(char *filePath, unsigned int recordLength, unsigned int recordsCount) {
    assert(filePath != NULL);
    assert(recordLength != 0);
    assert(recordsCount != 0);

    int fileDesc = open_file(filePath, O_RDWR);
    unsigned char *key = create_buffer(recordLength);
    unsigned char *mvd = create_buffer(recordLength);

    for (int i = 1; i < recordsCount; ++i) {
        lseek(fileDesc, (i - 1) * recordLength, SEEK_SET);
        read_from_file(fileDesc, mvd, recordLength);
        read_from_file(fileDesc, key, recordLength);

        int j = i - 1;
        while (mvd[0] > key[0]) {
            lseek(fileDesc, (j + 1) * recordLength, SEEK_SET);
            write_to_file(fileDesc, mvd, recordLength);
            j--;
            if (j < 0) { break; }
            lseek(fileDesc, j * recordLength, SEEK_SET);
            read_from_file(fileDesc, mvd, recordLength);
        }
        lseek(fileDesc, (j + 1) * recordLength, SEEK_SET);
        write_to_file(fileDesc, key, recordLength);
    }
    close(fileDesc);
}

void sys_copy(char *filePathFrom, char *filePathTo, unsigned int recordLength, unsigned int recordsCount) {
    assert(filePathFrom != NULL);
    assert(filePathTo != NULL);
    assert(recordLength != 0);
    assert(recordsCount != 0);

    int fileDescFrom = open_file(filePathFrom, O_RDONLY);
    int fileDescTo = create_or_overwrite_file(filePathTo);
    unsigned char *buffer = create_buffer(recordLength);

    for (int i = 0; i < recordsCount; ++i) {
        read_from_file(fileDescFrom, buffer, recordLength);
        write_to_file(fileDescTo, buffer, recordLength);
    }

    close(fileDescFrom);
    close(fileDescTo);
}