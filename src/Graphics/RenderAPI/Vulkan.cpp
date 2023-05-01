#include <vulkan/vulkan.h>

#ifdef DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    switch (messageSeverity)
    {
    default: break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        INFO("%s", pCallbackData->pMessage); break;
    
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        WARN("%s", pCallbackData->pMessage); break;
    
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        ERROR("%s", pCallbackData->pMessage); break;
    }

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) 
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

static const char* validation_layers[] = {
    "VK_LAYER_KHRONOS_validation"
};

bool validation_layers_supported()
{
    U32 layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    ScopedData<VkLayerProperties> props(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, props.ptr());

    for (const auto& layer : validation_layers)
    {
        bool layer_found = false;

        for (U32 i = 0; i < layer_count; i++)
            if (!strcmp(props[i].layerName, layer))
            {  layer_found = true; break;  }

        if (!layer_found) return false;
    }

    return true;
}

GDIDebugHandle create_debug_manager(GDIHandle handle)
{
    INFO("Creating debug messenger.");

    VkDebugUtilsMessengerCreateInfoEXT createInfo 
    {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback,
        .pUserData = nullptr,
    };

    VkDebugUtilsMessengerEXT messenger;
    VkResult result = CreateDebugUtilsMessengerEXT((VkInstance)handle, &createInfo, nullptr, &messenger);
    if (result != VK_SUCCESS)
    {
        ERROR("Debug messenger failed to create.");
        return nullptr;
    }

    INFO("Debug messenger created successfully.");
    return (GDIDebugHandle)messenger;
}

#endif

// Here we store the indices of each of the queues we need
// for various purposes 
struct QueueFamilyIndices
{
    std::optional<U32> graphics_index;
    std::optional<U32> present_index;
};

// Finds the indices of the queue families given a physical device
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    QueueFamilyIndices indices;

    U32 queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    ScopedData<VkQueueFamilyProperties> props(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, props.ptr());

    for (U32 i = 0; i < props.size(); i++)
    {
        if (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphics_index = i; 
        
        VkBool32 present_support;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
        if (present_support) indices.present_index = i;
    }

    return indices;
}

const char* required_extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct SwapChainSupport
{
    U32 format_count;
    U32 present_mode_count;
};  

// Determine whether or not a device is suitable
bool is_suitable_device(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    QueueFamilyIndices indices = findQueueFamilies(device, surface);

    // Check whether or not the physical device supports all the required extensions
    U32 extension_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

    ScopedData<VkExtensionProperties> props(extension_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, props.ptr());

    for (U32 i = 0; i < sizeof(required_extensions) / sizeof(void*); i++)
    {
        bool found = false;
        
        for (U32 j = 0; j < props.size(); j++)
            if (strcmp(props[j].extensionName, required_extensions[i]) == 0)
            {
                found = true;
                break;
            }

        if (!found) 
        {
            ERROR("Extension '%s' not supported.", required_extensions[i]);
            return false;
        }
    }
    
    // make sure there are format and present modes
    SwapChainSupport support;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &support.present_mode_count, nullptr);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &support.format_count, nullptr);

    return (support.format_count > 0 && support.present_mode_count > 0) && (indices.graphics_index.has_value() && indices.present_index.has_value());
}

