#pragma once

#include <vector>

#include "../Shader.h"

namespace cpp2d
{
    class DrawSurface;

    template<typename T>
    class Renderer
    {
    protected:
        std::vector<T>* objects;

    public:
        Renderer() : objects(nullptr) {}

        void submitObjects(std::vector<T>* _objects) { objects = _objects; }

        virtual void render(DrawSurface& surface, const Shader& shader) const = 0;
    };
}