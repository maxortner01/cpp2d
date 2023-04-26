#pragma once

#include "../util.h"
#include "GraphicsBuffer.h"

namespace cpp2d
{
    class VertexArray
    {
        // Data
        GraphicsBuffer** buffers;
        //U32* indices;

        // Reference info
        const U32 _buffer_count;
        U32  _index_count;
        U32  _id;
        U32* _buffer_ids;

    public:
        VertexArray(const U32& bufferCount);
        ~VertexArray();

        void bind() const;

        void setIndices(const U32* const indexData, const U32& count);
        U32  getIndexCount() const;
        const U32* const getIndices() const;

        const GraphicsBuffer* const getIndexBuffer() const;

        GraphicsBuffer& at(const U32& index) const;
        GraphicsBuffer& operator[](const U32& index) const;
    };
}