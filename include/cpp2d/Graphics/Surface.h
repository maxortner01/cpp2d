#pragma once

#include "../Util.h"
#include "Sizable.h"

namespace cpp2d::Graphics
{
    // Currently this will only work for a window, not a frame buffer,
    // Instead, a draw window should contain an array of surfaces, each representing a vkImage
    // the swapchain thus becomes a drawwindow exclusive object that represents the list 
    // of surfaces

    // Actually no... since only one renderpass is needed for multiple objects, i think this set up
    // is good. this way, a surface can contain as many images as you want to assign to a render pass
    // instead of set swap chain, this should be create images... the swapchain should be created in the
    // draw window constructor
    class CPP2D_DLL Surface :
        public Sizable,
        public Utility::NoCopy
    {
        struct 
        {
            U32                count;
            FormatHandle       format;
            GDIImage*          images;
            FramebufferHandle* framebuffers;
        } _swapchain_images;

        SurfaceHandle    _handle;
        SwapChainHandle  _swapchain;
        RenderPassHandle _render_pass;
        CommandPoolHandle _command_pool;

    protected:
        void setHandle(SurfaceHandle handle);
        void setSwapChain(SwapChainInfo info);
        void setRenderPass(RenderPassHandle handle);
        void createFramebuffers();

    public:
        Surface(const Vec2u& extent);
        ~Surface();

        void startRenderPass();
        void endRenderPass();

        FormatHandle  getFormat() const;
        SurfaceHandle getHandle() const;
        RenderPassHandle getRenderPass() const;

        virtual void display() const = 0;
    };
};