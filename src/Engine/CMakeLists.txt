cmake_minimum_required(VERSION 3.8)

project(cpp2d-engine)

# Set the output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)

# The necissary include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include 
    ${CMAKE_SOURCE_DIR}/include/entt/single_include
    ${CMAKE_SOURCE_DIR}/extern/glew/include 
)

# Set the location of the graphics lib
link_directories(
    ${CMAKE_SOURCE_DIR}/lib
)

# All we need the engine.cpp file
add_library(cpp2d-engine SHARED ../Engine.cpp ${CMAKE_SOURCE_DIR}/extern/glew/src/glew.c)

find_library(OPENGL OpenGL)

# We wish to link to the graphics library
target_link_libraries(cpp2d-engine ${OPENGL} libcpp2d-graphics.dylib)