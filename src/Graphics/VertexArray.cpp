#include <cpp2d/Graphics.h>

#include <GL/glew.h>

namespace cpp2d
{
    VertexArray::VertexArray(const uint32_t& bufferCount) :
        buffers(nullptr), _buffer_ids(nullptr), _index_count(0), _buffer_count(bufferCount)
    {
        GraphicsInstance::get().init();
        glGenVertexArrays(1, &_id);

        buffers     = (GraphicsBuffer**)std::malloc(sizeof(void*) * (bufferCount + 1));
        _buffer_ids = (uint32_t*)       std::malloc(sizeof(void*) * (bufferCount + 1));

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

    void VertexArray::setIndices(const uint32_t* const indexData, const uint32_t& count)
    {
        _index_count = count;
        buffers[_buffer_count]->setData(indexData, sizeof(uint32_t) * count);
    }

    uint32_t VertexArray::getIndexCount() const
    {
        return _index_count;
    }

    const uint32_t* const VertexArray::getIndices() const
    {
        // do a glmap to get data
        return nullptr;
    }

    GraphicsBuffer& VertexArray::operator[](const uint32_t& index) const
    {
        assert(index < _buffer_count);
        return *buffers[index];
    }
}