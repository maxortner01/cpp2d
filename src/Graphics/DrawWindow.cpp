#include <cpp2d/Graphics.h>

namespace cpp2d
{
    DrawWindow::DrawWindow(CU32& width, CU32& height, const char* title) :
        Window(width, height, title)
    {
        // Init surface with vulkan
        Graphics::GDI::get();
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