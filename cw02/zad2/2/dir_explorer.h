//
// Created by Arkadiusz Placha on 22.03.2018.
//

#ifndef SYSOPS_DIR_EXPLORER_H
#define SYSOPS_DIR_EXPLORER_H

#include <sys/stat.h>

enum Comparison {
    GREATER, EQUAL, SMALLER
};

enum Comparison parse_sign(char c);

void parse_date(char *stringDate, struct tm *time);

char *get_absolute_path(const char *relativePath);

int discover_file(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

enum Comparison compareTimes(time_t a, time_t b);

void print_file(const char *path, struct stat statistic);

char *get_file_permissions(mode_t mode);

char *create_buffer(unsigned int count);

#endif //SYSOPS_DIR_EXPLORER_H
