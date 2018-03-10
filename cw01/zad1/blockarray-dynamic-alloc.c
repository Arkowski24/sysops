#include "blockarray-dynamic-alloc.h"

struct CharBlockArray *create_array(unsigned int arrayLength, unsigned int blockLength) {
    struct CharBlockArray *arrPtr;
    arrPtr = (struct CharBlockArray *) calloc(sizeof(struct CharBlockArray), 1);
    arrPtr->arrayLength = arrayLength;
    arrPtr->blockLength = blockLength;
    arrPtr->array = (char **) calloc(sizeof(char *), arrayLength);
    return arrPtr;
}

//deleteOption = 1 -> delete array with stored blocks, otherwise -> preserve blocks.
void delete_array(struct CharBlockArray *blkArray, int deleteMode) {
    if (blkArray == NULL) return;
    if (blkArray->array != NULL) {
        if (deleteMode == 1) {
            for (int i = 0; i < blkArray->arrayLength; ++i) {
                free(blkArray->array[i]);
            }
        }
        free(blkArray->array);
    }
    free(blkArray);
}

//Return: -1 if failed, otherwise inserted element's position.
int insert_block(struct CharBlockArray *blkArray, char *block) {
    if (blkArray == NULL || blkArray->array == NULL) return (-1);
    for (int i = 0; i < blkArray->arrayLength; ++i) {
        if (blkArray->array[i] == NULL) {
            blkArray->array[i] = block;
            return i;
        }
    }
    return (-1);
}

//deleteOption = 1 -> remove block and delete it, otherwise just remove it from array.
void remove_block(struct CharBlockArray *blkArray, unsigned int index, int deleteMode) {
    if (blkArray == NULL || blkArray->array == NULL || index >= blkArray->arrayLength) return;

    if (deleteMode == 1) {
        free(blkArray->array[index]);
    }
    blkArray->array[index] = NULL;
}

int sum_in_block(const char *block, unsigned int blockLength) {
    if (block == NULL) return 0;

    int sumInBlock = 0;
    for (unsigned int i = 0; i < blockLength; ++i) {
        sumInBlock += block[i];
    }
    return sumInBlock;
}

char *find_nearest_sum_block(struct CharBlockArray *blkArray, char *block, unsigned int blockLength) {
    if (block == NULL || blkArray == NULL || blkArray->array == NULL) return NULL;

    int searchedSum = sum_in_block(block, blockLength);
    int minimalDifference = INT_MAX;
    char *chosenBlock = NULL;

    for (int i = 0; i < blkArray->arrayLength; ++i) {
        if (blkArray->array[i] != NULL) {
            int sumInCheckedBlock = sum_in_block(blkArray->array[i], blkArray->blockLength);
            int difference = abs(searchedSum - sumInCheckedBlock);
            if (difference < minimalDifference) {
                minimalDifference = difference;
                chosenBlock = blkArray->array[i];
            }
        }
    }
    return chosenBlock;
}
