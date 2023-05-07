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
        CommandPool   command_pool;
        CommandBuffer command_buffers;
        struct {
            SemaphoreHandle image_avaliable;
            SemaphoreHandle render_finished;
            FenceHandle     in_flight;
        } sync_objects;

        Frame(Surface* parent);
        ~Frame();

        void waitUntilReady() const;
    };

    /**
     * @brief Encapsulates a surface to render onto.
     * 
     * This object is the general surface upon which you can render objects. There is only ever
     * exposed *one* framebuffer at an instant, whose index can be retreived/set with get/setCurrentFrameindex().
     * 
     * In order to begin rendering, use the startRenderPass method. Then call your render methods and pass to them
     * the surface. Once rendering is finished, call endRenderPass.
     */
    class CPP2D_DLL Surface :
        public Sizable,
        public Graphics::GDILifetime,
        public Utility::NoCopy
    {
        struct FrameImage
        {
            GDIImage          image;
            FramebufferHandle framebuffer;
        };
        
        const U32 _OVERLAP = 1; 
        U32 _current_frame;
        U32 _current_image_index;

        U32              _image_count;
        Frame*           _frames;
        FrameImage*      _framebuffers;
        RenderPassHandle _render_pass;
        FormatHandle     _format;

    public:
        Surface(const Vec2u& extent);
        ~Surface();

        void create(const Vec2u& extent, const SwapChainInfo& swapchain);
        void create(const Vec2u& extent);

        /**
         * @brief Begins the render pass.
         * 
         * Begins render pass on the current frame. *Does not* attempt to wait on fences.
         */
        CommandBufferHandle startRenderPass();

        /**
         * @brief Ends the render pass.
         * 
         * Ends render pass on the current frame and submits the command buffer of the current frame
         * to the graphics queue.
         */
        void endRenderPass(const CommandBufferHandle& commandBuffer);

        Frame& getFrame() const;
        
        void setCurrentImageIndex(const U32& index);
        U32 getCurrentImageIndex() const;

        FormatHandle  getFormat() const;
        RenderPassHandle getRenderPass() const;

        virtual void display() const = 0;
    };
};