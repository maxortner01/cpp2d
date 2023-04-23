#pragma once

#include <cpp2d/Graphics.h>
#include "../System.h"

namespace cpp2d
{
namespace Systems
{
    struct Transform
    {
        Vec2f position;
        Vec2f scale;
        float rotation;
    };

    struct Sprite
    {
        uint32_t  texture;
        Color     color;
        Transform transform;
    };

    class SpriteRenderer :
        public System
    {
        VertexArray quad;
        Shader      spriteShader;

        void submitRender(Transform* transforms, Color* colors, uint32_t* textures, size_t count);

    public:
        SpriteRenderer();

        void update(Scene* const scene, double dt) override;
    };
}
}