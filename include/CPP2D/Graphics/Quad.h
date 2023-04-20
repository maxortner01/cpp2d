#pragma once

#include "Transform2D.h"
#include "../Utility.h"

namespace cpp2d
{
    class Quad :
        public Transform2D
    {
        FloatRect _texture_rect;
        Color _color;
        float _z;

    public:
        Quad();

        float getZ() const; 
        void  setZ(const float& z);

        FloatRect getTextureRect() const;
        void setTextureRect(const FloatRect& rect);

        void setColor(const Color& color);
        Color getColor() const;
    };
}