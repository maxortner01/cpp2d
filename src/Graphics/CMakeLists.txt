cmake_minimum_required(VERSION 3.8)

# CPP2d requires the gl3w.c and gl3w include folders
# as well as the glfw include folder currently and its associated .a files
project(cpp2d-graphics)

# Set the output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_definitions(-DGDI_VULKAN)

set(CMAKE_CXX_STANDARD 17)

# Get the GLFW dependencies
find_library(COCOA  Cocoa)
find_package(Vulkan REQUIRED)
find_library(IOKIT  IOKit)

# The necissary include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include 
    ${CMAKE_SOURCE_DIR}/extern/vk-bootstrap/src
    ${CMAKE_SOURCE_DIR}/extern/VulkanMemoryAllocator/include 
    ${CMAKE_SOURCE_DIR}/extern/glfw/include
    ${CMAKE_SOURCE_DIR}/extern/stb
    ${Vulkan_INCLUDE_DIRS}
)

# Set the location of the glfw lib file
link_directories(
    ${CMAKE_SOURCE_DIR}/build/extern/glfw/src
)

# Include the main source files as well as the retreived GL files for gl3w
add_library(cpp2d-graphics SHARED ../Graphics.cpp)

target_link_libraries(cpp2d-graphics glfw3 ${COCOA} ${IOKIT} ${Vulkan_LIBRARIES})