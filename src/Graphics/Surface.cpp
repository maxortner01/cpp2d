#include <cpp2d/Graphics.h>

#include <vulkan/vulkan.h>

namespace cpp2d::Graphics
{
#ifdef GDI_VULKAN
    Surface::~Surface()
    {

    }
#else
    Surface::~Surface()
    {   }
#endif

    SurfaceHandle Surface::getHandle() const
    {
        return _handle;
    }

    void Surface::setSwapChain(SwapChainHandle handle)
    {
        _swapchain = handle;
    }

    void Surface::setHandle(SurfaceHandle handle)
    {
        _handle = handle;
    }
}