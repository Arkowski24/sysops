#include <stdlib.h>
#include "arrtest.h"

static int initialized = 0;
static clock_t startTime;
static clock_t endTime;
static struct tms startCPU;
static struct tms endCPU;

void dynamic_init(struct CharBlockArray *blockArray, unsigned int arrayLength, unsigned int blockLength) {
    if (initialized == 1) {
        dynamic_clear(blockArray);
    }

    startTime = times(&startCPU);
    for (int i = 0; i < arrayLength; ++i) {
        char *inArr = malloc(sizeof(char) * blockLength);
        for (int j = 0; j < blockLength; ++j) {
            inArr[j] = (char) rand();
        }
        blockarray_dynamic_insert_block(blockArray, inArr);
    }
    endTime = times(&endCPU);

    printf("Array initialization times: \n");
    printf("Blocks count: %u, block's length: %u \n", arrayLength, blockLength);
    print_times(startTime, endTime, startCPU, endCPU);

    initialized = 1;
}

void dynamic_clear(struct CharBlockArray *blockArray) {
    startTime = times(&startCPU);
    blockarray_dynamic_clear_array(blockArray);
    endTime = times(&endCPU);

    printf("Dynamic array clear times: \n");
    print_times(startTime, endTime, startCPU, endCPU);
}

void dynamic_find_nearest(struct CharBlockArray *blockArray, unsigned int index) {
    startTime = times(&startCPU);
    blockarray_dynamic_find_nearest_sum_block(blockArray, index);
    endTime = times(&endCPU);

    printf("Find nearest-sum block in dynamic array: \n");
    print_times(startTime, endTime, startCPU, endCPU);
}

void dynamic_remove_and_insert(struct CharBlockArray *blockArray, unsigned int blocksCount) {
    startTime = times(&startCPU);
    for (unsigned int i = 0; i < blocksCount; ++i) {
        blockarray_dynamic_remove_block(blockArray, i, 1);
    }
    for (int j = 0; j < blocksCount; ++j) {
        char *inArr = create_random_block(blockArray->blockLength);
        blockarray_dynamic_insert_block(blockArray, inArr);
    }
    endTime = times(&endCPU);

    printf("Remove and then insert blocks in dynamic array: \n");
    printf("Number of removed/inserted blocks: %u \n", blocksCount);
    print_times(startTime, endTime, startCPU, endCPU);
}

void dynamic_alternatively_remove_and_insert(struct CharBlockArray *blockArray, unsigned int blocksCount) {
    startTime = times(&startCPU);
    for (unsigned int i = 0; i < blocksCount; ++i) {
        blockarray_dynamic_remove_block(blockArray, i, 1);

        char *inArr = create_random_block(blockArray->blockLength);
        blockarray_dynamic_insert_block(blockArray, inArr);
    }
    endTime = times(&endCPU);

    printf("Remove and insert alternatively in dynamic array: \n");
    printf("Number of removed/inserted blocks: %u \n", blocksCount);
    print_times(startTime, endTime, startCPU, endCPU);
}