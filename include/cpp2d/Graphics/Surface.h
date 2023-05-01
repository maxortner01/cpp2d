#pragma once

#include "../Util.h"
#include "Sizable.h"

namespace cpp2d::Graphics
{
    class CPP2D_DLL Surface :
        public Sizable
    {
        struct 
        {
            U32          count;
            FormatHandle format;
            ImageHandle* handles;
        } _swapchain_images;

        SurfaceHandle   _handle;
        SwapChainHandle _swapchain;

    protected:
        void setHandle(SurfaceHandle handle);
        void setSwapChain(SwapChainInfo info);

    public:
        Surface(const Vec2u& extent);
        ~Surface();

        SurfaceHandle getHandle() const;

        virtual void display() const = 0;
    };
};