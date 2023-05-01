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

        {
            void* arguments = std::malloc(sizeof(void*));
            std::memcpy(arguments, &_handle, sizeof(void*));

            _objects.emplace(
                GDIObjectInstance {
                    .type           = GDIObject::Instance,
                    .handle         = _handle,
                    .argument_count = 1,
                    .arguments      = arguments
                }
            );
        }
        
        _debug = create_debug_manager(_handle);
        if (!_debug)
            ERROR("Debug messenger failed to create.");
        else
        {
            void** arguments = (void**)std::malloc(sizeof(void*) * 2);
            arguments[0] = _handle;
            arguments[1] = _debug;
            _objects.emplace(
                GDIObjectInstance {
                    .type           = GDIObject::DebugMessenger,
                    .handle         = _debug,
                    .argument_count = 2,
                    .arguments      = arguments
                }
            );
        }

        vkEnumeratePhysicalDevices((VkInstance)_handle, &_physical_devices.device_count, nullptr);
        _physical_devices.handles = (GDIPhysicalDevice*)std::malloc(sizeof(GDIPhysicalDevice) * _physical_devices.device_count);
        vkEnumeratePhysicalDevices((VkInstance)_handle, &_physical_devices.device_count, (VkPhysicalDevice*)_physical_devices.handles);

        setState(GDIState::Initialized);
    }

    void GDI::_delete()
    {
        while (!_objects.empty())
        {
            GDIObjectInstance& object = _objects.top();

            char* it = (char*)object.arguments;
            switch (object.type)
            {
            case GDIObject::Instance:
                {
                    INFO("Destroying vulkan instance.");
                    VkInstance instance = *(VkInstance*)it;
                    vkDestroyInstance(instance, nullptr);
                    break;
                }

            case GDIObject::DebugMessenger:
                {
                    VkInstance instance = *(VkInstance*)it;
                    it += sizeof(VkInstance);
                    VkDebugUtilsMessengerEXT messenger = *(VkDebugUtilsMessengerEXT*)it;

                    INFO("Destroying debug messenger.");
                    DestroyDebugUtilsMessengerEXT(instance, messenger, nullptr);
                    break;
                }

            case GDIObject::Device:
                {
                    INFO("Destroying logic device.");
                    VkDevice device = *(VkDevice*)it;
                    vkDestroyDevice(device, nullptr);
                    break;
                }

            case GDIObject::Surface:
                {
                    INFO("Destroying surface.");
                    VkInstance instance = *(VkInstance*)it;
                    it += sizeof(VkInstance);
                    VkSurfaceKHR surface = *(VkSurfaceKHR*)it;

                    vkDestroySurfaceKHR(instance, surface, nullptr);
                    break;
                }
            }

            std::free(object.arguments);
            _objects.pop();
        }
    }

    SwapChainHandle GDI::createSwapChain(const Surface* surface)
    {
        VkPhysicalDevice _suitable_device = (VkPhysicalDevice)_physical_devices.handles[_suitable_device_index];

        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_suitable_device, (VkSurfaceKHR)surface->getHandle(), &capabilities);


        U32 format_count, present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(_suitable_device, (VkSurfaceKHR)surface->getHandle(), &present_mode_count, nullptr);
        vkGetPhysicalDeviceSurfaceFormatsKHR(_suitable_device, (VkSurfaceKHR)surface->getHandle(), &format_count, nullptr);

        ScopedData<VkSurfaceFormatKHR> formats(format_count);
        ScopedData<VkPresentModeKHR> present_modes(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(_suitable_device, (VkSurfaceKHR)surface->getHandle(), &present_mode_count, present_modes.ptr());
        vkGetPhysicalDeviceSurfaceFormatsKHR(_suitable_device, (VkSurfaceKHR)surface->getHandle(), &format_count, formats.ptr());

        VkSurfaceFormatKHR surface_format;
        VkPresentModeKHR   present_mode;
        VkExtent2D         extent;

        return nullptr;
    }

    SurfaceHandle GDI::getSurfaceHandle(const Window* window)
    {
        INFO("Creating surface.");
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface((VkInstance)_handle, (GLFWwindow*)window->getHandle(), nullptr, &surface) != VK_SUCCESS)
        {
            ERROR("GLFW failed to create window surface!");
            return nullptr;
        }

        void** arguments = (void**)std::malloc(sizeof(VkInstance) + sizeof(VkSurfaceKHR));
        arguments[0] = _handle;
        arguments[1] = surface;
        _objects.emplace(
            GDIObjectInstance {
                .type           = GDIObject::Surface,
                .handle         = surface,
                .argument_count = 2,
                .arguments      = arguments
            }
        );

        INFO("Surface created successfully.");

        _suitable_device_index = -1;
        for (U32 i = 0; i < _physical_devices.device_count; i++)
            if (is_suitable_device((VkPhysicalDevice)_physical_devices.handles[i], surface))
            {
                _suitable_device_index = i;
                break;
            }

        if (_suitable_device_index == -1)
            WARN("No suitable physical device found for surface.");

        _device = create_logic_device(_handle, surface, (VkPhysicalDevice*)_physical_devices.handles, _suitable_device_index); 
        if (!_device.handle)
        {
            FATAL("Failed to create logic device!");
            setState(GDIState::DeviceCreationFailed);
            return nullptr;
        }

        {
            void** arguments = (void**)std::malloc(sizeof(void*));
            arguments[0] = _device.handle;
            _objects.emplace(
                GDIObjectInstance {
                    .type           = GDIObject::Device,
                    .handle         = _device.handle,
                    .argument_count = 1,
                    .arguments      = arguments
                }
            );
        }

        return (SurfaceHandle)surface;
    }

#else
    // OpenGL init and cleaup functions
    void GDI::_init()
    {   }

    void GDI::_delete()
    {   }

    SurfaceHandle GDI::getSurfaceHandle() const
    { return nullptr; }
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

        if (_physical_devices.handles)
        {
            std::free(_physical_devices.handles);
            _physical_devices.handles = nullptr;
        }
    }

    GDIHandle GDI::getHandle() const
    {
        return _handle;
    }
}