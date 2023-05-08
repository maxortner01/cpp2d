#include <cpp2d/Graphics.h>

#ifdef GDI_VULKAN
#   include <vulkan/vulkan.h>
#endif

namespace cpp2d
{
    VertexArray::VertexArray(CU32& bufferCount) :
        ScopedData<Graphics::VertexBuffer>(bufferCount),
        _vertex_count(0)
    {
        for (U32 i = 0; i < bufferCount; i++)
            new(&get(i)) Graphics::VertexBuffer();
    }

    VertexArray::~VertexArray()
    {
        for (U32 i = 0; i < size(); i++)
            get(i).~VertexBuffer();
    }

    void VertexArray::setVertexCount(CU32& vertexCount)
    {
        _vertex_count = vertexCount;
    }

    Graphics::AttributeFrame VertexArray::getAttributeFrame() const
    {
        Graphics::AttributeFrame frame;

        frame.bindings.reserve(size());
        for (U32 i = 0; i < size(); i++)
        {
            // Copy the binding from the buffer
            frame.bindings.push_back(get(i).getBinding());

            // Copy the attributes from the buffer
            const std::vector<Graphics::Attribute> attributes = get(i).getAttributes();
            std::copy(attributes.begin(), attributes.end(), std::back_inserter(frame.attributes));
        }

        return frame;
    }

    void VertexArray::draw(const Graphics::FrameData& frameData)
    {
#   ifdef GDI_VULKAN
        ScopedData<VkBuffer> buffers(size());
        for (U32 i = 0; i < buffers.size(); i++)
            buffers[i] = static_cast<VkBuffer>(get(i).getHandle());

        auto attributes = getAttributeFrame();
        auto command_buffer = static_cast<VkCommandBuffer>(frameData.command_buffer);

        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(
            command_buffer,
            0,
            attributes.bindings.size(),
            buffers.ptr(),
            &offset
        );

        vkCmdDraw(command_buffer, _vertex_count, 1, 0, 0);
#   endif
    }
}