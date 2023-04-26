#include <cpp2d/Graphics.h>

#include <GL/glew.h>

namespace cpp2d
{
    VertexArray::VertexArray(const U32& bufferCount) :
        buffers(nullptr), _buffer_ids(nullptr), _index_count(0), _buffer_count(bufferCount)
    {
        GraphicsInstance::get().init();
        glGenVertexArrays(1, &_id);

        buffers     = (GraphicsBuffer**)std::malloc(sizeof(void*) * (bufferCount + 1));
        _buffer_ids = (U32*)       std::malloc(sizeof(void*) * (bufferCount + 1));

        bind();
        glGenBuffers(bufferCount + 1, _buffer_ids);

        for (int i = 0; i < bufferCount + 1; i++)
            buffers[i] = new GraphicsBuffer(_buffer_ids[i], _id);
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &_id);
        glDeleteBuffers(_buffer_count, _buffer_ids);

        if (buffers)
        {
            for (int i = 0; i < _buffer_count + 1; i++)
                delete buffers[i];

            std::memset(buffers, 0, sizeof(void*) * (_buffer_count + 1));
         
            std::free(buffers);
            buffers = nullptr;
        }
    }

    void VertexArray::bind() const
    {
        glBindVertexArray(_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer_ids[_buffer_count]);
    }

    void VertexArray::setIndices(const U32* const indexData, const U32& count)
    {
        _index_count = count;
        buffers[_buffer_count]->setData(indexData, sizeof(U32) * count);
    }

    U32 VertexArray::getIndexCount() const
    {
        return _index_count;
    }

    const U32* const VertexArray::getIndices() const
    {
        // do a glmap to get data
        return nullptr;
    }

    GraphicsBuffer& VertexArray::at(const U32& index) const
    {
        cppAssert(index > _buffer_count, "Index (%u) greater than buffer count (%u).", index, _buffer_count);
        return *buffers[index];
    }

    GraphicsBuffer& VertexArray::operator[](const U32& index) const
    {
        cppAssert(index > _buffer_count, "Index (%u) greater than buffer count (%u).", index, _buffer_count);
        return *buffers[index];
    }
}