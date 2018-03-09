#ifndef blockarray_static_h
#define blockarray_static_h

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define ARRAY_LENGTH 1000
#define BLOCK_LENGTH 1000

int insert_block(char array[][BLOCK_LENGTH], const char block[]);

void remove_block(char array[][BLOCK_LENGTH], unsigned int index);

int sum_in_block(const char *block, unsigned int blockLength);

char *find_nearest_sum_block(char array[][BLOCK_LENGTH], char block[], unsigned int blockLength);

#endif /* blockarray_static_h */
