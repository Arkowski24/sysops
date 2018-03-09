cmake_minimum_required(VERSION 3.8)
project(charBlockArray)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES cw01/zad1/blockarray-dynamic.c cw01/zad1/blockarray-dynamic.h cw01/zad1/blockarray-static.c cw01/zad1/blockarray-static.h)
add_library(charBlockArray ${SOURCE_FILES})