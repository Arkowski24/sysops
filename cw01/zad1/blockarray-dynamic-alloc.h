#ifndef blockarray_dynamic_h
#define blockarray_dynamic_h

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct CharBlockArray {
    unsigned int arrayLength;
    unsigned int blockLength;
    char **array;
};

struct CharBlockArray *create_array(unsigned int arrayLength, unsigned int blockLength);

void delete_array(struct CharBlockArray *blkArray, int deleteMode);

int insert_block(struct CharBlockArray *blkArray, char *block);

void remove_block(struct CharBlockArray *blkArray, unsigned int index, int deleteMode);

int sum_in_block(const char *block, unsigned int blockLength);

char *find_nearest_sum_block(struct CharBlockArray *blkArray, char *block, unsigned int blockLength);

#endif /* blockarray_dynamic_h */
