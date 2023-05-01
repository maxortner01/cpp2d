#include <cpp2d/Graphics.h>

namespace cpp2d::Graphics
{
    Surface::Surface(const Vec2u& extent) :
        Sizable(extent),
        _swapchain_images{
            .count = 0,
            .format = 0,
            .handles = nullptr
        }
    {   }

    Surface::~Surface()
    {   
        if (_swapchain_images.handles)
        {
            std::free(_swapchain_images.handles);
            _swapchain_images.handles = nullptr;
        }
    }

    SurfaceHandle Surface::getHandle() const
    {
        return _handle;
    }

    void Surface::setSwapChain(SwapChainInfo info)
    {
        _swapchain = info.handle;
#   ifdef GDI_VULKAN
        if (_swapchain_images.handles) std::free(_swapchain_images.handles);
        
        // info.images has already been malloced (GDI::createSwapChain), so no need to allocate more memory
        // we just need to be sure to free this on destruction
        _swapchain_images.handles = info.images;
        _swapchain_images.format = info.format;
        _swapchain_images.count  = info.image_count;
#   endif
    }

    void Surface::setHandle(SurfaceHandle handle)
    {
        _handle = handle;
    }
}