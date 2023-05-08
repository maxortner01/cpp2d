#include <cpp2d/Graphics.h>

#ifdef GDI_VULKAN
#   include <vulkan/vulkan.h>
#endif

#include <vk_mem_alloc.h>

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
        clearStack();
    }

    U32 GDI::getPresentIndex(DrawWindow* surface)
    {
        QueueFamilyIndices indices = findQueueFamilies(
            static_cast<VkPhysicalDevice>(getSuitablePhysicalDevice()), 
            static_cast<VkSurfaceKHR>(surface->getSurfaceHandle())
        );

        return indices.present_index.value();
    }

    GDILogicDevice GDI::getLogicDevice()
    {
        return _device;
    }

    FramebufferHandle GDI::createFramebuffer(const ImageViewHandle& imageView, const Surface* surface, GDILifetime* lifetime)
    {
        if (!lifetime) lifetime = this;

        cpp2dINFO("Creating framebuffer.");

        const VkImageView images[] = {
            static_cast<VkImageView>(imageView)
        };

        VkFramebufferCreateInfo create_info {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = static_cast<VkRenderPass>(surface->getRenderPass()),
            .attachmentCount = 1,
            .pAttachments = images,
            .width = surface->getExtent().x,
            .height = surface->getExtent().y,
            .layers = 1
        };

        VkFramebuffer framebuffer;
        const VkDevice device = static_cast<VkDevice>(_device.handle);
        VkResult result = vkCreateFramebuffer(device, &create_info, nullptr, &framebuffer);
        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Framebuffer creation failed.");
            return nullptr;
        }
        else
        {
            Utility::ArgumentList arguments;
            arguments.set(device, framebuffer);

            lifetime->pushObject(GDIObjectInstance {
                .type = GDIObject::Framebuffer,
                .handle = static_cast<void*>(framebuffer),
                .arguments = arguments
            });
        }

        cpp2dINFO("Framebuffer created successfully.");
        return static_cast<FramebufferHandle>(framebuffer);
    }

    RenderPassHandle GDI::createRenderPass(const Surface* surface)
    {
        cpp2dINFO("Creating render pass.");

        VkAttachmentDescription color_attachment {
            .format = static_cast<VkFormat>(surface->getFormat()),
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        };

        VkAttachmentReference color_ref {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };

        VkSubpassDescription subpass {
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &color_ref
        };  

        VkSubpassDependency dependency {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = 0,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
        };

        VkRenderPassCreateInfo create_info {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = 1,
            .pAttachments = &color_attachment,
            .subpassCount = 1,
            .pSubpasses = &subpass,
            .dependencyCount = 1,
            .pDependencies = &dependency
        };

        const VkDevice device = static_cast<VkDevice>(_device.handle);
        VkRenderPass renderPass;
        VkResult result = vkCreateRenderPass(device, &create_info, nullptr, &renderPass);
        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Render pass failed to create.");
            return nullptr;
        }
        else
        {
            Utility::ArgumentList arguments;
            arguments.set(
                device, 
                renderPass
            );

            _objects.push(GDIObjectInstance {
                .type = GDIObject::Renderpass,
                .handle = renderPass,
                .arguments = arguments
            });
        }

        cpp2dINFO("Render pass created successfully.");
        return static_cast<RenderPassHandle>(renderPass);
    }

    SwapChainInfo GDI::createSwapChain(const DrawWindow* surface)
    {
        cpp2dINFO("Creating swapchain for surface.");

        const VkSurfaceKHR     _surface = static_cast<VkSurfaceKHR>(surface->getSurfaceHandle());
        const VkPhysicalDevice _suitable_device = static_cast<VkPhysicalDevice>(getSuitablePhysicalDevice());

        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_suitable_device, _surface, &capabilities);

        U32 format_count, present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(_suitable_device, _surface, &present_mode_count, nullptr);
        vkGetPhysicalDeviceSurfaceFormatsKHR(_suitable_device, _surface, &format_count, nullptr);

        ScopedData<VkSurfaceFormatKHR> formats(format_count);
        ScopedData<VkPresentModeKHR> present_modes(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(_suitable_device, _surface, &present_mode_count, present_modes.ptr());
        vkGetPhysicalDeviceSurfaceFormatsKHR(_suitable_device, _surface, &format_count, formats.ptr());

        VkSurfaceFormatKHR surface_format = choose_swapchain_format(formats.ptr(), formats.size());
        VkPresentModeKHR   present_mode   = choose_swapchain_present_mode(present_modes.ptr(), present_modes.size());
        VkExtent2D         extent         = choose_swapchain_extent(capabilities, surface);

        // Clamp the image count
        U32 image_count = capabilities.minImageCount + 1;
        if (image_count > capabilities.maxImageCount) image_count = capabilities.maxImageCount;

        const bool vsync = true;
        VkSwapchainCreateInfoKHR create_info {
            .sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface          = _surface,
            .minImageCount    = image_count,
            .imageFormat      = surface_format.format,
            .imageColorSpace  = surface_format.colorSpace,
            .imageExtent      = extent,
            .imageArrayLayers = 1,
            .imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .preTransform     = capabilities.currentTransform,
            .compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode      = (vsync?present_mode:VK_PRESENT_MODE_IMMEDIATE_KHR),
            .clipped          = VK_TRUE,
            .oldSwapchain     = VK_NULL_HANDLE
        };

        QueueFamilyIndices indices = findQueueFamilies(_suitable_device, _surface);
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
        const VkDevice device = static_cast<VkDevice>(_device.handle);
        VkResult result = vkCreateSwapchainKHR(device, &create_info, nullptr, &swapchain);
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
        arguments.set(device, swapchain);

        pushObject(
            GDIObjectInstance {
                .type      = GDIObject::Swapchain,
                .handle    = swapchain,
                .arguments = arguments
            }
        );

        SwapChainInfo info {
            .handle = static_cast<SwapChainHandle>(swapchain),
            .format = static_cast<FormatHandle>(surface_format.format)
        };
        vkGetSwapchainImagesKHR(device, swapchain, &info.image_count, nullptr);

        ScopedData<VkImage> images(info.image_count);
        ScopedData<VkImageView> image_views(info.image_count);
        vkGetSwapchainImagesKHR(device, swapchain, &info.image_count, images.ptr());

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
                device,
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
        const VkInstance instance = static_cast<VkInstance>(_handle);
        if (glfwCreateWindowSurface(instance, (GLFWwindow*)window->getHandle(), nullptr, &surface) != VK_SUCCESS)
        {
            cpp2dERROR("GLFW failed to create window surface!");
            return nullptr;
        }

        Utility::ArgumentList arguments;
        arguments.set(instance, surface);
        
        pushObject(
            GDIObjectInstance {
                .type      = GDIObject::Surface,
                .handle    = surface,
                .arguments = arguments
            }
        );

        cpp2dINFO("Surface created successfully.");

        _suitable_device_index = -1;
        for (U32 i = 0; i < _physical_devices.device_count; i++)
            if (is_suitable_device(static_cast<VkPhysicalDevice>(getPhysicalDevice(i)), surface))
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

            pushObject(
                GDIObjectInstance {
                    .type      = GDIObject::Device,
                    .handle    = _device.handle,
                    .arguments = arguments
                }
            );
        }

        VmaAllocator allocator;
        VmaAllocatorCreateInfo allocator_create {
            .physicalDevice = static_cast<VkPhysicalDevice>(getSuitablePhysicalDevice()),
            .device = static_cast<VkDevice>(_device.handle),
            .instance = static_cast<VkInstance>(_handle)
        };

        VkResult result = vmaCreateAllocator(&allocator_create, &allocator);
        if (result != VK_SUCCESS)
            cpp2dERROR("Error creating VMA allocator.");
        else
        {
            _allocator = static_cast<AllocatorHandle>(allocator);
            
            Utility::ArgumentList arguments;
            arguments.set(allocator);

            pushObject(GDIObjectInstance {
                .type = GDIObject::Allocator,
                .handle = allocator,
                .arguments = arguments
            });
        }

        return static_cast<SurfaceHandle>(surface);
    }

    GDIHandle GDI::getHandle() const
    {
        return _handle;
    }

    AllocatorHandle GDI::getAllocator(const DeviceHandle& handle) const
    {
        assert(_device.handle == handle);
        return _allocator;
    }

    GDILogicDevice GDI::getCurrentLogicDevice() const
    {
        return _device;
    }

    GDILogicDevice GDI::getLogicDevice(const DeviceHandle& handle) const
    {
        assert(_device.handle == handle);
        return _device;
    }

    GDIPhysicalDevice GDI::getSuitablePhysicalDevice() const
    {
        return _physical_devices.handles[_suitable_device_index];
    }

    GDIPhysicalDevice GDI::getPhysicalDevice(CU32& index) const
    {
        assert(index < _physical_devices.device_count);
        return _physical_devices.handles[index];
    }

    QueueIndices GDI::getQueueIndices(const GDIPhysicalDevice& physicalDevice) const
    {
        QueueFamilyIndices indices = findQueueFamilies(
            static_cast<VkPhysicalDevice>(physicalDevice),
            nullptr
        );

        return QueueIndices{
            .graphics = indices.graphics_index.value(),
            .present = 0
        };
    }

    QueueIndices GDI::getQueueIndices(const GDIPhysicalDevice& physicalDevice, const DrawWindow& window) const
    {
        QueueFamilyIndices indices = findQueueFamilies(
            static_cast<VkPhysicalDevice>(physicalDevice),
            static_cast<VkSurfaceKHR>(window.getSurfaceHandle())
        );

        return QueueIndices{
            .graphics = indices.graphics_index.value(),
            .present = indices.present_index.value()
        };
    }

    U32 GDI::getNextImage(const SwapChainHandle& swapchain, const Frame& frame)
    {
        U32 image_index;
        VkFence        fence      = static_cast<VkFence>(frame.sync_objects.in_flight);
        VkSemaphore    semaphore  = static_cast<VkSemaphore>(frame.sync_objects.image_avaliable);
        VkSwapchainKHR _swapchain = static_cast<VkSwapchainKHR>(swapchain);
        const VkDevice device     = static_cast<VkDevice>(frame.command_pool.device);
        vkAcquireNextImageKHR(device, _swapchain, 10000000, semaphore, VK_NULL_HANDLE, &image_index);

        return image_index;
    }

    SemaphoreHandle GDI::createSemaphore(GDILifetime* lifetime)
    {
        if (!lifetime) lifetime = this;

        VkSemaphoreCreateInfo create_info {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };

        VkSemaphore semaphore;
        const VkDevice device = static_cast<VkDevice>(_device.handle);
        VkResult result = vkCreateSemaphore(device, &create_info, nullptr, &semaphore);
        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Semaphore creation failed.");
            return nullptr;
        }
        else
        {
            Utility::ArgumentList arguments;
            arguments.set(device, semaphore);

            lifetime->pushObject(GDIObjectInstance {
                .type = GDIObject::Semaphore,
                .handle = semaphore,
                .arguments = arguments
            });
        }

        return static_cast<SemaphoreHandle>(semaphore);
    }

    FenceHandle GDI::createFence(GDILifetime* lifetime)
    {
        if (!lifetime) lifetime = this;

        VkFenceCreateInfo create_info {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT
        };

        VkFence fence;
        const VkDevice device = static_cast<VkDevice>(_device.handle);
        VkResult result = vkCreateFence(device, &create_info, nullptr, &fence);
        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Fence creation failed.");
            return nullptr;
        }
        else
        {
            Utility::ArgumentList arguments;
            arguments.set(device, fence);

            lifetime->pushObject(GDIObjectInstance {
                .type = GDIObject::Fence,
                .handle = fence,
                .arguments = arguments
            });
        }

        return static_cast<FenceHandle>(fence);
    }

    ShaderHandle GDI::createShader(const U32* data, U32 count, GDILifetime* lifetime)
    {
        if (!lifetime) lifetime = this;

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

        /*
        this->pushObject(GDIObjectInstance{
            .type = GDIObject::Shader,
            .handle = shader,
            .arguments = arguments
        });*/

        return shader;
    }

    GDIPipeline GDI::createPipeline(const ScopedData<Shader*>& shaders, Surface* surface, const AttributeFrame& frame)
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

        const VkDynamicState dynamic_states[] = {
            VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo create_info {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = sizeof(dynamic_states) / sizeof(VkDynamicState),
            .pDynamicStates = dynamic_states
        };

        ScopedData<VkVertexInputBindingDescription>   bindings(frame.bindings.size());
        ScopedData<VkVertexInputAttributeDescription> attributes(frame.attributes.size());

        // Convert the bindings data
        for (U32 i = 0; i < bindings.size(); i++)
        {
            bindings[i] = VkVertexInputBindingDescription {
                .binding = frame.bindings[i].index,
                .stride  = frame.bindings[i].stride,
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
            };
        }

        // Convert the attributes data
        for (U32 i = 0; i < attributes.size(); i++)
        {
            VkFormat format = VK_FORMAT_UNDEFINED;
            switch (frame.attributes[i].element_count)
            {
            case 1: format = VK_FORMAT_R32_SFLOAT;          break;
            case 2: format = VK_FORMAT_R32G32_SFLOAT;       break;
            case 3: format = VK_FORMAT_R32G32B32_SFLOAT;    break;
            case 4: format = VK_FORMAT_R32G32B32A32_SFLOAT; break;
            default: cpp2dERROR("Format not supported");    break;
            }
            if (format == VK_FORMAT_UNDEFINED) break;

            attributes[i] = VkVertexInputAttributeDescription {
                .binding = frame.attributes[i].binding,
                .location = frame.attributes[i].location,
                .offset = frame.attributes[i].offset,
                .format = format,
            };
        }

        VkPipelineVertexInputStateCreateInfo vertex_create_info {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = bindings.size(),
            .pVertexBindingDescriptions = bindings.ptr(),
            .vertexAttributeDescriptionCount = attributes.size(),
            .pVertexAttributeDescriptions = attributes.ptr()
        };

        VkPipelineInputAssemblyStateCreateInfo input_assembly {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE
        };

        const U32 width  = static_cast<R32>(surface->getExtent().x);
        const U32 height = static_cast<R32>(surface->getExtent().y);

        VkViewport viewport {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(width),
            .height = static_cast<float>(height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };

        VkRect2D scissor {
            .offset = { 0, 0 },
            .extent = {
                .width  = width,
                .height = height
            }
        };

        VkPipelineViewportStateCreateInfo viewport_state {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .scissorCount = 1,
            .pViewports = &viewport,
            .pScissors = &scissor
        };

        VkPipelineRasterizationStateCreateInfo rasterizer {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .lineWidth = 1.f,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f
        };

        VkPipelineMultisampleStateCreateInfo multisampling {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .sampleShadingEnable = VK_FALSE,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .minSampleShading = 1.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE
        };

        VkPipelineColorBlendAttachmentState color_blend_attachment {
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
            .blendEnable = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD
        };

        VkPipelineColorBlendStateCreateInfo color_blending {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &color_blend_attachment,
            .blendConstants = {
                0.f, 0.f, 0.f, 0.f
            }
        };

        VkPipelineLayoutCreateInfo pipeline_layout_info {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr
        };

        const VkDevice device = static_cast<VkDevice>(_device.handle);
        VkPipelineLayout pipeline_layout;
        VkResult result = vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &pipeline_layout);
        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Pipeline layout creation failed.");
            return GDIPipeline {
                .handle = nullptr,
                .layout = nullptr
            };
        }
        else
        {
            Utility::ArgumentList arguments;
            arguments.set(
                device, 
                pipeline_layout
            );

            surface->pushObject(GDIObjectInstance {
                .type = GDIObject::PipelineLayout,
                .handle = pipeline_layout,
                .arguments = arguments
            });
        }

        VkGraphicsPipelineCreateInfo pipeline_create_info {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = create_infos.size(),
            .pStages = create_infos.ptr(),
            .pVertexInputState = &vertex_create_info,
            .pInputAssemblyState = &input_assembly,
            .pViewportState = &viewport_state,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &color_blending,
            .pDynamicState = &create_info,
            .layout = pipeline_layout,
            .renderPass = static_cast<VkRenderPass>(surface->getRenderPass()),
            .subpass = 0,
            .basePipelineHandle = nullptr,
            .basePipelineIndex = -1
        };

        VkPipeline pipeline;
        result = vkCreateGraphicsPipelines(device, nullptr, 1, &pipeline_create_info, nullptr, &pipeline);
        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Graphics pipeline failed to create.");
            return GDIPipeline {
                .handle = nullptr,
                .layout = static_cast<PipelineLayoutHandle>(pipeline_layout)
            };
        }
        else
        {
            Utility::ArgumentList arguments;
            arguments.set(
                device, 
                pipeline
            );

            surface->pushObject(GDIObjectInstance {
                .type = GDIObject::GraphicsPipeline,
                .handle = static_cast<void*>(pipeline),
                .arguments = arguments
            });
        }

        for (U32 i = 0; i < shaders.size(); i++)
        {
            auto module = static_cast<VkShaderModule>(shaders[i]->getHandle());
            vkDestroyShaderModule(device, module, nullptr);
        }

        cpp2dINFO("Graphics pipeline created successfully.");
        return GDIPipeline {
            .handle = static_cast<PipelineHandle>(pipeline),
            .layout = static_cast<PipelineLayoutHandle>(pipeline_layout)
        };
    }
    
    CommandPool GDI::createCommandPool(GDILifetime* lifetime)
    {
        if (!lifetime) lifetime = this;

        const VkPhysicalDevice physical_device = static_cast<VkPhysicalDevice>(getPhysicalDevice(_device.physical_device_index));
        QueueFamilyIndices indices = findQueueFamilies(physical_device, VK_NULL_HANDLE);

        VkCommandPoolCreateInfo create_info {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = indices.graphics_index.value()
        };

        VkCommandPool command_pool;
        const VkDevice device = static_cast<VkDevice>(_device.handle);
        VkResult result = vkCreateCommandPool(device, &create_info, nullptr, &command_pool);
        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Command pool creation failed.");
            return { nullptr, nullptr };
        }
        else
        {
            Utility::ArgumentList arguments;
            arguments.set(device, command_pool);

            lifetime->pushObject(GDIObjectInstance {
                .type = GDIObject::CommandPool,
                .handle = command_pool,
                .arguments = arguments
            });
        }

        cpp2dINFO("Command pool created successfully");
        return CommandPool {
            .handle = static_cast<CommandPoolHandle>(command_pool),
            .device = static_cast<DeviceHandle>(device)
        };
    }

    CommandBufferHandle GDI::createCommandBuffer(const CommandPoolHandle& commandPool)
    {
        VkCommandBufferAllocateInfo alloc_info {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = static_cast<VkCommandPool>(commandPool),
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        VkCommandBuffer command_buffer;
        const VkDevice device = static_cast<VkDevice>(_device.handle);
        VkResult result = vkAllocateCommandBuffers(device, &alloc_info, &command_buffer);
        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Error allocating command buffers.");
            return nullptr;
        }

        cpp2dINFO("Command buffer created successfully.");
        return static_cast<CommandBufferHandle>(command_buffer);
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
}