#pragma once

#include <cpp2d/Graphics.h>
#include "../System.h"

namespace cpp2d
{
namespace Systems
{
    struct Transform
    {
        Vec2f position = Vec2f(0, 0);
        Vec2f scale    = Vec2f(1, 1);
        float rotation = 0;
    };

    // If this is component is added to an entity, the sprite goes to the static
    // renderer
    enum class Static {};

    struct Sprite
    {
        float     z = 0.f;
        uint32_t  texture = 0;
        Color     color   = { 1, 1, 1, 1 };
        Transform transform;
        FloatRect rect = { 0, 0, 1, 1 };
    };

    // This way, we can load in the same quad/texture data but
    // let the per instance buffer data change
    class SpriteVertexArray :
        public VertexArray
    {
    public:
        SpriteVertexArray(const uint32_t& extraBuffers);
    };

    // We've got two renderers that use the same shader, no need
    // to load it in twice
    class SpriteShader :
        public Utility::Singleton<SpriteShader>,
        Shader
    {
    public:
        SpriteShader();

        using Shader::bind;
        using Shader::unbind;
        using Shader::setUniform;
    };

    class SpriteRenderer :
        public System
    {
    public:
        struct SceneData
        {
            Texture* textureList;
            uint32_t textureCount;
            struct 
            {
                Vec2f position = Vec2f(0, 0);
                float scale = 1.f;
            } camera;
        };

    private:
        SceneData _scene_data;

        SpriteVertexArray quad;

        void submitRender(Sprite* const sprites, size_t count);

    public:
        SpriteRenderer();

        SceneData& getSceneData();

        void setTextureList(Texture* textureList, const uint32_t& count);

        void update(Scene* const scene, double dt) override;
    };
}
}