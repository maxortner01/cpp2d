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

            case GDIObject::PipelineLayout:
                {
                    cpp2dINFO("Destroying pipeline layout.");
                    VkDevice device         = object.arguments.get<VkDevice>();
                    VkPipelineLayout layout = object.arguments.get<VkPipelineLayout>();

                    vkDestroyPipelineLayout(device, layout, nullptr);
                    break;
                }

            case GDIObject::Renderpass:
                {
                    cpp2dINFO("Destroying render pass.");
                    VkDevice device         = object.arguments.get<VkDevice>();
                    VkRenderPass renderpass = object.arguments.get<VkRenderPass>();

                    vkDestroyRenderPass(device, renderpass, nullptr);
                    break;
                }
            
            case GDIObject::GraphicsPipeline:
                {
                    cpp2dINFO("Destroying graphics pipeling");
                    VkDevice device     = object.arguments.get<VkDevice>();
                    VkPipeline pipeline = object.arguments.get<VkPipeline>();

                    vkDestroyPipeline(device, pipeline, nullptr);
                    break;
                }

            case GDIObject::Framebuffer:
                {
                    cpp2dINFO("Destroying framebuffer.");
                    VkDevice      device      = object.arguments.get<VkDevice>();
                    VkFramebuffer framebuffer = object.arguments.get<VkFramebuffer>();

                    vkDestroyFramebuffer(device, framebuffer, nullptr);
                    break;
                }

            case GDIObject::CommandPool:
                {
                    cpp2dINFO("Destroying command pool.");
                    VkDevice      device       = object.arguments.get<VkDevice>();
                    VkCommandPool command_pool = object.arguments.get<VkCommandPool>();

                    vkDestroyCommandPool(device, command_pool, nullptr);
                    break;
                }

            }

            object.arguments.free();
            _objects.pop();
        }
    }

    FramebufferHandle GDI::createFramebuffer(const ImageViewHandle& imageView, const Surface* surface)
    {
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

            _objects.push(GDIObjectInstance {
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

        VkRenderPassCreateInfo create_info {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = 1,
            .pAttachments = &color_attachment,
            .subpassCount = 1,
            .pSubpasses = &subpass
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

        const VkDevice device = static_cast<VkDevice>(_device.handle);
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

    GDIPipeline GDI::createPipeline(const ScopedData<Shader*>& shaders, const Surface* surface)
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

        VkPipelineVertexInputStateCreateInfo vertex_create_info {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = 0,
            .pVertexBindingDescriptions = nullptr,
            .vertexAttributeDescriptionCount = 0,
            .pVertexAttributeDescriptions = nullptr
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

            _objects.push(GDIObjectInstance {
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

            _objects.push(GDIObjectInstance {
                .type = GDIObject::GraphicsPipeline,
                .handle = static_cast<void*>(pipeline),
                .arguments = arguments
            });
        }

        cpp2dINFO("Graphics pipeline created successfully.");
        return GDIPipeline {
            .handle = static_cast<PipelineHandle>(pipeline),
            .layout = static_cast<PipelineLayoutHandle>(pipeline_layout)
        };
    }
    
    CommandPoolHandle GDI::createCommandPool(const Surface* surface)
    {
        const VkPhysicalDevice physical_device = static_cast<VkPhysicalDevice>(_physical_devices[_device.physical_device_index]);
        QueueFamilyIndices indices = findQueueFamilies(physical_device, static_cast<VkSurfaceKHR>(surface->getHandle()));

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
            return nullptr;
        }
        else
        {
            Utility::ArgumentList arguments;
            arguments.set(device, command_pool);

            _objects.push(GDIObjectInstance {
                .type = GDIObject::CommandPool,
                .handle = command_pool,
                .arguments = arguments
            });
        }

        cpp2dINFO("Command pool created successfully");
        return static_cast<CommandPoolHandle>(command_pool);
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
        VkResult result = vkAllocateCommandBuffers(device, nullptr, &command_buffer);
        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Error allocating command buffers.");
            return nullptr;
        }

        return static_cast<CommandBufferHandle>(command_buffer);
    }
}