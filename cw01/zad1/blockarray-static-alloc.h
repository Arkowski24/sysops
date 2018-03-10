#ifndef blockarray_static_h
#define blockarray_static_h

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_ARRAY_LENGTH 1000

void blockarray_static_clear_array();

int blockarray_static_count_free_places(const char block[], int blockLength);

int blockarray_static_insert_block(const char block[], int blockLength);

void blockarray_static_remove_block(unsigned int index, int blockLength);

int blockarray_static_sum_in_block(const char *block, unsigned int blockLength);

char *blockarray_static_find_nearest_sum_block(char block[], unsigned int blockLength);

#endif /* blockarray_static_h */
