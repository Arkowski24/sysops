#ifndef CHARBLOCKARRAY_ARRTEST_H
#define CHARBLOCKARRAY_ARRTEST_H

#include "../zad1/blockarray-dynamic-alloc.h"
#include "../zad1/blockarray-static-alloc.h"

struct Operation {
    short int operationType;
    unsigned int argument;
};

void execute_static();

void execute_dynamic();

void to_low_letters(char *str);

int parse_command_line(int argc, char **argv);

int get_allocation_method(char **argv);

int parse_options(int argc, char **argv);

char *create_random_block(unsigned int blocksCount);

void static_init(unsigned int arrayLength, unsigned int blockLength);

void static_find_nearest(unsigned int index);

void static_remove_and_insert(unsigned int blocksCount, unsigned int blockLength);

void static_alternatively_remove_and_insert(unsigned int blocksCount, unsigned int blockLength);

void dynamic_init(struct CharBlockArray *blockArray, unsigned int arrayLength, unsigned int blockLength);

void dynamic_clr(struct CharBlockArray *blockArray);

void dynamic_find_nearest(struct CharBlockArray *blockArray, unsigned int index);

void dynamic_remove_and_insert(struct CharBlockArray *blockArray, unsigned int blocksCount);

void dynamic_alternatively_remove_and_insert(struct CharBlockArray *blockArray, unsigned int blocksCount);

#endif //CHARBLOCKARRAY_ARRTEST_H
