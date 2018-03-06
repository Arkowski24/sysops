//
//  blockarray.h
//  lab01
//
//  Created by Arkadiusz Placha on 06.03.2018.
//  Copyright © 2018 FaraldCorp. All rights reserved.
//

#ifndef blockarray_h
#define blockarray_h

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "charblock.h"

#define SUCCESS 0
#define ERROR_OUT_OF_SPACE -1
#define ERROR_NULL_POINTER -2
#define ERROR_NOT_FOUND -3

struct CharBlockArray {
    unsigned int maxBlocksCount;
    struct CharBlock** array;
};

struct CharBlockArray* create_array(unsigned int maxBlocks);
void delete_array(struct CharBlockArray* blkArray);
void delete_array_with_blocks(struct CharBlockArray* blkArray);

int add_block(struct CharBlockArray* blkArray, struct CharBlock* block);
int add_blocks(struct CharBlockArray* blkArray, struct CharBlock** blocks, unsigned int blocksCount);

int remove_block(struct CharBlockArray* blkArray, struct CharBlock* block);
int remove_blocks(struct CharBlockArray* blkArray, struct CharBlock** blocks, unsigned int blocksCount);

unsigned int null_blocks_count(struct CharBlockArray* blkArray);
struct CharBlock* find_nearest_sum_block(struct CharBlockArray* blkArray, int number);

#endif /* blockarray_h */
