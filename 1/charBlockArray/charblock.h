#ifndef charblock_h
#define charblock_h

#include <stdio.h>
#include <stdlib.h>

struct CharBlock {
    unsigned int charCount;
    char *block;
};

struct CharBlock *create_block(unsigned int charCount);

void delete_block(struct CharBlock *charBlock);

int sum_in_block(struct CharBlock *charBlock);

#endif /* charblock_h */
