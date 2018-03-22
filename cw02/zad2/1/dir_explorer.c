#define _XOPEN_SOURCE       /* See feature_test_macros(7) */
#include <dirent.h>
#include <stddef.h>
#include <linux/limits.h>
#include <limits.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <sys/errno.h>
#include "dir_explorer.h"

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

DIR *open_directory(char *filePath) {
    DIR *dir = opendir(filePath);
    int error = errno;
    if (dir == NULL) {
        fprintf(stderr, "%s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
    return dir;
}

struct dirent *read_directory(DIR *dir) {
    struct dirent *dirEntry = readdir(dir);
    int error = errno;
    if (error != 0) {
        fprintf(stderr, "%s\n", strerror(error));
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

void explore_directory(char *filePath, enum Comparison comparison, time_t time) {
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
            explore_directory(checkedPath, comparison, time);
        }

        free(checkedPath);
        dirEntry = read_directory(dir);
    }
}

char *createNewFilePath(char *oldFilePath, char *fileName) {
    char *checkedPath = calloc(PATH_MAX, sizeof(char));
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
        return (daysA > daysB) ? GREATER : SMALLER;
    }

}

void print_file(char *path, struct stat statistic) {
    assert(path != NULL);

    char *permissions = get_file_permissions(statistic.st_mode);
    long long size = statistic.st_size;
    time_t time = statistic.st_mtime;
    char *modTime = asctime(localtime(&time));
    modTime[strlen(modTime) - 1] = '\0';
    char *p = calloc(PATH_MAX + 1, sizeof(char));
    realpath(path, p);

    printf("%s |%12lld | %s | %s \n", permissions, size, modTime, p);

    free(permissions);
}

char *get_file_permissions(mode_t mode) {
    char *permissions = malloc(sizeof(char) * 10);

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