#pragma once

#include <cstdlib>
#include <cassert>
#include <cstdint>

namespace cpp2d::Graphics
{
    typedef void* SurfaceHandle;
    typedef void* GDIHandle;
    typedef void* GDIDebugHandle;
    typedef void* GDIPhysicalDevice;
    typedef void* GDILogicDevice;
}

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