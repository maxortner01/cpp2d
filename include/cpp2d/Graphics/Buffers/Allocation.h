#pragma once

#include <cpp2d/Utility.h>
#include <cpp2d/Memory.h>

namespace cpp2d::Buffers
{
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
    };
}