#pragma once

#include <cpp2d/Utility.h>
#include <cpp2d/Memory.h>

namespace cpp2d::Buffers
{
    // So...
    // The ptr points to the heap data that the user requests, but that's
    // not all that's allocated.

    // Similarily to malloc, it'll look something like this
    // [bytes]     desc
    // [amt]       Allocation Data
    // [U32]       amt
    // [U32]       heap_size
    // [heap_size] Requested Heap <-- pointer given to here
    struct GraphicsAllocatorData
    {
        Graphics::BufferHandle     buffer;
        Graphics::AllocationHandle allocation;
    };

    class GraphicsAllocator :
        public Memory::Allocator<GraphicsAllocator>
    {   
        Graphics::AllocatorHandle _allocator;

        GraphicsAllocator();
        ~GraphicsAllocator();
    public:
        friend class Singleton<GraphicsAllocator>;

        GraphicsAllocatorData* extractData(const void* ptr) const;

        void* allocate(CU32& bytes) override;
        void free(void* ptr) override;
    };

    template<typename T>
    using GraphicsStackManager = Memory::StackManager<GraphicsAllocator, T>;
}