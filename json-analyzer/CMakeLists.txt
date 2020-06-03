# CMake entry point
cmake_minimum_required (VERSION 3.10.0 FATAL_ERROR)

project(json-analyzer LANGUAGES CXX)

file(GLOB_RECURSE HDRS *.h)
file(GLOB_RECURSE SRCS *.cpp)

set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS}         -g -O0 ")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS}  -g -O0 ")

add_library( ${PROJECT_NAME} STATIC
    ${HDRS}
    ${SRCS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "lib")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".a")

include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/../sockets/include/)

