#include <sys/times.h>
#include "arrtest.h"

static clock_t startTime;
static clock_t endTime;
static struct tms startCPU;
static struct tms endCPU;
static unsigned int availableBlocks = 0;
extern void *handle;

void (*lib_st_delete_arr)();

void (*lib_st_clear)();

int (*lib_st_insert)(char *, unsigned int);

void (*lib_st_remove)(unsigned int);

char *(*lib_st_fnd_nrst)(unsigned int);

int static_load_lib_functions() {
    lib_st_delete_arr = (void (*)()) dlsym(handle, "blockarray_static_delete_array");
    lib_st_clear = (void (*)()) dlsym(handle, "blockarray_static_clear_array");
    lib_st_insert = (int (*)(char *, unsigned int)) dlsym(handle, "blockarray_dynamic_insert_block");
    lib_st_remove = (void (*)(unsigned int)) dlsym(handle, "blockarray_dynamic_remove_block");
    lib_st_fnd_nrst = (char *(*)(unsigned int)) dlsym(handle, "blockarray_dynamic_find_nearest_sum_block");

    if (lib_st_delete_arr == NULL || lib_st_clear == NULL || lib_st_insert == NULL || lib_st_remove == NULL ||
        lib_st_fnd_nrst == NULL) { return 1; }
    else { return 0; }

}

void static_init(unsigned int arrayLength, unsigned int blockLength) {
    if (availableBlocks > 0) {
        static_clear();
    }

    startTime = times(&startCPU);
    for (int i = 0; i < arrayLength; ++i) {
        char *inArr = create_random_block(blockLength);
        (*lib_st_insert)(inArr, blockLength);
        free(inArr);
    }
    endTime = times(&endCPU);

    printf("Static array initialization times: \n");
    printf("Blocks count: %u, block's length: %u \n", arrayLength, blockLength);
    print_times(startTime, endTime, startCPU, endCPU);

    availableBlocks += arrayLength;
}

void static_clear() {
    startTime = times(&startCPU);
    (*lib_st_clear)();
    endTime = times(&endCPU);

    printf("Array clear times: \n");
    print_times(startTime, endTime, startCPU, endCPU);
    availableBlocks = 0;
}

void static_find_nearest(unsigned int index) {
    startTime = times(&startCPU);
    (*lib_st_fnd_nrst)(index);
    endTime = times(&endCPU);

    printf("Find nearest-sum block in static array: \n");
    print_times(startTime, endTime, startCPU, endCPU);
}

void static_remove_and_insert(unsigned int blocksCount, unsigned int blockLength) {
    startTime = times(&startCPU);
    for (unsigned int i = 0; i < blocksCount; ++i) {
        (*lib_st_remove)(i);
    }
    for (int j = 0; j < blocksCount; ++j) {
        char *inArr = create_random_block(blockLength);
        (*lib_st_insert)(inArr, blockLength);
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
        (*lib_st_remove)(i);
        char *inArr = create_random_block(blockLength);
        (*lib_st_insert)(inArr, blockLength);
        free(inArr);
    }
    endTime = times(&endCPU);

    printf("Remove and insert alternatively in static array: \n");
    printf("Number of removed/inserted blocks: %u \n", blocksCount);
    print_times(startTime, endTime, startCPU, endCPU);
}

void static_insert(unsigned int blocksCount, unsigned int blockLength) {
    startTime = times(&startCPU);
    for (int j = 0; j < blocksCount; ++j) {
        char *inArr = create_random_block(blockLength);
        (*lib_st_insert)(inArr, blockLength);
        free(inArr);
    }
    endTime = times(&endCPU);

    printf("Insert in static array: \n");
    printf("Number of inserted blocks: %u \n", blocksCount);
    print_times(startTime, endTime, startCPU, endCPU);

    availableBlocks += blocksCount;
}

void static_remove(unsigned int blocksCount) {
    if (blocksCount < availableBlocks) { blocksCount = availableBlocks; }

    startTime = times(&startCPU);
    for (unsigned int i = 0; i < blocksCount; ++i) {
        (*lib_st_remove)(i);
    }
    endTime = times(&endCPU);

    printf("Remove in static array: \n");
    printf("Number of removed blocks: %u \n", blocksCount);
    print_times(startTime, endTime, startCPU, endCPU);

    availableBlocks -= availableBlocks;
}