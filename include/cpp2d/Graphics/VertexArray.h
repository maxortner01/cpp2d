#pragma once

#include "../Utility.h"
#include "Surface.h"
#include "Buffers.h"

namespace cpp2d
{
    enum class BufferType
    {
        Vertex,
        Other,
        Index
    };

    class CPP2D_DLL VertexArray : 
        public ScopedData<Buffers::SubBuffer>,
        public Utility::NoCopy
    {
        Buffers::Allocation* _allocation;
        BufferType* const _types;

    public:
        VertexArray(const std::initializer_list<BufferType>& bufferTypes);
        ~VertexArray();

        void setBufferData(CU32& index, const void* data, CU32& bytes);

        Buffers::AttributeFrame getAttributeFrame() const;

        void draw(const Graphics::FrameData& frameData);
    };
}