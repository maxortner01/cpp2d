#pragma once

#include "../Allocators/HeapAllocator.h"
#include "../Manager.h"

namespace cpp2d::Memory
{
    template<typename _Allocator>
    class FreeManager :
        public Memory::Manager
    {
        std::vector<ManagedAllocation*> allocations;
        _Allocator* const _allocator;

    public:
        FreeManager(_Allocator* allocator);
        ~FreeManager();

        void request(ManagedAllocation* ptr, CU32& bytes) override;
        void release(ManagedAllocation* ptr) override;
    };

    template<typename _Allocator>
    FreeManager<_Allocator>::FreeManager(_Allocator* allocator) : 
        Manager(allocator, MemoryOwner::Allocator),
        _allocator(allocator)
    {   }

    template<typename _Allocator>
    FreeManager<_Allocator>::~FreeManager()
    {
        for (auto* allocation : allocations) release(allocation);
    }

    template<typename _Allocator>
    void FreeManager<_Allocator>::request(ManagedAllocation* ptr, CU32& bytes)
    {
        ptr->setPointer(
            _allocator->allocate(bytes), bytes
        );

        allocations.push_back(ptr);
    }

    template<typename _Allocator>
    void FreeManager<_Allocator>::release(ManagedAllocation* ptr) 
    {
        auto it = std::find(allocations.begin(), allocations.end(), ptr);
        assert(it != allocations.end());
        
        allocations.erase(it);
        _allocator->free(ptr->getPointer());
        ptr->setPointer(nullptr, 0);
    }

    typedef FreeManager<HeapAllocator> HeapManager;
}