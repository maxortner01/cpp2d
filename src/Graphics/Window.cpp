#include <cpp2d/Graphics.h>

#include <iostream>
#include <VkBootstrap.h>
#include <GLFW/glfw3.h>

namespace cpp2d
{
    Window::Window(const uint32_t& width, const uint32_t& height, const char* title) :
        UnsignedSizable({width, height})
    {
        if (!glfwInit())
        {
            setState(WindowState::glfwInitFailed);
            return;
        }
        
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

#   ifdef GDI_OPENGL
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#   elif GDI_VULKAN
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#   endif

#   if defined(__APPLE__) && defined(GDI_OPENGL)
        // Because Apple hates graphics programmers
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#   endif

        GLFWwindow* _window_instance = glfwCreateWindow(width, height, title, NULL, NULL);

        int32_t _width, _height;
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

    Window::~Window()
    {
#   ifdef GDI_VULKAN
        std::cout << (U32)getState() << "\n";
        if (getState() == WindowState::Success && Graphics::GDI::get().getState() == Graphics::GDIState::Initialized)
        {
            const Graphics::InstanceData& _instance = Graphics::GDI::get().getInstanceData();
            vkDestroySwapchainKHR((VkDevice)_instance.logic_device, (VkSwapchainKHR)_swap_chain, nullptr);

            for (U32 i = 0; i < _image_views.size(); i++)
                vkDestroyImageView((VkDevice)_instance.logic_device, (VkImageView)_image_views[i], nullptr);

            vkDestroySurfaceKHR((VkInstance)_instance.handle, (VkSurfaceKHR)_surface, nullptr);
        }
        
#   endif
    
        if (getState() == WindowState::Success)
            glfwDestroyWindow((GLFWwindow*)_window);

        setState(WindowState::Destroyed);
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