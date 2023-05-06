#pragma once

#include "Surface.h"

namespace cpp2d
{
    class Window;

    class CPP2D_DLL DrawWindow :
        public Window,
        public Graphics::Surface
    {
        Graphics::SurfaceHandle   _handle;
        Graphics::SwapChainHandle _swapchain;

    public:
        DrawWindow(CU32& width, CU32& height, const char* title);
        ~DrawWindow();

        void display() const override;

        void startRenderPass();
        void endRenderPass();

        Graphics::SurfaceHandle getSurfaceHandle() const;
    };
}