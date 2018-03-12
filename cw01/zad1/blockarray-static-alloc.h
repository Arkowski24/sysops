#ifndef blockarray_static_h
#define blockarray_static_h

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_ARRAY_LENGTH 10000
#define MAX_BLOCK_LENGTH 10000

int blockarray_static_create_array(unsigned int arrayLength, unsigned int blockLength);

void blockarray_static_clear_array();

void blockarray_static_delete_array();

int blockarray_static_is_free(const char block[]);

int blockarray_static_insert_block(const char block[], unsigned int blockLength);

void blockarray_static_remove_block(unsigned int index);

int blockarray_static_sum_in_block(const char block[], unsigned int blockLength);

char *blockarray_static_find_nearest_sum_block(unsigned int index);

#endif /* blockarray_static_h */
