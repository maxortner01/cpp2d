cmake_minimum_required(VERSION 3.8)

project(main)

# Compile dependencies first
# somehow we need to compile the glew auto makefile
add_custom_target(main
                  /usr/bin/make -f "${CMAKE_SOURCE_DIR}/extern/glew/auto/Makefile")

add_subdirectory(extern/glfw)

#Look for an executable called sphinx-build
find_program(SPHINX_EXECUTABLE
             NAMES sphinx-build
             DOC "Path to sphinx-build executable")

include(FindPackageHandleStandardArgs)

#Handle standard arguments to find_package like REQUIRED and QUIET
find_package_handle_standard_args(Sphinx
                                  "Failed to find sphinx-build executable"
                                  SPHINX_EXECUTABLE)

add_subdirectory(src)
add_subdirectory(docs)
