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
#include <sys/types.h>
#include <sys/stat.h>

int sys_create_or_overwrite_file(char *filePath) {
    int fileDesc = open(filePath, O_CREAT | O_EXCL | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IROTH);
    int error = errno;
    if (fileDesc < 0 && error == EEXIST) {
        char res;
        printf("File already exists. Overwrite it? (y/n) ");
        scanf("%c", &res);
        if (res == 'y' || res == 'Y') {
            fileDesc = open(filePath, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IROTH);
            error = errno;
        }
    }

    if (fileDesc < 0) {
        fprintf(stderr, "File open error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
    return fileDesc;
}

int sys_open_file(char *filePath, int flags) {
    int fileDesc = open(filePath, flags);
    int error = errno;
    if (fileDesc < 0) {
        fprintf(stderr, "File open error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }

    return fileDesc;
}

void sys_write_to_file(int fileDescriptor, unsigned char *buffer, unsigned int bitsCount) {
    long long writtenBits = write(fileDescriptor, buffer, bitsCount);
    int error = errno;
    if (writtenBits < 0) {
        fprintf(stderr, "Write to file error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    } else if (writtenBits != bitsCount) {
        fprintf(stderr, "Surpassed file size.\n");
        exit(EXIT_FAILURE);
    }
}

void sys_read_from_file(int fileDescriptor, unsigned char *buffer, unsigned int bitsCount) {
    long long readBits = read(fileDescriptor, buffer, bitsCount);
    int error = errno;
    if (readBits < 0) {
        fprintf(stderr, "Read from file error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    } else if(readBits != bitsCount){
        fprintf(stderr, "Surpassed file size.\n");
        exit(EXIT_FAILURE);
    }
}

void sys_generate(char *filePath, unsigned int recordsCount, unsigned int recordLength) {
    assert(filePath != NULL);
    assert(recordLength > 0);
    assert(recordsCount > 0);

    unsigned char *buf = create_buffer(recordLength);
    int fileDesc = sys_create_or_overwrite_file(filePath);

    for (unsigned int i = 0; i < recordsCount; ++i) {
        for (int j = 0; j < recordLength; ++j) {
            buf[j] = (unsigned char) rand();
        }
        sys_write_to_file(fileDesc, buf, recordLength);
    }
    close(fileDesc);
}

void sys_sort(char *filePath, unsigned int recordsCount, unsigned int recordLength) {
    assert(filePath != NULL);
    assert(recordLength != 0);
    assert(recordsCount != 0);

    int fileDesc = sys_open_file(filePath, O_RDWR);
    unsigned char *keyBuffer = create_buffer(recordLength);
    unsigned char *movedBuffer = create_buffer(recordLength);

    for (int i = 1; i < recordsCount; ++i) {
        lseek(fileDesc, (i - 1) * recordLength, SEEK_SET);
        sys_read_from_file(fileDesc, movedBuffer, recordLength);
        sys_read_from_file(fileDesc, keyBuffer, recordLength);

        int j = i - 1;
        while (movedBuffer[0] > keyBuffer[0]) {
            lseek(fileDesc, (j + 1) * recordLength, SEEK_SET);
            sys_write_to_file(fileDesc, movedBuffer, recordLength);
            j--;
            if (j < 0) { break; }
            lseek(fileDesc, j * recordLength, SEEK_SET);
            sys_read_from_file(fileDesc, movedBuffer, recordLength);
        }
        lseek(fileDesc, (j + 1) * recordLength, SEEK_SET);
        sys_write_to_file(fileDesc, keyBuffer, recordLength);
    }
    close(fileDesc);
}

void sys_copy(char *filePathFrom, char *filePathTo, unsigned int recordsCount, unsigned int recordLength) {
    assert(filePathFrom != NULL);
    assert(filePathTo != NULL);
    assert(recordLength != 0);
    assert(recordsCount != 0);

    int fileDescFrom = sys_open_file(filePathFrom, O_RDONLY);
    int fileDescTo = sys_create_or_overwrite_file(filePathTo);
    unsigned char *buffer = create_buffer(recordLength);

    for (int i = 0; i < recordsCount; ++i) {
        sys_read_from_file(fileDescFrom, buffer, recordLength);
        sys_write_to_file(fileDescTo, buffer, recordLength);
    }

    close(fileDescFrom);
    close(fileDescTo);
}