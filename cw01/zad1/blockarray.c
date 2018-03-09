#include "blockarray.h"

struct CharBlockArray *create_array(unsigned int maxBlocksCount) {
    struct CharBlockArray *arrPtr;
    arrPtr = (struct CharBlockArray *) malloc(sizeof(struct CharBlockArray));
    arrPtr->maxBlocksCount = maxBlocksCount;
    arrPtr->array = (struct CharBlock **) malloc(sizeof(struct CharBlock *) * maxBlocksCount);
    for (int i = 0; i < maxBlocksCount; i++) {
        arrPtr->array[i] = NULL;
    }
    return arrPtr;
}

//Blocks remain in memory, because only pointers to them are stored
void delete_array(struct CharBlockArray *blkArray) {
    if (blkArray == NULL) return;
    if (blkArray->array != NULL) {
        free(blkArray->array);
    }

    free(blkArray);
}

//Blocks whose pointers are stored in array are also removed from memory
void delete_array_with_blocks(struct CharBlockArray *blkArray) {
    if (blkArray == NULL) return;
    if (blkArray->array != NULL) {
        for (int i = 0; i < blkArray->maxBlocksCount; i++) {
            delete_block(blkArray->array[i]);
        }
    }
    delete_array(blkArray);
}

int add_block(struct CharBlockArray *blkArray, struct CharBlock *block) {
    if (blkArray == NULL || blkArray->array == NULL || block == NULL) return ERROR_NULL_POINTER;

    for (int i = 0; i < blkArray->maxBlocksCount; i++) {
        if (blkArray->array[i] == NULL) {
            blkArray->array[i] = block;
            return SUCCESS;
        }
    }
    return ERROR_OUT_OF_SPACE;
}

int add_blocks(struct CharBlockArray *blkArray, struct CharBlock **blocks, unsigned int blocksCount) {
    if (blkArray == NULL || blkArray->array == NULL || blocks == NULL) return ERROR_NULL_POINTER;
    if (null_blocks_count(blkArray) < blocksCount) return ERROR_OUT_OF_SPACE;

    unsigned int blkItr = 0;
    for (int i = 0; i < blkArray->maxBlocksCount; i++) {
        if (blkArray->array[i] == NULL) {
            blkArray->array[i] = blocks[blkItr];
            blkItr++;
        }
    }
    return SUCCESS;
}

//Deletes first appearance of given pointer
int remove_block(struct CharBlockArray *blkArray, struct CharBlock *block) {
    if (blkArray == NULL || blkArray->array == NULL || block == NULL) return ERROR_NULL_POINTER;

    for (int i = 0; i < blkArray->maxBlocksCount; i++) {
        if (blkArray->array[i] == block) {
            blkArray->array[i] = NULL;
            return SUCCESS;
        }
    }
    return ERROR_NOT_FOUND;
}

//Deletes blocks in given array, blocks that weren't found are ignored
int remove_blocks(struct CharBlockArray *blkArray, struct CharBlock **blocks, unsigned int blocksCount) {
    if (blkArray == NULL || blkArray->array == NULL || blocks == NULL) return ERROR_NULL_POINTER;

    for (int j = 0; j < blocksCount; j++) {
        remove_block(blkArray, blocks[j]);
    }
    return SUCCESS;
}

unsigned int null_blocks_count(struct CharBlockArray *blkArray) {
    if (blkArray == NULL || blkArray->array == NULL) return 0;

    unsigned int freeBlocks = 0;
    for (int i = 0; i < blkArray->maxBlocksCount; i++) {
        if (blkArray->array[i] == NULL) {
            freeBlocks++;
        }
    }
    return freeBlocks;
}

struct CharBlock *find_nearest_sum_block(struct CharBlockArray *blkArray, int number) {
    if (blkArray == NULL || blkArray->array == NULL) return NULL;

    int minimalDifference = INT_MAX;
    struct CharBlock *chosenBlock = NULL;
    for (int i = 0; i < blkArray->maxBlocksCount; i++) {
        if (blkArray->array[i] != NULL) {
            int difference = abs(number - sum_in_block(blkArray->array[i]));
            if (difference < minimalDifference) {
                minimalDifference = difference;
                chosenBlock = blkArray->array[i];
            }
        }
    }
    return chosenBlock;
}
