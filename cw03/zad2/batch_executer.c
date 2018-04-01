//
// Created by Arkadiusz Placha on 30.03.2018.
//

#include <zconf.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_ARGUMENTS_COUNT 32

void print_error_and_exit(int errorNumber) {
    fprintf(stderr, "%s \n", strerror(errorNumber));
    exit(EXIT_FAILURE);
}

FILE *open_file(char *filePath) {
    assert(filePath != NULL);

    FILE *file = fopen(filePath, "rb");
    int error = errno;
    if (file == NULL) {
        print_error_and_exit(error);
    } else {
        return file;
    }
}

int execute_command(char *program, char *args[]) {
    int pid = fork();
    if (pid == 0) {
        execvp(program, args);
        int error = errno;
        exit(error);
    } else {
        int result;
        wait(&result);
        return result;
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

int main(int argc, char *argv[]) {
    if (argc < 1) {
        printf("%s\n", "File path required.");
        exit(EXIT_FAILURE);
    }

    FILE *batchFile = open_file(argv[1]);
    char **command = fetch_command(batchFile);
    int commandsCount = 0;

    while (command != NULL) {
        commandsCount++;
        int commandResult = execute_command(command[0], command);
        if (commandResult != 0) {
            printf("Line %i: Error %i with command: %s", commandsCount, commandResult, command[0]);
            exit(EXIT_FAILURE);
        }
        free(command);
        command = fetch_command(batchFile);
    }

    return 0;
}

