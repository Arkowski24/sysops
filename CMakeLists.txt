cmake_minimum_required(VERSION 3.8)
project(sysops)

set(CMAKE_C_STANDARD 99)

find_package(Threads)
add_executable(PROD_CONS cw09/fifo/circular_fifo.c cw09/fifo/circular_fifo.h cw09/mutex/prod_cons.c cw09/mutex/prod_cons.h cw09/mutex/prod_cons_rout.c)
target_link_libraries(PROD_CONS pthread)