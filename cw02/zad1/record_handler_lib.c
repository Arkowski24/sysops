//
// Created by Arkadiusz Placha on 18.03.2018.
//
#include <sys/errno.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "record_handler.h"

FILE *lib_create_or_overwrite_file(char *filePath) {
    FILE *file = fopen(filePath, "r+");
    int error = errno;
    if (file != NULL) {
        char res;
        printf("File already exists. Overwrite it? (y/n) ");
        scanf("%c", &res);
        if (res == 'y' || res == 'Y') {
            file = fopen(filePath, "w+");
            error = errno;
        }
    } else{
        file = fopen(filePath, "w+");
        error = errno;
    }

    if (file == NULL) {
        fprintf(stderr, "File open error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }

    return file;
}

FILE *lib_open_file(char *filePath, char *mode) {
    FILE *fileDesc = fopen(filePath, mode);
    int error = errno;
    if (fileDesc == NULL) {
        fprintf(stderr, "File open error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }

    return fileDesc;
}

void lib_write_to_file(FILE *file, unsigned char *buffer, unsigned int bitsCount) {
    unsigned long long writtenElements = fwrite(buffer, sizeof(char), bitsCount, file);
    int error = errno;
    if (writtenElements != bitsCount && ferror(file)) {
        fprintf(stderr, "Write to file error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    } else if (writtenElements != bitsCount) {
        fprintf(stderr, "Surpassed file size. \n");
        exit(EXIT_FAILURE);
    }
}

void lib_read_from_file(FILE *file, unsigned char *buffer, unsigned int bitsCount) {
    unsigned long long readElements = fread(buffer, sizeof(char), bitsCount, file);
    int error = errno;
    if (readElements != bitsCount && ferror(file) != 0) {
        fprintf(stderr, "Read from file error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    } else if (readElements != bitsCount) {
        fprintf(stderr, "Surpassed file size. \n");
        exit(EXIT_FAILURE);
    }
}

void lib_sort(char *filePath, unsigned int recordsCount, unsigned int recordLength) {
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

void lib_copy(char *filePathFrom, char *filePathTo, unsigned int recordsCount, unsigned int recordLength) {
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