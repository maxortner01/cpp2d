#pragma once

namespace cpp2d
{
    class DrawSurface;

    template<typename T>
    class Renderer
    {
    public:
        virtual void render(DrawSurface& surface, T& object) const = 0;
    };
}