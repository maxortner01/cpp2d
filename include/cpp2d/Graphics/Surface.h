#pragma once

#include "../Util.h"
#include "Sizable.h"

namespace cpp2d::Graphics
{
    class CPP2D_DLL Surface :
        public Sizable
    {
        SurfaceHandle   _handle;
        SwapChainHandle _swapchain;

    protected:
        void setHandle(SurfaceHandle handle);
        void setSwapChain(SwapChainHandle handle);

    public:
        Surface(const Vec2u& extent);
        ~Surface();

        SurfaceHandle getHandle() const;

        virtual void display() const = 0;
    };
};