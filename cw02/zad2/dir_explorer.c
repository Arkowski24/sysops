#include <sys/types.h>
#include <dirent.h>
#include <stddef.h>
#include <limits.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <sys/errno.h>
#include "dir_explorer.h"

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Not enough arguments.\n");
        exit(EXIT_FAILURE);
    } else {
        char *directoryName = argv[1];
        enum Comparison cmp;
        struct tm readTime = {0};
        char *modDate = argv[3];

        if (strptime(modDate, "%d.%m.%Y", &readTime) == NULL) {
            fprintf(stderr, "Date parsing error. \n");
            exit(EXIT_FAILURE);
        }

        switch (argv[2][0]) {
            case '>':
                cmp = GREATER;
                break;
            case '=':
                cmp = EQUAL;
                break;
            case '<':
                cmp = SMALLER;
                break;
            default:
                fprintf(stderr, "Unrecognizable sign. \n");
                exit(EXIT_FAILURE);
        }
        explore_directory(directoryName, cmp, mktime(&readTime));
    }
}

void explore_directory(char *filePath, enum Comparison comparison, time_t time) {
    int error;

    DIR *dir = opendir(filePath);
    error = errno;
    if (dir == NULL) {
        fprintf(stderr, strerror(error));
        exit(EXIT_FAILURE);
    }

    struct dirent *dirEntry = readdir(dir);
    if (dirEntry == NULL) {
        fprintf(stderr, strerror(error));
        exit(EXIT_FAILURE);
    }

    while (dirEntry != NULL) {
        char *checkedPath = createNewFilePath(filePath, dirEntry->d_name);
        struct stat fileStat;

        if (lstat(checkedPath, &fileStat) < 0) {
            error = errno;
            fprintf(stderr, "%s\n", strerror(error));
            exit(EXIT_FAILURE);
        }

        if (S_ISREG(fileStat.st_mode) != 0 && compareTimes(fileStat.st_mtime, time) == comparison) {
            print_file(checkedPath, fileStat);
        } else if (S_ISDIR(fileStat.st_mode) != 0 && strcmp(dirEntry->d_name, ".") != 0 &&
                   strcmp(dirEntry->d_name, "..") != 0) {
            explore_directory(checkedPath, comparison, time);
        }

        free(checkedPath);
        dirEntry = readdir(dir);
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
    printf("%s |%12lld | %s | %s \n", permissions, size, modTime, path);

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