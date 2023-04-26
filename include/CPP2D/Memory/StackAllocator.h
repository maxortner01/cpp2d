#pragma once

#include "../util.h"

namespace cpp2d::Memory
{
    class StackAllocator
    {
        const size_t _heap_size;
        void* _ptr;

        size_t _counter;

    public:
        StackAllocator(size_t heapSize);
        virtual ~StackAllocator();

        size_t currentPosition() const;

        void* allocate(size_t byteSize);
        void  pop();

        void clear();
        
        template<typename T>
        T* allocate()
        {
            return (T*)allocate(sizeof(T));
        }
    };
}