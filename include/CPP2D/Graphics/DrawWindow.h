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

        void clear() const;

        Vec2f getSize() const override;
    };
}