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

        GDIHandle      _handle;
        GDIDebugHandle _debug;
        GDILogicDevice _device;

        void _init();
        void _delete();

    public:
        GDI();
        ~GDI();

        SurfaceHandle getSurfaceHandle(const Window* window);
        GDIHandle     getHandle() const;
    };
}