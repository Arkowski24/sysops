cmake_minimum_required(VERSION 3.8)
project(sysops)

set(CMAKE_C_STANDARD 99)

add_executable(recordGen cw02/zad1/record_handler.c cw02/zad1/record_handler_sys.c cw02/zad1/record_handler_lib.c cw02/zad1/record_handler.h)