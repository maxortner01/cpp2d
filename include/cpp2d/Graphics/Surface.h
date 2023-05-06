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
        CommandPool         command_pool;
        CommandBufferHandle command_buffers;
        struct {
            SemaphoreHandle image_avaliable;
            SemaphoreHandle render_finished;
            FenceHandle     in_flight;
        } sync_objects;

        Frame(Surface* parent);
    };

    
    class CPP2D_DLL Surface :
        public Sizable,
        public Graphics::GDILifetime,
        public Utility::NoCopy
    {
        struct _Framebuffer
        {
            GDIImage          image;
            FramebufferHandle framebuffer;
        };
        
        const U32 _OVERLAP = 1; 
        U32 _current_frame;
        U32 _current_image_index;

        U32              _image_count;
        Frame*           _frames;
        _Framebuffer*    _framebuffers;
        RenderPassHandle _render_pass;
        FormatHandle     _format;
    
    protected:
        void setCurrentImageIndex(const U32& index);

    public:
        Surface(const Vec2u& extent);
        ~Surface();

        void create(const Vec2u& extent, const SwapChainInfo& swapchain);
        void create(const Vec2u& extent);

        void _startRenderPass();
        void _endRenderPass();

        Frame& getFrame() const;

        U32 getCurrentImageIndex() const;


        FormatHandle  getFormat() const;
        RenderPassHandle getRenderPass() const;

        virtual void display() const = 0;
    };
};