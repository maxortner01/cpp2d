#pragma once

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

    class CPP2D_DLL GDI :
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

        // Get a platform independent handle for the render api,
        // GDI is NOT responsible for destroying this object, only
        // creating it
        SurfaceHandle getSurfaceHandle(const Window* window) const;
        GDIHandle     getHandle() const;
    };
}