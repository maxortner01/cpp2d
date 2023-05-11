#pragma once

#include <cpp2d/Utility.h>
#include <cpp2d/Memory.h>

namespace cpp2d::Buffers
{
    struct GraphicsAllocatorData
    {
        void* data;
        Graphics::BufferHandle     buffer;
        Graphics::AllocationHandle allocation;
        U32 allocation_size;
    };

    class GraphicsAllocator :
        public Memory::Allocator<GraphicsAllocator>
    {   
        GraphicsAllocator();
        ~GraphicsAllocator();
    public:
        friend class Singleton<GraphicsAllocator>;

        void** allocate(CU32& bytes) override;
        void free(void** ptr) override;
    };

    template<typename T>
    using GraphicsStackManager = Memory::StackManager<GraphicsAllocator, T>;

    /*
    struct Allocation
    {
        Graphics::BufferHandle     _handle;
        Graphics::AllocationHandle _allocation;
        U32   _allocation_size;
        void* _data;
    };

    class GraphicsAllocator :
        public Utility::Singleton<GraphicsAllocator>,
        public Utility::NoCopy
    {
        GraphicsAllocator();
        ~GraphicsAllocator();

    public:
        friend class Utility::Singleton<GraphicsAllocator>;

        //void setData(const void* data, CU32& bytes, CU32& offset);
        void reallocate(CU32& new_size, Allocation* allocation_data);
        void allocate(CU32& bytes, Allocation* allocation_data);
        void free(Allocation* allocation_data);
    };*/
}