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
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .pfnUserCallback = debugCallback,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pUserData = nullptr,
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT
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
};

// Finds the indices of the queue families given a physical device
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    U32 queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    ScopedData<VkQueueFamilyProperties> props(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, props.ptr());

    for (U32 i = 0; i < props.size(); i++)
        if (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphics_index = i; 

    return indices;
}

// Determine whether or not a device is suitable
bool is_suitable_device(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = findQueueFamilies(device);

    return indices.graphics_index.has_value();
}

const char* required_extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Find and pick out the physical device that satisfies all our requirements
GDILogicDevice create_logic_device(GDIHandle handle)
{
    INFO("Creating logic device.");
    VkInstance instance = (VkInstance)handle;

    U32 device_count;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

    if (!device_count) return nullptr;

    ScopedData<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.ptr());

    VkPhysicalDevice device = VK_NULL_HANDLE;
    for (U32 i = 0; i < devices.size(); i++)
        if (is_suitable_device(devices[i]))
        {
            device = devices[i];
            break;
        }

    if (device == VK_NULL_HANDLE) 
    {
        FATAL("No suitable physical devices found.");
        return nullptr;
    }

    // Now we grab the extensions needed for the device
    U32 present_index = 0;

#ifdef __APPLE__
    U32 extra_index = 1;
#else
    U32 extra_index = 0;
#endif

    CU32 required_count = sizeof(required_extensions) / sizeof(const char*);
    CU32 total_extensions = required_count + extra_index;
    ScopedData<const char*> device_extensions(total_extensions);
    std::memcpy(device_extensions.ptr(), required_extensions, sizeof(required_extensions));

#ifdef __APPLE__
    device_extensions[required_count + present_index++] = "VK_KHR_portability_subset";
#endif

    QueueFamilyIndices indices = findQueueFamilies(device);

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo
    {
        .queueFamilyIndex = indices.graphics_index.value(),
        .pQueuePriorities = &queuePriority,
        .queueCount       = 1,
        .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO
    };

    VkPhysicalDeviceFeatures features{};
    VkDeviceCreateInfo createInfo
    {
        .ppEnabledExtensionNames = device_extensions.ptr(),
        .enabledExtensionCount  = device_extensions.size(),
        .queueCreateInfoCount   = 1,
        .pQueueCreateInfos      = &queueCreateInfo,
        .pEnabledFeatures       = &features,
        .sType                  = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
#   ifdef DEBUG
        .ppEnabledLayerNames = validation_layers,
        .enabledLayerCount   = sizeof(validation_layers) / sizeof(const char*)
#   else
        .enabledLayerCount = 0
#   endif
    };

    VkDevice _device;
    VkResult result = vkCreateDevice(device, &createInfo, nullptr, &_device);
    if (result != VK_SUCCESS)
    {
        FATAL("Failed to create device!");
        return nullptr;
    }


    INFO("Successfully created logic device.");
    return (GDILogicDevice)_device;
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
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pApplicationName   = "cpp2d app",
        .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName        = "app2d",
        .apiVersion         = VK_API_VERSION_1_0,
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO
    };

    VkInstanceCreateInfo createInfo
    {
        .ppEnabledExtensionNames = extensions.ptr(),
        .enabledExtensionCount   = extensions.size(),
        .enabledLayerCount       = 0,
        .pApplicationInfo        = &appInfo,
        .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO
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
