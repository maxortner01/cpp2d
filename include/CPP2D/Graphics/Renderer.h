#pragma once

#include <vector>

#include "Shader.h"

namespace cpp2d
{
    class DrawSurface;

    template<typename T>
    class Renderer
    {
    protected:
        std::vector<T> objects;

    public:
        template<typename... Args>
        T& emplaceObject(Args... args)
        {
            objects.emplace_back(args...);
            return objects[objects.size() - 1];
        }
        
        virtual void render(DrawSurface& surface, const Shader& shader) const = 0;
    };
}