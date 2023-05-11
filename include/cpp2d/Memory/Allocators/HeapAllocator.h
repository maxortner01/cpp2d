#pragma once

#include "../../Util.h"
#include "../Allocator.h"

namespace cpp2d::Memory
{
    class HeapAllocator :
        public Allocator<HeapAllocator>
    {
        HeapAllocator() = default;
        ~HeapAllocator() = default;

    public:
        friend class Utility::Singleton<HeapAllocator>;

        void** allocate(CU32&) override;
        void free(void**) override;
    };

    void** HeapAllocator::allocate(CU32& bytes) 
    {
        void** ptr = (void**)std::malloc(sizeof(void*));
        *ptr = std::malloc(bytes);
        return ptr;
    }

    void HeapAllocator::free(void** ptr) 
    {
        std::free(ptr);
        std::free(*ptr);
    }
}