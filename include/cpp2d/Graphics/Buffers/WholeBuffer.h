#pragma once

#include "AttributeBuffer.h"

namespace cpp2d::Buffers
{
    /**
     * @brief Represents an entire GPU memory buffer with attribute information.
     * 
     * This class has ownership of a GPU allocation, it is freed upon the destruction
     * of this object.
     */
    class WholeBuffer :
        public AttributeBuffer
    {
    public:
        WholeBuffer();
        ~WholeBuffer();

        void setData(const void* data, CU32& bytes) override;
    };
}