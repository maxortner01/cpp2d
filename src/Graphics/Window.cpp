#include <CPP2D/Graphics.h>

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace cpp2d
{
    Window::Window(const uint32_t& width, const uint32_t& height, const char* title)
    {
        if (!glfwInit())
        {
            setState(WindowState::glfwInitFailed);
            return;
        }
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        GLFWwindow* _window_instance = glfwCreateWindow(width, height, title, NULL, NULL);

        int32_t _width, _height;
        glfwGetFramebufferSize(_window_instance, &_width, &_height);

        if (!_window_instance)
        {
            setState(WindowState::glfwWindowCreateFailed);
            return;
        }

        glfwMakeContextCurrent(_window_instance);

        GraphicsInstance::get().init();
        glViewport(0, 0, _width, _height);

        _window = _window_instance;
        setState(WindowState::Success);
    }

    Window::~Window()
    {
        glfwDestroyWindow((GLFWwindow*)_window);
    }

    void Window::pollEvent() 
    {
        glfwPollEvents();
    }

    void Window::display()
    {
        glfwSwapBuffers((GLFWwindow*)_window);
    }

    bool Window::isOpen() const
    {
        return !glfwWindowShouldClose((GLFWwindow*)_window);
    }
}