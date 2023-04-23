#pragma once

#include <entt/entt.hpp>

#include "../util.h"

namespace cpp2d
{
    class Scene;

    class Entity
    {
        Scene* const _scene;
        const entt::entity _uid;

    public:
        Entity(Scene* const scene);
        virtual ~Entity();

        template<typename T>
        T& addComponent()
        {
            return _scene->addComponent<T>(_uid);
        }

        template<typename T>
        T& getComponent()
        {
            return _scene->getComponent<T>(_uid);
        }
    };
}