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
    
    class BufferArray;

    //using BufferArrayMemoryManager = Buffers::GraphicsStackManager<BufferArray>;
    //using VertexBuffer = Buffers::AttributeBuffer<BufferArrayMemoryManager>;
    class CPP2D_DLL BufferArray : 
        public Utility::NoCopy
    {
        //Buffers::Allocation* _allocation;
        BufferType* const _types;

        std::vector<Buffers::BufferArrayInstance*> _buffers;

    public:
        BufferArray(const std::initializer_list<BufferType>& bufferTypes);
        ~BufferArray();

        void setBufferData(CU32& index, const void* data, CU32& bytes);

        Buffers::AttributeFrame getAttributeFrame() const;

        void bind(const Graphics::FrameObject<Graphics::FrameData>& frameData);

        void draw(const Graphics::FrameObject<Graphics::FrameData>& frameData);

        template<typename _BufferType>
        Buffers::BufferArrayInstance* emplaceBuffer();
    };

    template<typename _BufferType>
    Buffers::BufferArrayInstance* BufferArray::emplaceBuffer()
    {
        static_assert(std::is_base_of<Buffers::BufferArrayInstance, _BufferType>::value);
        _buffers.push_back(new _BufferType());
        return _buffers[_buffers.size() - 1];
    }
}