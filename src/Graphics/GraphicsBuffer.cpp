#include <CPP2D/Graphics.h>
#include <GL/glew.h>

namespace cpp2d
{
    GraphicsBuffer::GraphicsBuffer(const uint32_t& id, const uint32_t& vaoID) :
        _id(id), _vao_id(vaoID), _dynamic(false), _allocated_bytes(0)
    {   }

    void GraphicsBuffer::setDynamic(bool dynamic)
    {
        _dynamic = dynamic;
    }

    void GraphicsBuffer::bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, _id);
    }

    void GraphicsBuffer::setAttributeData(const AttributeData* const attributes, const uint32_t attributeCount)
    {
        for (int i = 0; i < attributeCount; i++)
            setAttributeData(attributes[i]);
    }

    void GraphicsBuffer::setAttributeData(const AttributeData& attribute)
    {
        glBindVertexArray(_vao_id);
        bind();
        glVertexAttribPointer(attribute.index, attribute.element_count, GL_FLOAT, GL_FALSE, attribute.stride, (void*)attribute.offset);
        glEnableVertexAttribArray(attribute.index);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if (attribute.instanced)
            glVertexAttribDivisor(attribute.index, 1);

    }

    void GraphicsBuffer::setData(const void* const data, const size_t& bytesize)
    {
        glBindVertexArray(_vao_id);
        bind();

        // If the currently allocated size is different than the requested size, allocate a new
        // buffer. Otherwise, modify the currently existing data
        if (_allocated_bytes != bytesize)
        {
            glBufferData(GL_ARRAY_BUFFER, bytesize, data, (_dynamic?GL_DYNAMIC_DRAW:GL_STATIC_DRAW));
            _allocated_bytes = bytesize;
        }
        else
            glBufferSubData(GL_ARRAY_BUFFER, 0, bytesize, data);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}