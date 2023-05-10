#pragma once

#include <cpp2d/Util.h>

namespace cpp2d::Memory
{
    struct Allocator
    {
        virtual void* reallocate(CU32&, void*) = 0;
        virtual void* allocate(CU32&, void*) = 0;
        virtual void free(void*, void*) = 0;
    };
}