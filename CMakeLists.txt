cmake_minimum_required(VERSION 3.8)
project(sysops)

set(CMAKE_C_STANDARD 99)

find_package(Threads)
add_executable(BARBER cw07/sysV/barber/barber.c cw07/sysV/barber/barber.h cw07/sysV/fifo/circular_fifo.c cw07/sysV/fifo/circular_fifo.h)
add_executable(CLIENT cw07/sysV/client/client.c cw07/sysV/client/client.h cw07/sysV/fifo/circular_fifo.c cw07/sysV/fifo/circular_fifo.h)