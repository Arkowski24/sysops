#define _XOPEN_SOURCE 600
#define _USE_XOPEN 1

#include <linux/limits.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <dirent.h>
#include <assert.h>
#include <sys/errno.h>
#include <zconf.h>
#include "dir_explorer.h"
#include <sys/wait.h>

enum Comparison parse_sign(char c) {
    switch (c) {
        case '>':
            return GREATER;
        case '=':
            return EQUAL;
        case '<':
            return SMALLER;
        default:
            fprintf(stderr, "%s\n", "Unrecognizable sign.");
            exit(EXIT_FAILURE);
    }
}

void parse_date(char *stringDate, struct tm *time) {
    if (strptime(stringDate, "%d.%m.%Y", time) == NULL) {
        fprintf(stderr, "%s\n", "Date parsing error.");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "%s\n", "Not enough arguments.");
        exit(EXIT_FAILURE);
    } else {
        char *directoryName = argv[1];
        enum Comparison cmp = parse_sign(argv[2][0]);
        struct tm readTime = {0};
        parse_date(argv[3], &readTime);

        explore_directory(directoryName, cmp, mktime(&readTime));
    }
    return 0;
}

DIR *open_directory(const char *filePath) {
    DIR *dir = opendir(filePath);
    int error = errno;
    if (dir == NULL) {
        fprintf(stderr, "%s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
    return dir;
}

struct dirent *read_directory(DIR *dir) {
    int errorBefore = errno;
    struct dirent *dirEntry = readdir(dir);
    int errorAfter = errno;
    if (dirEntry == NULL && errorBefore != errorAfter) {
        fprintf(stderr, "%s\n", strerror(errorAfter));
        exit(EXIT_FAILURE);
    }
    return dirEntry;
}

void get_stats(char *path, struct stat *fileStat) {
    int error;
    if (lstat(path, fileStat) < 0) {
        error = errno;
        fprintf(stderr, "%s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
}

char *create_buffer(unsigned int count) {
    char *buf = calloc(count, sizeof(char));
    int error = errno;
    if (buf == NULL) {
        fprintf(stderr, "Memory allocation error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
    return buf;
}

char *get_absolute_path(const char *relativePath) {
    char *directoryName = create_buffer(PATH_MAX + 1);
    if (realpath(relativePath, directoryName) == NULL) {
        int error = errno;
        fprintf(stderr, "%s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
    return directoryName;
}

void explore_directory(const char *filePath, enum Comparison comparison, time_t time) {
    DIR *dir = open_directory(filePath);
    struct dirent *dirEntry = read_directory(dir);

    while (dirEntry != NULL) {
        char *checkedPath = createNewFilePath(filePath, dirEntry->d_name);
        struct stat fileStat;
        get_stats(checkedPath, &fileStat);

        if (S_ISREG(fileStat.st_mode) != 0 && compareTimes(fileStat.st_mtime, time) == comparison) {
            print_file(checkedPath, fileStat);
        } else if (S_ISDIR(fileStat.st_mode) != 0 && strcmp(dirEntry->d_name, ".") != 0 &&
                   strcmp(dirEntry->d_name, "..") != 0) {
            pid_t child = fork();
            if (child == 0) {
                explore_directory(checkedPath, comparison, time);
            } else {
                wait(NULL);
            }
        }

        free(checkedPath);
        dirEntry = read_directory(dir);
    }
    exit(EXIT_SUCCESS);
}

char *createNewFilePath(const char *oldFilePath, char *fileName) {
    char *checkedPath = create_buffer(PATH_MAX + 1);
    strcat(checkedPath, oldFilePath);
    strcat(checkedPath, "/");
    strcat(checkedPath, fileName);
    return checkedPath;
}

enum Comparison compareTimes(time_t a, time_t b) {
    int yearA, yearB, daysA, daysB;

    struct tm *time = localtime(&a);
    yearA = time->tm_year;
    daysA = time->tm_yday;
    time = localtime(&b);
    yearB = time->tm_year;
    daysB = time->tm_yday;

    if (yearA == yearB) {
        if (daysA == daysB) {
            return EQUAL;
        } else {
            return (daysA > daysB) ? GREATER : SMALLER;
        }
    } else {
        return (yearA > yearB) ? GREATER : SMALLER;
    }
}

void print_file(char *path, struct stat statistic) {
    assert(path != NULL);

    char *permissions = get_file_permissions(statistic.st_mode);
    long long size = statistic.st_size;
    time_t time = statistic.st_mtime;
    char *modTime = asctime(localtime(&time));
    modTime[strlen(modTime) - 1] = '\0';
    char *absPath = get_absolute_path(path);

    printf("%s |%12lld | %s | %s \n", permissions, size, modTime, absPath);

    free(permissions);
    free(absPath);
}

char *get_file_permissions(mode_t mode) {
    char *permissions = create_buffer(10);

    permissions[0] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (mode & S_IWUSR) ? 'w' : '-';
    if (mode & S_ISUID) {
        permissions[2] = (mode & S_IXUSR) ? 's' : 'S';
    } else {
        permissions[2] = (mode & S_IXUSR) ? 'x' : '-';
    }

    permissions[3] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (mode & S_IWGRP) ? 'w' : '-';
    if (mode & S_ISGID) {
        permissions[5] = (mode & S_IXGRP) ? 's' : 'S';
    } else {
        permissions[5] = (mode & S_IXGRP) ? 'x' : '-';
    }

    permissions[6] = (mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (mode & S_IWOTH) ? 'w' : '-';
    if (mode & S_ISVTX) {
        permissions[8] = (mode & S_IXOTH) ? 's' : 'S';
    } else {
        permissions[8] = (mode & S_IXOTH) ? 'x' : '-';
    }
    permissions[9] = '\0';

    return permissions;
}
