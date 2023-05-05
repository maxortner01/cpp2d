#include <cpp2d/Graphics.h>

namespace cpp2d::Graphics
{
    // Currently, deriative classes are expected to initialize all the handles...
    // but I don't really like this, so lets move the code from the drawwindow constructor
    // into a .create() method that is called by the drawwindow 
    Surface::Surface(const Vec2u& extent) :
        Sizable(extent),
        _swapchain_images{
            .count = 0,
            .format = 0,
            .images = nullptr,
            .framebuffers = nullptr
        }
    {   }

    Surface::~Surface()
    {   
        if (_swapchain_images.images)
        {
            std::free(_swapchain_images.images);
            _swapchain_images.images = nullptr;
        }

        if (_swapchain_images.framebuffers)
        {
            std::free(_swapchain_images.framebuffers);
            _swapchain_images.framebuffers = nullptr;
        }
    }

    FormatHandle Surface::getFormat() const
    {
        return _swapchain_images.format;
    }

    SurfaceHandle Surface::getHandle() const
    {
        return _handle;
    }

    RenderPassHandle Surface::getRenderPass() const
    {
        return _render_pass;
    }

    void Surface::createFramebuffers() 
    {
        _swapchain_images.framebuffers = (FramebufferHandle*)std::malloc(sizeof(FramebufferHandle) * _swapchain_images.count);
        for (U32 i = 0; i < _swapchain_images.count; i++)
            _swapchain_images.framebuffers[i] = GDI::get().createFramebuffer(_swapchain_images.images[i].image_view, this);
    }

    void Surface::setSwapChain(SwapChainInfo info)
    {
        _swapchain = info.handle;
#   ifdef GDI_VULKAN
        if (_swapchain_images.images) std::free(_swapchain_images.images);
        
        // info.images has already been malloced (GDI::createSwapChain), so no need to allocate more memory
        // we just need to be sure to free this on destruction
        _swapchain_images.images = info.images;
        _swapchain_images.format = info.format;
        _swapchain_images.count  = info.image_count;
#   endif
    }

    void Surface::setHandle(SurfaceHandle handle)
    {
        _handle = handle;
    }

    void Surface::setRenderPass(RenderPassHandle handle)
    {
        _render_pass = handle;
    }
}