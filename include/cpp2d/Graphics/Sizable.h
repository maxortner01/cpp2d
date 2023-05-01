#pragma once

#include "../Math.h"

namespace cpp2d::Graphics
{
    class Sizable
    {
        Vec2u _extent;
    protected:
        void setExtent(const Vec2u& extent);

    public:
        Sizable(const Vec2u& extent);

        Vec2u getExtent() const;
    };

    void Sizable::setExtent(const Vec2u& extent)
    {
        _extent = extent;
    }

    Sizable::Sizable(const Vec2u& extent) :
        _extent(extent)
    {   }

    Vec2u Sizable::getExtent() const
    {
        return _extent;
    }
}