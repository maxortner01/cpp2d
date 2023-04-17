#include <CPP2D/Graphics.h>

#include <GLFW/glfw3.h>
#include <GL/glew.h>

namespace cpp2d
{
    void DrawWindow::clear() const
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    Vec2f DrawWindow::getSize() const
    {
        int32_t dimensions[] = { 0, 0 };
        glfwGetWindowSize((GLFWwindow*)_window, &dimensions[0], &dimensions[1]);
        return Vec2f(dimensions[0], dimensions[1]);
    }
}