#pragma once

#include <cstdlib>
#include <cassert>
#include <cstdint>

#ifdef _WIN32
#   ifdef CPP2D_GRAPHICS_BUILD
#       define CPP2D_DLL __declspec(dllexport) 
#   else 
#       define CPP2D_DLL __declspec(dllimport)
#   endif
#else
#   define CPP2D_DLL
#endif

typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef const U16 CU16;
typedef const U32 CU32;
typedef const U64 CU64;

typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;

typedef const I16 CI16;
typedef const I32 CI32;
typedef const I64 CI64;

typedef float  R32;
typedef double R64;

typedef const R32 CR32;
typedef const R64 CR64;

namespace cpp2d::Graphics
{
    typedef void* GDIHandle;
    typedef void* GDIDebugHandle;
    typedef void* GDIPhysicalDevice;
    typedef void* GDIDeviceHandle;

    typedef U32   FormatHandle;
    typedef void* ImageHandle;
    typedef void* ImageViewHandle;
    typedef void* SurfaceHandle;
    typedef void* SwapChainHandle;
    typedef void* ShaderHandle;
    typedef void* PipelineHandle;
    typedef void* PipelineLayoutHandle;
    typedef void* RenderPassHandle;
    typedef void* FramebufferHandle;
    typedef void* CommandPoolHandle;
    typedef void* CommandBufferHandle;

    struct CommandPool
    {
        CommandBufferHandle handle;
        GDIDeviceHandle device;
    };

    struct GDIPipeline
    {
        PipelineHandle       handle;
        PipelineLayoutHandle layout;
    };

    struct GDIImage
    {
        ImageHandle     image;
        ImageViewHandle image_view;
    };

    // Dangerous since images must be freed...
    struct SwapChainInfo
    {
        SwapChainHandle handle;
        U32 image_count;
        GDIImage* images;
        FormatHandle format;
    };

    struct GDILogicDevice
    {
        GDIDeviceHandle handle;
        I32 physical_device_index;
    };
}