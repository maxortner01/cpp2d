cmake_minimum_required(VERSION 3.8)

# CPP2d requires the gl3w.c and gl3w include folders
# as well as the glfw include folder currently and its associated .a files
project(cpp2d-memory)

# Set the output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)

# The necissary include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include 
)

# Include the main source files as well as the retreived GL files for gl3w
add_library(cpp2d-memory SHARED ../Memory.cpp)