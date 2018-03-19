#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/errno.h>
#include "record_handler.h"

void exit_failure_unknown_type(char *type) {
    fprintf(stderr, "Unknown %s.\n", type);
    exit(EXIT_FAILURE);
}

void exit_failure_not_enough_arguments() {
    fprintf(stderr, "Not enough arguments.\n");
    exit(EXIT_FAILURE);
}

void exit_failure_zero_argument(char *argumentName) {
    fprintf(stderr, "Argument %s cannot be 0.\n", argumentName);
    exit(EXIT_FAILURE);
}

void parseCommandLine(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc < 4) {
        exit_failure_not_enough_arguments();
    }
    parseCommandLine(argc, argv);

    return 0;
}

void parseCommandLine(int argc, char *argv[]) {
    char *command = argv[1];
    if (strcmp(command, "generate") == 0) {
        if (argc < 5) { exit_failure_not_enough_arguments(); }

        char *filePath = argv[2];
        unsigned int recordCount = strtoul(argv[3], NULL, 10);
        unsigned int recordLength = strtoul(argv[4], NULL, 10);
        if (recordLength == 0 || recordCount == 0) { exit_failure_zero_argument("record length/count"); }

        sys_generate(filePath, recordCount, recordLength);
    } else if (strcmp(command, "sort") == 0) {
        if (argc < 6) { exit_failure_not_enough_arguments(); }

        char *filePath = argv[2];
        unsigned int recordCount = strtoul(argv[3], NULL, 10);
        unsigned int recordLength = strtoul(argv[4], NULL, 10);
        char *variant = argv[5];
        if (recordLength == 0 || recordCount == 0) { exit_failure_zero_argument("record length/count"); }

        if (strcmp(variant, "sys") == 0) {
            sys_sort(filePath, recordCount, recordLength);
        } else if (strcmp(variant, "lib") == 0) {
            lib_sort(filePath, recordCount, recordLength);
        } else {
            exit_failure_unknown_type("variant");
        }
    } else if (strcmp(command, "copy") == 0) {
        if (argc < 7) { exit_failure_not_enough_arguments(); }

        char *filePathFrom = argv[2];
        char *filePathTo = argv[3];
        unsigned int recordCount = strtoul(argv[4], NULL, 10);
        unsigned int recordLength = strtoul(argv[5], NULL, 10);
        char *variant = argv[6];
        if (recordLength == 0 || recordCount == 0) { exit_failure_zero_argument("record length/count"); }

        if (strcmp(variant, "sys") == 0) {
            sys_copy(filePathFrom, filePathTo, recordCount, recordLength);
        } else if (strcmp(variant, "lib") == 0) {
            lib_copy(filePathFrom, filePathTo, recordCount, recordLength);
        } else {
            exit_failure_unknown_type("variant");
        }
    } else {
        exit_failure_unknown_type("command");
    }
}

unsigned char *create_buffer(unsigned int size) {
    unsigned char *buf = malloc(sizeof(unsigned char) * size);
    int error = errno;
    if (buf == NULL) {
        fprintf(stderr, "Memory allocation error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
    return buf;
}