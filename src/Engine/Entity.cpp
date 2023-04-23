#include <cpp2d/Engine.h>

namespace cpp2d
{
    Entity::Entity(Scene* const scene) :
        _scene(scene),
        _uid(scene->createEntity())
    {

    }

    Entity::~Entity()
    {
        _scene->destroyEntity(_uid);
    }
}