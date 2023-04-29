#include <cpp2d/Graphics.h>

#include <GLFW/glfw3.h>

namespace cpp2d
{
    Window::Window(CU32& width, CU32& height, const char* title) :
        _handle(nullptr),
        State<WindowState>(WindowState::NotInitialized)
    {
        if (!glfwInit())
        {
            setState(WindowState::glfwInitFailed);
            return;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE,  GLFW_FALSE );
        _handle = glfwCreateWindow(width, height, title, NULL, NULL);

        I32 _width, _height;
        glfwGetFramebufferSize((GLFWwindow*)_handle, &_width, &_height);

        if (!_handle)
        {
            setState(WindowState::glfwWindowCreateFailed);
            return;
        }

        glfwMakeContextCurrent((GLFWwindow*)_handle);
        glfwSwapInterval(0);

        //glViewport(0, 0, _width, _height);

        setState(WindowState::Success);
    }

    Window::~Window()
    {
        if (_handle)
        {
            glfwDestroyWindow((GLFWwindow*)_handle);
            
            // while we're doing single window applications
            // in the future perhaps we keep a static stack of 
            // created windows and when this stack is empty, call this
            glfwTerminate();
            _handle = nullptr;
        }
    }

    bool Window::isOpen() const
    {
        assert(_handle);
        return (glfwWindowShouldClose((GLFWwindow*)_handle) != GLFW_TRUE);
    }

    void Window::close()
    {
        assert(_handle);
        glfwSetWindowShouldClose((GLFWwindow*)_handle, true);
    }

    void Window::pollEvents() const
    {
        glfwPollEvents();
    }
}