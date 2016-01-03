cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fpic -g")
set(SOURCE_FILES readers_writers.c)

add_executable(readers_writers_m ${SOURCE_FILES})
target_link_libraries(readers_writers_m drawer pthread)