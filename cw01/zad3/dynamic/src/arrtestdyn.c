#include <stdlib.h>
#include "arrtest.h"

static clock_t startTime;
static clock_t endTime;
static struct tms startCPU;
static struct tms endCPU;
static unsigned int availableBlocks = 0;
extern void *handle;

void (*lib_dyn_delete_arr)(struct CharBlockArray *, int);

void (*lib_dyn_clear)(struct CharBlockArray *);

int (*lib_dyn_insert)(struct CharBlockArray *, char *);

int (*lib_dyn_remove)(struct CharBlockArray *, unsigned int, int);

char *(*lib_dyn_fnd_nrst)(struct CharBlockArray *, unsigned int);

int dynamic_load_lib_functions() {
    lib_dyn_delete_arr = (void (*)(struct CharBlockArray *, int)) dlsym(handle, "blockarray_dynamic_delete_array");
    lib_dyn_clear = (void (*)(struct CharBlockArray *)) dlsym(handle, "blockarray_dynamic_clear_array");
    lib_dyn_insert = (int (*)(struct CharBlockArray *, char *)) dlsym(handle, "blockarray_dynamic_insert_block");
    lib_dyn_remove = (int (*)(struct CharBlockArray *, unsigned int, int)) dlsym(handle,
                                                                                 "blockarray_dynamic_remove_block");
    lib_dyn_fnd_nrst = (char *(*)(struct CharBlockArray *, unsigned int)) dlsym(handle,
                                                                                "blockarray_dynamic_find_nearest_sum_block");

    if (lib_dyn_delete_arr == NULL || lib_dyn_clear == NULL || lib_dyn_insert == NULL || lib_dyn_remove == NULL ||
        lib_dyn_fnd_nrst == NULL) { return 1; }
    else { return 0; }

}

void dynamic_init(struct CharBlockArray *blockArray, unsigned int arrayLength, unsigned int blockLength) {
    if (availableBlocks > 0) {
        dynamic_clear(blockArray);
    }

    startTime = times(&startCPU);
    for (int i = 0; i < arrayLength; ++i) {
        char *inArr = malloc(sizeof(char) * blockLength);
        for (int j = 0; j < blockLength; ++j) {
            inArr[j] = (char) rand();
        }
        (*lib_dyn_insert)(blockArray, inArr);
    }
    endTime = times(&endCPU);

    printf("Dynamic array initialization times: \n");
    printf("Blocks count: %u, block's length: %u \n", arrayLength, blockLength);
    print_times(startTime, endTime, startCPU, endCPU);

    availableBlocks = arrayLength;
}

void dynamic_delete(struct CharBlockArray *blockArray) {
    (*lib_dyn_delete_arr)(blockArray, 1);
}

void dynamic_clear(struct CharBlockArray *blockArray) {
    startTime = times(&startCPU);
    (*lib_dyn_clear)(blockArray);
    endTime = times(&endCPU);

    printf("Dynamic array clear times: \n");
    print_times(startTime, endTime, startCPU, endCPU);

    availableBlocks = 0;
}

void dynamic_find_nearest(struct CharBlockArray *blockArray, unsigned int index) {
    startTime = times(&startCPU);
    (*lib_dyn_fnd_nrst)(blockArray, index);
    endTime = times(&endCPU);

    printf("Find nearest-sum block in dynamic array: \n");
    print_times(startTime, endTime, startCPU, endCPU);
}

void dynamic_remove_and_insert(struct CharBlockArray *blockArray, unsigned int blocksCount) {
    startTime = times(&startCPU);
    for (unsigned int i = 0; i < blocksCount; ++i) {
        (*lib_dyn_remove)(blockArray, i, 1);
    }
    for (int j = 0; j < blocksCount; ++j) {
        char *inArr = create_random_block(blockArray->blockLength);
        (*lib_dyn_insert)(blockArray, inArr);
    }
    endTime = times(&endCPU);

    printf("Remove and then insert blocks in dynamic array: \n");
    printf("Number of removed/inserted blocks: %u \n", blocksCount);
    print_times(startTime, endTime, startCPU, endCPU);
}

void dynamic_alternatively_remove_and_insert(struct CharBlockArray *blockArray, unsigned int blocksCount) {
    startTime = times(&startCPU);
    for (unsigned int i = 0; i < blocksCount; ++i) {
        (*lib_dyn_remove)(blockArray, i, 1);

        char *inArr = create_random_block(blockArray->blockLength);
        (*lib_dyn_insert)(blockArray, inArr);
    }
    endTime = times(&endCPU);

    printf("Remove and insert alternatively in dynamic array: \n");
    printf("Number of removed/inserted blocks: %u \n", blocksCount);
    print_times(startTime, endTime, startCPU, endCPU);
}

void dynamic_insert(struct CharBlockArray *blockArray, unsigned int blocksCount) {
    unsigned int inserted = 0;

    startTime = times(&startCPU);
    for (int j = 0; j < blocksCount; ++j) {
        char *inArr = create_random_block(blockArray->blockLength);
        if ((*lib_dyn_insert)(blockArray, inArr) != -1) { inserted++; }
    }
    endTime = times(&endCPU);

    printf("Insert blocks in dynamic array: \n");
    printf("Number of inserted blocks: %u \n", inserted);
    print_times(startTime, endTime, startCPU, endCPU);

    availableBlocks += inserted;
}

void dynamic_remove(struct CharBlockArray *blockArray, unsigned int blocksCount) {
    if (availableBlocks < blocksCount) { blocksCount = availableBlocks; }
    unsigned int removed = 0;

    startTime = times(&startCPU);
    for (unsigned int i = 0; i < blockArray->arrayLength || removed < blocksCount; ++i) {
        if ((*lib_dyn_remove)(blockArray, i, 1) == 0) {
            removed++;
        }
    }
    endTime = times(&endCPU);

    printf("Remove in dynamic array: \n");
    printf("Number of removed blocks: %u \n", blocksCount);
    print_times(startTime, endTime, startCPU, endCPU);

    availableBlocks -= removed;
}
