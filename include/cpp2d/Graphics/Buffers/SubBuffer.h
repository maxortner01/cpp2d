#pragma once

#include "AttributeBuffer.h"

namespace cpp2d::Buffers
{
    class SubBuffer :
        public AttributeBuffer
    {
        U32 _offset;
        U32 _allocated_size;

    public:
        SubBuffer(Allocation* allocation);

        void setOffset(CU32& offset);
        void setData(const void* data, CU32& bytes) override;
        
        U32 getAllocatedBytes() const;
    };
}