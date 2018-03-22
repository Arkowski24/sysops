#define _XOPEN_SOURCE 700
#define _USE_XOPEN 1

#include <ftw.h>
#include <stddef.h>
#include <limits.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <sys/errno.h>
#include "dir_explorer.h"

static enum Comparison comparisonMode;
static time_t searchedTime;

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "%s\n", "Not enough arguments.");
        exit(EXIT_FAILURE);
    } else {
        char *directoryName = get_absolute_path(argv[1]);

        comparisonMode = parse_sign(argv[2][0]);
        struct tm readTime = {0};
        parse_date(argv[3], &readTime);
        searchedTime = mktime(&readTime);

        nftw(directoryName, discover_file, 32, FTW_PHYS);
        free(directoryName);
    }
    return 0;
}

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

char *get_absolute_path(char *relativePath) {
    char *directoryName = calloc(PATH_MAX + 1, sizeof(char));
    if (realpath(relativePath, directoryName) == NULL) {
        int error = errno;
        fprintf(stderr, "%s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
    return directoryName;
}

int discover_file(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) {
        if (compareTimes(sb->st_mtime, searchedTime) == comparisonMode) {
            print_file(fpath, *sb);
        }
    }
    return 0;
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

void print_file(const char *path, struct stat statistic) {
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
