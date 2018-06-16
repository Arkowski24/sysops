cmake_minimum_required(VERSION 3.8)
project(sysops)

set(CMAKE_C_STANDARD 99)

find_package(Threads)
add_executable(MATH_SERVER cw10/dgram/server/math_server.c cw10/dgram/server/math_server.h cw10/dgram/server/fifo/circular_fifo.c cw10/dgram/server/fifo/circular_fifo.h)
add_executable(MATH_CLIENT cw10/dgram/client/math_client.c cw10/dgram/client/math_client.h)
target_link_libraries(MATH_SERVER pthread)
target_link_libraries(MATH_CLIENT pthread)