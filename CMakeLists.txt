cmake_minimum_required(VERSION 3.8)
project(sysops)

set(CMAKE_C_STANDARD 99)

add_executable(BARBER cw07/posix/barber/barber.c cw07/posix/barber/barber.h cw07/fifo/circular_fifo.c cw07/fifo/circular_fifo.h)
add_executable(CLIENT cw07/posix/client/client.c cw07/posix/client/client.h cw07/fifo/circular_fifo.c cw07/fifo/circular_fifo.h)