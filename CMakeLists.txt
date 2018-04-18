cmake_minimum_required(VERSION 3.8)
project(sysops)

set(CMAKE_C_STANDARD 99)

add_executable(fifoMaster cw05/zad2/master/fifo_master.c cw05/zad2/master/fifo_master.h)
add_executable(fifoSlave cw05/zad2/slave/fifo_slave.c cw05/zad2/slave/fifo_slave.h)