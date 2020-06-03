# CMake entry point
cmake_minimum_required (VERSION 3.10.0 FATAL_ERROR)

project(qt-view LANGUAGES CXX)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# to always look for includes there:
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui Network)

file(GLOB_RECURSE HDRS *.h)
file(GLOB_RECURSE SRCS *.cpp)
file(GLOB_RECURSE UIS  *.ui)
file(GLOB_RECURSE MOC_HDRS_DISP ${PROJECT_BINARY_DIR}/*.h)
file(GLOB_RECURSE MOC_SRCS_DISP ${PROJECT_BINARY_DIR}/*.cpp)
file(GLOB_RECURSE MOC_UIS ${PROJECT_BINARY_DIR}/*.ui)

set(project_ui
    ${UIS})

set(project_headers
    ${HDRS})

set(project_sources
    ${SRCS})

qt5_wrap_ui(project_headers_wrapped ${project_ui})
qt5_wrap_cpp(project_sources_moc ${project_headers})

set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS}         -g -O0 ")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS}  -g -O0 ")

add_executable(
    ${PROJECT_NAME}
    ${project_headers}
    ${project_sources}
    ${project_sources_moc}
    ${project_headers_wrapped}
    resources.qrc
)

include_directories(
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
)

include_directories(${PROJECT_SOURCE_DIR}/)
include_directories(${PROJECT_BINARY_DIR}/)

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX "${EXE_SUFFIX}")

include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/../sockets/include/)
include_directories(${PROJECT_SOURCE_DIR}/../json-analyzer/include/)

target_link_libraries(${PROJECT_NAME}
    ${PROJECT_BINARY_DIR}/../json-analyzer/libjson-analyzer.a
    ${PROJECT_BINARY_DIR}/../sockets/libsockets.a
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Core> $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Gui> $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Widgets> $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
