#include <cpp2d/Graphics.h>

#ifdef GDI_VULKAN
#   include <vulkan/vulkan.h>
#   include <vk_mem_alloc.h>
#endif

namespace cpp2d::Graphics
{
    GDILifetime::~GDILifetime()
    {
        clearStack();
    }

    void GDILifetime::pushObject(const GDIObjectInstance&& object)
    {
        _objects.push(object);
    }

#ifdef GDI_VULKAN
    void GDILifetime::clearStack()
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

            case GDIObject::Semaphore:
                {
                    cpp2dINFO("Destroying semaphore.");
                    VkDevice    device       = object.arguments.get<VkDevice>();
                    VkSemaphore command_pool = object.arguments.get<VkSemaphore>();

                    vkDestroySemaphore(device, command_pool, nullptr);
                    break;
                }

            case GDIObject::Fence:
                {
                    cpp2dINFO("Destroying fence.");
                    VkDevice device       = object.arguments.get<VkDevice>();
                    VkFence  command_pool = object.arguments.get<VkFence>();

                    vkDestroyFence(device, command_pool, nullptr);
                    break;
                }

            case GDIObject::DestroyAllocators:
                {
                    cpp2dINFO("Destroying allocators.");
                    auto* gdi = object.arguments.get<Graphics::GDI*>();

                    gdi->clearAllocations();
                }

            /*
            case GDIObject::Allocator:
                {
                    cpp2dINFO("Destroying allocator.");
                    VmaAllocator allocator = object.arguments.get<VmaAllocator>();

                    vmaDestroyAllocator(allocator);
                    break;
                }*/

            }

            object.arguments.free();
            _objects.pop();
        }
    }
#else
    void GDILifetime::clearStack()
    {   }
#endif
}