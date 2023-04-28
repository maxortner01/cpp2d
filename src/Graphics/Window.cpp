#include <cpp2d/Graphics.h>

namespace cpp2d
{
    Window::Window(const Vec2u& size, const char* title) :
        Window(size.x, size.y, title)
    {   }

    Window::Window(const U32& width, const U32& height, const char* title) :
#   ifdef GDI_VULKAN2
        _surface(nullptr),
        _swap_chain(nullptr),
        _image_format(0),
        //_images{0, nullptr, nullptr},
#   endif
        UnsignedSizable({width, height})
    {
        if (!glfwInit())
        {
            setState(WindowState::glfwInitFailed);
            return;
        }

        if (!glfwVulkanSupported())
        {
            setState(WindowState::glfwVulkanNotSupported);
            return;
        }
        
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

#   ifdef GDI_OPENGL
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#       ifdef __APPLE__
        // Because Apple hates graphics programmers
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#       endif
#   elif GDI_VULKAN
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#   endif

        GLFWwindow* _window_instance = glfwCreateWindow(width, height, title, NULL, NULL);

        I32 _width, _height;
        glfwGetFramebufferSize(_window_instance, &_width, &_height);

        if (!_window_instance)
        {
            setState(WindowState::glfwWindowCreateFailed);
            return;
        }

        glfwMakeContextCurrent(_window_instance);
        glfwSwapInterval( 0 );

        _window = _window_instance;

        //Graphics::GDI::get().init(_window);
        //GraphicsInstance::get().init();
        //GraphicsInstance::get().setViewport({ 0, 0, (U32__width, (U32)_height });

        setState(WindowState::Success);
    }

    // Seg faulting... why? Freaking vectors...
    Window::~Window()
    {
#   ifdef GDI_VULKAN
        if (getState() == WindowState::Success && Graphics::GDI::get() && Graphics::GDI::get()->getState() == Graphics::GDIState::Initialized)
        {
            const Graphics::InstanceData& _instance = Graphics::GDI::get()->getInstanceData();
            vkDestroySwapchainKHR((VkDevice)_instance.logic_device, (VkSwapchainKHR)_swap_chain, nullptr);

            for (U32 i = 0; i < _images.size(); i++)
                vkDestroyImageView((VkDevice)_instance.logic_device, (VkImageView)_image_views[i], nullptr);

            vkDestroySurfaceKHR((VkInstance)_instance.handle, (VkSurfaceKHR)_surface, nullptr);
        }
#   endif

        if (getState() == WindowState::Success && _window)
        {
            glfwDestroyWindow((GLFWwindow*)_window);
            glfwTerminate();

            _window = nullptr;

            setState(WindowState::Destroyed);
        }
    }

    void Window::pollEvent() 
    {
        glfwPollEvents();
    }

    void Window::display()
    {
        glfwSwapBuffers((GLFWwindow*)_window);
    }

    void Window::setTitle(const std::string& title) const
    {
        glfwSetWindowTitle((GLFWwindow*)_window, title.c_str());
    }
    
    void Window::close() const
    {
        glfwSetWindowShouldClose((GLFWwindow*)_window, 1);
    }

    bool Window::isOpen() const
    {
        return !glfwWindowShouldClose((GLFWwindow*)_window);
    }
}