cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fpic -g")
set(SOURCE_FILES barber.c)

add_executable(barber ${SOURCE_FILES})
target_link_libraries(barber drawer pthread)