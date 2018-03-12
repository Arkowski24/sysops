#include <stdlib.h>
#include "arrtest.h"

static int initialized = 0;

void dynamic_init(struct CharBlockArray *blockArray, unsigned int arrayLength, unsigned int blockLength) {
    if (initialized == 1) {
        dynamic_clr(blockArray);
    }

    for (int i = 0; i < arrayLength; ++i) {
        char *inArr = malloc(sizeof(char) * blockLength);
        for (int j = 0; j < blockLength; ++j) {
            inArr[j] = (char) rand();
        }
        blockarray_dynamic_insert_block(blockArray, inArr);
    }

    initialized = 1;
}

void dynamic_clr(struct CharBlockArray *blockArray) {
    blockarray_dynamic_clear_array(blockArray);
}

void dynamic_find_nearest(struct CharBlockArray *blockArray, unsigned int index) {
    blockarray_dynamic_find_nearest_sum_block(blockArray, index);
}

void dynamic_remove_and_insert(struct CharBlockArray *blockArray, unsigned int blocksCount) {
    for (unsigned int i = 0; i < blocksCount; ++i) {
        blockarray_dynamic_remove_block(blockArray, i, 1);
    }
    for (int j = 0; j < blocksCount; ++j) {
        char *inArr = create_random_block(blockArray->blockLength);
        blockarray_dynamic_insert_block(blockArray, inArr);
    }
}

void dynamic_alternatively_remove_and_insert(struct CharBlockArray *blockArray, unsigned int blocksCount) {
    for (unsigned int i = 0; i < blocksCount; ++i) {
        blockarray_dynamic_remove_block(blockArray, i, 1);

        char *inArr = create_random_block(blockArray->blockLength);
        blockarray_dynamic_insert_block(blockArray, inArr);
    }
}