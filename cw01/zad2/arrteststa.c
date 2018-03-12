#include "arrtest.h"

static int initialized = 0;

void static_init(unsigned int arrayLength, unsigned int blockLength) {
    if (initialized == 1) {
        blockarray_static_delete_array();
    }

    for (int i = 0; i < arrayLength; ++i) {
        char *inArr = create_random_block(blockLength);
        blockarray_static_insert_block(inArr, blockLength);
        free(inArr);
    }

    initialized = 1;
}

void static_find_nearest(unsigned int index) {
    blockarray_static_find_nearest_sum_block(index);
}

void static_remove_and_insert(unsigned int blocksCount, unsigned int blockLength) {
    for (unsigned int i = 0; i < blocksCount; ++i) {
        blockarray_static_remove_block(i);
    }
    for (int j = 0; j < blocksCount; ++j) {
        char *inArr = create_random_block(blockLength);
        blockarray_static_insert_block(inArr, blockLength);
        free(inArr);
    }
}

void static_alternatively_remove_and_insert(unsigned int blocksCount, unsigned int blockLength) {
    for (unsigned int i = 0; i < blocksCount; ++i) {
        blockarray_static_remove_block(i);
        char *inArr = create_random_block(blockLength);
        blockarray_static_insert_block(inArr, blockLength);
        free(inArr);
    }
}