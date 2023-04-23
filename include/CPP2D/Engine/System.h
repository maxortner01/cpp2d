#pragma once

namespace cpp2d
{
    class Scene;

    class System
    {
    public:
        virtual void update(Scene* const scene, double dt) = 0;
    };
}