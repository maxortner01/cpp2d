cmake_minimum_required(VERSION 3.8)

project(cpp2d-graphics)

# We will be compiling with c++17 and vulkan
set(CMAKE_CXX_STANDARD 17)
set(DEFINITIONS GDI_VULKAN)

# Here's our cross platform dependencies, specifically Vulkan and glfw
find_package(Vulkan REQUIRED)

set(LIBS ${Vulkan_LIBRARIES} libglfw3.a)
if (${APPLE})
    find_library(COCOA Cocoa)
    find_library(IOKIT IOKit)

    list(APPEND LIBS ${COCOA} ${IOKIT})
endif()

# include the main include directory, glfw, vulkan, and loguru if in debug mode
set(INCLUDES 
    ${CMAKE_SOURCE_DIR}/include/
    ${CMAKE_SOURCE_DIR}/extern/glfw/include
    ${Vulkan_INCLUDE_DIR}
)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    list(APPEND DEFINITIONS DEBUG)
    list(APPEND INCLUDES ${CMAKE_SOURCE_DIR}/extern/loguru)
endif()

include_directories(${INCLUDES})
link_directories(
    ${CMAKE_BINARY_DIR}/extern/glfw/src
)

add_compile_definitions(${DEFINITIONS})

add_library(cpp2d-graphics SHARED ../Graphics.cpp)

target_link_libraries(cpp2d-graphics ${LIBS})