#pragma once

#include "../util.h"

namespace cpp2d
{
    struct AttributeData
    {
        uint32_t index;
        uint32_t element_count;
        bool     instanced;

        AttributeData(const uint32_t& _index = 0, const uint32_t& _element_count = 0, bool _instanced = false) :
            index(_index), element_count(_element_count), instanced(_instanced)
        {   }
    };

    class GraphicsBuffer
    {
        const uint32_t _id;
        const uint32_t _vao_id;

    public:
        GraphicsBuffer(const uint32_t& id, const uint32_t& vaoID);

        void bind() const;

        void setAttributeData(const AttributeData& attribute);
        void setData(const void* const data, const size_t& bytesize) const;
    };
}