#include "charblock.h"

struct CharBlock *create_block(unsigned int charCount) {
    struct CharBlock *charBlock = (struct CharBlock *) malloc(sizeof(struct CharBlock));
    charBlock->charCount = charCount;
    charBlock->block = calloc(charCount, sizeof(char));
    return charBlock;
}

void delete_block(struct CharBlock *charBlock) {
    if (charBlock == NULL) return;

    if (charBlock->block != NULL) {
        free(charBlock->block);
    }
    free(charBlock);
}

int sum_in_block(struct CharBlock *charBlock) {
    if (charBlock == NULL || charBlock->block == NULL) return 0;

    int sum = 0;
    for (int i = 0; i < charBlock->charCount; i++) {
        sum += charBlock->block[i];
    }
    return sum;
}
