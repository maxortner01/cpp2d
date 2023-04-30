#pragma once

#include "../Utility.h"

namespace cpp2d::Graphics
{
    typedef void* GDIHandle;
    typedef void* GDIDebugHandle;
    typedef void* GDIPhysicalDevice;
    typedef void* GDILogicDevice;

    enum class GDIState
    {
        Initialized,
        NotInitialized,
        InstanceCreateFailed
    };

    class GDI :
        public Utility::Singleton<GDI>,
        public Utility::State<GDIState>
    {
        GDIHandle      _handle;
        GDIDebugHandle _debug;
        GDILogicDevice _device;

        void _init();
        void _delete();

    public:
        GDI();
        ~GDI();
    };
}