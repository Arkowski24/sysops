cmake_minimum_required(VERSION 3.8)
project(charBlockArray)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES blockarray.c blockarray.h charblock.c charblock.h)
add_library(charBlockArray ${SOURCE_FILES})