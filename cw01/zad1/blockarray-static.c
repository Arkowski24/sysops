#include "blockarray-static.h"

//Return: -1 if failed, otherwise inserted element's position.
int insert_block(char array[][BLOCK_LENGTH], const char block[]) {
    for (int i = 0; i < ARRAY_LENGTH; ++i) {
        for (int j = 0; j < BLOCK_LENGTH; ++j) {
            if (array[i][j] != 0) {
                continue;
            }
        }
        for (int j = 0; j < BLOCK_LENGTH; ++j) {
            array[i][j] = block[j];
        }
        return i;
    }
    return (-1);
}

void remove_block(char array[][BLOCK_LENGTH], unsigned int index) {
    if (index >= ARRAY_LENGTH) return;

    for (int j = 0; j < BLOCK_LENGTH; ++j) {
        array[index][j] = 0;
    }
}

int sum_in_block(const char block[], unsigned int blockLength) {
    if (block == NULL) return 0;

    int sumInBlock = 0;
    for (unsigned int i = 0; i < blockLength; ++i) {
        sumInBlock += block[i];
    }
    return sumInBlock;
}

char *find_nearest_sum_block(char array[][BLOCK_LENGTH], char block[], unsigned int blockLength) {
    if (array == NULL || block == NULL) return NULL;

    int searchedSum = sum_in_block(block, blockLength);
    int minimalDifference = INT_MAX;
    char *chosenBlock = NULL;

    for (int i = 0; i < ARRAY_LENGTH; ++i) {
        int sumInCheckedBlock = sum_in_block(array[i], BLOCK_LENGTH);
        int difference = abs(searchedSum - sumInCheckedBlock);
        if (difference < minimalDifference) {
            minimalDifference = difference;
            chosenBlock = array[i];
        }
    }
    return chosenBlock;
}
