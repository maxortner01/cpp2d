#pragma once

#include "../Allocators/HeapAllocator.h"
#include "../Manager.h"

namespace cpp2d::Memory
{
    class HeapManager :
        public Memory::Manager<HeapManager, HeapAllocator>
    {
    public:
        AddrDist offset(void* const * ptr) const override;
        void request(void** ptr, CU32& bytes) override;
        void release(void** ptr) override;
    };

    AddrDist HeapManager::offset(void* const* ptr) const
    {
        return 0;
    }

    void HeapManager::request(void** ptr, CU32& bytes)
    {
        HeapAllocator& allocator = HeapAllocator::get();
        *ptr = allocator.allocate(bytes);
    }

    void HeapManager::release(void** ptr) 
    {
        HeapAllocator& allocator = HeapAllocator::get();
        allocator.free(*ptr);
        *ptr = nullptr;
    }
}