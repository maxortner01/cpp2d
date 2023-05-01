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
        Surface
    };

    struct GDIObjectInstance
    {
        GDIObject type;
        void*     handle;
        U32       argument_count;
        void*     arguments;
    };

    class CPP2D_DLL GDI :
        public Utility::Singleton<GDI>,
        public Utility::State<GDIState>
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

    public:
        GDI();
        ~GDI();

        SwapChainHandle createSwapChain(const Surface* window);
        SurfaceHandle   getSurfaceHandle(const Window* window);
        GDIHandle       getHandle() const;
    };
}