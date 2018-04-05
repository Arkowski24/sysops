//
// Created by Arkadiusz Placha on 30.03.2018.
//

#include <sys/wait.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "batch_executer.h"

#define MAX_ARGUMENTS_COUNT 32
#define MEMORY_CALCULATION_BASE 100000

static rlim_t cpuTimeLimit;
static rlim_t virtualMemoryLimit;

void print_error(int errorNumber) {
    fprintf(stderr, "%s \n", strerror(errorNumber));
}

FILE *open_file(char *filePath) {
    assert(filePath != NULL);

    FILE *file = fopen(filePath, "rb");
    int error = errno;
    if (file == NULL) {
        print_error(error);
        exit(EXIT_FAILURE);
    } else {
        return file;
    }
}

void read_usage(struct rusage *usage) {
    assert(usage != NULL);

    int result = getrusage(RUSAGE_CHILDREN, usage);
    int error = errno;
    if (result != 0) {
        print_error(error);
        exit(EXIT_FAILURE);
    }
}

void print_task_usage(unsigned int taskNumber, struct rusage *usage) {
    assert(usage != NULL);

    fprintf(stderr, "Task %u: System time: %li:%li s, User time: %li:%li s, Maximum RSS: %li kB.\n", taskNumber,
            usage->ru_utime.tv_sec,
            usage->ru_utime.tv_usec, usage->ru_stime.tv_sec, usage->ru_stime.tv_usec, usage->ru_maxrss);
}

int execute_command(char *program, char *args[], unsigned int taskNumber) {
    int pid = fork();
    if (pid == 0) {
        set_limit(RLIMIT_CPU, cpuTimeLimit);
        set_limit(RLIMIT_AS, virtualMemoryLimit);

        execvp(program, args);
        int error = errno;
        exit(error);
    } else {
        int taskResult;
        wait(&taskResult);

        struct rusage usage;
        read_usage(&usage);
        print_task_usage(taskNumber, &usage);

        return taskResult;
    }
}

void delete_line_chars(char *string) {
    assert(string != NULL);

    while (*string != '\0') {
        if (*string == '\n' || *string == '\r') {
            *string = ' ';
        }
        string++;
    }
}

char **fetch_command(FILE *file) {
    assert(file != NULL);
    assert(MAX_ARGUMENTS_COUNT > 0);

    char *line = NULL;
    size_t lineLength = 0;

    ssize_t charsRead = getline(&line, &lineLength, file);
    if (charsRead <= 0) {
        return NULL;
    }
    delete_line_chars(line);

    char **arguments = calloc(MAX_ARGUMENTS_COUNT + 1, sizeof(char *));
    arguments[0] = strtok(line, " ");
    for (int i = 1; i < MAX_ARGUMENTS_COUNT; ++i) {
        arguments[i] = strtok(NULL, " ");
        if (arguments[i] == NULL) {
            break;
        }
    }
    return arguments;
}

void set_limit(int limitType, rlim_t resourceLimit) {
    struct rlimit limit;
    int result = getrlimit(limitType, &limit);
    int error = errno;
    if (result != 0) {
        print_error(error);
        exit(EXIT_FAILURE);
    }

    if (resourceLimit < limit.rlim_max) {
        limit.rlim_cur = resourceLimit;
    }
    result = setrlimit(limitType, &limit);
    error = errno;
    if (result != 0) {
        print_error(error);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("%s\n", "Usage: <file name> <CPU time limit[s]> <Virtual memory limit[MB]>");
        exit(EXIT_FAILURE);
    }

    cpuTimeLimit = strtoull(argv[2], NULL, 0);
    double memLimit = strtod(argv[3], NULL);
    virtualMemoryLimit = (rlim_t) memLimit * MEMORY_CALCULATION_BASE;

    FILE *batchFile = open_file(argv[1]);
    char **command = fetch_command(batchFile);
    unsigned int commandsCount = 0;

    while (command != NULL) {
        commandsCount++;
        int commandResult = execute_command(command[0], command, commandsCount);
        if (commandResult != 0) {
            fprintf(stderr, "Task %i: Program (%s) terminated with error code %i.\n", commandsCount, command[0],
                    commandResult);
            exit(EXIT_FAILURE);
        }
        free(command);
        command = fetch_command(batchFile);
    }

    return 0;
}

