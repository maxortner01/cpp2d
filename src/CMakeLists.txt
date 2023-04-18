cmake_minimum_required(VERSION 3.8)

# CPP2d requires the gl3w.c and gl3w include folders
# as well as the glfw include folder currently and its associated .a files
project(cpp2d)

# Set the output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 11)

# The necissary include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include 
    ${CMAKE_SOURCE_DIR}/extern/glew/include 
    ${CMAKE_SOURCE_DIR}/extern/glfw/include
    ${CMAKE_SOURCE_DIR}/extern/stb
)

# Set the location of the glfw lib file
link_directories(
    ${CMAKE_SOURCE_DIR}/build/extern/glfw/src
)

# Get the source files
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)

# Get the GLFW dependencies
find_library(COCOA  Cocoa)
find_library(OPENGL OpenGL)
find_library(IOKIT  IOKit)

# Include the main source files as well as the retreived GL files for gl3w
add_executable(cpp2d ${SOURCES} ${CMAKE_SOURCE_DIR}/extern/glew/src/glew.c)

target_link_libraries(cpp2d glfw3 ${COCOA} ${OPENGL} ${IOKIT})