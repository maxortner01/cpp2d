#pragma once

#include <stack>

#include "../Util.h"
#include "../Utility.h"

namespace cpp2d::Graphics
{
    enum class GDIState
    {
        Initialized,
        NotInitialized,
        InstanceCreateFailed,
        DeviceCreationFailed
    };

    enum class GDIObject
    {
        Instance,
        DebugMessenger,
        Device,
        Surface,
        Swapchain,
        ImageView
    };

    struct GDIObjectInstance
    {
        GDIObject type;
        void*     handle;
        Utility::ArgumentList arguments;
    };

    class CPP2D_DLL GDI :
        public Utility::Singleton<GDI>,
        public Utility::State<GDIState>,
        public Utility::NoCopy
    {
        std::stack<GDIObjectInstance> _objects;

        struct 
        {
            U32                device_count;
            GDIPhysicalDevice* handles;
        } _physical_devices;

        GDIHandle      _handle;
        GDIDebugHandle _debug;
        GDILogicDevice _device;
        I32 _suitable_device_index;

        void _init();
        void _delete();

        GDI();
        ~GDI();

    public:
        friend class Singleton<GDI>;

        SwapChainInfo createSwapChain(const Surface* window);
        SurfaceHandle getSurfaceHandle(const Window* window);
        GDIHandle     getHandle() const;
    };
}