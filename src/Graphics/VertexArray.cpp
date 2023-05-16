#include <cpp2d/Graphics.h>

#ifdef GDI_VULKAN
#   include <vulkan/vulkan.h>
#endif

namespace cpp2d
{
    VertexArray::VertexArray(const std::initializer_list<BufferType>& bufferTypes) :
        ScopedData<VertexBuffer>(bufferTypes.size()),
        _types((BufferType*)std::malloc(sizeof(BufferType) * bufferTypes.size()))
    {
        std::memcpy(_types, bufferTypes.begin(), sizeof(BufferType) * bufferTypes.size());

        for (U32 i = 0; i < bufferTypes.size(); i++)
        {
            new(&get(i)) VertexBuffer();
            if (_types[i] == BufferType::Index)
            {
                get(i).setBinding(Buffers::Binding {
                    .index = i,
                    .stride = sizeof(U32)
                });
            }
        }
    }

    VertexArray::~VertexArray()
    {
        for (U32 i = 0; i < size(); i++)
            get(i).~VertexBuffer();
    }

    void VertexArray::setBufferData(CU32& index, const void* data, CU32& bytes)
    {
        VertexBuffer& buffer = get(index);
        buffer.setData(data, bytes);
    }

    Buffers::AttributeFrame VertexArray::getAttributeFrame() const
    {
        Buffers::AttributeFrame frame;

        frame.bindings.reserve(size());
        for (U32 i = 0; i < size(); i++)
        {
            if (_types[i] == BufferType::Index) continue;

            // Copy the binding from the buffer
            frame.bindings.push_back(get(i).getBinding());

            // Copy the attributes from the buffer
            const std::vector<Buffers::Attribute>& attributes = get(i).getAttributes();
            std::copy(attributes.begin(), attributes.end(), std::back_inserter(frame.attributes));
        }

        return frame;
    }

    void VertexArray::bind(const Graphics::FrameObject<Graphics::FrameData>& frameData)
    {
        
    }

    void VertexArray::draw(const Graphics::FrameObject<Graphics::FrameData>& frameData)
    {
#   ifdef GDI_VULKAN
        auto attributes = getAttributeFrame();
        auto command_buffer = static_cast<VkCommandBuffer>(frameData.object().command_buffer);

        VkDeviceSize vertex_offset = 0;
        VkDeviceSize index_offset  = 0;
        U32 vertex_count = 0;
        U32 index_count  = 0;

        U32 count = 0;
        for (U32 i = 0; i < size(); i++)
        {
            if (_types[i] != BufferType::Index) count++;
            else
            {
                index_count = get(i).getAllocatedSize() / get(i).getBinding().stride;
                index_offset = static_cast<VkDeviceSize>(get(i).offset() + sizeof(Buffers::GraphicsAllocatorData) + sizeof(U32) * 2);
            }

            if (_types[i] == BufferType::Vertex)
            {
                vertex_count  = get(i).getAllocatedSize() / get(i).getBinding().stride;
                vertex_offset = static_cast<VkDeviceSize>(get(i).offset() + sizeof(Buffers::GraphicsAllocatorData) + sizeof(U32) * 2);
            }
        }

        // Not sure how to remove binding here unless each buffer type has its own allocation...
        // Maybe, we have an attribute buffer that is templated to a type that represents
        // the objects inside it. Then maybe we request an allocation for this specific type
        // ... yeah, I like that...
        // This way we can be sure that when we draw a vertex array the data inside is all of one type
        // Mesh data would end up all together... index data would all be togther... this way (for basic
        // mesh data) we have only two bind calls

        const void* _heap = VertexArrayMemoryManager::get().getHeap();
        auto* allocatordata = Buffers::GraphicsAllocator::get().extractData(_heap);
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
        );

        //vkCmdDraw(command_buffer, vertex_count, 1, 0, 0);
        vkCmdDrawIndexed(command_buffer, index_count, 1, 0, 0, 0);
#   endif
    }
}