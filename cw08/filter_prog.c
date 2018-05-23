//
// Created by Arkadiusz Placha on 22.05.2018.
//

#include "filter_prog.h"

pgma_img_t inputImg;

int main(int argc, char *argv[]) {
    read_pgma_image(argv[2], &inputImg);

    return 0;
}