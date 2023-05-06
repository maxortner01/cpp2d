#include <cpp2d/Graphics.h>

#include <GLFW/glfw3.h>

namespace cpp2d
{
    DrawWindow::DrawWindow(CU32& width, CU32& height, const char* title) :
        Window(width, height, title),
        Surface({width, height})
    {
        Window* window = (Window*)this;

        I32 _width, _height;
        glfwGetFramebufferSize((GLFWwindow*)window->getHandle(), &_width, &_height);
        setExtent({ (U32)_width, (U32)_height });

        _handle = Graphics::GDI::get().getSurfaceHandle(this);

        Graphics::SwapChainInfo info = Graphics::GDI::get().createSwapChain(this);
        _swapchain = info.handle;

        create({ (U32)_width, (U32)_height }, info);
        std::free(info.images);
    }

    DrawWindow::~DrawWindow()
    {   }

    void DrawWindow::display() const
    {
        // swap buffers vulkan version
    }

    Graphics::SurfaceHandle DrawWindow::getSurfaceHandle() const
    {
        return _handle;
    }
}