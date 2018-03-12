#include <stdlib.h>
#include <time.h>
#include "arrtest.h"

int oprC = 0;
extern char blockArray[MAX_ARRAY_LENGTH][MAX_BLOCK_LENGTH];
struct Operation *operations[3] = {NULL};
unsigned int allocationType, arrayLength, blockLength;

int main(int argc, char *argv[]) {
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
}

void execute_static() {
    blockarray_static_create_array(arrayLength, blockLength);
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
            default:
                break;
        }
    }
}

void execute_dynamic() {
    struct CharBlockArray *blockArray = blockarray_dynamic_create_array(arrayLength, blockLength);
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
            default:
                break;
        }
    }
}