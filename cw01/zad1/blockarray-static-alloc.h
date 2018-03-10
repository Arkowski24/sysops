#ifndef blockarray_static_h
#define blockarray_static_h

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_ARRAY_LENGTH 1000

extern static char *blockArray[MAX_ARRAY_LENGTH];

void clear_array();

int count_free_places(const char block[], int blockLength);

int insert_block(const char block[], int blockLength);

void remove_block(unsigned int index, int blockLength);

int sum_in_block(const char *block, unsigned int blockLength);

char *find_nearest_sum_block(char block[], unsigned int blockLength);

#endif /* blockarray_static_h */
