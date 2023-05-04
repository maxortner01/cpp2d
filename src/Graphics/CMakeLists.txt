cmake_minimum_required(VERSION 3.8)

project(cpp2d-graphics)

# We will be compiling with c++20 and vulkan
set(CMAKE_CXX_STANDARD 20)
set(DEFINITIONS GDI_VULKAN CPP2D_GRAPHICS_BUILD)

# Here's our cross platform dependencies, specifically Vulkan and glfw
find_package(Vulkan REQUIRED)
find_library(GLFW3 glfw3 HINTS ${CMAKE_BINARY_DIR}/extern/glfw/src)

# set to shaderc_combined if in Release, otherwise shaderc_combinedd
set(SHADERC_LIB_NAME shaderc_combined)
# !--- Need to check if in debug mode
cmake_path(GET Vulkan_LIBRARIES PARENT_PATH Vulkan_LIBRARY_DIR)
find_library(SHADERC ${SHADERC_LIB_NAME} HINTS ${Vulkan_LIBRARY_DIR})

set(LIBS ${Vulkan_LIBRARIES} ${GLFW3})
if (${SHADERC} MATCHES "SHADERC-NOTFOUND")
    message("Shaderc not being used")
else()
    list(APPEND DEFINITIONS USE_SHADERC)
    list(APPEND LIBS ${SHADERC})
endif()

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

# I still don't know how to do this properly
#if (CMAKE_BUILD_TYPE MATCHES "Debug")
    list(APPEND DEFINITIONS DEBUG)
    list(APPEND INCLUDES ${CMAKE_SOURCE_DIR}/extern/loguru)
#endif()

include_directories(${INCLUDES})
add_compile_definitions(${DEFINITIONS})

add_library(cpp2d-graphics SHARED ../Graphics.cpp)

target_link_libraries(cpp2d-graphics ${LIBS})