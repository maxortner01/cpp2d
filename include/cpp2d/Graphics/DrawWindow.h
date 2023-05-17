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
        DrawWindow(Graphics::GDI& gdi, CU32& width, CU32& height, const char* title);
        ~DrawWindow();

        void display() const override;

        Memory::ManagedObject<Graphics::FrameData> beginFrame(Graphics::GDI& gdi, Memory::Manager* manager);
        void endFrame(const Memory::ManagedObject<Graphics::FrameData>& frameData);

        Graphics::SurfaceHandle getSurfaceHandle() const;
    };
}