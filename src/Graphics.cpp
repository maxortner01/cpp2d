#ifdef DEBUG
#   include <loguru.hpp>
#   include <loguru.cpp>

#   define FATAL(...) LOG_F(FATAL, __VA_ARGS__)
#   define ERROR(...) LOG_F(ERROR, __VA_ARGS__)
#   define WARN(...)  LOG_F(WARNING, __VA_ARGS__)
#   define INFO(...)  LOG_F(INFO, __VA_ARGS__) 
#else
#   define FATAL(...) 
#   define ERROR(...) 
#   define WARN(...)
#   define INFO(...)
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "./Graphics/Window.cpp"
#include "./Graphics/DrawWindow.cpp"
#include "./Graphics/GDI.cpp"
#include "./Graphics/Surface.cpp"