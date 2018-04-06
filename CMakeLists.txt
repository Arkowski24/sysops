cmake_minimum_required(VERSION 3.8)
project(sysops)

set(CMAKE_C_STANDARD 99)

add_executable(dirExplorer cw04/zad3/comm_tester.c cw04/zad3/comm_tester_child.c cw04/zad3/comm_tester.h)