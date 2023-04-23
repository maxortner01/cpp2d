#pragma once

#include "../Graphics.h"
#include "../Utility.h"
#include <entt/entt.hpp>

namespace cpp2d
{
    class System;

    enum class SceneState
    {
        Running,
        Done
    };

    class Scene :
        public Utility::State<SceneState>,
        entt::registry
    {
        std::vector<System*> systems;

    protected:
        DrawTexture _texture;

    public:
        Scene(const Vec2u& size);
        virtual ~Scene();

        Vec2u        getSize() const;
        DrawTexture& getDrawTexture();

        virtual void update(DrawSurface* surface) = 0;

        entt::entity createEntity();
        void         destroyEntity(const entt::entity& uid);

        using entt::registry::view;
        using entt::registry::sort;

        template<typename T, typename... Args>
        T& addSystem(Args... args)
        {
            systems.push_back(new T(args...));
            return *(T*)systems[systems.size() - 1];
        }

        std::vector<System*>& getSystems();

        template<typename T>
        T& addComponent(const entt::entity& entity) 
        {
            return emplace<T>(entity);
        }

        template<typename T>
        T& getComponent(const entt::entity& entity)
        {
            return get<T>(entity);
        }
    };
}