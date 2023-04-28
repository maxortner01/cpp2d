#pragma once

#include <stdlib.h>
#include <cstdio>
#include <cstddef>
#include <stdint.h>

#define FREE_CHECK(ptr) \
    if (ptr) { std::free(ptr); ptr = nullptr; }

#define U8  uint8_t
#define U32 uint32_t
#define U64 uint64_t

#define CU8  const uint8_t
#define CU32 const uint32_t
#define CU64 const uint64_t

#define I8  int8_t
#define I32 int32_t
#define I64 int64_t

#define CI8  const int8_t
#define CI32 const int32_t
#define CI64 const int64_t

// get the 32-bit float
#define R32 float
#define R64 double

#ifdef GDI_VULKAN
#   define gdiHandle void*
#   define gdiSurface void*
#   define gdiImage void*
#   define gdiImageView void*
#   define gdiPhysical void*
#   define gdiLogical void*
#   define gdiSwapChain void*
#   define gdiFormat U32
#endif

#define cppAssert(expr, ...) if (!(expr)){ printf("cppAssert triggered in %s on line %i.\n", __FILE__, __LINE__); printf(__VA_ARGS__); exit(1); }