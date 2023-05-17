#pragma once

#include <cpp2d/Util.h>
#include <cpp2d/Utility.h>

namespace cpp2d::Memory
{
    // There will be a singletone instance for every
    // type of base class put here
    struct Allocator :
        public Utility::NoCopy
    {
        virtual void* allocate(CU32&) = 0;
        virtual void free(void*) = 0;
    };

    template<typename _Header>
    struct AllocatorHeader :
        public Allocator
    {
        inline static CU32 HeaderBytes = sizeof(_Header);
        _Header* extractData(const void* heap) const;
    };

    template<typename _Header>
    _Header* AllocatorHeader<_Header>::extractData(const void* heap) const
    {
        return (_Header*)((U8*)heap - sizeof(_Header));
    }
}