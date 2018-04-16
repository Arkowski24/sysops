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
        return NULL;
    } else {
        return file;
    }
}

void execute_next_command(char *args[]) {
    int offset = 0;
    while (!(args[offset] == NULL || (args[offset][0] == '|' && args[offset][1] == '\0'))) {
        offset++;
    }

    if (args[offset] == NULL) {
        execvp(args[0], args);
        int error = errno;
        exit(error);
    } else {
        args[offset] = NULL;
        int fn[2];
        if (pipe(fn) < 0) {
            print_error_and_exit(errno);
        }

        int child = fork();
        if (child == 0) {
            close(fn[0]);
            dup2(fn[1], STDOUT_FILENO);
            execvp(args[0], args);
            exit(EXIT_FAILURE);
        } else {
            close(fn[1]);
            dup2(fn[0], STDIN_FILENO);
            execute_next_command(args + offset + 1);
        }
    }
}

int execute_line_of_commands(char *args[]) {
    int child = fork();
    if (child < 0) {
        print_error_and_exit(errno);
    }

    int failed = 0;
    if (child == 0) {
        execute_next_command(args);
    } else {
        int res;
        while (wait(&res) > 0) {
            if (!(WIFEXITED(res) && WEXITSTATUS(res) == 0)) {
                failed++;
            }
        }
    }
    return failed;
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

char **fetch_commands(FILE *file) {
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
    if (argc < 2) {
        fprintf(stderr, "%s\n", "File path required.");
        exit(EXIT_FAILURE);
    }

    FILE *batchFile = open_file(argv[1]);
    char **command = fetch_commands(batchFile);
    unsigned int commandsCount = 0;

    while (command != NULL) {
        commandsCount++;
        int commandResult = execute_line_of_commands(command);
        if (commandResult != 0) {
            fprintf(stderr, "Task %i: One of the programs failed.\n", commandsCount);
            exit(EXIT_FAILURE);
        }
        free(command);
        command = fetch_commands(batchFile);
    }

    return 0;
}

