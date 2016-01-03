cmake_minimum_required(VERSION 2.6)
project(SOLAB1)
set(CMAKE_C_FLAGS "-o -Wall")


include_directories(${SOLAB1_SOURCE_DIR})
link_directories(${SOLAB1_BINARY_DIR})

add_library(listStatic STATIC list.c)
add_library(listShared SHARED list.c)



