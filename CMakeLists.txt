# CMake entry point
cmake_minimum_required (VERSION 3.10.0 FATAL_ERROR)
project(ikbo152_json-analyzer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS}           -g -O0")
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS}         -g -O0")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS}  -g -O0")

add_subdirectory(json-analyzer)
add_subdirectory(sockets)
add_subdirectory(qt-view)

add_dependencies(qt-view json-analyzer sockets)
add_dependencies(json-analyzer sockets)