// Find and pick out the physical device that satisfies all our requirements
GDILogicDevice create_logic_device(GDIHandle handle, VkSurfaceKHR surface, VkPhysicalDevice* devices, I32 suitableDeviceIndex)
{
    INFO("Creating logic device.");
    VkInstance instance = (VkInstance)handle;

    // Now we grab the extensions needed for the device
    U32 present_index = 0;

#ifdef __APPLE__
    // Apple devices need an extra extension
    U32 extra_index = 1;
#else
    U32 extra_index = 0;
#endif

    if (suitableDeviceIndex < 0)
    {
        FATAL("No suitable devices found for logic device creation.");
        return GDILogicDevice{
            .handle = nullptr,
            .physical_device_index = -1
        };
    }

    VkPhysicalDevice suitable_device = devices[suitableDeviceIndex];

    CU32 required_count = sizeof(required_extensions) / sizeof(const char*);
    CU32 total_extensions = required_count + extra_index;
    ScopedData<const char*> device_extensions(total_extensions);
    std::memcpy(device_extensions.ptr(), required_extensions, sizeof(required_extensions));

#ifdef __APPLE__
    // Load in the extra extension needed for apple devices
    device_extensions[required_count + present_index++] = "VK_KHR_portability_subset";
#endif

    QueueFamilyIndices indices = findQueueFamilies(suitable_device, surface);

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo
    {
        .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = indices.graphics_index.value(),
        .queueCount       = 1,
        .pQueuePriorities = &queuePriority,
    };

    VkPhysicalDeviceFeatures features{};
    VkDeviceCreateInfo createInfo
    {
        .sType                  = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount   = 1,
        .pQueueCreateInfos      = &queueCreateInfo,
#   ifdef DEBUG
        .enabledLayerCount = sizeof(validation_layers) / sizeof(const char*),
        .ppEnabledLayerNames = validation_layers,
#   else
        .enabledLayerCount = 0
#   endif
        .enabledExtensionCount = device_extensions.size(),
        .ppEnabledExtensionNames = device_extensions.ptr(),
        .pEnabledFeatures       = &features,
    };

    VkDevice _device;
    VkResult result = vkCreateDevice(suitable_device, &createInfo, nullptr, &_device);
    if (result != VK_SUCCESS)
    {
        FATAL("Failed to create device!");
        return GDILogicDevice{
            .handle = nullptr,
            .physical_device_index = -1
        };
    }


    INFO("Successfully created logic device.");
    return GDILogicDevice {
        .handle = (GDIDeviceHandle)_device,
        .physical_device_index = suitableDeviceIndex
    };
}

// Creates a vulkan instance
GDIHandle create_instance()
{
    INFO("Creating vulkan instance.");

    // Get the extensions needed for glfw
    
    // for iterating through the extra extensions
    U32 present_index = 0;
    U32 glfw_extension_count = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

#ifdef __APPLE__
    U32 extra_indices = 2;
#else
    U32 extra_indices = 0;
#endif

#ifdef DEBUG
    if (!validation_layers_supported())
    {
        FATAL("In debug mode, but no validation layers are supported.");
        return nullptr;
    }

    extra_indices += 1;
#endif

    ScopedData<const char*> extensions(glfw_extension_count + extra_indices);
    std::memcpy(extensions.ptr(), glfwExtensions, sizeof(const char*) * glfw_extension_count);

#ifdef __APPLE__
        extensions[glfw_extension_count + present_index++] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
        extensions[glfw_extension_count + present_index++] = VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME;
#endif

#ifdef DEBUG
        extensions[glfw_extension_count + present_index++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
#endif

    VkApplicationInfo appInfo
    {
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName   = "cpp2d app",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName        = "app2d",
        .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion         = VK_API_VERSION_1_0,
    };

    VkInstanceCreateInfo createInfo
    {
        .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo        = &appInfo,
        .enabledLayerCount       = 0,
        .enabledExtensionCount   = extensions.size(),
        .ppEnabledExtensionNames = extensions.ptr(),
    };

#ifdef __APPLE__
    // Because apple is special
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

#ifdef DEBUG
    CU32 validation_layer_count = sizeof(validation_layers) / sizeof(const char*);
    INFO("Enabling %u validation layer(s).", validation_layer_count);
    createInfo.enabledLayerCount   = validation_layer_count;
    createInfo.ppEnabledLayerNames = validation_layers;
#endif

    VkInstance instance;
    VkResult   result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) 
    {
        FATAL("vkCreateInstance returned %i", result);
        return nullptr;
    }

    INFO("Vulkan instance created successfully.");
    return (GDIHandle)instance;
}
