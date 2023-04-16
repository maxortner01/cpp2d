#pragma once

#include "Window.h"
#include "DrawSurface.h"

namespace cpp2d
{
    class DrawWindow :
        public Window,
        public DrawSurface
    {
    public:
        using Window::Window;
    };
}