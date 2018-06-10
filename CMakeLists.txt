cmake_minimum_required(VERSION 3.8)
project(sysops)

set(CMAKE_C_STANDARD 99)

find_package(Threads)
add_executable(MATH_SERVER cw10/dgram/server/math_server.c cw10/dgram/server/math_server.h cw10/dgram/fifo/circular_fifo.c cw10/dgram/fifo/circular_fifo.h cw10/dgram/server/math_server_local.c cw10/dgram/server/math_server_network.c cw10/dgram/server/math_server_network.h cw10/dgram/server/math_server_worker.c cw10/dgram/server/math_server_worker.h cw10/dgram/server/math_server_local.h cw10/dgram/server/math_server_network.h)
target_link_libraries(MATH_SERVER pthread)