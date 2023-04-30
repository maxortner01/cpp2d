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

        _device = create_logic_device(_handle); 
        if (!_device)
        {
            FATAL("Failed to create logic device!");
            setState(GDIState::DeviceCreationFailed);
            return;
        }

        {
            void** arguments = (void**)std::malloc(sizeof(void*));
            arguments[0] = _device;
            _objects.emplace(
                GDIObjectInstance {
                    .type           = GDIObject::Device,
                    .handle         = _device,
                    .argument_count = 1,
                    .arguments      = arguments
                }
            );
        }

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
    }

    GDIHandle GDI::getHandle() const
    {
        return _handle;
    }
}