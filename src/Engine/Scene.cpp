#include <cpp2d/Engine.h>

#include <entt/entt.hpp>

#define REGISTRY_CAST (entt::registry*)_registry

namespace cpp2d
{
    Scene::Scene(const Vec2u& size) :
        _texture(size), 
        _registry(nullptr),
        Utility::State<SceneState>(SceneState::Running)
    {   
        _registry = (void*)(new entt::registry());

        entt::registry* reg = REGISTRY_CAST;
        entt::entity entity = reg->create();
        entt::entity entity2 = reg->create();
        std::cout << (uint32_t)entity << "\n";
        std::cout << (uint32_t)entity2 << "\n";
    }

    Scene::~Scene()
    {
        entt::registry* reg = REGISTRY_CAST;

        if (reg)
        {
            reg->clear();
            delete reg;
            _registry = nullptr;
        }
    }

    uint32_t Scene::createEntity() const
    {
        entt::registry* reg = REGISTRY_CAST;
        return (uint32_t)reg->create();
    }

    void Scene::destroyEntity(const uint32_t& entity) const
    {
        entt::registry* reg = REGISTRY_CAST;
        reg->destroy((entt::entity)entity);
    }

    Vec2u Scene::getSize() const
    {
        return _texture.getSize();
    }

    DrawTexture& Scene::getDrawTexture() 
    {
        return _texture;
    }

}

#undef REGISTRY_CAST