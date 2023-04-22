#pragma once

#include "../util.h"

namespace cpp2d
{
    class Scene;

    class Entity
    {
        const Scene* const _scene;
        const uint32_t _uid;

    public:
        Entity(const Scene* const scene);
        virtual ~Entity();
    };
}