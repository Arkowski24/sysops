#include "blockarray-static-alloc.h"

char blockArray[MAX_ARRAY_LENGTH][MAX_BLOCK_LENGTH] = {0};
unsigned int currentArrayLength = 0;
unsigned int currentBlockLength = 0;

//1 if failed (illegal length of block or array), otherwise 0
int blockarray_static_create_array(unsigned int arrayLength, unsigned int blockLength) {
    if (arrayLength > MAX_ARRAY_LENGTH || blockLength > MAX_BLOCK_LENGTH) {
        return 1;
    }
    currentArrayLength = arrayLength;
    currentBlockLength = blockLength;
    return 0;
}

void blockarray_static_clear_array() {
    for (int i = 0; i < currentArrayLength; ++i) {
        for (int j = 0; j < currentBlockLength; ++j) {
            blockArray[i][j] = 0;
        }
    }
}

void blockarray_static_delete_array() {
    blockarray_static_clear_array();
    currentArrayLength = 0;
    currentBlockLength = 0;
}

//1 if block is not free (all elements are not 0), otherwise 0
int blockarray_static_is_free(const char block[]) {
    for (int i = 0; i < currentBlockLength; ++i) {
        if (block[i] != 0) {
            return 1;
        }
    }
    return 0;
}

//Return: -1 if failed, otherwise inserted element's position.
int blockarray_static_insert_block(const char block[], unsigned int blockLength) {
    int chosenBlock = -1;
    for (int i = 0; i < currentArrayLength; ++i) {
        if (blockarray_static_is_free(blockArray[i]) == 0) {
            chosenBlock = i;
            break;
        }
    }
    if (chosenBlock != -1) {
        if (blockLength >= currentArrayLength) {
            blockLength = currentArrayLength - 1;
        }
        for (int i = 0; i < blockLength; ++i) {
            blockArray[chosenBlock][i] = block[i];
        }
    }
    return chosenBlock;
}

void blockarray_static_remove_block(unsigned int index) {
    if (index >= currentArrayLength) return;

    for (int j = 0; j < currentBlockLength; ++j) {
        blockArray[index][j] = 0;
    }
}

int blockarray_static_sum_in_block(const char block[], unsigned int blockLength) {
    if (block == NULL) return 0;

    int sumInBlock = 0;
    for (unsigned int i = 0; i < blockLength; ++i) {
        sumInBlock += block[i];
    }
    return sumInBlock;
}

char *blockarray_static_find_nearest_sum_block(unsigned int index) {
    if (index >= currentArrayLength) return NULL;

    int searchedSum = blockarray_static_sum_in_block(blockArray[index], currentBlockLength);
    int minimalDifference = INT_MAX;
    char *chosenBlock = NULL;

    for (int i = 0; i < currentArrayLength; ++i) {
        if (i != index) {
            int sumInCheckedBlock = blockarray_static_sum_in_block(blockArray[i], currentBlockLength);
            int difference = abs(searchedSum - sumInCheckedBlock);
            if (difference < minimalDifference) {
                minimalDifference = difference;
                chosenBlock = blockArray[i];
            }
        }
    }
    return chosenBlock;
}
