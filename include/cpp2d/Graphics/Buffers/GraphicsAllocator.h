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

    struct AllocationHeader
    {
        GraphicsAllocatorData data;
        U32 data_size;
        U32 heap_size;
    };

    class CPP2D_DLL GraphicsAllocator :
        public Memory::AllocatorHeader<AllocationHeader>
    {   
        Graphics::AllocatorHandle _allocator;

        std::vector<void*> pointers;

    public:
        GraphicsAllocator(Graphics::GDI& gdi);
        ~GraphicsAllocator();

        void* allocate(CU32& bytes) override;
        void free(void* ptr) override;
    };

    //template<typename T>
    //using GraphicsStackManager = Memory::StackManager<GraphicsAllocator, Memory::HeapManager, T>;
    //using BaseGraphicsStackManager = Memory::StackManager<GraphicsAllocator, Memory::FrameManager<Memory::HeapAllocator>, Memory::BaseStack>;

    //template<typename T>
    //using GraphicsStackManager = Memory::StackManager<BaseGraphicsStackManager, Memory::FrameManager<Memory::HeapAllocator>, T>;
}