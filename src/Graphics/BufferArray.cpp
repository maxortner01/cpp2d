#include <cpp2d/Graphics.h>

#ifdef GDI_VULKAN
#   include <vulkan/vulkan.h>
#endif

namespace cpp2d
{
    BufferArray::BufferArray(Memory::Manager* manager, const std::initializer_list<BufferType>& bufferTypes) :
        _types((BufferType*)std::malloc(sizeof(BufferType) * bufferTypes.size())),
        _manager(manager)
    {
        std::memcpy(_types, bufferTypes.begin(), sizeof(BufferType) * bufferTypes.size());
    }

    BufferArray::~BufferArray()
    {
        for (U32 i = 0; i < _buffers.size(); i++)
            delete _buffers[i];

        _buffers.clear();
    }

    void BufferArray::setBufferData(CU32& index, const void* data, CU32& bytes)
    {
        auto* buffer = _buffers[index];
        buffer->setData(data, bytes);
    }

    Buffers::AttributeFrame BufferArray::getAttributeFrame() const
    {
        Buffers::AttributeFrame frame;

        frame.bindings.reserve(_buffers.size());
        for (U32 i = 0; i < _buffers.size(); i++)
        {
            if (_types[i] == BufferType::Index) continue;

            // Copy the binding from the buffer
            frame.bindings.push_back(_buffers[i]->getBinding());

            // Copy the attributes from the buffer
            const std::vector<Buffers::Attribute>& attributes = _buffers[i]->getAttributes();
            std::copy(attributes.begin(), attributes.end(), std::back_inserter(frame.attributes));
        }

        return frame;
    }

    void BufferArray::bind(const Memory::ManagedObject<Graphics::FrameData>& frameData, Memory::StackManager* baseStackManager)
    {
        auto attributes = getAttributeFrame();
        auto command_buffer = static_cast<VkCommandBuffer>(frameData.object().command_buffer);

        VkDeviceSize vertex_offset = 0;
        VkDeviceSize index_offset  = 0;

        for (U32 i = 0; i < _buffers.size(); i++)
        {
            if (_types[i] == BufferType::Index)
            {
                std::cout << "Index\n";
                index_offset = _buffers[i]->getManagerOffset();// + Buffers::GraphicsAllocator::HeaderBytes;
                std::cout << "done\n";
            }

            if (_types[i] == BufferType::Vertex)
            {
                std::cout << "Vertex\n";
                vertex_offset = _buffers[i]->getManagerOffset();// + Buffers::GraphicsAllocator::HeaderBytes;
                std::cout << "done\n";
            }
        }

        // Not sure how to remove binding here unless each buffer type has its own allocation...
        // Maybe, we have an attribute buffer that is templated to a type that represents
        // the objects inside it. Then maybe we request an allocation for this specific type
        // ... yeah, I like that...
        // This way we can be sure that when we draw a vertex array the data inside is all of one type
        // Mesh data would end up all together... index data would all be togther... this way (for basic
        // mesh data) we have only two bind calls

        // Pull the buffer/allocation info from the heap for binding
        const void* _heap   = baseStackManager->getHeap();
        auto* allocatordata = reinterpret_cast<Memory::AllocatorHeader<Buffers::AllocationHeader>*>(baseStackManager->getAllocator())->extractData(_heap);
        auto buffer = static_cast<VkBuffer>(allocatordata->data.buffer);
        if (index_offset != 0)
        {
            vkCmdBindIndexBuffer(
                command_buffer,
                buffer,
                index_offset,
                VK_INDEX_TYPE_UINT32
            );
        }

        vkCmdBindVertexBuffers(
            command_buffer,
            0,
            1,
            &buffer,
            &vertex_offset
        );
    }

    void BufferArray::draw(const Memory::ManagedObject<Graphics::FrameData>& frameData)
    {
#   ifdef GDI_VULKAN
        auto attributes = getAttributeFrame();
        auto command_buffer = static_cast<VkCommandBuffer>(frameData.object().command_buffer);

        //VkDeviceSize vertex_offset = 0;
        //VkDeviceSize index_offset  = 0;
        //U32 vertex_count = 0;
        U32 index_count  = 0;

        U32 start_index = 0;
        U32 start_vertex = 0;

        for (U32 i = 0; i < _buffers.size(); i++)
        {
            if (_types[i] == BufferType::Index)
            {
                index_count = _buffers[i]->getAllocatedSize() / _buffers[i]->getBinding().stride;
                start_index = static_cast<U32>(_buffers[i]->offset() / _buffers[i]->getBinding().stride);
            }

            if (_types[i] == BufferType::Vertex)
                start_vertex = static_cast<U32>(_buffers[i]->offset() / _buffers[i]->getBinding().stride);
        }

        // Not sure how to remove binding here unless each buffer type has its own allocation...
        // Maybe, we have an attribute buffer that is templated to a type that represents
        // the objects inside it. Then maybe we request an allocation for this specific type
        // ... yeah, I like that...
        // This way we can be sure that when we draw a vertex array the data inside is all of one type
        // Mesh data would end up all together... index data would all be togther... this way (for basic
        // mesh data) we have only two bind calls

        // Pull the buffer/allocation info from the heap for binding
        /*
        const void* _heap   = Buffers::BaseGraphicsStackManager::get().getHeap();
        auto* allocatordata = Buffers::BaseGraphicsStackManager::Allocator::get().extractData(_heap);
        auto buffer = static_cast<VkBuffer>(allocatordata->buffer);
        if (index_offset != 0)
        {
            vkCmdBindIndexBuffer(
                command_buffer,
                buffer,
                index_offset,
                VK_INDEX_TYPE_UINT32
            );
        }

        vkCmdBindVertexBuffers(
            command_buffer,
            0,
            1,
            &buffer,
            &vertex_offset
        );*/

        /*
        U32* indices = (U32*)std::malloc(sizeof(U32) * index_count);
        std::memcpy(indices, (char*)_heap + index_offset, sizeof(U32) * index_count);
        for (U32 i = 0; i < index_count; i++)
            std::cout << indices[i] << " ";
        std::cout << "\n\n";
        std::free(indices);

        //std::cout << _buffers[0]->getBinding().stride * vertex_count << "\n";

        std::cout << "vertex offset: " << vertex_offset << "\n";
        float* vertices = (float*)std::malloc(vertex_count * _buffers[0]->getBinding().stride);
        std::memcpy(vertices, (char*)_heap + vertex_offset, _buffers[0]->getBinding().stride * vertex_count);
        for (U32 i = 0; i < vertex_count * _buffers[0]->getBinding().stride / sizeof(float); i++)
            std::cout << vertices[i] << " ";
        std::cout << "\n\n";
        std::free(vertices);*/

        //vkCmdDraw(command_buffer, vertex_count, 1, 0, 0);
        vkCmdDrawIndexed(command_buffer, index_count, 1, start_index, start_vertex, 0);
#   endif
    }
}