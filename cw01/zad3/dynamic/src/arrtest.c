#include <stdlib.h>
#include <time.h>
#include "arrtest.h"

int oprC = 0;
extern char blockArray[MAX_ARRAY_LENGTH][MAX_BLOCK_LENGTH];
struct Operation *operations[3] = {NULL};
unsigned int allocationType, arrayLength, blockLength;
void *handle;

int main(int argc, char *argv[]) {
    handle = dlopen("libblockarray.so", RTLD_LAZY);
    if (handle == NULL) {
        printf("Couldn't load library. \n");
        return 1;
    }
    srand(time(NULL));

    if (parse_command_line(argc, argv) != 0) return 1;
    if (allocationType == 0) {
        execute_static();
    } else {
        execute_dynamic();
    }

    for (int i = 0; i < oprC; ++i) {
        free(operations[i]);
    }

    dlclose(handle);
}

void execute_static() {
    int (*lib_st_create_arr)(unsigned int, unsigned int);
    lib_st_create_arr = (int (*)(unsigned int, unsigned int)) dlsym(handle, "blockarray_static_create_array");

    if (lib_st_create_arr == NULL || static_load_lib_functions() != 0) {
        printf("Couldn't find library functions. \n");
        return;
    }

    if ((*lib_st_create_arr)(arrayLength, blockLength) != 0) {
        printf("Couldn't create static array. Size limit surpassed. \n");
        return;
    }

    for (int i = 0; i < oprC; ++i) {
        switch (operations[i]->operationType) {
            case 0:
                static_init(arrayLength, blockLength);
                break;
            case 1:
                static_find_nearest(operations[i]->argument);
                break;
            case 2:
                static_remove_and_insert(operations[i]->argument, blockLength);
                break;
            case 3:
                static_alternatively_remove_and_insert(operations[i]->argument, blockLength);
                break;
            case 4:
                static_remove(operations[i]->argument);
                break;
            case 5:
                static_insert(operations[i]->argument, blockLength);
                break;
            default:
                break;
        }
    }
}

void execute_dynamic() {
    struct CharBlockArray *(*lib_dyn_create_arr)(unsigned int, unsigned int);
    lib_dyn_create_arr = (struct CharBlockArray *(*)(unsigned int, unsigned int)) dlsym(handle,
                                                                                        "blockarray_dynamic_create_array");

    if (lib_dyn_create_arr == NULL || dynamic_load_lib_functions() != 0) {
        printf("Couldn't find library functions. \n");
        return;
    }

    struct CharBlockArray *blockArray = (*lib_dyn_create_arr)(arrayLength, blockLength);
    if (blockArray == NULL) {
        printf("Couldn't create dynamic array. Size limit surpassed. \n");
        return;
    }

    for (int i = 0; i < oprC; ++i) {
        switch (operations[i]->operationType) {
            case 0:
                dynamic_init(blockArray, arrayLength, blockLength);
                break;
            case 1:
                dynamic_find_nearest(blockArray, operations[i]->argument);
                break;
            case 2:
                dynamic_remove_and_insert(blockArray, operations[i]->argument);
                break;
            case 3:
                dynamic_alternatively_remove_and_insert(blockArray, operations[i]->argument);
                break;
            case 4:
                dynamic_remove(blockArray, operations[i]->argument);
                break;
            case 5:
                dynamic_insert(blockArray, operations[i]->argument);
                break;
            default:
                break;
        }
    }
    dynamic_delete(blockArray);
}