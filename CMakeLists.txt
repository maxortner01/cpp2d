cmake_minimum_required(VERSION 3.8)

project(main)

# Compile dependencies first
# somehow we need to compile the glew auto makefile
add_custom_target(main
                  /usr/bin/make -f "${CMAKE_SOURCE_DIR}/extern/glew/auto/Makefile")

add_subdirectory(extern/glfw)

add_subdirectory(src)
add_subdirectory(client)
