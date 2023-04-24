#pragma once

#include "../util.h"
#include "GraphicsBuffer.h"

namespace cpp2d
{
    class VertexArray
    {
        // Data
        GraphicsBuffer** buffers;
        //uint32_t* indices;

        // Reference info
        const uint32_t _buffer_count;
        uint32_t  _index_count;
        uint32_t  _id;
        uint32_t* _buffer_ids;

    public:
        VertexArray(const uint32_t& bufferCount);
        ~VertexArray();

        void bind() const;

        void     setIndices(const uint32_t* const indexData, const uint32_t& count);
        uint32_t getIndexCount() const;
        const uint32_t* const getIndices() const;

        const GraphicsBuffer* const getIndexBuffer() const;

        GraphicsBuffer& at(const uint32_t& index) const;
        GraphicsBuffer& operator[](const uint32_t& index) const;
    };
}