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
        const Memory::Manager* _manager;
        BufferType* const _types;

        std::vector<Buffers::BufferArrayInstance*> _buffers;

    public:
        BufferArray(Memory::Manager* manager, const std::initializer_list<BufferType>& bufferTypes);
        ~BufferArray();

        void setBufferData(CU32& index, const void* data, CU32& bytes);

        Buffers::AttributeFrame getAttributeFrame() const;

        void bind(const Memory::ManagedObject<Graphics::FrameData>& frameData, Memory::StackManager* baseStackManager);

        void draw(const Memory::ManagedObject<Graphics::FrameData>& frameData);

        void pushBuffer(Buffers::BufferArrayInstance* buffer);
    };

    void BufferArray::pushBuffer(Buffers::BufferArrayInstance* buffer)
    {
        _buffers.push_back(buffer);
    }
}