#include <sys/times.h>
#include "arrtest.h"

static int initialized = 0;
static clock_t startTime;
static clock_t endTime;
static struct tms startCPU;
static struct tms endCPU;

void static_init(unsigned int arrayLength, unsigned int blockLength) {
    if (initialized == 1) {
        static_clear();
    }

    startTime = times(&startCPU);
    for (int i = 0; i < arrayLength; ++i) {
        char *inArr = create_random_block(blockLength);
        blockarray_static_insert_block(inArr, blockLength);
        free(inArr);
    }
    endTime = times(&endCPU);

    printf("Static array initialization times: \n");
    printf("Blocks count: %u, block's length: %u \n", arrayLength, blockLength);
    print_times(startTime, endTime, startCPU, endCPU);

    initialized = 1;
}

void static_clear() {
    startTime = times(&startCPU);
    blockarray_static_clear_array();
    endTime = times(&endCPU);

    printf("Array clear times: \n");
    print_times(startTime, endTime, startCPU, endCPU);
}

void static_find_nearest(unsigned int index) {
    startTime = times(&startCPU);
    blockarray_static_find_nearest_sum_block(index);
    endTime = times(&endCPU);

    printf("Find nearest-sum block in static array: \n");
    print_times(startTime, endTime, startCPU, endCPU);
}

void static_remove_and_insert(unsigned int blocksCount, unsigned int blockLength) {
    startTime = times(&startCPU);
    for (unsigned int i = 0; i < blocksCount; ++i) {
        blockarray_static_remove_block(i);
    }
    for (int j = 0; j < blocksCount; ++j) {
        char *inArr = create_random_block(blockLength);
        blockarray_static_insert_block(inArr, blockLength);
        free(inArr);
    }
    endTime = times(&endCPU);

    printf("Remove and then insert blocks in static array: \n");
    printf("Number of removed/inserted blocks: %u \n", blocksCount);
    print_times(startTime, endTime, startCPU, endCPU);
}

void static_alternatively_remove_and_insert(unsigned int blocksCount, unsigned int blockLength) {
    startTime = times(&startCPU);
    for (unsigned int i = 0; i < blocksCount; ++i) {
        blockarray_static_remove_block(i);
        char *inArr = create_random_block(blockLength);
        blockarray_static_insert_block(inArr, blockLength);
        free(inArr);
    }
    endTime = times(&endCPU);

    printf("Remove and insert alternatively in static array: \n");
    printf("Number of removed/inserted blocks: %u \n", blocksCount);
    print_times(startTime, endTime, startCPU, endCPU);
}