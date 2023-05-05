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

        // Init surface with vulkan
        setHandle(
            Graphics::GDI::get().getSurfaceHandle(this)
        );

        setSwapChain(
            Graphics::GDI::get().createSwapChain(this)
        );

        setRenderPass(
            Graphics::GDI::get().createRenderPass(this)
        );

        createFramebuffers();
    }

    DrawWindow::~DrawWindow()
    {
        // Delete surface
        Graphics::GDI::get().destroy();
    }

    void DrawWindow::display() const
    {
        // swap buffers vulkan version
    }
}