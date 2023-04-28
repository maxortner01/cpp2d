#include <cpp2d/Graphics.h>


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VkBootstrap.h>

namespace cpp2d::Graphics
{
    GDI::GDI() :
        Utility::State<GDIState>(GDIState::NotInitialized)
    {   }

    GDI::~GDI()
    {
        if (getState() == GDIState::Initialized)
        {
            vkDestroyDevice((VkDevice)_instance.logic_device, nullptr);

            vkb::destroy_debug_utils_messenger((VkInstance)_instance.handle, (VkDebugUtilsMessengerEXT)_instance.debug_messenger);

            vkDestroyInstance((VkInstance)_instance.handle, nullptr);

            setState(GDIState::Destroyed);
        }
    }

    const InstanceData& GDI::getInstanceData() const
    {  return _instance;  }

    // vkguides rules
    void GDI::init(const InitGDIData& data) 
    {
        if (getState() != GDIState::NotInitialized) return;

        // Initialize the builder
        vkb::InstanceBuilder builder;
        auto inst_ret = builder.set_app_name("cpp2d-app")
            .request_validation_layers()
            .use_default_debug_messenger()
            .build();

        if (!inst_ret) {
            std::cerr << "Failed to create Vulkan instance. Error: " << inst_ret.error().message() << "\n";
            return;
        }


        // for some reason, setting _instance.handle is seg faulting...
        // Grab basic instance handle
        vkb::Instance vkb_inst = inst_ret.value();
        _instance.handle = vkb_inst.instance;
        _instance.debug_messenger = (void*)vkb_inst.debug_messenger;

        // Initialize the glfw surface with Vulkan
        VkSurfaceKHR _surface;
        VkResult result = glfwCreateWindowSurface((VkInstance)_instance.handle, (GLFWwindow*)data.window->_window, NULL, &_surface);
        if (result != VK_SUCCESS) { setState(GDIState::SurfaceFailed); std::cerr << "Surface failed!\n"; return; }
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

        // build 

        // Skeptical about these rvalue type conversions... need to check that this is legit 
        // once the system is up and running
        data.window->_swap_chain = vkb_swapchain.swapchain;
        data.window->_image_format = vkb_swapchain.image_format;

        //data.window->_images.image_count = vkb_swapchain.image_count;
        //data.window->_images.images = (gdiImage*)&(*vkb_swapchain.get_images().value().begin());
        //data.window->_images.image_views = (gdiImageView*)&(*vkb_swapchain.get_image_views().value().begin());

        data.window->_images = (std::vector<gdiImage>&)vkb_swapchain.get_images().value();
        data.window->_image_views = (std::vector<gdiImageView>&)vkb_swapchain.get_image_views().value();

        std::cout << "here\n";

        //setState(GDIState::Initialized);

        /*
        vkb::InstanceBuilder builder;
        auto inst_ret = builder.set_app_name ("Example Vulkan Application")
                            .request_validation_layers ()
                            .use_default_debug_messenger ()
                            .build ();
        if (!inst_ret) {
            std::cerr << "Failed to create Vulkan instance. Error: " << inst_ret.error().message() << "\n";
            return;
        }
        vkb::Instance vkb_inst = inst_ret.value ();

        VkSurfaceKHR _surface;
        VkResult result = glfwCreateWindowSurface((VkInstance)vkb_inst.instance, (GLFWwindow*)data.window->_window, NULL, &_surface);
        if (result != VK_SUCCESS) { setState(GDIState::SurfaceFailed); std::cerr << "Surface failed!\n"; return; }
        data.window->_surface = _surface;

        vkb::PhysicalDeviceSelector selector{ vkb_inst };
        auto phys_ret = selector.set_surface (_surface)
                            .set_minimum_version (1, 1) // require a vulkan 1.1 capable device
                            //.require_dedicated_transfer_queue ()
                            .select ();
        if (!phys_ret) {
            std::cerr << "Failed to select Vulkan Physical Device. Error: " << phys_ret.error().message() << "\n";
            return;
        }

        vkb::DeviceBuilder device_builder{ phys_ret.value () };
        // automatically propagate needed data from instance & physical device
        auto dev_ret = device_builder.build ();
        if (!dev_ret) {
            std::cerr << "Failed to create Vulkan device. Error: " << dev_ret.error().message() << "\n";
            return;
        }
        vkb::Device vkb_device = dev_ret.value ();

        // Get the VkDevice handle used in the rest of a vulkan application
        VkDevice device = vkb_device.device;

        // Get the graphics queue with a helper function
        auto graphics_queue_ret = vkb_device.get_queue (vkb::QueueType::graphics);
        if (!graphics_queue_ret) {
            std::cerr << "Failed to get graphics queue. Error: " << graphics_queue_ret.error().message() << "\n";
            return;
        }
        VkQueue graphics_queue = graphics_queue_ret.value ();*/
    }

    /*
    void GDI::destroy()
    {
        if (getState() == GDIState::Initialized)
        {
            vkDestroyDevice((VkDevice)_instance.logic_device, nullptr);

            vkb::destroy_debug_utils_messenger((VkInstance)_instance.handle, (VkDebugUtilsMessengerEXT)_instance.debug_messenger);

            vkDestroyInstance((VkInstance)_instance.handle, nullptr);

            setState(GDIState::Destroyed);
        }
    }*/
}