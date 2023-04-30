#pragma once

#include "../Util.h"

namespace cpp2d::Graphics
{
    class CPP2D_DLL Surface
    {
        SurfaceHandle _handle;

    protected:
        void setHandle(SurfaceHandle handle);

    public:
        ~Surface();

        SurfaceHandle getHandle() const;

        virtual void display() const = 0;
    };
};