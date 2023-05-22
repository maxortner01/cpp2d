#pragma once

#include "../../Util.h"
#include "../Allocator.h"

namespace cpp2d::Memory
{
    class HeapAllocator :
        public Allocator
    {
    public:
        HeapAllocator() = default;
        ~HeapAllocator() = default;

        void* allocate(CU32&) override;
        void free(void*) override;
    };

    void* HeapAllocator::allocate(CU32& bytes) 
    {
        return std::malloc(bytes);
    }

    void HeapAllocator::free(void* ptr) 
    {
        if (ptr) std::free(ptr);
    }
}