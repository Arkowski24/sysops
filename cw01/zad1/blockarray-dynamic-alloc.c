#include "blockarray-dynamic-alloc.h"

struct CharBlockArray *blockarray_dynamic_create_array(unsigned int arrayLength, unsigned int blockLength) {
    struct CharBlockArray *arrPtr;
    arrPtr = (struct CharBlockArray *) calloc(sizeof(struct CharBlockArray), 1);
    arrPtr->arrayLength = arrayLength;
    arrPtr->blockLength = blockLength;
    arrPtr->array = (char **) calloc(sizeof(char *), arrayLength);
    return arrPtr;
}

//deleteOption = 1 -> delete array with stored blocks, otherwise -> preserve blocks.
void blockarray_dynamic_delete_array(struct CharBlockArray *blkArray, int deleteMode) {
    if (blkArray == NULL) return;
    if (blkArray->array != NULL) {
        if (deleteMode == 1) {
            blockarray_dynamic_clear_array(blkArray);
        }
        free(blkArray->array);
    }
    free(blkArray);
}

void blockarray_dynamic_clear_array(struct CharBlockArray *blkArray) {
    if (blkArray == NULL) return;
    if (blkArray->array != NULL) {
        for (int i = 0; i < blkArray->arrayLength; ++i) {
            free(blkArray->array[i]);
        }
    }
}

//Return: -1 if failed, otherwise inserted element's position.
int blockarray_dynamic_insert_block(struct CharBlockArray *blkArray, char *block) {
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
void blockarray_dynamic_remove_block(struct CharBlockArray *blkArray, unsigned int index, int deleteMode) {
    if (blkArray == NULL || blkArray->array == NULL || index >= blkArray->arrayLength) return;

    if (deleteMode == 1) {
        free(blkArray->array[index]);
    }
    blkArray->array[index] = NULL;
}

int blockarray_dynamic_sum_in_block(const char *block, unsigned int blockLength) {
    if (block == NULL) return 0;

    int sumInBlock = 0;
    for (unsigned int i = 0; i < blockLength; ++i) {
        sumInBlock += block[i];
    }
    return sumInBlock;
}

char *blockarray_dynamic_find_nearest_sum_block(struct CharBlockArray *blkArray, unsigned int index) {
    if (blkArray == NULL || blkArray->array == NULL || index >= blkArray->arrayLength || blkArray->array[index] == NULL) return NULL;

    int searchedSum = blockarray_dynamic_sum_in_block(blkArray->array[index], blkArray->blockLength);
    int minimalDifference = INT_MAX;
    char *chosenBlock = NULL;

    for (int i = 0; i < blkArray->arrayLength; ++i) {
        if (blkArray->array[i] != NULL && i != index) {
            int sumInCheckedBlock = blockarray_dynamic_sum_in_block(blkArray->array[i], blkArray->blockLength);
            int difference = abs(searchedSum - sumInCheckedBlock);
            if (difference < minimalDifference) {
                minimalDifference = difference;
                chosenBlock = blkArray->array[i];
            }
        }
    }
    return chosenBlock;
}
