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

    class CPP2D_DLL GraphicsAllocator :
        public Memory::Allocator<GraphicsAllocator>
    {   
        Graphics::AllocatorHandle _allocator;

        std::vector<void*> pointers;

        GraphicsAllocator();
        ~GraphicsAllocator();
    public:
        friend class Singleton<GraphicsAllocator>;

        GraphicsAllocatorData* extractData(const void* ptr) const;

        void* allocate(CU32& bytes) override;
        void free(void* ptr) override;

        inline static CU32 HEADER_SIZE = sizeof(U32) * 2 + sizeof(GraphicsAllocatorData);
    };

    //template<typename T>
    //using GraphicsStackManager = Memory::StackManager<GraphicsAllocator, Memory::HeapManager, T>;
    using BaseGraphicsStackManager = Memory::StackManager<GraphicsAllocator, Memory::FrameManager<Memory::HeapAllocator>, Memory::BaseStack>;

    template<typename T>
    using GraphicsStackManager = Memory::StackManager<BaseGraphicsStackManager, Memory::FrameManager<Memory::HeapAllocator>, T>;
}