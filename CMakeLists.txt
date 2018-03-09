cmake_minimum_required(VERSION 3.8)
project(charBlockArray)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES cw01/zad1/blockarray.c cw01/zad1/blockarray.h cw01/zad1/charblock.c cw01/zad1/charblock.h)
add_library(charBlockArray ${SOURCE_FILES})