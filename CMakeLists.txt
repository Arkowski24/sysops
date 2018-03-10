cmake_minimum_required(VERSION 3.8)
project(charBlockArray)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES cw01/zad1/dynamic-allocation/blockarray.c cw01/zad1/dynamic-allocation/blockarray.h cw01/zad1/static-allocation/blockarray.c cw01/zad1/static-allocation/blockarray.h cw01/zad2/arrtest.c cw01/zad2/arrtest.h)
add_library(charBlockArray ${SOURCE_FILES})