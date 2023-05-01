#pragma once

#include "../Util.h"

namespace cpp2d::Graphics
{
    class CPP2D_DLL Surface
    {
        SurfaceHandle   _handle;
        SwapChainHandle _swapchain;

    protected:
        void setHandle(SurfaceHandle handle);
        void setSwapChain(SwapChainHandle handle);

    public:
        ~Surface();

        SurfaceHandle getHandle() const;

        virtual void display() const = 0;
    };
};