#pragma once

#include <stack>

#include "../Util.h"
#include "../Utility.h"
#include "GDILifetime.h"

namespace cpp2d
{
    class Shader;
}

namespace cpp2d::Graphics
{
    class Surface;
    class Frame;

    enum class GDIState
    {
        Initialized,
        NotInitialized,
        InstanceCreateFailed,
        DeviceCreationFailed
    };

    struct QueueIndices
    {
        U32 graphics;
        U32 present;
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

        // Currently, we only keep one logic device,
        // but in the future if we want more, we can keep
        // a map that lets us take device handles to their 
        // logid device struct
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

        U32 getPresentIndex(DrawWindow* surface);
        GDILogicDevice getLogicDevice();

        FramebufferHandle createFramebuffer(const ImageViewHandle& imageView, const Surface* renderPass, GDILifetime* lifetime = nullptr);
        RenderPassHandle  createRenderPass(const Surface* surface);
        SwapChainInfo     createSwapChain(const DrawWindow* window);
        SurfaceHandle     getSurfaceHandle(const Window* window);
        GDIHandle         getHandle() const;

        GDILogicDevice    getLogicDevice(const DeviceHandle& handle) const;
        GDIPhysicalDevice getSuitablePhysicalDevice() const;
        GDIPhysicalDevice getPhysicalDevice(CU32& index) const;
        QueueIndices      getQueueIndices(const GDIPhysicalDevice& physicalDevice) const;
        QueueIndices      getQueueIndices(const GDIPhysicalDevice& physicalDevice, const DrawWindow& window) const;

        U32 getNextImage(const SwapChainHandle& swapchain, const Frame& frame);

        SemaphoreHandle createSemaphore(GDILifetime* lifetime = nullptr);
        FenceHandle     createFence(GDILifetime* lifetime = nullptr);

        ShaderHandle createShader(const U32* data, U32 count, GDILifetime* lifetime = nullptr);
        GDIPipeline  createPipeline(const ScopedData<Shader*>& shaders, Surface* surface);
        CommandPool  createCommandPool(GDILifetime* lifetime = nullptr);
        CommandBufferHandle createCommandBuffer(const CommandPoolHandle& commandPool);
    };
}