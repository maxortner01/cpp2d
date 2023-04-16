namespace cpp2d
{
    VertexArray::VertexArray(const uint32_t& bufferCount) :
        buffers(nullptr), _buffer_ids(nullptr), indices(nullptr), _index_count(0), _buffer_count(bufferCount)
    {
        GraphicsInstance::get().init();
        glGenVertexArrays(1, &_id);

        buffers     = (GraphicsBuffer**)std::malloc(sizeof(void*) * bufferCount);
        _buffer_ids = (uint32_t*)       std::malloc(sizeof(void*) * bufferCount);

        glGenBuffers(bufferCount, _buffer_ids);

        for (int i = 0; i < bufferCount; i++)
            buffers[i] = new GraphicsBuffer(_buffer_ids[i], _id);
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &_id);
        glDeleteBuffers(_buffer_count, _buffer_ids);

        if (buffers)
        {
            for (int i = 0; i < _buffer_count; i++)
                delete buffers[i];

            std::memset(buffers, 0, sizeof(void*) * _buffer_count);
         
            std::free(buffers);
            buffers = nullptr;
        }

        if (indices)
        {
            std::free(indices);
            indices = nullptr;
        }
    }

    void VertexArray::bind() const
    {
        glBindVertexArray(_id);
    }

    void VertexArray::setIndices(const uint32_t* const indexData, const size_t& count)
    {
        _index_count = count;
        if (indices) std::free(indices);

        indices = (uint32_t*)std::malloc(sizeof(uint32_t) * count);
        std::memcpy(indices, indexData, count * sizeof(uint32_t));
    }

    uint32_t VertexArray::getIndexCount() const
    {
        return _index_count;
    }

    const uint32_t* const VertexArray::getIndices() const
    {
        return indices;
    }

    GraphicsBuffer& VertexArray::operator[](const uint32_t& index) const
    {
        assert(index < sizeof(buffers) / sizeof(GraphicsBuffer*));
        return *buffers[index];
    }
}