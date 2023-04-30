#include <cpp2d/Graphics.h>

#include <vulkan/vulkan.h>

namespace cpp2d::Graphics
{
#ifdef GDI_VULKAN
    Surface::~Surface()
    {
        if (_handle && GDI::get().getState() == GDIState::Initialized)
        {
            INFO("Destroying surface.");
            vkDestroySurfaceKHR((VkInstance)GDI::get().getHandle(), (VkSurfaceKHR)_handle, nullptr);
            _handle = nullptr;
        }

        Graphics::GDI::get().destroy();
    }
#else
    Surface::~Surface()
    {   }
#endif

    SurfaceHandle Surface::getHandle() const
    {
        return _handle;
    }

    void Surface::setHandle(SurfaceHandle handle)
    {
        _handle = handle;
    }
}