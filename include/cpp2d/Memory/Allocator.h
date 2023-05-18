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
        virtual U32 headerSize() const { return 0; }
        virtual void* allocate(CU32&) = 0;
        virtual void free(void*) = 0;
    };

    template<typename _Header>
    struct AllocatorHeader :
        public Allocator
    {
        // need to remove this... redundant now
        inline static CU32 HeaderBytes = sizeof(_Header);

        U32 headerSize() const override;
        _Header* extractData(const void* heap) const;
    };

    template<typename _Header>
    _Header* AllocatorHeader<_Header>::extractData(const void* heap) const
    {
        return (_Header*)((U8*)heap - sizeof(_Header));
    }

    template<typename _Header>
    U32 AllocatorHeader<_Header>::headerSize() const
    {
        return sizeof(_Header);
    }
}