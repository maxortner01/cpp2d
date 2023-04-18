#pragma once

#include "Window.h"
#include "Interfaces.h"

namespace cpp2d
{
    class DrawWindow :
        public Window,
        public DrawSurface
    {
    public:
        using Window::Window;

        void bind() override;
        void unbind() override;
        
        Vec2u getSize() const override;
        void clear() override;
    };
}