# CMake entry point
cmake_minimum_required (VERSION 3.0)

project(sockets)

set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS}           -g -O0")
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS}         -g -O0")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS}  -g -O0")

file(GLOB_RECURSE HDRS *.h)
file(GLOB_RECURSE SRCS *.cpp *.c)

add_library(${PROJECT_NAME} STATIC ${SRCS} ${HDRS})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "lib")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".a")

include_directories(${PROJECT_SOURCE_DIR}/include/)
