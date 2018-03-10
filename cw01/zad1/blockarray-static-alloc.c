#include "blockarray-static-alloc.h"

static char *blockArray[MAX_ARRAY_LENGTH];

void blockarray_static_clear_array() {
    for (int i = 0; i < MAX_ARRAY_LENGTH; ++i) {
        blockArray[i] = 0;
    }
}

int blockarray_static_count_free_places(const char block[], int blockLength) {
    int freePlaces = 0;
    for (int i = 0; i < blockLength; ++i) {
        if (block[i] == 0) {
            freePlaces += 1;
        }
    }
    return freePlaces;
}

//Return: -1 if failed, otherwise inserted element's position.
int blockarray_static_insert_block(const char block[], int blockLength) {
    for (int i = 0; i < MAX_ARRAY_LENGTH; ++i) {
        if (blockarray_static_count_free_places(blockArray[i], blockLength) == blockLength) {
            for (int j = 0; j < blockLength; ++j) {
                blockArray[i][j] = block[j];
            }
            return i;
        }
    }
    return (-1);
}

void blockarray_static_remove_block(unsigned int index, int blockLength) {
    if (index >= MAX_ARRAY_LENGTH) return;

    for (int j = 0; j < blockLength; ++j) {
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

char *blockarray_static_find_nearest_sum_block(char block[], unsigned int blockLength) {
    if (block == NULL) return NULL;

    int searchedSum = blockarray_static_sum_in_block(block, blockLength);
    int minimalDifference = INT_MAX;
    char *chosenBlock = NULL;

    for (int i = 0; i < MAX_ARRAY_LENGTH; ++i) {
        int sumInCheckedBlock = blockarray_static_sum_in_block(blockArray[i], blockLength);
        int difference = abs(searchedSum - sumInCheckedBlock);
        if (difference < minimalDifference) {
            minimalDifference = difference;
            chosenBlock = blockArray[i];
        }
    }
    return chosenBlock;
}
