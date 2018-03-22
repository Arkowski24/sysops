//
// Created by Arkadiusz Placha on 22.03.2018.
//

#ifndef SYSOPS_DIR_EXPLORER_H
#define SYSOPS_DIR_EXPLORER_H

#include <sys/stat.h>

enum Comparison {
    GREATER, EQUAL, SMALLER
};

void explore_directory(char *filePath, enum Comparison comparison, time_t time);

char *get_file_permissions(mode_t mode);

void print_file(char *path, struct stat statistic);

enum Comparison compareTimes(time_t a, time_t b);

char *createNewFilePath(char *oldFilePath, char *fileName);


#endif //SYSOPS_DIR_EXPLORER_H
