#pragma once

#include "../util.h"

namespace cpp2d
{
    struct AttributeData
    {
        uint32_t index = 0;
        uint32_t element_count = 0;
        bool     instanced = false;
        uint32_t stride = 0;
        size_t   offset = 0;
    };

    class GraphicsBuffer
    {
        bool _dynamic;
        const uint32_t _id;
        const uint32_t _vao_id;
        uint32_t       _allocated_bytes;

    public:
        GraphicsBuffer(const uint32_t& id, const uint32_t& vaoID);

        void setDynamic(bool dynamic);

        void bind() const;

        void setAttributeData(const AttributeData* const attributes, const uint32_t attributeCount);
        void setAttributeData(const AttributeData& attribute);
        void setData(const void* const data, const size_t& bytesize);
    };
}