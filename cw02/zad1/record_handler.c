#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/errno.h>
#include "record_handler.h"

void exit_failure_unknown_type(char *type) {
    printf("Unknown %s.", type);
    exit(EXIT_FAILURE);
}

void exit_failure_not_enough_arguments() {
    printf("Not enough arguments.");
    exit(EXIT_FAILURE);
}

void exit_failure_zero_argument(char *argumentName) {
    printf("Argument %s cannot be 0.", argumentName);
    exit(EXIT_FAILURE);
}

void parseCommandLine(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc < 6) {
        exit_failure_not_enough_arguments();
    }
    parseCommandLine(argc, argv);

    return 0;
}

void parseCommandLine(int argc, char *argv[]) {
    char *command = argv[1];
    if (strcmp(command, "copy") == 0) {
        if (argc < 7) {
            exit_failure_not_enough_arguments();
        }
        char *filePathFrom = argv[2];
        char *filePathTo = argv[3];
        unsigned int recordLength = strtoul(argv[4], NULL, 10);
        unsigned int recordCount = strtoul(argv[5], NULL, 10);
        char *variant = argv[6];

        if (recordLength == 0 || recordCount == 0) {
            exit_failure_zero_argument("record length/count");
        }

        if (strcmp(variant, "sys") == 0) {
            sys_copy(filePathFrom, filePathTo, recordLength, recordCount);
        } else if (strcmp(variant, "lib") == 0) {
            lib_copy(filePathFrom, filePathTo, recordLength, recordCount);
        } else {
            exit_failure_unknown_type("variant");
        }

    } else {
        char *filePath = argv[2];
        unsigned int recordLength = strtoul(argv[3], NULL, 10);
        unsigned int recordCount = strtoul(argv[4], NULL, 10);
        char *variant = argv[5];

        if (strcmp(variant, "sys") == 0) {
            if (strcmp(command, "generate") == 0) {
                sys_generate(filePath, recordLength, recordCount);
            } else if (strcmp(command, "sort") == 0) {
                sys_sort(filePath, recordLength, recordCount);
            } else {
                exit_failure_unknown_type("command");
            }
        } else if (strcmp(variant, "lib") == 0) {
            if (strcmp(command, "generate") == 0) {
                lib_generate(filePath, recordLength, recordCount);
            } else if (strcmp(command, "sort") == 0) {
                lib_sort(filePath, recordLength, recordCount);
            } else {
                exit_failure_unknown_type("command");
            }
        } else {
            exit_failure_unknown_type("variant");
        }
    }
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