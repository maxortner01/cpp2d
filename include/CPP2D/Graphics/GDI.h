#pragma once

#include "Window.h"
#include "../Utility.h"

namespace cpp2d::Graphics
{
    struct InstanceData
    {
#ifdef GDI_VULKAN
        gdiHandle     handle;
        gdiLogical    logic_device;
        gdiPhysical   physical_device;

// IFDEF DEBUG
        void* debug_messenger;
#endif
    };

    struct InitGDIData
    {
        Window* window;
    };

    enum class GDIState
    {
        NotInitialized,
        Initialized,
        Destroyed,
        SurfaceFailed
    };

    class GDI :
        public Utility::Singleton<GDI>,
        public Utility::State<GDIState>
    {
        InstanceData _instance;

    public:
        GDI();
        ~GDI();

        const InstanceData& getInstanceData() const;

        void init(const InitGDIData& data);
        //void destroy();
    };
}