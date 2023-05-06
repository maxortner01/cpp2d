#pragma once

#include <stack>

#include "../Util.h"
#include "../Utility.h"
#include "GDILifetime.h"

namespace cpp2d
{
    class Shader;
    class Surface;
}

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
        public Utility::State<GDIState>,
        public Utility::NoCopy,
        public GDILifetime
    {
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

        FramebufferHandle createFramebuffer(const ImageViewHandle& imageView, const Surface* renderPass, GDILifetime* lifetime = nullptr);
        RenderPassHandle  createRenderPass(const Surface* surface);
        SwapChainInfo     createSwapChain(const DrawWindow* window);
        SurfaceHandle     getSurfaceHandle(const Window* window);
        GDIHandle         getHandle() const;

        ShaderHandle createShader(const U32* data, U32 count, GDILifetime* lifetime = nullptr);
        GDIPipeline  createPipeline(const ScopedData<Shader*>& shaders, Surface* surface);
        CommandPool  createCommandPool(GDILifetime* lifetime = nullptr);
        CommandBufferHandle createCommandBuffer(const CommandPoolHandle& commandPool);
    };
}