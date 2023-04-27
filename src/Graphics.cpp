#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <cpp2d/Graphics.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VkBootstrap.h>
#include <VkBootstrap.cpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "./Graphics/GDI.cpp"
#include "./Graphics/Window.cpp"
#include "./Graphics/DrawWindow.cpp"
#include "./Graphics/Sizable.cpp"
#include "./Graphics/Timer.cpp"
#include "./Graphics/Transform2D.cpp"

/*
#include "./Graphics/VertexArray.cpp"
#include "./Graphics/GraphicsBuffer.cpp"
#include "./Graphics/QuadRenderer.cpp"
#include "./Graphics/Shader.cpp"
#include "./Graphics/Quad.cpp"
#include "./Graphics/Texture.cpp"
#include "./Graphics/DrawTexture.cpp"*/

#include "./Graphics/Interfaces/Renderer.cpp"