#pragma once

#include "../Allocators/HeapAllocator.h"
#include "../Manager.h"

namespace cpp2d::Memory
{
    class HeapManager :
        public Memory::Manager<HeapManager>
    {
    public:
        AddrDist offset(const void* ptr) const override;
        void request(ManagedAllocation* ptr, CU32& bytes) override;
        void release(ManagedAllocation* ptr) override;
    };

    AddrDist HeapManager::offset(const void* ptr) const
    {
        return 0;
    }

    void HeapManager::request(ManagedAllocation* ptr, CU32& bytes)
    {
        HeapAllocator& allocator = HeapAllocator::get();

        ptr->setPointer(
            allocator.allocate(bytes), bytes
        );
    }

    void HeapManager::release(ManagedAllocation* ptr) 
    {
        HeapAllocator& allocator = HeapAllocator::get();
        allocator.free(ptr->getPointer());
        ptr->setPointer(nullptr, 0);
    }
}