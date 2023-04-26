#include <cpp2d/Memory.h>

#include <cstring>
#include <cassert>
#include <cstdlib>

namespace cpp2d::Memory
{
    StackAllocator::StackAllocator(size_t heapSize) :
        _heap_size(heapSize),
        _counter(0),
        _ptr(std::malloc(heapSize))
    {   }

    StackAllocator::~StackAllocator()
    {
        if (_ptr)
        {
            std::free(_ptr);
            _ptr = nullptr;
        }
    }

    size_t StackAllocator::currentPosition() const
    {
        return _counter;
    }

    void* StackAllocator::allocate(size_t byteSize)
    {
        cppAssert(_counter + byteSize < _heap_size, "Requesting location past top.");

        char* begin = (char*)_ptr + _counter;
        _counter += byteSize + sizeof(uint32_t);

        std::memset(begin, 0, byteSize + sizeof(uint32_t));
        *(uint32_t*)(begin + byteSize) = byteSize;

        return (void*)begin;
    }

    void StackAllocator::pop()
    {
        char*    end_of_block = (char*)_ptr + _counter;
        uint32_t block_size   = *((uint32_t*)end_of_block - 1);
        _counter -= block_size + sizeof(uint32_t);
    }

    void StackAllocator::clear()
    {
        _counter = 0;
    }
}