#include <cpp2d/Graphics.h>

namespace cpp2d::Buffers
{
    SubBuffer::SubBuffer(Allocation* allocation)
    {   
        _allocation = allocation;
    }

    void SubBuffer::setOffset(CU32& offset)
    {
        _offset = offset;
    }    

    void SubBuffer::setData(const void* data, CU32& bytes)
    {
        assert(_allocation);
        GraphicsAllocator& allocator = GraphicsAllocator::get();
        
        //std::cout << "requesting " << bytes << " bytes at an offset of " << _offset << "\n";

        if (_allocation->_allocation_size < bytes + _offset)
            allocator.reallocate(bytes + _offset, _allocation);
        
        void* ptr = (void*)((char*)_allocation->_data + _offset);
        std::memcpy(ptr, data, bytes);

        _allocated_size = bytes;
    }

    U32 SubBuffer::getAllocatedBytes() const
    {
        return _allocated_size;
    }
}