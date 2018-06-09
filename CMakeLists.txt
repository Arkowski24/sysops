cmake_minimum_required(VERSION 3.8)
project(sysops)

set(CMAKE_C_STANDARD 99)

find_package(Threads)
add_executable(MATH_SERVER cw10/dgram/server/math_server.c cw10/dgram/server/math_server.h cw10/fifo/circular_fifo.c cw10/fifo/circular_fifo.h cw10/dgram/server/math_server_local.c cw10/dgram/server/math_server_network.c)
target_link_libraries(MATH_SERVER pthread)