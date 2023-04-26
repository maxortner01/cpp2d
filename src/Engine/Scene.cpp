#include <cpp2d/Engine.h>

namespace cpp2d
{
    Scene::Scene(const Vec2u& size) :
        _texture(size), 
        _systems_modified(false),
        Utility::State<SceneState>(SceneState::Running)
    {   }

    Scene::~Scene()
    {  this->clear();  }

    entt::entity Scene::createEntity()
    {  return create();  }

    void Scene::destroyEntity(const entt::entity& entity)
    {  destroy(entity);  }

    bool Scene::systemsModified() const
    {  return _systems_modified;  }

    std::vector<System*>& Scene::getSystems()
    {  return systems;  }

    Vec2u Scene::getSize() const
    {  return _texture.getSize();  }

    DrawTexture& Scene::getDrawTexture() 
    {  return _texture;  }

    float Scene::aspectRatio() const
    {  return (float)_texture.getSize().x / (float)_texture.getSize().y;  }

}