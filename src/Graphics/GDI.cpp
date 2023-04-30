#include <cpp2d/Graphics.h>

#include <optional>
#include <string>
#include <cstring>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>


namespace cpp2d::Graphics
{
#ifdef GDI_VULKAN
#   include "./RenderAPI/Vulkan.cpp"

    void GDI::_init()
    {
        _handle = create_instance();
        if (!_handle)
        {
            FATAL("Vulkan instance failed to create.");
            setState(GDIState::InstanceCreateFailed);
            return;
        }

        _debug  = create_debug_manager(_handle);
        _device = create_logic_device(_handle); 

        setState(GDIState::Initialized);
    }

    void GDI::_delete()
    {
        if (_device)
        {
            vkDestroyDevice((VkDevice)_device, nullptr);
            _device = nullptr;
        }

        if (_debug)
        {
            INFO("Destroying debug messenger.");
            DestroyDebugUtilsMessengerEXT((VkInstance)_handle, (VkDebugUtilsMessengerEXT)_debug, nullptr);
            _debug = nullptr;
        }

        if (_handle)
        {
            INFO("Destroying vulkan instance.");
            vkDestroyInstance((VkInstance)_handle, nullptr);
            _handle = nullptr;
        }
    }

#else
    // OpenGL init and cleaup functions
    void GDI::_init()
    {   }

    void GDI::_delete()
    {   }
#endif

    GDI::GDI() :
        Utility::State<GDIState>(GDIState::NotInitialized)
    {
        // Init by default?
        _init();
    }

    GDI::~GDI()
    {
        _delete();
    }
}