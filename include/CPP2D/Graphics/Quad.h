#pragma once

#include "Transform2D.h"
#include "../Utility.h"

namespace cpp2d
{
    class Quad :
        public Transform2D
    {
        Color _color;

    public:
        void setColor(const Color& color);
        Color getColor() const;
    };
}