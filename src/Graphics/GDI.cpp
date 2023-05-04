#include <cpp2d/Graphics.h>

#ifdef GDI_VULKAN
#   include <vulkan/vulkan.h>
#endif

#include <limits>
#include <optional>
#include <string>
#include <cstring>
#include <GLFW/glfw3.h>

#include <cpp2d/Utility.h>

namespace cpp2d::Graphics
{
#ifdef GDI_VULKAN
#   include "./RenderAPI/Vulkan.cpp"

    void GDI::_init()
    {
        _handle = create_instance();
        if (!_handle)
        {
            cpp2dFATAL("Vulkan instance failed to create.");
            setState(GDIState::InstanceCreateFailed);
            return;
        }

        {
            Utility::ArgumentList arguments;
            arguments.set((VkInstance)_handle);

            _objects.push(
                GDIObjectInstance {
                    .type      = GDIObject::Instance,
                    .handle    = _handle,
                    .arguments = arguments
                }
            );
        }
        
        _debug = create_debug_manager(_handle);
        if (!_debug)
            cpp2dERROR("Debug messenger failed to create.");
        else
        {
            Utility::ArgumentList arguments;
            arguments.set(
                (VkInstance)_handle,
                (VkDebugUtilsMessengerEXT)_debug
            );
            
            _objects.push(
                GDIObjectInstance {
                    .type      = GDIObject::DebugMessenger,
                    .handle    = _debug,
                    .arguments = arguments
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

            switch (object.type)
            {
            case GDIObject::Instance:
                {
                    cpp2dINFO("Destroying vulkan instance.");
                    VkInstance instance = object.arguments.get<VkInstance>();
                    vkDestroyInstance(instance, nullptr);
                    break;
                }

            case GDIObject::DebugMessenger:
                {
                    VkInstance                instance = object.arguments.get<VkInstance>();
                    VkDebugUtilsMessengerEXT messenger = object.arguments.get<VkDebugUtilsMessengerEXT>();

                    cpp2dINFO("Destroying debug messenger.");
                    DestroyDebugUtilsMessengerEXT(instance, messenger, nullptr);
                    break;
                }

            case GDIObject::Device:
                {
                    cpp2dINFO("Destroying logic device.");
                    VkDevice device = object.arguments.get<VkDevice>();
                    vkDestroyDevice(device, nullptr);
                    break;
                }

            case GDIObject::Surface:
                {
                    cpp2dINFO("Destroying surface.");
                    VkInstance instance  = object.arguments.get<VkInstance>();
                    VkSurfaceKHR surface = object.arguments.get<VkSurfaceKHR>();

                    vkDestroySurfaceKHR(instance, surface, nullptr);
                    break;
                }

            case GDIObject::Swapchain:
                {
                    cpp2dINFO("Destroying swapchain.");
                    VkDevice          device = object.arguments.get<VkDevice>();
                    VkSwapchainKHR swapchain = object.arguments.get<VkSwapchainKHR>();

                    vkDestroySwapchainKHR(device, swapchain, nullptr);
                    break;
                }

            case GDIObject::ImageView:
                {
                    cpp2dINFO("Destroying image view.");
                    VkDevice device        = object.arguments.get<VkDevice>();
                    VkImageView image_view = object.arguments.get<VkImageView>();

                    vkDestroyImageView(device, image_view, nullptr);
                    break;
                }
            
            case GDIObject::Shader:
                {
                    cpp2dINFO("Destroying shader module.");
                    VkDevice device       = object.arguments.get<VkDevice>();
                    VkShaderModule shader = object.arguments.get<VkShaderModule>();

                    vkDestroyShaderModule(device, shader, nullptr);
                    break;
                }

            }

            object.arguments.free();
            _objects.pop();
        }
    }

    SwapChainInfo GDI::createSwapChain(const Surface* surface)
    {
        cpp2dINFO("Creating swapchain for surface.");
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
            cpp2dERROR("Error creating swapchain: %i", result);
            return SwapChainInfo {
                .handle = nullptr,
                .image_count = 0,
                .images = nullptr,
                .format = 0,
            };
        }

        Utility::ArgumentList arguments;
        arguments.set(
            (VkDevice)_device.handle,
            (VkSwapchainKHR)swapchain
        );

        _objects.push(
            GDIObjectInstance {
                .type      = GDIObject::Swapchain,
                .handle    = swapchain,
                .arguments = arguments
            }
        );

        SwapChainInfo info;
        info.handle = (SwapChainHandle)swapchain;
        vkGetSwapchainImagesKHR((VkDevice)_device.handle, swapchain, &info.image_count, nullptr);

        ScopedData<VkImage> images(info.image_count);
        ScopedData<VkImageView> image_views(info.image_count);
        vkGetSwapchainImagesKHR((VkDevice)_device.handle, swapchain, &info.image_count, images.ptr());

        info.images = (GDIImage*)std::malloc(sizeof(GDIImage) * info.image_count);
        for (U32 i = 0; i < info.image_count; i++)
        {
            // Call vkcreateimage view
            VkImageViewCreateInfo create_info
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = images[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = surface_format.format,
                .components = { 
                    VK_COMPONENT_SWIZZLE_IDENTITY,
                    VK_COMPONENT_SWIZZLE_IDENTITY,
                    VK_COMPONENT_SWIZZLE_IDENTITY,
                    VK_COMPONENT_SWIZZLE_IDENTITY
                },
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                }
            };

            VkResult result = vkCreateImageView((VkDevice)_device.handle, &create_info, nullptr, &image_views[i]);
            if (result != VK_SUCCESS)
            {
                cpp2dERROR("Image view (%u) creation failed.", i);
                continue;
            }

            // Pass the image view to the stack
            Utility::ArgumentList arguments;
            arguments.set(
                (VkDevice)_device.handle,
                image_views[i]
            );

            _objects.push(GDIObjectInstance{
                .type = GDIObject::ImageView,
                .handle = image_views[i],
                .arguments = arguments
            });

            info.images[i].image = images[i];
            info.images[i].image_view = image_views[i];
        }


        cpp2dINFO("Swapchain created successfully.");
        return info;
    }

