#include <stdlib.h>
#include <mem.h>
#include <ctype.h>
#include "arrtest.h"
#include "blockarray-dynamic-alloc.h"
#include "blockarray-static-alloc.h"

struct Operation{
    int operationType;
    int argument;
};

int oprC = 0;
struct Operation *operations[3] = {NULL};
int allocationType, arrayLength, blockLength;

int main(int argc, char *argv[]) {
   if(parseCommandLine(argc, argv) != 0) return 1;

}

void executeStatic(){
    
}

void executeDynamic(){

}

void toLowLetters(char *str){
    for (int i = 0; str[i]; ++i) {
        str[i] = (char) tolower(str[i]);
    }
}

int parseCommandLine(int argc, char *argv[]){
    if (argc < 5) {
        printf("Not enough arguments.");
        return 1;
    }
    toLowLetters((argv[1]));
    if (strcmp(argv[1], "static") == 0 || strcmp(argv[1], "s") == 0) {
        allocationType = 0;
    } else {
        allocationType = 1;
    }
    arrayLength = atoi(argv[2]);
    blockLength = atoi(argv[3]);
    if (arrayLength < 1 || blockLength < 1) {
        printf("Incorrect array or block length.");
    }
    for (int i = 4; i < argc; ++i) {
        toLowLetters((argv[i]));
        if (strcmp(argv[i], "init") == 0) {
            operations[oprC] = malloc(sizeof(struct Operation));
            operations[oprC]->operationType = 0;
            oprC++;
        } else {
            if (i + 1 >= argc) {
                printf("Not enough arguments.");
                return 1;
            }
            int arg = atoi(argv[i + 1]);
            int optType = -1;
            if(arg < 0 || arg > arrayLength) {
                printf("Invalid size/index.");
                return 1;
            }
            if(strcmp(argv[i], "fnd") == 0){
                optType = 1;
            } else if (strcmp(argv[i], "adddel") == 0){
                optType = 2;
            } else if ((strcmp(argv[i], "altadddel") == 0)) {
                optType = 3;
            }
            if(optType == -1){
                printf("Unknown argument: ");
                printf(argv[i]);
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