cmake_minimum_required(VERSION 3.8)

project(cpp2d-graphics)

include_directories(
    ${CMAKE_SOURCE_DIR}/include/
    ${CMAKE_SOURCE_DIR}/extern/glfw/include
)

link_directories(
    ${CMAKE_BINARY_DIR}/extern/glfw/src
)

set(CMAKE_CXX_STANDARD 17)

# Here's our cross platform dependencies, specifically Vulkan and glfw
find_library(VULKAN Vulkan)

set(LIBS ${VULKAN} libglfw3.a)
if (${APPLE})
    find_library(COCOA Cocoa)
    find_library(IOKIT IOKit)

    list(APPEND LIBS ${COCOA} ${IOKIT})
endif()

add_library(cpp2d-graphics SHARED ../Graphics.cpp)

target_link_libraries(cpp2d-graphics ${LIBS})