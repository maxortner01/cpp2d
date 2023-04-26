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
        bool _systems_modified;
        std::vector<System*> systems;

    protected:
        DrawTexture _texture;

    public:
        // So that _systems_modified can be reset every time the dependency
        // graph is recalculated without exposing it
        friend class Application;

        Scene(const Vec2u& size);
        virtual ~Scene();

        // Retreival Methods
        Vec2u        getSize() const;
        DrawTexture& getDrawTexture();
        float        aspectRatio() const;
        bool         systemsModified() const;
        std::vector<System*>& getSystems();

        // Update method
        virtual void update(double dt) { };

        // For Entities
        entt::entity createEntity();
        void         destroyEntity(const entt::entity& uid);

        // Passing along of registry functions
        using entt::registry::view;
        using entt::registry::sort;

        // Templated methods
        template<typename T>
        T& emplaceSystem()
        {
            static_assert(std::is_base_of<System, T>::value, "T must derive from System.");
            
            static T* val = nullptr;

            if (!val)
            {
                val = new T();
                systems.push_back(val);
            }

            _systems_modified = true;

            return *val;
        }

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