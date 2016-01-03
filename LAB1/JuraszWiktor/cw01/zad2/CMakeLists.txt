cmake_minimum_required(VERSION 2.6)
project(SOLAB1)
set(CMAKE_C_FLAGS "-o -Wall")
set(STATIC_EXE listTestStatic)
set(SHARED_EXE listTestShared)
set(SHARED_DYNAMIC_EXE listTestSharedDynamic)

include_directories(${SOLAB1_SOURCE_DIR})
link_directories(${SOLAB1_BINARY_DIR})

add_executable(${STATIC_EXE} listTest.c list.h)
add_executable(${SHARED_EXE} listTest.c list.h)
add_executable(${SHARED_DYNAMIC_EXE} listTestDynamic.c list.h)

add_library(listStatic STATIC list.c)
add_library(listShared SHARED list.c)

set_target_properties(${STATIC_EXE} PROPERTIES COMPILE_FLAGS "-static -L.")
set_target_properties(${SHARED_EXE} PROPERTIES COMPILE_FLAGS "-L.")
set_target_properties(${SHARED_DYNAMIC_EXE} PROPERTIES COMPILE_FLAGS "-ldl")

target_link_libraries(${STATIC_EXE} listStatic)
target_link_libraries(${SHARED_EXE} listShared)
target_link_libraries(${SHARED_DYNAMIC_EXE} ${CMAKE_DL_LIBS})

add_custom_command(
	TARGET ${STATIC_EXE}
	POST_BUILD	
	COMMAND ./${STATIC_EXE} 
)

add_custom_command(
	TARGET ${SHARED_EXE}
	POST_BUILD	
	COMMAND ./${SHARED_EXE} 
)

add_custom_command(
	TARGET ${SHARED_DYNAMIC_EXE}
	POST_BUILD	
	COMMAND ./${SHARED_DYNAMIC_EXE} 

)
