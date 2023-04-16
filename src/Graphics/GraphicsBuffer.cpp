namespace cpp2d
{
    GraphicsBuffer::GraphicsBuffer(const uint32_t& id, const uint32_t& vaoID) :
        _id(id), _vao_id(vaoID)
    {   }

    void GraphicsBuffer::bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, _id);
    }

    void GraphicsBuffer::setAttributeData(const AttributeData& attribute)
    {
        glBindVertexArray(_vao_id);
        bind();
        glVertexAttribPointer(attribute.index, attribute.element_count, GL_FLOAT, false, 0, nullptr);
        glEnableVertexAttribArray(attribute.index);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void GraphicsBuffer::setData(const void* const data, const size_t& bytesize) const
    {
        glBindVertexArray(_vao_id);
        bind();
        glBufferData(GL_ARRAY_BUFFER, bytesize, data, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}