    SurfaceHandle GDI::getSurfaceHandle(const Window* window)
    {
        cpp2dINFO("Creating surface.");
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface((VkInstance)_handle, (GLFWwindow*)window->getHandle(), nullptr, &surface) != VK_SUCCESS)
        {
            cpp2dERROR("GLFW failed to create window surface!");
            return nullptr;
        }

        Utility::ArgumentList arguments;
        arguments.set(
            (VkInstance)_handle,
            (VkSurfaceKHR)surface
        );
        
        _objects.push(
            GDIObjectInstance {
                .type      = GDIObject::Surface,
                .handle    = surface,
                .arguments = arguments
            }
        );

        cpp2dINFO("Surface created successfully.");

        _suitable_device_index = -1;
        for (U32 i = 0; i < _physical_devices.device_count; i++)
            if (is_suitable_device((VkPhysicalDevice)_physical_devices.handles[i], surface))
            {
                _suitable_device_index = i;
                break;
            }

        if (_suitable_device_index == -1)
            cpp2dWARN("No suitable physical device found for surface.");

        _device = create_logic_device(_handle, surface, (VkPhysicalDevice*)_physical_devices.handles, _suitable_device_index); 
        if (!_device.handle)
        {
            cpp2dFATAL("Failed to create logic device!");
            setState(GDIState::DeviceCreationFailed);
            return nullptr;
        }

        {
            Utility::ArgumentList arguments;
            arguments.set((VkDevice)_device.handle);

            _objects.push(
                GDIObjectInstance {
                    .type      = GDIObject::Device,
                    .handle    = _device.handle,
                    .arguments = arguments
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

    ShaderHandle GDI::createShader(const U32* data, U32 count)
    {
        assert(_device.handle);

        cpp2dINFO("Creating shader");
        VkShaderModuleCreateInfo create_info{
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = count * sizeof(U32),
            .pCode = data
        };
        
        VkShaderModule shader;
        VkResult result = vkCreateShaderModule((const VkDevice)_device.handle, &create_info, nullptr, &shader);
        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Error creating shader module %u", result);
            return nullptr;
        }

        Utility::ArgumentList arguments;
        arguments.set(
            (VkDevice)_device.handle,
            shader
        );

        _objects.push(GDIObjectInstance{
            .type = GDIObject::Shader,
            .handle = shader,
            .arguments = arguments
        });

        return shader;
    }

    PipelineHandle GDI::createPipeline(const ScopedData<Shader*>& shaders)
    {
        ScopedData<VkPipelineShaderStageCreateInfo> create_infos(shaders.size());
        for (U32 i = 0; i < create_infos.size(); i++)
        {
            VkShaderStageFlagBits type;
            switch (shaders[i]->getType())
            {
            case ShaderType::Vertex:    type = VK_SHADER_STAGE_VERTEX_BIT;      break;
            case ShaderType::Fragment:  type = VK_SHADER_STAGE_FRAGMENT_BIT;    break;
            }

            create_infos[i] = VkPipelineShaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = type,
                .module = static_cast<VkShaderModule>(shaders[i]->getHandle()),
                .pName = "main"
            };
        }
    }
}