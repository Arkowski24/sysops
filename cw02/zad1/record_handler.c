#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "record_handler.h"

void generate_sys(char *filePath, unsigned int recordLength, unsigned int recordsCount);

int main(int argc, char *argv[]) {
    srand(time(NULL));
    if (strcmp(argv[1], "generate") == 0) {
        char *path = argv[2];
        unsigned int rL = atoi(argv[3]);
        unsigned int rC = atoi(argv[4]);
        generate_sys(path, rL, rC);
    }

    return 0;
}

void generate_lib(char *filePath, unsigned int recordLength, unsigned int recordsCount) {

}

void sort();

void copy();

void open_file_sys();