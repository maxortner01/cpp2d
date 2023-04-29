#include <cpp2d/Graphics.h>

namespace cpp2d
{
    DrawWindow::DrawWindow(CU32& width, CU32& height, const char* title) :
        Window(width, height, title)
    {
        // Init surface with vulkan
    }

    DrawWindow::~DrawWindow()
    {
        // Delete surface
    }
}