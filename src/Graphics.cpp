#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <CPP2D/Graphics.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "./Graphics/Window.cpp"
#include "./Graphics/GraphicsInstance.cpp"
#include "./Graphics/VertexArray.cpp"
#include "./Graphics/GraphicsBuffer.cpp"
#include "./Graphics/DrawWindow.cpp"
#include "./Graphics/QuadRenderer.cpp"
#include "./Graphics/Transform2D.cpp"
#include "./Graphics/Shader.cpp"
#include "./Graphics/Quad.cpp"
#include "./Graphics/Sizable.cpp"
#include "./Graphics/Texture.cpp"
#include "./Graphics/DrawTexture.cpp"

#include "./Graphics/Interfaces/Renderer.cpp"