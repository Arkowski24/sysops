#include <ctype.h>
#include <string.h>
#include "arrtest.h"

extern int oprC;
extern struct Operation *operations[3];
extern unsigned int allocationType, arrayLength, blockLength;

void to_low_letters(char *str) {
    for (int i = 0; str[i]; ++i) {
        str[i] = (char) tolower(str[i]);
    }
}

int parse_command_line(int argc, char **argv) {
    if (argc < 5) {
        printf("Not enough arguments.");
        return 1;
    }
    if (get_allocation_method(argv) == 1) {
        return 1;
    }

    arrayLength = (unsigned int) atoi(argv[2]);
    blockLength = (unsigned int) atoi(argv[3]);
    if (arrayLength == 0 || blockLength == 0 || arrayLength > MAX_ARRAY_LENGTH || blockLength > MAX_BLOCK_LENGTH) {
        printf("Incorrect array or block length.");
        return 1;
    }

    if (parse_options(argc, argv) == 1) {
        return 1;
    }

    return 0;
}

int get_allocation_method(char **argv) {
    to_low_letters((argv[1]));
    if (strcmp(argv[1], "static") == 0 || strcmp(argv[1], "s") == 0) {
        allocationType = 0;
    } else if (strcmp(argv[1], "dynamic") == 0 || strcmp(argv[1], "d") == 0) {
        allocationType = 1;
    } else {
        printf("Unknown allocation type: ");
        printf("%s", argv[1]);
        return 1;
    }
    return 0;
}

int parse_options(int argc, char **argv) {
    for (int i = 4; i < argc; ++i) {
        to_low_letters((argv[i]));

        //init - Initialize table, no additional arguments
        if (strcmp(argv[i], "init") == 0) {
            operations[oprC] = malloc(sizeof(struct Operation));
            operations[oprC]->operationType = 0;
            oprC++;
        } else {
            if (i + 1 >= argc) {
                printf("Not enough arguments.");
                return 1;
            }

            unsigned int arg = (unsigned int) atoi(argv[i + 1]);
            short int optType = -1;
            if (arg > arrayLength) {
                printf("Invalid size/index.");
                return 1;
            }

            //fnd - Find closest block to the block of given index.
            //rmIn - remove and then insert given number of blocks
            //aRmIn - remove and insert blocks alternatively
            if (strcmp(argv[i], "fnd") == 0) {
                optType = 1;
            } else if (strcmp(argv[i], "rmin") == 0) {
                optType = 2;
            } else if ((strcmp(argv[i], "armin") == 0)) {
                optType = 3;
            }

            if (optType == -1) {
                printf("Unknown argument: ");
                printf("%s", argv[i]);
                return 1;
            }
            operations[oprC] = malloc(sizeof(struct Operation));
            operations[oprC]->operationType = optType;
            operations[oprC]->argument = arg;
            oprC++;
        }
    }
    return 0;
}

char *create_random_block(unsigned int blockLength) {
    char *inArr = malloc(sizeof(char) * blockLength);
    for (int i = 0; i < blockLength; ++i) {
        inArr[i] = (char) rand();
    }
    return inArr;
}