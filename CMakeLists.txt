cmake_minimum_required(VERSION 3.8)
project(charBlockArray)

set(CMAKE_C_STANDARD 99)

set(LIBRARY_FILES cw01/zad1/blockarray-static-alloc.h cw01/zad1/blockarray-dynamic-alloc.h cw01/zad1/blockarray-static-alloc.c cw01/zad1/blockarray-dynamic-alloc.c)
set(DYNAMIC_LOADED_SOURCES cw01/zad3/dynamic/src)

add_library(libblockarray.a STATIC ${LIBRARY_FILES})
add_library(libblockarray.so.1.0.1 SHARED ${LIBRARY_FILES})

add_executable(arrTest ${DYNAMIC_LOADED_SOURCES}/arrtest.h ${DYNAMIC_LOADED_SOURCES}/arrtest.c ${DYNAMIC_LOADED_SOURCES}//arrtestdyn.c ${DYNAMIC_LOADED_SOURCES}/arrteststa.c ${DYNAMIC_LOADED_SOURCES}/arrtestutil.c)
target_link_libraries(arrTest libblockarray.a)