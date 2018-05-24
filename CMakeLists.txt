cmake_minimum_required(VERSION 3.8)
project(sysops)

set(CMAKE_C_STANDARD 99)

find_package(Threads)
add_executable(FILTER_PROG cw08/filter_prog.c cw08/filter_prog.h cw08/filter_prog_io.c)
add_executable(FILTER_GENERATOR cw08/utils/filter_generator/filter_gen.c)