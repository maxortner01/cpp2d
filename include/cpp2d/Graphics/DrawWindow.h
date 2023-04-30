#pragma once

#include "Surface.h"

namespace cpp2d
{
    class Window;

    class DrawWindow :
        public Window,
        public Graphics::Surface
    {
    public:
        DrawWindow(CU32& width, CU32& height, const char* title);
        ~DrawWindow();

        void display() const override;
    };
}