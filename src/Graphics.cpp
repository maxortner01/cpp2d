#ifdef DEBUG
#   include <loguru.hpp>
#   include <loguru.cpp>

#   define cpp2dFATAL(...) LOG_F(FATAL, __VA_ARGS__)
#   define cpp2dERROR(...) LOG_F(ERROR, __VA_ARGS__)
#   define cpp2dWARN(...)  LOG_F(WARNING, __VA_ARGS__)
#   define cpp2dINFO(...)  LOG_F(INFO, __VA_ARGS__) 
#else
#   define cpp2dFATAL(...) 
#   define cpp2dERROR(...) 
#   define cpp2dWARN(...)
#   define cpp2dINFO(...)
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "./Graphics/Window.cpp"
#include "./Graphics/DrawWindow.cpp"
#include "./Graphics/GDI.cpp"
#include "./Graphics/Surface.cpp"
#include "./Graphics/Shader.cpp"
#include "./Graphics/GraphicsPipeline.cpp"