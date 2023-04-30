#pragma once

#include "../Util.h"

namespace cpp2d::Graphics
{
    class Surface
    {
        SurfaceHandle _handle;

    public:
        SurfaceHandle getHandle() const;

        virtual void display() const = 0;
    };

    SurfaceHandle Surface::getHandle() const
    {
        return _handle;
    }
};