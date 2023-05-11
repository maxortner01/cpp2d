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

    void VertexArray::draw(const Graphics::FrameData& frameData)
    {
#   ifdef GDI_VULKAN
        auto attributes = getAttributeFrame();
        auto command_buffer = static_cast<VkCommandBuffer>(frameData.command_buffer);

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
                index_offset = static_cast<VkDeviceSize>(get(i).offset());
            }

            if (_types[i] == BufferType::Vertex)
            {
                vertex_count  = get(i).getAllocatedSize() / get(i).getBinding().stride;
                vertex_offset = static_cast<VkDeviceSize>(get(i).offset());
            }
        }

        // Need to rework this... A vertex array needs types Vertex and Other to be contiguous while
        // index is either first or last in memory.
        // There is only one vkBuffer here, so i need to stop all this collection of buffers thing
        // instead i need to find the offset of the first index section, and the offset of the first
        // index section... Then pass the regular buffer into both and the count of vertex/other buffers
        // as well as relevant offsets
        // ...

        // just kidding we need to store a list of offsets for each vertex/other buffer
        auto allocatordata = (Buffers::GraphicsAllocatorData*)(VertexArrayMemoryManager::get().getHeapData());
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
            count,
            &buffer,
            &vertex_offset
        );

        //vkCmdDraw(command_buffer, vertex_count, 1, 0, 0);
        vkCmdDrawIndexed(command_buffer, index_count, 1, 0, 0, 0);
#   endif
    }
}