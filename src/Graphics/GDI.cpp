#include <cpp2d/Graphics.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VkBootstrap.h>

namespace cpp2d::Graphics
{
    GDI::GDI() :
        Utility::State<GDIState>(GDIState::NotInitialized)
    {   }

    const InstanceData& GDI::getInstanceData() const
    {  return _instance;  }

    // vkguides rules
    void GDI::init(const InitGDIData& data) 
    {
        if (getState() != GDIState::NotInitialized) return;

        // Initialize the builder
        vkb::InstanceBuilder builder;
        auto inst_ret = builder.set_app_name("cpp2d-app")
            .request_validation_layers(true)
            .require_api_version(1, 1, 0)
            .use_default_debug_messenger()
            .build();

        // Grab basic instance handle
        vkb::Instance vkb_inst = inst_ret.value();
        _instance.handle = (void*)vkb_inst.instance;
        _instance.debug_messenger = (void*)vkb_inst.debug_messenger;

        // Initialize the glfw surface with Vulkan
        VkSurfaceKHR _surface;
        VkResult result = glfwCreateWindowSurface((VkInstance)_instance.handle, (GLFWwindow*)data.window->_window, NULL, &_surface);
        if (result != VK_SUCCESS) { setState(GDIState::SurfaceFailed); return; }
        data.window->_surface = _surface;

        // Grab the physical device
        vkb::PhysicalDeviceSelector selector{vkb_inst};
        vkb::PhysicalDevice vkb_physical = selector
            .set_minimum_version(1, 1)
            .set_surface(_surface)
            .select()
            .value();

        // Create the logic device
        vkb::DeviceBuilder device_builder{ vkb_physical };
        vkb::Device vkb_device = device_builder.build().value();

        _instance.logic_device    = vkb_device.device;
        _instance.physical_device = vkb_physical.physical_device;

        // Now we make the swap chain for the given window
        vkb::SwapchainBuilder swapchain_builder{ (VkPhysicalDevice)_instance.physical_device, (VkDevice)_instance.logic_device, (VkSurfaceKHR)data.window->_surface };
        vkb::Swapchain vkb_swapchain = swapchain_builder
            .use_default_format_selection()
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_extent(data.window->getSize().x, data.window->getSize().y)
            .build()
            .value();

        // Skeptical about these rvalue type conversions... need to check that this is legit 
        // once the system is up and running
        data.window->_swap_chain = vkb_swapchain.swapchain;
        data.window->_image_format = vkb_swapchain.image_format;
        //data.window->_images = (std::vector<gdiImage>&&)vkb_swapchain.get_images().value();
        //data.window->_image_views = (std::vector<gdiImageView>&&)vkb_swapchain.get_image_views().value();

        setState(GDIState::Initialized);
    }

    void GDI::destroy()
    {
        if (getState() == GDIState::Initialized)
        {
            vkDestroyDevice((VkDevice)_instance.logic_device, nullptr);

            vkb::destroy_debug_utils_messenger((VkInstance)_instance.handle, (VkDebugUtilsMessengerEXT)_instance.debug_messenger);

            vkDestroyInstance((VkInstance)_instance.handle, nullptr);

            setState(GDIState::Destroyed);
        }
    }
}