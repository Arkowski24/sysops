cmake_minimum_required(VERSION 3.8)
project(charBlockArray)

set(CMAKE_C_STANDARD 99)

set(LIBRARY_FILES cw01/zad1/blockarray-static-alloc.h cw01/zad1/blockarray-dynamic-alloc.h cw01/zad1/blockarray-static-alloc.c cw01/zad1/blockarray-dynamic-alloc.c)

add_library(libblockarray.a STATIC ${LIBRARY_FILES})
add_executable(arrTest cw01/zad2/arrtest.h cw01/zad2/arrtest.c cw01/zad2/arrtestdyn.c cw01/zad2/arrteststa.c cw01/zad2/arrtestutil.c)
target_link_libraries(arrTest libblockarray.a)