#pragma once

#include "Interfaces.h"
#include "Sizable.h"
#include "Texture.h"

namespace cpp2d
{
    class DrawTexture :
        public UnsignedSizable,
        public DrawSurface
    {
        uint32_t _id;
        Texture  _color;
        Vec2u    _last_dimensions;

    public:
        DrawTexture(const Vec2u& size);
        ~DrawTexture();

        void clear() override;

        // I don't like the lack of const here... needed because bind is also not const
        // and we need to be able to bind the texture
        Texture& getTexture();

        void create(const Vec2u& size);
        Vec2u getSize() const override;

        void bind() override;
        void unbind() override;
    };
}