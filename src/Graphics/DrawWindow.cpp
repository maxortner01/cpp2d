#include <CPP2D/Graphics.h>

#include <GLFW/glfw3.h>
#include <GL/glew.h>

namespace cpp2d
{
    void DrawWindow::bind() 
    {
        // Maybe store the currently bound frame buffer and re-bind
        // on calling this->unbind()
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DrawWindow::unbind() 
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    Vec2u DrawWindow::getSize() const
    {
        return ((Window*)this)->getSize();
    }

    void DrawWindow::clear()
    {
        bind();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        unbind();
    }

    /*
    Vec2f DrawWindow::getSize() const
    {
        int32_t dimensions[] = { 0, 0 };
        glfwGetWindowSize((GLFWwindow*)_window, &dimensions[0], &dimensions[1]);
        return Vec2f(dimensions[0], dimensions[1]);
    }*/
}