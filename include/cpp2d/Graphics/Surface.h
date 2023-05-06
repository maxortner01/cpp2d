#pragma once

#include "../Util.h"
#include "Sizable.h"
#include "GDILifetime.h"

namespace cpp2d::Graphics
{
    class Surface;

    // This is an object that represents a singular surface to be drawn on
    // at a time. The frame has its own command pool and subjugated command buffers,
    // it handles their creation and destruction
    //
    // When rendering occurs, the recording is done into the current frame in flight's command buffer
    struct CPP2D_DLL Frame :
        public Graphics::GDILifetime
    {
        GDIImage            image;
        CommandPoolHandle   command_pool;
        FramebufferHandle   framebuffer;
        CommandBufferHandle command_buffers;

        Frame(GDIImage _image, Surface* parent);
    };

    
    class CPP2D_DLL Surface :
        public Sizable,
        public Graphics::GDILifetime,
        public Utility::NoCopy
    {
        
        U32              _frame_count;
        Frame*           _frames;
        RenderPassHandle _render_pass;
        FormatHandle     _format;

    public:
        Surface(const Vec2u& extent);
        ~Surface();

        void create(const Vec2u& extent, const SwapChainInfo& swapchain);
        void create(const Vec2u& extent);

        void startRenderPass();
        void endRenderPass();

        Frame& getFrame(const U32& index) const;

        FormatHandle  getFormat() const;
        RenderPassHandle getRenderPass() const;

        virtual void display() const = 0;
    };
};