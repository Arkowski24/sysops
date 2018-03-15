#ifndef CHARBLOCKARRAY_ARRTEST_H
#define CHARBLOCKARRAY_ARRTEST_H

#define OPERATIONS_NUMBER 4

#include <sys/times.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "../../../zad1/blockarray-dynamic-alloc.h"
#include "../../../zad1/blockarray-static-alloc.h"

struct Operation {
    short int operationType;
    unsigned int argument;
};

void execute_static();

void execute_dynamic();

int dynamic_load_lib_functions();

int static_load_lib_functions();

void to_low_letters(char *str);

int parse_command_line(int argc, char **argv);

int get_allocation_method(char **argv);

int parse_options(int argc, char **argv);

void print_times(clock_t startTime, clock_t endTime, struct tms startCPU, struct tms endCPU);

char *create_random_block(unsigned int blocksCount);

void static_init(unsigned int arrayLength, unsigned int blockLength);

void static_clear();

void static_find_nearest(unsigned int index);

void static_remove_and_insert(unsigned int blocksCount, unsigned int blockLength);

void static_alternatively_remove_and_insert(unsigned int blocksCount, unsigned int blockLength);

void static_insert(unsigned int blocksCount, unsigned int blockLength);

void static_remove(unsigned int blocksCount);

void dynamic_init(struct CharBlockArray *blockArray, unsigned int arrayLength, unsigned int blockLength);

void dynamic_delete(struct CharBlockArray *blockArray);

void dynamic_clear(struct CharBlockArray *blockArray);

void dynamic_find_nearest(struct CharBlockArray *blockArray, unsigned int index);

void dynamic_remove_and_insert(struct CharBlockArray *blockArray, unsigned int blocksCount);

void dynamic_alternatively_remove_and_insert(struct CharBlockArray *blockArray, unsigned int blocksCount);

void dynamic_insert(struct CharBlockArray *blockArray, unsigned int blocksCount);

void dynamic_remove(struct CharBlockArray *blockArray, unsigned int blocksCount);

#endif //CHARBLOCKARRAY_ARRTEST_H
