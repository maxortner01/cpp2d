#pragma once

#include "../Allocators/HeapAllocator.h"
#include "../Manager.h"

namespace cpp2d::Memory
{
    class HeapManager :
        public Memory::Manager
    {
        HeapAllocator* const _allocator;

    public:
        HeapManager(HeapAllocator* allocator);

        void request(ManagedAllocation* ptr, CU32& bytes) override;
        void release(ManagedAllocation* ptr) override;
    };

    HeapManager::HeapManager(HeapAllocator* allocator) : 
        Manager(allocator, MemoryOwner::Allocator),
        _allocator(allocator)
    {   }

    void HeapManager::request(ManagedAllocation* ptr, CU32& bytes)
    {
        ptr->setPointer(
            _allocator->allocate(bytes), bytes
        );
    }

    void HeapManager::release(ManagedAllocation* ptr) 
    {
        _allocator->free(ptr->getPointer());
        ptr->setPointer(nullptr, 0);
    }
}