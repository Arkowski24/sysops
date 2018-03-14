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

struct CharBlockArray *blockarray_dynamic_create_array(unsigned int arrayLength, unsigned int blockLength);

void blockarray_dynamic_delete_array(struct CharBlockArray *blkArray, int deleteMode);

void blockarray_dynamic_clear_array(struct CharBlockArray *blkArray);

int blockarray_dynamic_insert_block(struct CharBlockArray *blkArray, char *block);

int blockarray_dynamic_remove_block(struct CharBlockArray *blkArray, unsigned int index, int deleteMode);

int blockarray_dynamic_sum_in_block(const char *block, unsigned int blockLength);

char *blockarray_dynamic_find_nearest_sum_block(struct CharBlockArray *blkArray, unsigned int index);

#endif /* blockarray_dynamic_h */
