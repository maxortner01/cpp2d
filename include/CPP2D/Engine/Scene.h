#pragma once

namespace cpp2d
{
    enum class SceneState
    {
        Running,
        Done
    };

    class Scene :
        public Utility::State<SceneState>
    {
        void* _registry;

    protected:
        DrawTexture _texture;

    public:
        Scene(const Vec2u& size);
        virtual ~Scene();

        uint32_t createEntity() const;
        void     destroyEntity(const uint32_t& uid) const;

        Vec2u        getSize() const;
        DrawTexture& getDrawTexture();

        virtual void update(DrawSurface* surface) = 0;
    };
}