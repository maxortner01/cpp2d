#include <cpp2d/Graphics.h>

#include <limits>
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

            Utility::TypeIterator it(object.arguments);
            switch (object.type)
            {
            case GDIObject::Instance:
                {
                    INFO("Destroying vulkan instance.");
                    VkInstance instance = it.get<VkInstance>();
                    vkDestroyInstance(instance, nullptr);
                    break;
                }

            case GDIObject::DebugMessenger:
                {
                    VkInstance               instance  = it.get<VkInstance>();
                    VkDebugUtilsMessengerEXT messenger = it.get<VkDebugUtilsMessengerEXT>();

                    INFO("Destroying debug messenger.");
                    DestroyDebugUtilsMessengerEXT(instance, messenger, nullptr);
                    break;
                }

            case GDIObject::Device:
                {
                    INFO("Destroying logic device.");
                    VkDevice device = it.get<VkDevice>();
                    vkDestroyDevice(device, nullptr);
                    break;
                }

            case GDIObject::Surface:
                {
                    INFO("Destroying surface.");
                    VkInstance instance  = it.get<VkInstance>();
                    VkSurfaceKHR surface = it.get<VkSurfaceKHR>();

                    vkDestroySurfaceKHR(instance, surface, nullptr);
                    break;
                }

            case GDIObject::Swapchain:
                {
                    INFO("Destroying swapchain.");
                    VkDevice device          = it.get<VkDevice>();
                    VkSwapchainKHR swapchain = it.get<VkSwapchainKHR>();

                    vkDestroySwapchainKHR(device, swapchain, nullptr);
                    break;
                }
            }

            std::free(object.arguments);
            _objects.pop();
        }
    }

    SwapChainHandle GDI::createSwapChain(const Surface* surface)
    {
        INFO("Creating swapchain for surface.");
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

        VkSurfaceFormatKHR surface_format = choose_swapchain_format(formats.ptr(), formats.size());
        VkPresentModeKHR   present_mode   = choose_swapchain_present_mode(present_modes.ptr(), present_modes.size());
        VkExtent2D         extent         = choose_swapchain_extent(capabilities, surface);

        // Clamp the image count
        U32 image_count = capabilities.minImageCount + 1;
        if (image_count > capabilities.maxImageCount) image_count = capabilities.maxImageCount;

        VkSwapchainCreateInfoKHR create_info {
            .sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface          = (VkSurfaceKHR)surface->getHandle(),
            .minImageCount    = image_count,
            .imageFormat      = surface_format.format,
            .imageColorSpace  = surface_format.colorSpace,
            .imageExtent      = extent,
            .imageArrayLayers = 1,
            .imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .preTransform     = capabilities.currentTransform,
            .compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode      = present_mode,
            .clipped          = VK_TRUE,
            .oldSwapchain     = VK_NULL_HANDLE
        };

        QueueFamilyIndices indices = findQueueFamilies(_suitable_device, (VkSurfaceKHR)surface->getHandle());
        U32 queue_family_indices[] = {
            indices.graphics_index.value(),
            indices.present_index.value()
        };

        if (indices.graphics_index != indices.present_index)
        {
            create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = queue_family_indices;
        }
        else
        {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            create_info.queueFamilyIndexCount = 0; 
            create_info.pQueueFamilyIndices = nullptr; 
        }

        VkSwapchainKHR swapchain;
        VkResult result = vkCreateSwapchainKHR((VkDevice)_device.handle, &create_info, nullptr, &swapchain);
        if (result != VK_SUCCESS)
        {
            ERROR("Error creating swapchain: %i", result);
            return nullptr;
        }

        void** arguments = (void**)std::malloc(sizeof(void*) * 2);
        arguments[0] = _device.handle;
        arguments[1] = swapchain;

        _objects.emplace(
            GDIObjectInstance {
                .type           = GDIObject::Swapchain,
                .handle         = swapchain,
                .argument_count = 2,
                .arguments      = arguments
            }
        );

        INFO("Swapchain created successfully.");
        return (SwapChainHandle)swapchain;
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