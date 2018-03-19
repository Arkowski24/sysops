//
// Created by Arkadiusz Placha on 18.03.2018.
//
#include <sys/errno.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include <stdlib.h>
#include "record_handler.h"

FILE *lib_create_or_overwrite_file(char *filePath) {
    FILE *fileDesc = fopen(filePath, "r+b");
    int error = errno;
    if (fileDesc < 0 && error == EEXIST) {
        char res;
        printf("File already exists. Overwrite it? (y/n) ");
        scanf("%c", &res);
        if (res == 'y' || res == 'Y') {
            fileDesc = fopen(filePath, "w+b");
            error = errno;
        }
    }

    if (fileDesc < 0) {
        printf("File open error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
    return fileDesc;
}

FILE *lib_open_file(char *filePath, char *mode) {
    FILE *fileDesc = fopen(filePath, mode);
    int error = errno;
    if (fileDesc < 0) {
        printf("File open error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }

    return fileDesc;
}

void lib_write_to_file(FILE *file, unsigned char *buffer, unsigned int bitsCount) {
    unsigned long long writtenBits = fwrite(buffer, bitsCount, 1, file);
    int error = errno;
    if (writtenBits != bitsCount) {
        printf("Write to file error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
}

void lib_read_from_file(FILE *file, unsigned char *buffer, unsigned int bitsCount) {
    unsigned long long readBits = fread(buffer, bitsCount, 1, file);
    int error = errno;
    if (readBits != bitsCount) {
        printf("Read from file error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
}

void lib_generate(char *filePath, unsigned int recordLength, unsigned int recordsCount) {
    assert(filePath != NULL);
    assert(recordLength > 0);
    assert(recordsCount > 0);

    unsigned char *buf = create_buffer(recordLength);
    FILE *file = lib_create_or_overwrite_file(filePath);

    for (unsigned int i = 0; i < recordsCount; ++i) {
        for (int j = 0; j < recordLength; ++j) {
            buf[j] = (unsigned char) rand();
        }
        lib_write_to_file(file, buf, recordLength);
    }
    fclose(file);
}

void lib_sort(char *filePath, unsigned int recordLength, unsigned int recordsCount) {
    assert(filePath != NULL);
    assert(recordLength != 0);
    assert(recordsCount != 0);

    FILE *file = lib_open_file(filePath, "r+b");
    unsigned char *keyBuffer = create_buffer(recordLength);
    unsigned char *movedBuffer = create_buffer(recordLength);

    for (int i = 1; i < recordsCount; ++i) {
        fseek(file, (i - 1) * recordLength, SEEK_SET);
        lib_read_from_file(file, movedBuffer, recordLength);
        lib_read_from_file(file, keyBuffer, recordLength);

        int j = i - 1;
        while (movedBuffer[0] > keyBuffer[0]) {
            fseek(file, (j + 1) * recordLength, SEEK_SET);
            lib_write_to_file(file, movedBuffer, recordLength);
            j--;
            if (j < 0) { break; }
            fseek(file, j * recordLength, SEEK_SET);
            lib_read_from_file(file, movedBuffer, recordLength);
        }
        fseek(file, (j + 1) * recordLength, SEEK_SET);
        lib_write_to_file(file, keyBuffer, recordLength);
    }
    fclose(file);
}

void lib_copy(char *filePathFrom, char *filePathTo, unsigned int recordLength, unsigned int recordsCount) {
    assert(filePathFrom != NULL);
    assert(filePathTo != NULL);
    assert(recordLength != 0);
    assert(recordsCount != 0);

    FILE *fromFile = lib_open_file(filePathFrom, "r+b");
    FILE *toFile = lib_create_or_overwrite_file(filePathTo);
    unsigned char *buffer = create_buffer(recordLength);

    for (int i = 0; i < recordsCount; ++i) {
        lib_read_from_file(fromFile, buffer, recordLength);
        lib_write_to_file(toFile, buffer, recordLength);
    }

    fclose(fromFile);
    fclose(toFile);
}